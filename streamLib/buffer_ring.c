#include <memory.h>
#include <stdio.h>
#include "buffer_ring.h"
#include "commonLib.h"

int ring_init(ring_buffer_t *pRingBuf, int size)
{
	pRingBuf->buf = malloc(size);
	if(pRingBuf->buf == NULL)
	{
		return -1;
	}
    memset(pRingBuf->buf,0,size);
	pRingBuf->bufLen = size;

	return 0;
}

void ring_destroy(ring_buffer_t *pRingBuf)
{
	if(pRingBuf->buf != NULL)
	{
		free(pRingBuf->buf);
		pRingBuf->buf = NULL;
	}

	pRingBuf->bufLen = 0;
}

void ring_reset(ring_buffer_t *pRingBuf)
{
	pRingBuf->dataEnd = 0;
	pRingBuf->dataStart = 0;
}

/***/
unsigned int ring_used_size(ring_buffer_t *pRingBuf)
{
	if(pRingBuf ==NULL)
	{
		Printf("pRingBuf NULL fail \n");
		return -1;
	}
	if(pRingBuf->bufLen <= 0)
	{
		return -1;
	}
	 #if 1
	if( 0!=pRingBuf->dataEnd && pRingBuf->dataEnd == pRingBuf->dataStart)
	{
	       Printf("start =%d ,end =%d \r\n",pRingBuf->dataStart,pRingBuf->dataEnd);
          return   pRingBuf->bufLen  - pRingBuf->dataStart ;
	}
   	#endif
   // Printf( "used_size =%d \r\n",(pRingBuf->dataEnd - pRingBuf->dataStart + (pRingBuf->bufLen)) % (pRingBuf->bufLen));
	return (pRingBuf->dataEnd - pRingBuf->dataStart + (pRingBuf->bufLen)) % (pRingBuf->bufLen);
}

/***/
unsigned int ring_free_size(ring_buffer_t *pRingBuf)
{
	if(pRingBuf->bufLen <= 0)
	{
		return -1;
	}

	return (pRingBuf->dataStart - pRingBuf->dataEnd + (pRingBuf->bufLen)) % (pRingBuf->bufLen);
}

/***/
int ring_isfull(ring_buffer_t *pRingBuf)
{
	if(pRingBuf->bufLen <= 0)
	{
		return -1;
	}

	return ((pRingBuf->dataEnd) % (pRingBuf->bufLen) == pRingBuf->dataStart);
}

/***/
int ring_isempty(ring_buffer_t *pRingBuf)
{
	if(pRingBuf->bufLen <= 0)
	{
		return -1;
	}

	return pRingBuf->dataStart == pRingBuf->dataEnd;
}

/**buflen.0,-1*/
int ring_push_n(ring_buffer_t *pRingBuf, unsigned char *buf, unsigned int len)
{
	unsigned i = 0;

	if(ring_free_size(pRingBuf) < len)
	{
		return -1;
	}

	for(i = 0; i < len; i++)
	{
		pRingBuf->buf[(pRingBuf->dataEnd+i)%(pRingBuf->bufLen)] = buf[i];
	}
	pRingBuf->dataEnd = (pRingBuf->dataEnd + len) % (pRingBuf->bufLen);

	return 0;
}

/**P*/
int ring_push(ring_buffer_t *pRingBuf, unsigned char *pBuf)
{
	if(pRingBuf->bufLen <= 0)
	{
		return -1;
	}

	return ring_push_n(pRingBuf, pBuf, 1);
}


/**lenP*/
int ring_pop_n(ring_buffer_t *pRingBuf, unsigned char *pBuf, unsigned int len,unsigned int BuffSize)
{
	unsigned i = 0;

	if((ring_used_size(pRingBuf) < len) || (pBuf == NULL) || (len <= 0) || (len >BuffSize))
	{
	       Printf("size fail len =%d ,buffsize =%d\r\n",len,BuffSize);
		return -1;
	}

	for(i = 0; i < len; i++)
	{
		//pBuf[i] = pRingBuf->buf[pRingBuf->dataStart];
		//pRingBuf->dataStart = (pRingBuf->dataStart + 1) % (pRingBuf->bufLen);
		pBuf[i] = pRingBuf->buf[(pRingBuf->dataStart+i)%(pRingBuf->bufLen)];
	}
	pRingBuf->dataStart = (pRingBuf->dataStart + len) % (pRingBuf->bufLen);
	//memcpy(pBuf,&pRingBuf->buf[pRingBuf->dataStart],len);
	//pRingBuf->dataStart = (pRingBuf->dataStart + len) % (pRingBuf->bufLen);
	//	Printf("start %d. end %d\r\n", pRingBuf->dataStart, pRingBuf->dataEnd);

	return 0;
}

/**
 *
 */
int ring_pop(ring_buffer_t *pRingBuf, unsigned char *pBuf)
{
	if(pRingBuf->bufLen <= 0)
	{
		return -1;
	}

	return ring_pop_n(pRingBuf, pBuf, 1,MAX_BUFFER_SIZE);
}

/**lenP*/
int private_protocol_ring_pop_n(ring_buffer_t *pRingBuf, unsigned char *pBuf, unsigned int len)
{
	unsigned int i = 0;

	if((ring_used_size(pRingBuf) < len) || (pBuf == NULL) || (len <= 0) )
	{
		Printf("ring_pop_n %d  %d err!!!!!!!!!!!!!!!!!!!!!\r\n",len,ring_used_size(pRingBuf));
		return -1;
	}

	for(i = 0; i < len; i++)
	{
		pBuf[i] = pRingBuf->buf[(pRingBuf->dataStart+i)%(pRingBuf->bufLen)];
	}
	pRingBuf->dataStart = ( pRingBuf->dataStart + len) % (pRingBuf->bufLen);

	//  Printf("start %d. end %d  len =%d \r\n", pRingBuf->dataStart, pRingBuf->dataEnd,len);

	return 0;
}

/**
 *
 */
unsigned int ring_printf(ring_buffer_t *pRingBuf)
{
	unsigned int i = 0;

	for(i = pRingBuf->dataStart; i < pRingBuf->dataStart + ring_used_size(pRingBuf); i++)
	{
		Printf("%02X , ", pRingBuf->buf[i % (pRingBuf->bufLen)]);
	}

	return ring_used_size(pRingBuf);
}


/*******************************************************************/
/* buf */
unsigned char * ring_get_start_buf(ring_buffer_t *pRingBuf)
{
	return pRingBuf->buf + pRingBuf->dataEnd;
}

/* buffer */
int ring_free_size_ex(ring_buffer_t *pRingBuf)
{
	if(pRingBuf->bufLen <= 0)
	{
		return -1;
	}
	if(pRingBuf->dataEnd!=0 &&  pRingBuf->dataEnd == pRingBuf->dataStart  )
	{
	    //Printf("  start=%d,end =%d\r\n",pRingBuf->dataStart,pRingBuf->dataEnd);
        return  0;
	}
	if(pRingBuf->dataStart > pRingBuf->dataEnd)
	{
		return pRingBuf->dataStart - pRingBuf->dataEnd;
	}
	else
	{
		return pRingBuf->bufLen - pRingBuf->dataEnd;
	}
}

/*  */
void ring_set_data_end(ring_buffer_t *pRingBuf, int offset)
{
	pRingBuf->dataEnd = (pRingBuf->dataEnd + offset) % (pRingBuf->bufLen);
}




//added by cq  20140703
int ring_pop_temp(ring_buffer_t *pRingBuf, unsigned char *pBuf, unsigned int len)
{
	unsigned i = 0;
	int dataStart=0;
	if((ring_used_size(pRingBuf) < len) || (pBuf == NULL) || (len <= 0) )
	{
		Printf("size fail  len =%d \r\n",len);
		return -1;
	}
	dataStart = pRingBuf->dataStart;
	for(i = 0; i < len; i++)
	{
		pBuf[i] = pRingBuf->buf[(dataStart+i)%(pRingBuf->bufLen)];
	}
	dataStart = (dataStart + len) % (pRingBuf->bufLen);

	return 0;
}

void ring_set_data_start(ring_buffer_t *pRingBuf, int offset)
{
	pRingBuf->dataStart = (pRingBuf->dataStart + offset ) % (pRingBuf->bufLen);
}





/**************************************/
// packet buffer new code
/**************************************/


int PacketBuffer_init(Packet_buffer_t * PacketBuff , unsigned int size)
{
    PacketBuff->BufferAddr = malloc(size);
    if( PacketBuff->BufferAddr == NULL)
    {
        return -1;
    }
    memset( PacketBuff->BufferAddr,0,size);
    PacketBuff->BufferLen = size;

    return 0;
}


void PacketBuffer_destroy(Packet_buffer_t * PacketBuff)
{
    if(PacketBuff->BufferAddr != NULL)
    {
        free(PacketBuff->BufferAddr);
        PacketBuff->BufferAddr = NULL;
    }
    PacketBuff->BufferLen = 0;
}


unsigned char * GetPacketBufferAddr(Packet_buffer_t * PacketBuff,FRAME_FLAG Type)
{
    if(FRAME_START==Type)
    {
        PacketBuff->DataEnd = 0;
        PacketBuff->DataStart = 0;
        return PacketBuff->BufferAddr ;
    }
    return PacketBuff->BufferAddr + PacketBuff->DataEnd;

}


void SetPacketBufferEnd(Packet_buffer_t * PacketBuff, unsigned int offset)
{
    PacketBuff->DataEnd = (PacketBuff->DataEnd + offset) ;
}


int GetPacketBufferEnd(Packet_buffer_t * PacketBuff)
{
  return  PacketBuff->DataEnd;
}

#if 0
void PacketBuffer_Reset(Packet_buffer_t *PacketBuff)
{
    PacketBuff->DataEnd = 0;
    PacketBuff->DataStart = 0;
}

#endif


