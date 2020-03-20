#include "atmosphere_ringbuffer.h"

ATMO_BOOL_t ATMO_RingBuffer_Init( ATMO_RingBuffer_t *buf, uint8_t capacity, uint32_t elementSize, ATMO_RingBuffer_Free_t freeCb )
{
	buf->entry = ATMO_Malloc( capacity * elementSize );

	if ( buf->entry == NULL )
	{
		return false;
	}

	buf->head = 0;
	buf->tail = 0;
	buf->count = 0;
	buf->capacity = capacity;
	buf->elementSize = elementSize;
	buf->freeCb = freeCb;
	return true;
}

ATMO_BOOL_t ATMO_RingBuffer_Free( ATMO_RingBuffer_t *buf )
{
	ATMO_Free( buf->entry );
	return true;
}

ATMO_BOOL_t ATMO_RingBuffer_InitWithBuf( ATMO_RingBuffer_t *buf, uint8_t *bufData, uint8_t capacity, uint32_t elementSize, ATMO_RingBuffer_Free_t freeCb )
{
	buf->entry = bufData;

	if ( buf->entry == NULL )
	{
		return false;
	}

	buf->head = 0;
	buf->tail = 0;
	buf->count = 0;
	buf->capacity = capacity;
	buf->elementSize = elementSize;
	buf->freeCb = freeCb;
	return true;
}


ATMO_BOOL_t ATMO_RingBuffer_Full( ATMO_RingBuffer_t *buf )
{
	return buf->count >= buf->capacity;
}

ATMO_BOOL_t ATMO_RingBuffer_Empty( ATMO_RingBuffer_t *buf )
{
	return buf->count == 0;
}

void *ATMO_RingBuffer_Head( ATMO_RingBuffer_t *buf )
{
	uint8_t *bufBytes = ( uint8_t * )buf->entry;
	return &bufBytes[buf->head * buf->elementSize];
}

void *ATMO_RingBuffer_Index( ATMO_RingBuffer_t *buf, uint8_t index )
{
	if ( index > buf->count )
	{
		return NULL;
	}

	uint8_t *bufBytes = ( uint8_t * )buf->entry;
	return &bufBytes[index * buf->elementSize];
}

void *ATMO_RingBuffer_Tail( ATMO_RingBuffer_t *buf )
{
	uint8_t *bufBytes = ( uint8_t * )buf->entry;
	return &bufBytes[buf->tail * buf->elementSize];
}

void *ATMO_RingBuffer_Pop( ATMO_RingBuffer_t *buf )
{
	if ( ATMO_RingBuffer_Empty( buf ) )
	{
		return NULL;
	}

	void *entry = ATMO_RingBuffer_Head( buf );
	buf->head = ( buf->head + 1 ) % buf->capacity;
	buf->count--;
	return entry;
}

ATMO_BOOL_t ATMO_RingBuffer_Push( ATMO_RingBuffer_t *buf, void *data )
{
	if ( ATMO_RingBuffer_Full( buf ) )
	{
		ATMO_PLATFORM_DebugPrint( "Buffer full\r\n" );
		void *entry = ATMO_RingBuffer_Pop( buf );

		if ( buf->freeCb != NULL )
		{
			buf->freeCb( entry );
		}
	}

	void *entry = ATMO_RingBuffer_Tail( buf );
	memcpy( entry, data, buf->elementSize );
	buf->tail = ( buf->tail + 1 ) % buf->capacity;
	buf->count++;
	return true;
}
