/*
 *  Copyright (C) 2019-2023, VadRov, all right reserved.
 *
*AVI playback
  *
  * Free distribution allowed.
  * For any method of distribution, attribution to the author is MANDATORY.
  * In case of changes and distribution of modifications, the indication of the original author is MANDATORY.
  * Distributed “as is”, that is, use is at your own peril and risk.
  * The author does not provide any guarantees.
 *
 *  https://www.youtube.com/@VadRov
 *  https://dzen.ru/vadrov
 *  https://vk.com/vadrov
 *  https://t.me/vadrov_channel
 */
#include "display.h"
#include "riff_read_header.h"
#include "jpeg_chan.h"
#include "mp3_callback.h"
#include "pcm5102.h"
#include "ff.h"
#include "mystring.h"
#include "player.h"
#include "avi.h"
#include "microgl2d.h"
#include "textures.h"
#include "keyboard.h"
#include <stdlib.h>
#include <string.h>

extern uint32_t millis;				//"System time" counter.

extern iPicture_jpg fon_file;		//file with a background in Jpeg format in the flash of the microcontroller.

extern uint32_t OSD_cycle_counter; 	//Counter of clock cycles spent on OSD display
									//when outputting the current frame.
uint8_t AVI_color_mode = 1;			//AVI color format: 1 - R5G6B5 color, 3 - R5G6B5 grayscale.

uint8_t AVI_volume = 15;			//Sound volume: from 0 (minimum, off) to 16 (maximum).

MGL_OBJ *AVI_osd = 0;				//Pointer to OSD object

#define  LCD_Load_JPG(lcd, x, y, w, h, image_stream, location) \
		 ((uint8_t) LCD_Load_JPG_chan ((LCD_Handler*)(lcd), (uint16_t)(x), (uint16_t)(y), (uint16_t)(w), (uint16_t)(h), (void*)(image_stream), (PictureLocation)(location)))

/*
* Play video in a specified window:
  * dir - path to the file
  * fname - file name
  * lcd - pointer to display handler
  * x, y - coordinates of the upper left corner of the window for displaying the video frame
  * win_wdt, win_hgt - width and height of the video frame output window
 */
void PlayAVI(char *dir, char *fname, LCD_Handler *lcd, uint16_t x, uint16_t y, uint16_t win_wdt, uint16_t win_hgt)
{
	FIL file; //File object
	//"Let's merge the path to the file with the file name
	char *tmp = my_strsplice(2, "/", dir, fname);
	//Let's open the file for reading (FA_READ) with an error check when opening res
	FRESULT res = f_open(&file, tmp, FA_READ);
	free(tmp);
	if (res != FR_OK) return; //Error opening file
	RiffHDR RHdr; //Let's declare a handler riff
	memset(&RHdr, 0, sizeof(RiffHDR)); //Initialize the handler riff
	//Now let's check whether this is really an AVI file. If this is him, then we consider everything necessary for
	//playback options
	if (Read_RIFF_Header(&RHdr, &file) == RIFF_OK && RHdr.mediaType == FourccType_AVI) {
		//The file read pointer is moved to the first data frame
		f_lseek(&file, RHdr.startPosData);
		//Let's declare a structure that defines a loader (chunk, chunk, "piece") of a frame in avi streams
		struct {
			uint8_t streamCode[4];	//The type of frame by which we will determine which frame
									//we are dealing (with a video frame or an audio frame)
			uint32_t len;			//Frame data length. It must be a multiple of 2, if not, then we must
									//increase this value by 1.
		} chunk;
		//We specify the coordinates of the upper left corner of the window into which we will display video frames (centering in a given window)
		if (win_hgt > RHdr.AVIHeader.dwHeight) {
			y += (win_hgt - (uint16_t)RHdr.AVIHeader.dwHeight)/2;
		}
		if (win_wdt > RHdr.AVIHeader.dwWidth) {
			x += (win_wdt - (uint16_t)RHdr.AVIHeader.dwWidth)/2;
		}
		if ((RHdr.vidFmt.biCompression != 0x47504A4D) || 	//If the video stream codec is not MJPEG or
			(RHdr.audFmt.wFormatTag != 0x0055)          ) {	//codec audio stream not mp3,
			if (RHdr.AVIStreams) free(RHdr.AVIStreams); 	//then clear the avi stream parameters,
			f_close(&file); 								//close the file file
			return;											//and stop the player (exit)
		}
		//We make a copy of the AVI file object, since we will have two independent streams:
		//one is an audio stream and the other is a video stream.
		FIL file1 = file;
		//------------------------------ Setting the audio stream, the decoder mp3 и DAC ---------------------------------
		//Let's declare a variable with decoder user data
		Player_client_data playdata;
		//Let's allocate memory for mp3 decoder data
		Player_client_mp3 *mp3  = (Player_client_mp3 *)calloc(1, sizeof(Player_client_mp3));
		//Let's create a player
		player1 = PlayerNew();
		//Initializing decoder user data
		playdata.client_data_decoder = mp3;
		playdata.file = &file;
		//Initializing the player
		PlayerInit(player1, mp3_read_header_callback, mp3_start_play_callback, mp3_update_play_callback, mp3_stop_callback, &playdata);
		//Reading the audio stream loader to configure decoder parameters and prepare for starting the player
		PlayerReadHeader(player1);
		//We start the player (a PCM buffer is created, the first two frames from the audio stream are read and decoded into the PCM buffer)
		PlayerStartPlay(player1);
		//I'm setting up an audio CAP (audio DAC)
		Setup_DAC(AudioDAC_I2S, player1->audioCFG.samplerate, LL_I2S_DATAFORMAT_16B_EXTENDED);
		//-------------------------------------------------------------------------------------------------------------
		uint64_t t_audio, t_video = 0; //Time counters by audio/video frames (for synchronization), μs
		uint32_t ms_per_frame = 0; //Video frame decoding time counter
		uint32_t MksPerFrame = RHdr.AVIHeader.dwMicroSecPerFrame; //Display time of one video frame, μs
		UINT read_b = 0; //Variable for the actual number of bytes read from the file stream
		uint8_t res1 = 0, skip_frame = 0; //Flags: file stream errors and frame skipping
		iFile_jpg jpg_file; //Structure with frame data in the file stream (pointer to file handler and frame length)
		FSIZE_t pos; //Current position in the file stream
		uint32_t p;
		char buff[20];
		jpg_file.file = &file1;

		AVI_osd = 0;		//prohibiting the display of the OSD menu when the player starts
		AVI_color_mode = 1;	//initializing the color format (1 - color, 3 - grayscale)
		uint32_t keys;		//variable for "keys" "buttons"-events
		uint8_t m_p_all = 2, m_p = 0; //Total items in the OSD menu and the number of the current item under the cursor

		//---- Setting up OSD information window objects (options) --------
		MGL_OBJ *osd1 = MGL_ObjectAdd(0, MGL_OBJ_TYPE_FILLRECTANGLE);
		MGL_SetRectangle(osd1, 0, 0, 200, 80, COLOR_DARKGREY);
		MGL_ObjectSetTransparency(osd1, 128);
		MGL_OBJ *border = MGL_ObjectAdd(osd1, MGL_OBJ_TYPE_RECTANGLE);
		MGL_SetRectangle(border, 3, 3, 197, 77, COLOR_WHITE);
		char text_clr_md[50] = "Color mode:  "; //13 characters
		my_strcpy(&text_clr_md[13], AVI_color_mode == 1 ? " Coloured" : "Grayscale");
		MGL_OBJ *pl_clr = MGL_ObjectAdd(osd1, MGL_OBJ_TYPE_TEXT);
		MGL_SetText(pl_clr, 10, 10, text_clr_md, &Font_8x13, 0, COLOR_YELLOW);
		char text_fr_core[50] = "Core speed, MHz:   "; //19 characters
		if (SystemCoreClock >= 100000000) {
			utoa(SystemCoreClock/1000000, &text_fr_core[19], 10);
		}
		else {
			text_fr_core[19] = ' ';
			utoa(SystemCoreClock/1000000, &text_fr_core[20], 10);
		}
		MGL_OBJ *pl_core = MGL_ObjectAdd(osd1, MGL_OBJ_TYPE_TEXT);
		MGL_SetText(pl_core, 10, 23, text_fr_core, &Font_8x13, 0, COLOR_YELLOW);
		char text_fps[50] = "Decoding time, ms: "; //19 characters
		MGL_OBJ *pl_fps = MGL_ObjectAdd(osd1, MGL_OBJ_TYPE_TEXT);
		MGL_SetText(pl_fps, 10, 45, text_fps, &Font_8x13, 0, COLOR_YELLOW);
		MGL_GRADIENT *grad1 = MGL_GradientCreate(MGL_GRADIENT_LINEAR);
		MGL_GradientAddColor(grad1, 0, COLOR_RED, 1);
		MGL_GradientAddColor(grad1, 50, COLOR_GREEN, 1);
		MGL_GradientAddColor(grad1, 100, COLOR_BLUE, 1);
		MGL_GradientSetDeg(grad1, 90);
		MGL_OBJ *rect = MGL_ObjectAdd(osd1, MGL_OBJ_TYPE_FILLRECTANGLE);
		MGL_SetRectangle(rect, 10, 60, 190, 70, COLOR_BLUE);
		MGL_ObjectSetGradient(rect, grad1);
		char text_prnm[] = "(c)VadRov  AVI Player";
		MGL_OBJ *pl_name = MGL_ObjectAdd(osd1, MGL_OBJ_TYPE_TEXT);
		MGL_SetText(pl_name, 16, 59, text_prnm, &Font_8x13, 0, COLOR_WHITE);
		MGL_OBJ *m_cur = MGL_ObjectAdd(osd1, MGL_OBJ_TYPE_FILLRECTANGLE);
		MGL_SetRectangle(m_cur, 6, 10 + m_p * 13, 194, 23 + m_p * 13, COLOR_RED);
		MGL_ObjectSetTransparency(m_cur, 128);
		//move the OSD window to the center of the display
		MGL_ObjectListMove(osd1, (lcd->Width - 200) / 2, (lcd->Height - 80) / 2);
		//--------------------------------------------------------------------------

		//----- Setting up OSD information window objects (volume) -------
		MGL_OBJ *osd2 = MGL_ObjectAdd(0, MGL_OBJ_TYPE_FILLRECTANGLE);
		MGL_SetRectangle(osd2, 0, 0, 200, 41, COLOR_DARKGREY);
		MGL_ObjectSetTransparency(osd2, 128);
		MGL_OBJ *border1 = MGL_ObjectAdd(osd2, MGL_OBJ_TYPE_RECTANGLE);
		MGL_SetRectangle(border1, 3, 3, 197, 38, COLOR_WHITE);
		MGL_TEXTURE texture_volume = {(MGL_IMAGE *)&image_volume, 0, 0};
		MGL_OBJ *volume = MGL_ObjectAdd(osd2, MGL_OBJ_TYPE_FILLCIRCLE);
		MGL_SetCircle(volume, 21, 20, 16, 0);
		MGL_ObjectSetTexture(volume, &texture_volume);
		MGL_OBJ *volume_bar = MGL_ObjectAdd(osd2, MGL_OBJ_TYPE_FILLRECTANGLE);
		MGL_TEXTURE texture_x = {(MGL_IMAGE *)&image_x, 0, 0};
		MGL_OBJ *volume_x = MGL_ObjectAdd(osd2, MGL_OBJ_TYPE_FILLCIRCLE);
		MGL_SetCircle(volume_x, 21, 20, 16, 0);
		MGL_ObjectSetTexture(volume_x, &texture_x);
		char text_mute[] = "Mute"; //19 characters
		MGL_OBJ *mute = MGL_ObjectAdd(osd2, MGL_OBJ_TYPE_TEXT);
		MGL_SetText(mute, 40, 14, text_mute, &Font_8x13, 0, COLOR_YELLOW);
		if (!AVI_volume) {
			MGL_ObjectSetVisible(volume_bar, 0);
		}
		else {
			uint32_t col_bar = (AVI_volume > 3 && AVI_volume < 10) ? COLOR_YELLOW : ((AVI_volume >= 10) ? COLOR_GREEN : COLOR_RED);
			MGL_SetRectangle(volume_bar, 40, 14, 40 + ((AVI_volume * (194 - 40 + 1 - 2)) >> 4), 26, col_bar);
			MGL_ObjectSetVisible(volume_x, 0);
			MGL_ObjectSetVisible(mute, 0);
		}
		//move the OSD window to the center of the display
		MGL_ObjectListMove(osd2, (lcd->Width - 200) / 2, (lcd->Height - 42) / 2);
		//--------------------------------------------------------------------------

		uint32_t osd_time = millis; //This is a time counter for the period of user inactivity
									//when the OSD "Options" window is on.
		uint32_t osd_time1 = millis;//This is a time counter for the period of user inactivity
									//when the OSD window "Volume" is turned on.
		LCD_Fill(lcd, COLOR_BLACK); //Paint the display black.
		//We start the audio stream (start the DAC). Additional coefficient 2 in samples
		//used for data in the format DATAFORMAT_16B и DATAFORMAT_16B_EXTENDED, т.к. DMA in mode half word.
		Start_DAC(AudioDAC_I2S, AudioDAC_DMA, AudioDAC_DMA_Stream, player1->outbuf, 2 * 2 * player1->audioCFG.samples);
		while (!res1 && file1.fptr < file1.obj.objsize) { //In a loop we read frames from the stream until
														  //until there is a read error or end of stream
			//We prohibit interruptions of the DMA flow serving the DAC (otherwise collisions cannot be avoided,
			//related to simultaneous SPI access to the SD card).
			NVIC_DisableIRQ(AudioDAC_IRQn);
			//We read the frame loader - chunk (it contains information about the type and length of the frame).
			res = f_read(&file1, &chunk, 8, &read_b);
			//Allow interruptions of the DMA thread serving DAC.
			NVIC_EnableIRQ(AudioDAC_IRQn);
			if (res || read_b != 8) break; //If an error occurs while reading, interrupt the cycle
			//-------------------------- checking for the next "piece" of the AVI file --------------------------
			//If the avi file is more than 1 GB, then the video can be “cut” into “pieces” starting with a chunk RIFF
			//"piece" header format "RIFF[nnnn]AVIXLIST[mmmm]movi, where:
			//nnnn and mmmm chunk lengths. After the movi chunk, frames continue to follow.
			if (*((uint32_t*)chunk.streamCode) == chunk_RIFF) { //Found a new "piece" of AVI?
				NVIC_DisableIRQ(AudioDAC_IRQn);
				res = f_read(&file1, buff, 16, &read_b);
				NVIC_EnableIRQ(AudioDAC_IRQn);
				if (res || read_b != 16) break; //If an error occurs while reading, we interrupt the cycle.
				if ( *((uint32_t*)&buff[0]) == TYPE_AVIX  &&   //Checking the format of the "chunk" header
					 *((uint32_t*)&buff[4]) == chunk_LIST &&
					 *((uint32_t*)&buff[12]) == chunk_movi   ) continue; //Success -> resume reading frames.
				else break; //The avi continuation header format is incorrect -> we interrupt the loop, ending the player.
			}
			pos = file1.fptr; //We remember the position of the read pointer in the file stream.
			jpg_file.size = chunk.len & 1 ? chunk.len + 1 : chunk.len; //The frame data size must be even.
			if ((chunk.streamCode[2]=='d' && chunk.streamCode[3]=='c') ||
				(chunk.streamCode[2]=='D' && chunk.streamCode[3]=='C')	 ) { //Checks that the current frame is a video frame.
				t_video += MksPerFrame; //We increase the duration counter by video frames.
				if (!jpg_file.size) { //We skip the empty frame.
					continue;
				}
				if (!skip_frame) { //If the video frame skip flag is cleared, then we decode the current frame.
					ms_per_frame  = millis; //We determine the frame decoding time: initialization of the counter.
					res1 = LCD_Load_JPG(lcd, x, y, win_wdt, win_hgt, &jpg_file, PICTURE_IN_FILE);
					ms_per_frame = millis - ms_per_frame - OSD_cycle_counter / (SystemCoreClock/1000); //Subtract the "costs" of OSD
					utoa(ms_per_frame, &text_fps[19], 10); //Let's convert the frame decoding time into a string.
				}
			}
			if (jpg_file.size) { //We move the reading position pointer in the file only if
								 //if you haven't subtracted the frame. For example, if a frame was skipped for synchronization purposes
								 //or it was an audio frame and should also be skipped.
				NVIC_DisableIRQ(AudioDAC_IRQn);
				res = f_lseek(&file1, pos + jpg_file.size);
				NVIC_EnableIRQ(AudioDAC_IRQn);
				if (res) break;
			}
			NVIC_DisableIRQ(AudioDAC_IRQn); //We turn off the interrupt that serves the DAC so that there are no collisions with player1,
											//which can be cleared in an interruption at the most inopportune time.
			if (player1) { //If the player is active (suddenly an error occurs in playing the audio stream)
				t_audio = player1->PlayTimeMks; //Time elapsed since the audio stream started playing
				NVIC_EnableIRQ(AudioDAC_IRQn); //Enable the service interrupt DAC.
				//--------- Synchronization with tolerance within the duration of display of one video frame ----------
				//Synchronization is not needed if the lag/advance between threads is within the duration of the video frame.
				if ((t_video >= t_audio - MksPerFrame / 2) && (t_video <= t_audio + MksPerFrame / 2)) {
					skip_frame = 0;
				}
				//There is an advance of the video stream over the audio stream: we wait for the audio stream to “catch up” with the video stream.
				else if (t_video > t_audio) {
					p = (uint32_t)((t_video - t_audio) / 1000);
					if (p) LL_mDelay(p);
					skip_frame = 0;
				}
				//There is an advance of the audio stream over the video stream: we skip the next video frame to catch up with the audio stream.
				else {
					skip_frame = 1;
				}
				//--------------------------------------------------------------------------------------------------------
			}
			else {
				NVIC_EnableIRQ(AudioDAC_IRQn); //Enable the service interrupt DAC.
				break; //The audio stream has ended -> interrupt the main loop.
			}
			//------------------------------------- OSD and encoder event processing -------------------------------------
			if (AVI_osd == osd1 && millis - osd_time >= 10000) { //Hide the OSD menu "Options" if there is no activity
				AVI_osd = 0;							 		 //user for 10 seconds or more.
			}
			if (AVI_osd == osd2 && millis - osd_time1 >= 5000) { //Hide the OSD menu "Volume" if there is no activity
				AVI_osd = 0;							 		 //user for 5 or more seconds.
			}

			uint8_t flagVideo=0;//para entrar o salid de video con misma tecla
			if (KEYB_kbhit()) { 				//Checking for an event from the encoder.
				keys = KEYB_Inkeys(); 			//We read the keys of the “buttons”-events of the encoder.
				if (AVI_osd != osd1) {	//OSD "Options" is hidden.
					if ( (keys & (1 << KEYB_RIGHT)) && !flagVideo) { //Short press the button -> show osd.
						AVI_osd = osd1;
						osd_time = millis; //Initializing the user's "inactivity" time.
						flagVideo=1;
					}
					if ( (keys & (1 << KEYB_RIGHT)) && flagVideo) break;//comentar esto para las funciones de color grayscale
					//else if (!AVI_osd && (keys & (1 << KEYB_LEFT))) break; //Hold the button -> exit the player to the file manager.
					else if (keys & (1 << KEYB_UP)) { //Increase volume.
						if (AVI_volume < 16) {
							AVI_volume++;
							uint32_t col_bar = (AVI_volume > 3 && AVI_volume < 10) ? COLOR_YELLOW : ((AVI_volume >= 10) ? COLOR_GREEN : COLOR_RED);
							MGL_SetRectangle(volume_bar, 40, 14, 40 + ((AVI_volume * (194 - 40 + 1 - 2)) >> 4), 26, col_bar);
							MGL_ObjectMove(volume_bar, (lcd->Width - 200) / 2, (lcd->Height - 42) / 2);
							MGL_ObjectSetVisible(volume_bar, 1);
							MGL_ObjectSetVisible(volume_x, 0);
							MGL_ObjectSetVisible(mute, 0);
						}
						AVI_osd = osd2;
						osd_time1 = millis; //Initializing the user's "inactivity" time.
					}
					else if (keys & (1 << KEYB_DOWN)) { //Decrease volume.
						if (AVI_volume) {
							AVI_volume--;
							if (!AVI_volume) {
								MGL_ObjectSetVisible(volume_bar, 0);
								MGL_ObjectSetVisible(volume_x, 1);
								MGL_ObjectSetVisible(mute, 1);
							}
							else {
								uint32_t col_bar = (AVI_volume > 3 && AVI_volume < 10) ? COLOR_YELLOW : ((AVI_volume >= 10) ? COLOR_GREEN : COLOR_RED);
								MGL_SetRectangle(volume_bar, 40, 14, 40 + ((AVI_volume * (194 - 40 + 1 - 2)) >> 4), 26, col_bar);
								MGL_ObjectMove(volume_bar, (lcd->Width - 200) / 2, (lcd->Height - 42) / 2);
								MGL_ObjectSetVisible(volume_x, 0);
								MGL_ObjectSetVisible(mute, 0);
							}
						}
						AVI_osd = osd2;
						osd_time1 = millis; //Initializing the user's "inactivity" time
					}
				}
				else { //OSD displayed.
					if (keys & (1 << KEYB_RIGHT)) { //Short press the button -> hide osd.
						AVI_osd = 0;
					}
					else if (keys & (1 << KEYB_UP)) { //Move up 1 item in the OSD menu.
						if (m_p) {
							m_p--;
							MGL_ObjectMove(m_cur, 0, -13); //Up.
						}
					}
					else if (keys & (1 << KEYB_DOWN)) { //Move the OSD menu down 1 item.
						if (m_p < m_p_all - 1) {
								m_p++;
								MGL_ObjectMove(m_cur, 0, 13); //Down.
						}
					}
					else if ((keys & (1 << KEYB_F1)) || (keys & (1 << KEYB_F2))) { //Change the option under the cursor.
						uint32_t pllN;
						switch (m_p) { //Select a procedure depending on the number of the menu item under the cursor.
							case 0: //Change shape color.
								AVI_color_mode = AVI_color_mode == 1 ? 3 : 1;
								my_strcpy(&text_clr_md[13], AVI_color_mode == 1 ? " Coloured" : "Grayscale");
								break;
							case 1: //Change the core frequency: nominal 84 MHz, overclocked 120 MHz
									//(the frequency of the peripheral device buses also changes).
//--------------------------- Overclocking for STM32F401xB/C, STM32F401xD/E ----------------------------
#if defined(STM32F401xB) || defined(STM32F401xC) || defined(STM32F401xD) || defined(STM32F401xE)
								if (SystemCoreClock == 84000000) {
									LL_FLASH_SetLatency(LL_FLASH_LATENCY_3); 				//Moving to a higher frequency,
									while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_3) {  }	//preliminarily increase the flash memory read delay.
									pllN = 240;												//PLLN multiplier to increase frequency (overclocking).
								}
								else {														//Transition to the maximum rated frequency.
									pllN = 168;												//PLLN multiplier for maximum rated frequency.
								}
								LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSE);			//We select HSE as the clock source.
								while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSE) {  }	//We are expecting a transition to a new clock source.
								LL_RCC_PLL_Disable();													//Turn off the module PLL.
								LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_25, pllN, LL_RCC_PLLP_DIV_2); //Changing the modulus coefficients PLL.
								LL_RCC_PLL_Enable();										//Turn on the module PLL.
								while(LL_RCC_PLL_IsReady() != 1) {  }						//We are waiting for the module to be ready PLL.
								LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);			//Select PLL as clock source.
								while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) {  } //We are expecting a transition to a new clock source.
								if (SystemCoreClock == 120000000) {							//If we switch to the nominal frequency, we reduce the flash memory latency
									LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);				//Set a reduced delay.
									while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_2) {  }	//We are waiting for the set value to be activated.
									SystemCoreClock = 84000000;								//We redefine the core frequency to the maximum nominal frequency.
								}
								else {														//Transition to higher frequency.
									SystemCoreClock = 120000000;							//We redefine the core frequency to a higher frequency.
								}
								SysTick_Config(SystemCoreClock/1000);						//Due to the change in core frequency, we reconfigure
																							//system timer SysTick for correct operation of the "system time" counter.
#endif //Overclocking for STM32F401xB/C, STM32F401xD/E
								if (SystemCoreClock >= 100000000) {							//Frequency value greater than 100 MHz (three characters) ?
									utoa(SystemCoreClock/1000000, &text_fr_core[19], 10);	//Convert the core frequency to MHz and convert its value into a string of text.
								}
								else {
									text_fr_core[19] = ' ';									//It’s the same here, but for a frequency less than 100 MHz (two characters).
									utoa(SystemCoreClock/1000000, &text_fr_core[20], 10);	//Let's move the frequency value 1 character to the right (to align
								}															//data in the OSD window on the right edge).
								break;
							default:
								break;
						}
					}
					osd_time = millis; //Reinitializing the user's "inactivity" time.
				}
			}
			//----------------------------------------------------------------------------------------------------
		}
		//If the audio stream is still active, you need to turn it off before exiting the player:
		NVIC_DisableIRQ(AudioDAC_IRQn); //We turn off the interrupt that serves the DAC so that there are no collisions with player1,
										//which can be cleared in an interruption at the most inopportune time.
		if (player1) {
			Stop_DAC(AudioDAC_I2S, AudioDAC_DMA, AudioDAC_DMA_Stream); //I'm staying DAC;
			PlayerStop(player1);		//turn off the player;
			PlayerDelete(player1);		//remove the player;
			player1 = 0;				//initialize the pointer to the player.
		}
		NVIC_ClearPendingIRQ(AudioDAC_IRQn); //Clearing a pending interrupt from a thread DMA DAC.
		NVIC_EnableIRQ(AudioDAC_IRQn); //Enable the interrupt serving the thread DMA DAC.
		f_close(&file1); //Closing the file file1.
		if (RHdr.AVIStreams) free(RHdr.AVIStreams); //Clearing stream parameters avi.
		MGL_GradientDelete(grad1); 		//Removing MGL objects used
		MGL_ObjectsListDelete(osd1);	//to create OSD images
		MGL_ObjectsListDelete(osd2);
		return;
	}
	f_close(&file); //Closing the file file.
	if (RHdr.AVIStreams) free(RHdr.AVIStreams); //Clearing stream parameters avi.
}

/*
static void Render2D (LCD_Handler *lcd, MGL_OBJ *obj, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	uint16_t mem_size = lcd->Width;
	uint8_t use_dma = 0;
	if (lcd->spi_data.dma_tx.dma) {
		mem_size *= 2;
		use_dma = 1;
	}
	uint16_t render_buf[mem_size];
	uint16_t *render_ptr = render_buf;
	MGL_OBJ *obj_ptr;
	LCD_SetActiveWindow(lcd, x1, y1, x2, y2);
	lcd->cs_control = lcd->dc_control = 1;
	LCD_ResCS(lcd);
	LCD_SetDC(lcd);
	for (uint16_t y = y1; y <= y2; y++)	{
		obj_ptr = obj;
		while (obj_ptr) {
			MGL_RenderObjects(obj_ptr, x1, y, x2, y, render_ptr);
			obj_ptr = obj_ptr->next;
		}
		if (use_dma) {
			while(LCD_GetState(lcd) != LCD_STATE_READY) {  }
			LCD_WriteDataDMA(lcd, render_ptr, (uint32_t)mem_size / 2);
			render_ptr = (render_ptr == render_buf) ? render_buf + mem_size / 2: render_buf;
		}
		else {
			LCD_WriteData(lcd, render_ptr, (uint32_t)mem_size);
		}
	}
	if (use_dma) {
		while(LCD_GetState(lcd) != LCD_STATE_READY) {  }
	}
	lcd->cs_control = lcd->dc_control = 0;
	LCD_SetCS(lcd);
}
*/

void AVIIntro(LCD_Handler *lcd)
{
	int dx = (lcd->Width - (30+70+45+5))/2;
	int dy = (lcd->Height - 45)/2;

	MGL_OBJ *intro = MGL_ObjectAdd(0, MGL_OBJ_TYPE_FILLRECTANGLE);
	MGL_SetRectangle(intro, 0, 0, lcd->Width - 1, lcd->Height - 1, COLOR_BLACK);

	MGL_GRADIENT *grad_sym = MGL_GradientCreate(MGL_GRADIENT_LINEAR);
	MGL_GradientAddColor(grad_sym,   0, COLOR_YELLOW, 1);
	MGL_GradientAddColor(grad_sym, 100, COLOR_RED,   1);
	MGL_GradientSetDeg(grad_sym, 0);

	MGL_OBJ *sym_A = MGL_ObjectAdd(intro, MGL_OBJ_TYPE_FILLTRIANGLE);
	MGL_SetTriangle(sym_A, 15, 0, 0, 45, 30, 45, 0);
	MGL_ObjectSetGradient(sym_A, grad_sym);

	MGL_OBJ *sym_V = MGL_ObjectAdd(intro, MGL_OBJ_TYPE_FILLTRIANGLE);
	MGL_SetTriangle(sym_V, 0+20, 0, 30+20, 0, 15+20, 45, 0);
	MGL_ObjectSetGradient(sym_V, grad_sym);

	MGL_OBJ *sym_I = MGL_ObjectAdd(intro, MGL_OBJ_TYPE_FILLRECTANGLE);
	MGL_SetRectangle(sym_I, 0+55, 0, 10+55, 45, 0);
	MGL_ObjectSetGradient(sym_I, grad_sym);

	MGL_TEXTURE texture_avatar = {(MGL_IMAGE *)&image_avatar, 0, 0};
	MGL_OBJ *avatar = MGL_ObjectAdd(intro, MGL_OBJ_TYPE_FILLCIRCLE);
	MGL_SetCircle(avatar, 0+70+45/2, 45/2, 45/2, 0);
	MGL_ObjectSetTexture(avatar, &texture_avatar);

	MGL_OBJ *play = MGL_ObjectAdd(intro, MGL_OBJ_TYPE_FILLTRIANGLE);
	MGL_SetTriangle(play, 0+70+45+5, 0+7, 0+70+45+5, 30+7, 30+70+45+5, 15+7, 0);
	MGL_ObjectSetGradient(play, grad_sym);

	MGL_ObjectListMove(sym_A, dx, dy);

	char txt_prnm[] = "STM32 AVI Player";
	char txt_cprt[] = "(C)2023 VadRoV";
	MGL_OBJ *prnm = MGL_ObjectAdd(intro, MGL_OBJ_TYPE_TEXT);
	MGL_SetText(prnm, lcd->Width, dy - 13, txt_prnm, &Font_8x13, 0, COLOR_WHITE);
	MGL_OBJ *cprt = MGL_ObjectAdd(intro, MGL_OBJ_TYPE_TEXT);
	MGL_SetText(cprt, -8 * my_strlen(txt_cprt) - 8 * (my_strlen(txt_prnm) - my_strlen(txt_cprt)) / 2, dy + 45, txt_cprt, &Font_8x13, 0, COLOR_WHITE);

	AVI_osd = intro;

	int tr = 255, tr1 = 0;
	int x = lcd->Width - dx - ((30+70+45+5) - 8 * my_strlen(txt_prnm))/2;
	while (x > 0) {
		MGL_ObjectSetTransparency(intro, tr1);
		MGL_ObjectListTransparency(sym_A, tr);
		MGL_ObjectMove(prnm, -4, 0);
		MGL_ObjectMove(cprt, +4, 0);
		if (LCD_Load_JPG(lcd, 0, 0, lcd->Width, lcd->Height, &fon_file, PICTURE_IN_MEMORY)) break;
		texture_avatar.alpha += 8;
		if (texture_avatar.alpha > 360) texture_avatar.alpha = 360;
		tr -= 8;
		tr1 += 10;
		if (tr < 0) tr = 0;
		if (tr1 > 255) {
			MGL_ObjectSetVisible(intro, 0);
		}
		x -= 4;
	}

	LL_mDelay(2000);

	tr = 0;
	while (tr <= 255) {
		if (LCD_Load_JPG(lcd, 0, 0, lcd->Width, lcd->Height, &fon_file, PICTURE_IN_MEMORY)) break;
		MGL_ObjectListMove(sym_A, 0, -4);
		tr += 15;
		MGL_ObjectListTransparency(sym_A, tr);
	}

	MGL_GradientDelete(grad_sym);
	MGL_ObjectsListDelete(intro);
	AVI_osd = 0;
}
