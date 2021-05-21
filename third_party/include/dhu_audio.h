
#ifndef __DHU_AUDIO_H
#define __DHU_AUDIO_H

#define AUDIO_DEVICE 32
#define AUDIO_CARD   0
#define PERIOD_SIZE 1024
#define PERIOD_COUNT 4

typedef enum {
    MEDIA_STATE_STOP =0,  //media stop state
    MEDIA_STATE_PLAYING,  //media playing state
    MEDIA_STATE_ERR,
}MediaState;

typedef enum {
    SET_PLAYER_LOOP_COUNT    = 0,  //media stop state
    SET_PLAYER_INTERVAL_TIME ,  //media playing state
    SET_PLAYER_MAX_PARAM,
}SetPlayerParam;


/**
   * this func is used to start play wav file, only support wav media type.
   *
   * @param : filename ,the play file name
   *
   * @Returns: 0 if OK, error number on failure
**/
int startPlayMedia(char *filename);

/**
   * this func is used to get the player state.
   *
   * @Returns: MEDIA_STATE_STOP is stopped, MEDIA_STATE_PLAYING is playing.
**/
MediaState getPlayerState(void);

/**
   * this func is used to stop the player.
**/
void stopPlayMedia(void);

/**
   * this func is used to set the player param.
   *
   * @param : set param , for examle ,SET_PLAYER_LOOP_COUNT,SET_PLAYER_INTERVAL_TIME
   * @value : the value of the param.
**/
void setPlayerParam(SetPlayerParam param, int value );

#endif
