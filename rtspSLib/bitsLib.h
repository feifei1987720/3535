/*****************************************************************************
 * bits.h
 *****************************************************************************
 
 *****************************************************************************/
#pragma once

typedef struct bits_buffer_s
{
	int     i_size;
	int     i_data;
	unsigned char *p_data;
	unsigned char i_mask;
	unsigned char unused[3];
}bits_buffer_t;

static inline int bits_initwrite( bits_buffer_t *p_buffer, int i_size, void *p_data )
{
	p_buffer->i_size 	= i_size;
	p_buffer->i_data 	= 0;
	p_buffer->i_mask 	= 0x80;
	p_buffer->p_data 	= (unsigned char *)p_data;
	//p_buffer->p_data[0] = 0;
	if( !p_buffer->p_data )
	{
		if( !( p_buffer->p_data = (unsigned char *)malloc(i_size)))
			return -1;
	}

	p_buffer->p_data[0] = 0;/**/
	return 0;
}

static inline void bits_align( bits_buffer_t *p_buffer )
{
	if( p_buffer->i_mask != 0x80 && p_buffer->i_data < p_buffer->i_size )
	{
		p_buffer->i_mask = 0x80;
		p_buffer->i_data++;
		p_buffer->p_data[p_buffer->i_data] = 0x00;
	}
}

static inline void bits_write( bits_buffer_t *p_buffer, int i_count, unsigned long long i_bits )
{
	while( i_count > 0 )
	{
		i_count--;

		if( ( i_bits >> i_count )&0x01 )
		{
			p_buffer->p_data[p_buffer->i_data] |= p_buffer->i_mask;
		}
		else
		{
			p_buffer->p_data[p_buffer->i_data] &= ~p_buffer->i_mask;
		}
		p_buffer->i_mask >>= 1;
		if( p_buffer->i_mask == 0 )
		{
			p_buffer->i_data++;
			p_buffer->i_mask = 0x80;
		}
	}
}


