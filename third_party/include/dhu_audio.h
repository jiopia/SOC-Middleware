/* Copyright Statement:
 *
 * This software/firmware and related documentation ("ECARX Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to ECARX Co.Ltd and/or its licensors. Without
 * the prior written permission of ECARX Co.Ltd and/or its licensors, any
 * reproduction, modification, use or disclosure of ECARX Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * ECARX Co.Ltd (C) 2019. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("ECARX SOFTWARE")
 * RECEIVED FROM ECARX AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. ECARX EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES ECARX PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE ECARX SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN ECARX
 * SOFTWARE. ECARX SHALL ALSO NOT BE RESPONSIBLE FOR ANY ECARX SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND ECARX'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE ECARX SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT ECARX'S OPTION, TO REVISE OR REPLACE THE
 * ECARX SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO ECARX FOR SUCH ECARX SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("ECARX
 * Software") have been modified by ECARX Co.Ltd All revisions are subject to
 * any receiver's applicable license agreements with ECARX Co.Ltd.
 */


#ifndef __DHU_AUDIO_H
#define __DHU_AUDIO_H

#define AUDIO_DEVICE 32
#define AUDIO_CARD   0
#define PERIOD_SIZE 1024
#define PERIOD_COUNT 4

#define MAX_VOLUME 40

typedef enum {
    MEDIA_STATE_STOP =0,  //media stop state
    MEDIA_STATE_PLAYING,  //media playing state
    MEDIA_STATE_ERR,
}MediaState;

typedef enum {
    SET_PLAYER_LOOP_COUNT    = 0,  //media stop state
    SET_PLAYER_INTERVAL_TIME ,     //media playing state
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

/**
   * this func is used to set the audio volume.
   *
   * @index : volume index , 0-40 level. 
   * @Returns: 0 if OK, error number on failure
**/
int setVolume(int index );

#endif
