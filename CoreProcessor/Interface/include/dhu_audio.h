
#ifndef __DHU_AUDIO_H
#define __DHU_AUDIO_H

#define AUDIO_DEVICE 32
#define AUDIO_CARD 0
#define PERIOD_SIZE 1024
#define PERIOD_COUNT 4

#define WARN_NAME "warning3.wav"
#define TICK_NAME "tick-441.wav"
#define TOCK_NAME "tock-441.wav"

typedef enum
{
   MEDIA_STATE_STOP = 0, //media stop state
   MEDIA_STATE_PLAYING,  //media playing state
   MEDIA_STATE_ERR,
} MediaState;

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

#endif
