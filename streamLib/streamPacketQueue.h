#ifndef __STREAM_QUEUE_H__
#define __STREAM_QUEUE_H__


#define MAX_STREAM_FRAME_SIZE	180*1024
#define MAX_PACK_IN_ONE_FRAME	16



#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif

/* total 12Bytes */
typedef struct rtp_head_t
{
#if (BYTE_ORDER == LITTLE_ENDIAN)
	/* byte 0 */
	unsigned short cc      :4;   /* CSRC count */
	unsigned short x       :1;   /* header extension flag */
	unsigned short p       :1;   /* padding flag */
	unsigned short version :2;   /* protocol version */

	/* byte 1 */
	unsigned short pt      :7;   /* payload type */
	unsigned short marker  :1;   /* marker bit */
#elif (BYTE_ORDER == BIG_ENDIAN)
	/* byte 0 */
	unsigned short version :2;   /* protocol version */
	unsigned short p       :1;   /* padding flag */
	unsigned short x       :1;   /* header extension flag */
	unsigned short cc      :4;   /* CSRC count */
	/*byte 1*/
	unsigned short marker  :1;   /* marker bit */
	unsigned short pt      :7;   /* payload type */ 
#else
#error YOU MUST DEFINE BYTE_ORDER == LITTLE_ENDIAN OR BIG_ENDIAN !
#endif  
	/* bytes 2, 3 */
	unsigned short seqno  :16;   /* sequence number */
	/* bytes 4-7 */
	unsigned int ts;            /* timestamp in ms */
	/* bytes 8-11 */
	unsigned int ssrc;          /* synchronization source */
}rtp_head_t;



#endif//__STREAM_QUEUE_H__

