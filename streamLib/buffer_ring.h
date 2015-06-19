#ifndef BUFFER_pRingBuf_H
#define BUFFER_pRingBuf_H

#define  MAX_BUFFER_SIZE  (1024*1024)

typedef struct
{
    unsigned char * buf;
	int bufLen;
	unsigned int dataStart;
	unsigned int dataEnd;
}ring_buffer_t;

typedef struct
{
    unsigned char *BufferAddr;
    int BufferLen;
    unsigned int DataStart;
    unsigned int DataEnd;
}Packet_buffer_t;




typedef enum
{
    FRAME_START = 0,
    FRAME_NOT_START,

}FRAME_FLAG;



int PacketBuffer_init(Packet_buffer_t * PacketBuff , unsigned int size);


void PacketBuffer_destroy(Packet_buffer_t * PacketBuff);

unsigned char * GetPacketBufferAddr(Packet_buffer_t * PacketBuff,FRAME_FLAG Type);

void SetPacketBufferEnd(Packet_buffer_t * PacketBuff, unsigned int offset);

int GetPacketBufferEnd(Packet_buffer_t * PacketBuff);








int ring_free_size_ex(ring_buffer_t *pRingBuf);

void ring_set_data_end(ring_buffer_t *pRingBuf, int offset);

int ring_pop_temp(ring_buffer_t *pRingBuf, unsigned char *pBuf, unsigned int len);

void ring_set_data_start(ring_buffer_t *pRingBuf, int offset);


/*  */
int ring_init(ring_buffer_t *pRingBuf, int size);

/*  */
void ring_destroy(ring_buffer_t *pRingBuf);

/***/
void ring_reset(ring_buffer_t *pRingBuf);

/**()*/
unsigned int ring_used_size(ring_buffer_t *pRingBuf);

/***/
unsigned int ring_free_size(ring_buffer_t *pRingBuf);

/***/
int ring_isfull(ring_buffer_t *pRingBuf);

/***/
int ring_isempty(ring_buffer_t *pRingBuf);

/**buflen .true,false*/
int ring_push_n(ring_buffer_t *pRingBuf, unsigned char *buf, unsigned int len);
int ring_push(ring_buffer_t *pRingBuf, unsigned char *pBuf); 

/**len*/
int ring_pop_n(ring_buffer_t *pRingBuf, unsigned char *pBuf, unsigned int len,unsigned int BuffSize);

int private_protocol_ring_pop_n(ring_buffer_t *pRingBuf, unsigned char *pBuf, unsigned int len);
int ring_pop(ring_buffer_t *pRingBuf, unsigned char *pBuf);

/**,*/
unsigned int ring_printf(ring_buffer_t *pRingBuf);

#endif
