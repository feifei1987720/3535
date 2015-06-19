/***********************************************************************************
* Filename:	streamLib.c

* Description:	   yuv
************************************************************************************/

#include "streamLib.h"
#include "streamPacketQueue.h"
#include "streamFrameQueue.h"
#include "streamYuvQueue.h"

/*  */
int streamLibInit()
{
#if 0
	int i = 0;
	resource_info_t *pResourceInfo = NULL;
	stream_packet_queue_t *pPacketQueue = NULL;
	stream_frame_queue_t *pVideoFrameQueue = NULL, *pAudioFrameQueue = NULL;
	stream_yuv_queue_t *pYuvQueue = NULL;

	pResourceInfo = statusGetResourceInfo();

	for(i = 0; i < MAX_DISPLAY_CHANNEL_NUM; i++)
	{
		pPacketQueue = &pResourceInfo->streamInfo[i].packetQueue;
		pVideoFrameQueue = &pResourceInfo->streamInfo[i].frameQueue[FRAME_TYPE_VIDEO];
		pAudioFrameQueue = &pResourceInfo->streamInfo[i].frameQueue[FRAME_TYPE_AUDIO];
		pYuvQueue = &pResourceInfo->streamInfo[i].yuvQueue;

		/*  */
		if(streamInitPacketQueue(pPacketQueue) == -1)
		{//
			Printf("init packet queue error\r\n");
			return -1;
		}
		if(streamInitFrameQueue(pVideoFrameQueue) == -1)
		{
			Printf("init frame queue error\r\n");
			return -1;
		}
		if(streamInitFrameQueue(pAudioFrameQueue) == -1)
		{
			Printf("init frame queue error\r\n");
			return -1;
		}
		if(streamInitYuvQueue(pYuvQueue) == -1)
		{
			Printf("init yuv queue error\r\n");
			return -1;
		}
	}
#endif
	streamInitAlarmQueue(NULL);

	return 0;
}

/*  */
int streamLibUninit()
{
#if 0
	int i = 0;
	resource_info_t *pResourceInfo = NULL;
	stream_packet_queue_t *pPacketQueue = NULL;
	stream_frame_queue_t *pVideoFrameQueue = NULL, *pAudioFrameQueue = NULL;
	stream_yuv_queue_t *pYuvQueue = NULL;

	pResourceInfo = statusGetResourceInfo();

	for(i = 0; i < MAX_DISPLAY_CHANNEL_NUM; i++)
	{
		pPacketQueue = &pResourceInfo->streamInfo[i].packetQueue;
		pVideoFrameQueue = &pResourceInfo->streamInfo[i].frameQueue[FRAME_TYPE_VIDEO];
		pAudioFrameQueue = &pResourceInfo->streamInfo[i].frameQueue[FRAME_TYPE_AUDIO];
		pYuvQueue = &pResourceInfo->streamInfo[i].yuvQueue;

		streamUninitPacketQueue(pPacketQueue);
		streamUninitFrameQueue(pVideoFrameQueue);
		streamUninitFrameQueue(pAudioFrameQueue);
		streamUninitYuvQueue(pYuvQueue);
	}
#endif
	streamUninitAlarmQueue(NULL);

	return 0;
}


