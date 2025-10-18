/*
 *  Библиотека работы с аудио-форматами (кодеки)
 *
 *  Author: VadRov
 *  Copyright (C) 2019, VadRov, all right reserved.
 *
 *  Допускается свободное распространение.
 *  При любом способе распространения указание автора ОБЯЗАТЕЛЬНО.
 *  В случае внесения изменений и распространения модификаций указание первоначального автора ОБЯЗАТЕЛЬНО.
 *  Распространяется по типу "как есть", то есть использование осуществляется на свой страх и риск.
 *  Автор не предоставляет никаких гарантий.
 *
 *  https://www.youtube.com/c/VadRov
 *  https://zen.yandex.ru/vadrov
 *  https://vk.com/vadrov
 *  https://t.me/vadrov_channel
 *
 */
#include "player.h"
#include "mystring.h"
#include <string.h>
#include <stdlib.h>

Player *player1 = 0;

/* Creates a player and returns a pointer to it */
Player* PlayerNew (void)
{
	return (Player*)calloc(1, sizeof(Player));
}

/* Removes the player and clears all data */
void PlayerDelete (Player *player)
{
	if (player)	{
		void *decoder = ((Player_client_data *)player->client_data)->client_data_decoder;
		if (decoder) free (decoder);
		if (player->outbuf) free(player->outbuf);
		if (player->filename) free(player->filename);
		FreeAudioMeta(&player->audioMeta);
		free(player);
	}
}

/*Initializes the player*/
PlayerInitStatus PlayerInit (Player *player, PlayerReadHeaderCallback header_callback, PlayerStartPlayCallback start_callback,
							 PlayerUpdateCallback update_callback, PlayerStopCallback stop_callback, void *client_data)
{
	if ( player == 0 || header_callback == 0 || start_callback == 0 || update_callback == 0 || stop_callback == 0)
		return PLAYER_INIT_ERROR;
	player->header_callback = header_callback;
	player->start_callback = start_callback;
	player->update_callback = update_callback;
	player->stop_callback = stop_callback;
	player->client_data = client_data;
	player->status = PLAYER_INIT;
	return PLAYER_INIT_OK;
}

/* Reads the stream header, generates setup data for the player */
PlayerReadHeaderStatus PlayerReadHeader(Player *player)
{
	player->status = PLAYER_READ_HEADER;
	return player->header_callback(player);
}

/* Prepares the player to start playback
    After calling this function you can start DAC */
PlayerStartPlayStatus PlayerStartPlay(Player *player)
{
	AudioConfig *audioCFG = &player->audioCFG;
	//based on the number of samples in the frame, we allocate the required amount of memory buffer (memory for 2 frames of samples)
	uint32_t *outbuf = calloc(2 * audioCFG->samples, sizeof(uint32_t));
	if (outbuf == 0) return PLAYER_START_ERROR;
	player->offset = 0;
	player->outbuf = outbuf;
	//decode the first frame
	if (player->start_callback(player) != PLAYER_START_OK) return PLAYER_START_ERROR;
	player->offset = audioCFG->samples;
	//decode the second frame
	if (player->update_callback(player) != PLAYER_UPDATE_OK) return PLAYER_START_ERROR;
	player->status = PLAYER_START_PLAY;
	player->fl_update = 0;
	player->PlayTimeMks = 0; //playback counter, in ms
	//duration of playback of 1 frame, in µs
	player->FrameTimeMks = (1000000 * audioCFG->samples) / audioCFG->samplerate;
	return PLAYER_START_OK;
}

/* Updates data in the freed part of the PCM buffer */
PlayerUpdateStatus PlayerUpdate(Player *player)
{
	AudioConfig *audioCFG = &player->audioCFG;
	player->status = PLAYER_UPDATE_PLAY;
	player->offset = (player->offset==0) ? audioCFG->samples : 0;
	int res = player->update_callback(player);
	player->fl_update = 0;
	return res;
}

/* Stops the player.
    Before calling this function, you must stop DAC */
PlayerStopStatus PlayerStop(Player *player)
{
	player->status = PLAYER_STOP;
	return player->stop_callback(player);
}

/* Clears song metadata */
void FreeAudioMeta(AudioMeta *audioMeta)
{
	if (audioMeta) {
		if (audioMeta->title) free(audioMeta->title);
		if (audioMeta->artist) free(audioMeta->artist);
		if (audioMeta->album) free(audioMeta->album);
		if (audioMeta->genre) free(audioMeta->genre);
		if (audioMeta->date) free(audioMeta->date);
		if (audioMeta->comments) free(audioMeta->comments);
		if (audioMeta->trackNumber) free(audioMeta->trackNumber);
	}
}
