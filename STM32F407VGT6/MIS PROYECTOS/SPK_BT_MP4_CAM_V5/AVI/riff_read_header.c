/*
  * Library for working with audio formats (codecs)
  *
  * Author: VadRov
  * Copyright (C) 2019, VadRov, all rights reserved.
  *
  * Free distribution allowed.
  * For any method of distribution, attribution to the author is MANDATORY.
  * In case of changes and distribution of modifications, the indication of the original author is MANDATORY.
  * Distributed “as is”, that is, use is at your own peril and risk.
  * The author does not provide any guarantees.
 *
 *  https://www.youtube.com/c/VadRov
 *  https://zen.yandex.ru/vadrov
 *  https://vk.com/vadrov
 *  https://t.me/vadrov_channel
 *
 */
#include "riff_read_header.h"
#include <stdlib.h>

//Reading the header of the file with the riff container
//input is a pointer to the riffHeader structure with information about playback parameters
//output status
RIFF_HDR_STATUS Read_RIFF_Header (RiffHDR *RHdr, FIL *file)
{
	Riff_Chunk chunk;
	UINT b_read = 0;
	FRESULT res;
	res = f_read(file, &chunk, 12, &b_read); //read the first 12 bytes of the header
	if (res || b_read != 12) return RIFF_IO_ERROR;
	if (chunk.chunk_id != chunk_RIFF) return RIFF_FMT_ERROR; //chunk check RIFF
	if (chunk.chunk_type != TYPE_AVI) return RIFF_FMT_ERROR; //type checking AVI
	RHdr->AllStreams = 0; //initializing the number of threads in AVI
	FSIZE_t len_stream = 0;
	while (file->fptr < file->obj.objsize) { //read chunks from AVI until we view the entire file
		res = f_read(file, &chunk, 12, &b_read); //read the current chunk with information about id, size and type
		if (res || b_read != 12) return RIFF_IO_ERROR;
		switch (chunk.chunk_id) { //check the chunk by id
			case chunk_LIST:	//LIST
				switch(chunk.chunk_type) { //checking the type:
					case chunk_hdrl:	//this is the bootloader AVI
						res = f_read(file, &RHdr->AVIHeader, sizeof(MainAVIHeader), &b_read);
						if (res || b_read != sizeof(MainAVIHeader)) return RIFF_IO_ERROR;
						if (RHdr->AVIHeader.ChunID != chunk_avih) return RIFF_FMT_ERROR;
						break;
					case chunk_strl: //this is a new thread AVI
						len_stream = chunk.chunk_size - 4;
						RHdr->AllStreams++; //increase the thread counter by AVI
						//override array size with threads AVI
						RHdr->AVIStreams = (AVIStreamHeader *)realloc(RHdr->AVIStreams, RHdr->AllStreams * sizeof(AVIStreamHeader));
						res = f_read(file, &chunk, 8, &b_read); //read the next chunk
						if (res || b_read != 8) return RIFF_IO_ERROR;
						len_stream -= 8;
						if (chunk.chunk_id != chunk_strh) return RIFF_FMT_ERROR; //it should be a chunk strh - stream loader
						//read the stream loader
						res = f_read(file, &RHdr->AVIStreams[RHdr->AllStreams-1], sizeof(AVIStreamHeader), &b_read);
						if (res || b_read != sizeof(AVIStreamHeader)) return RIFF_IO_ERROR;
						len_stream -= sizeof(AVIStreamHeader);
						res = f_read(file, &chunk, 8, &b_read); //read the next chunk
						if (res || b_read != 8) return RIFF_IO_ERROR;
						len_stream -= 8;
						if (chunk.chunk_id != chunk_strf) return RIFF_FMT_ERROR; //it should be a strf chunk
						switch(RHdr->AVIStreams[RHdr->AllStreams-1].fccType) { //depending on the type of stream, we read its loader
							case chunk_vids: //this is a video stream
								res = f_read(file, &RHdr->vidFmt, sizeof(BITMAPINFOHEADER), &b_read);
								if (res || b_read != sizeof(BITMAPINFOHEADER)) return RIFF_IO_ERROR;
								len_stream -= sizeof(BITMAPINFOHEADER);
								break;
							case chunk_auds: //this is an audio stream
								res = f_read(file, &RHdr->audFmt, sizeof(WAVEFORMAT), &b_read);
								if (res || b_read != sizeof(WAVEFORMAT)) return RIFF_IO_ERROR;
								len_stream -= sizeof(WAVEFORMAT);
								break;
							default: //skip unknown/unnecessary stream
								break;
						}
						f_lseek(file, file->fptr + len_stream); //skip unread data
						break;
					case chunk_movi: //movi chunk - followed by audio and video frames
						RHdr->lenghtData = chunk.chunk_size; //remember the length of the chunk and
						RHdr->startPosData = file->fptr;	 //frame start location
						f_lseek(file, file->fptr + chunk.chunk_size - 4);
						break;
					default:
						f_lseek(file, file->fptr + chunk.chunk_size - 4);
						break;
				}
				break;
			case chunk_idx1: //chunk with index information (for rewinding), remember its location
				RHdr->lenghtIdx1 = chunk.chunk_size;
				RHdr->startPosIdx1 = file->fptr - 4;
				f_lseek(file, file->fptr + chunk.chunk_size - 4); //skip index information
				break;
			default: //skip unknown/unnecessary chunk
				f_lseek(file, file->fptr + chunk.chunk_size - 4);
				break;
		}
	}
	return RIFF_OK;
}
