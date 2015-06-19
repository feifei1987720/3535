#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <termios.h>
#include<netdb.h>



#define SNDRV_PCM_HW_PARAM_ACCESS	((__force snd_pcm_hw_param_t) 0) /* Access type */
#define Audio_memFree(ptr)        free(ptr)
#define Audio_memAlloc(size)      (void*)malloc((size))
#define SND_PCM_FORMAT_S16_LE		2	/* Little endian signed 16*/
#define SND_PCM_STREAM_PLAYBACK		0	/* PCM stream playback mode*/
#define SND_PCM_ACCESS_RW_INTERLEAVED		3	/* PCM access interleaved*/


#define  DRV_AUDIO_DEBUG

#define 	GS_FAIL			(-1)
#define 	GS_SUCCESS		(0)

typedef struct AudioConfig {

  unsigned short deviceId;
  unsigned short samplingRate;
  unsigned short numChannels;
  unsigned short buff_Of_Samples;
  unsigned short bufferSize;

} AudioConfig;

typedef struct snd_pcm_hw_params snd_pcm_hw_params_t;

struct snd_pcm_hw_params {
	unsigned int flags;
	unsigned int rmask;		/* W: requested masks */
	unsigned int cmask;		/* R: changed masks */
	unsigned int info;			/* R: Info flags for returned setup */
	unsigned int msbits;		/* R: used most significant bits */
	unsigned int rate_num;		/* R: rate numerator */
	unsigned int rate_den;		/* R: rate denominator */
	unsigned long fifo_size;	/* R: chip FIFO size in frames */
	unsigned char reserved[64];	/* reserved for future */
};


typedef struct {

  void *hndl;

} AudioHndl;


typedef struct {

	AudioHndl 	audioHndl;

} AUDIO_Ctrl;



typedef struct {
	void *sound_handle;
} AudioObj;


/*open audio handle*/
int AudioplayOpen(AudioHndl *hndl, AudioConfig *config);

/*write  audio data to audio_driver*/
int AudioWrite(AudioHndl *hndl, void *pInBuf, int writeSamples);

  /**/

extern int snd_strerror(int rc);
extern int snd_pcm_open(void *sound_handle, char buffer[], int mode , int state);
extern int  snd_pcm_hw_params_malloc(void *sound_params);
extern int snd_pcm_hw_params_any(void *sound_handle,snd_pcm_hw_params_t *sound_params);
extern int snd_pcm_hw_params_set_access(void *sound_handle,snd_pcm_hw_params_t *sound_params,int interlace);
extern int snd_pcm_hw_params_set_format(void *sound_handle,snd_pcm_hw_params_t *sound_params,int siged);
extern int snd_pcm_hw_params_set_channels(void *sound_handle,snd_pcm_hw_params_t *sound_params,int numchannels);
extern int snd_pcm_hw_params_set_rate_near(void *sound_handle,snd_pcm_hw_params_t *sound_params,unsigned int *val ,int *dir);
extern int snd_pcm_hw_params_set_period_size_near(void *sound_handle,snd_pcm_hw_params_t *sound_params,unsigned long *buffer_frames,int *dir);
extern int snd_pcm_hw_params_set_buffer_size_near(void *sound_handle,snd_pcm_hw_params_t *sound_params,signed long *buffer_frames);
extern int snd_pcm_hw_params(void *sound_handle,snd_pcm_hw_params_t *sound_params);
extern int snd_pcm_writei(void *sound_handle,void  *pInBuf, int writeSamples);
extern int snd_pcm_prepare(void *sound_handle);
extern int snd_pcm_drain(void *sound_handle);
extern int snd_pcm_close(void *sound_handle);

int startAudioThread(int pos);
void ulaw2linear_g(unsigned char *inbuf, short *outbuf, int count);


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */
