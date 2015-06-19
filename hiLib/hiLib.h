/*
*******************************************************************************
**
**  : 
*******************************************************************************
*/

#ifndef __HI_LIB_H__
#define __HI_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/mman.h>

#include "sample_comm.h"
#include "streamLib.h"
#include "global.h"

extern int gs_AppDisplayNum;


/* mpp  */
int hi_dec_mppInit(VIDEO_NORM_E videoNorm, PIC_SIZE_E picSize);

/*  */
int hi_dec_vpssStart(int channelNum, PIC_SIZE_E picSize);

/*  */
int hi_dec_voStart(PIC_SIZE_E picSize);

/*  */
int hi_dec_voBindVpss(int Channel, HI_BOOL Bind);

/*  */
int hi_dec_openChannel(int channel, PAYLOAD_TYPE_E enType, PIC_SIZE_E picSize);

/*  */
int hi_dec_decBindVpss(int Channel, HI_BOOL Bind);

/*  */
//int hi_dec_sendStream(int channel, char *pFrameBuf, int frameLen);
int hi_dec_sendStream(int channel, char *pFrameBuf, int frameLen, int pts);;

/*  */
int hi_dec_closeChannel(int channel);

/*  */
int hi_dec_closeAllChannel(int channelNum);

/* vo */
void hi_dec_decUnbindVpss(int channel);

/* vovpss */
void hi_dec_voUnbindVpss(int channel);

/* vpss */
void hi_dec_vpssStop(int channelNum);

/* vo */
void hi_dec_voStop();

/* mpp */
void hi_dec_mppDestroy();

/* Rotate */
int hi_dec_SetRotate(int chn, int Rotate);


/*  */
int hi_dec_disableChannel(int channel);

/*  */
int hi_dec_disableAllChannel(int channelNum);

/*  */
int hi_dec_setDisplayNum(int picSize, int displayNum);



/* sample_audio.c */
int hi_audio_init(int type);
int hi_audio_decode(char *pFrameBuf, int len);
void hi_audio_destroy();


#ifdef __cplusplus
}
#endif


#endif//__HI_LIB_H__

