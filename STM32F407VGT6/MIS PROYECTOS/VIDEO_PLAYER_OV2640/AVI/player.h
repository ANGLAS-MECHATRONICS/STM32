/*
* Library for working with audio formats (codecs)
 *
 *  Author: VadRov
 *  Copyright (C) 2019, VadRov, all right reserved.
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
#ifndef PLAYER_H_
#define PLAYER_H_

#include "main.h"

//statuses for the player initialization function
typedef enum {
	PLAYER_INIT_OK = 0,
	PLAYER_INIT_ERROR
} PlayerInitStatus;

//statuses for the header reading function
typedef enum {
	PLAYER_READ_HEADER_OK = 0,
	PLAYER_READ_HEADER_ERROR
} PlayerReadHeaderStatus;

//statuses for the playback start function
typedef enum {
	PLAYER_START_OK = 0,
	PLAYER_START_ERROR
} PlayerStartPlayStatus;

//statuses for the playback update function
typedef enum {
	PLAYER_UPDATE_OK = 0,
	PLAYER_UPDATE_END_OF_FILE,
	PLAYER_UPDATE_ERROR
} PlayerUpdateStatus;

//statuses for the playback stop function
typedef enum {
	PLAYER_STOP_OK=0,
	PLAYER_STOP_ERROR
}PlayerStopStatus;

//statuses of the current player state
typedef enum {
	PLAYER_INIT = 0,
	PLAYER_OPEN_FILE,
	PLAYER_READ_HEADER,
	PLAYER_START_PLAY,
	PLAYER_UPDATE_PLAY,
	PLAYER_STOP
} PlayerStatus;

typedef enum {
	PLAY_FILE_OK,
	PLAY_FILE_ALLOC_ERROR,
	PLAY_FILE_INIT_ERROR,
	PLAY_FILE_OPEN_ERROR,
	PLAY_FILE_HEADER_ERROR,
	PLAY_FILE_PICTURE_ERROR,
	PLAY_FILE_START_ERROR,
	PLAY_FILE_DAC_ERROR,
	PLAY_FILE_UPDATE_ERROR,
	PLAY_FILE_UNKNOW_FORMAT
} PlayerPlayFileStatus;

struct Player;

//defining types for "callback" functions
typedef PlayerReadHeaderStatus (*PlayerReadHeaderCallback)(struct Player *player);
typedef PlayerStartPlayStatus (*PlayerStartPlayCallback)(struct Player *player);
typedef PlayerUpdateStatus (*PlayerUpdateCallback)(struct Player *player);
typedef PlayerStopStatus (*PlayerStopCallback)(struct Player *player);

//defining the Player_client_data type with client data
typedef struct {
	void *file;					//input stream
	void *client_data_decoder;  //pointer to data for a specific user decoder
} Player_client_data;

//structure with audio parameters
typedef struct {
	uint32_t channels;							//number of channels (mono, stereo)
	uint32_t bitpersample;						//data bit size/accuracy (8 bits, 16 bits, 24 bits, 32 bits)
	uint32_t samplerate;						//sampling frequency
	uint32_t samples;							//number of samples
} AudioConfig;

typedef struct {
	char mimestr[40];		//MIME type/subtype of the image, for example: image/jpeg, image/png
	unsigned long startpos;	//starting position of the image in the music file
} PictureData;

typedef struct {
	char *title;								//title of the composition
	char *artist;								//executor
	char *album;								//album
	char *genre;								//genre
	char *comments;								//a comment
	char *date;									//year
	char *trackNumber;							//serial number of the composition in the album
	PictureData picdata;						//cover details
	uint32_t duration;							//duration of the composition, in seconds
	volatile uint32_t current_duration;			//time from the start of playback, in seconds
} AudioMeta;

//определение типа Player
typedef struct Player {
	char *filename;							    //file name
	int mediatype;								//music media type (wav, mp3, ...)
	AudioConfig audioCFG;						//audio options
	AudioMeta audioMeta;						//description of the composition (title, performer, etc.)
	uint32_t *outbuf;							//pointer to the output PCM buffer. The data in the buffer is 32-bit (word), arranged channel by channel: LLRR (2 bytes per channel)
	volatile int32_t offset;					//offset in output buffer
	PlayerReadHeaderCallback header_callback;	//pointer to a file header reading function
	PlayerStartPlayCallback start_callback;		//pointer to the playback start function
	PlayerUpdateCallback update_callback;		//pointer to the playback data update function
	PlayerStopCallback stop_callback;			//pointer to stop playback function
	volatile PlayerStatus status;				//current status of the player (what it did, i.e. the last operation performed:
												//initialization, read header, open file, start, update, stop)
	void *client_data;							//pointer to client data
	volatile uint8_t fl_update;					//update request flag (0 - no update required, 1 - update required)
	volatile uint64_t PlayTimeMks;				//time from the start of playback, μs
	uint64_t FrameTimeMks;						//playback time of one frame, μs
} Player;

extern Player* player1;

//creates a player
Player* PlayerNew (void);

//удаляет плейер
void PlayerDelete (Player *player);

//initializes the player for the specific type of file being played
PlayerInitStatus PlayerInit(Player *player, PlayerReadHeaderCallback header_callback, PlayerStartPlayCallback start_callback,
							PlayerUpdateCallback update_callback, PlayerStopCallback stop_callback, void *client_data);

//reads the file header
PlayerReadHeaderStatus PlayerReadHeader(Player *player);

//playback starts
PlayerStartPlayStatus PlayerStartPlay(Player *player);

//updates playback data
PlayerUpdateStatus PlayerUpdate(Player *player);

//stops playback
PlayerStopStatus PlayerStop(Player *player);

//frees memory occupied by the AudioMeta structure (song name, artist, etc.)
void FreeAudioMeta(AudioMeta *audioMeta);

#endif /* PLAYER_H_ */
