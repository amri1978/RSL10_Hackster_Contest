#ifndef _ATMO_RINGBUFFER_H_
#define _ATMO_RINGBUFFER_H_

/** @defgroup RingBuffer Ring Buffer
 *  @{
 */

/** \addtogroup RingBuffer
 *  The ring buffer driver is used for providing a generic ring buffer data structure.
 *  @{
 */

#include "../app_src/atmosphere_platform.h"
#include <stdint.h>

typedef void ( *ATMO_RingBuffer_Free_t )( void *value );

#ifndef ATMO_STATIC_CORE
/// \cond DO_NOT_DOCUMENT
typedef struct
{
	uint32_t head;
	uint32_t tail;
	uint32_t count;
	uint8_t capacity;
	uint32_t elementSize;
	ATMO_RingBuffer_Free_t freeCb;
	void *entry;
} ATMO_RingBuffer_t;
#else
/// \cond DO_NOT_DOCUMENT
typedef struct
{
	uint8_t head;
	uint8_t tail;
	uint8_t count;
	uint8_t capacity;
	uint8_t elementSize;
	ATMO_RingBuffer_Free_t freeCb;
	void *entry;
} ATMO_RingBuffer_t;
#endif
/// \endcond

/**
 * Initialize ring buffer storage.
 *
 * @param[in] buf - Pointer to ring buffer
 * @param[in] capacity - Number of elements
 * @param[in] elementSize - Size in bytes of each element
 *
 * @return true if successful, false if error
 */
ATMO_BOOL_t ATMO_RingBuffer_Init( ATMO_RingBuffer_t *buf, uint8_t capacity, uint32_t elementSize, ATMO_RingBuffer_Free_t freeCb );

/**
 * Free ring buffer storage
 *
 * @param[in] buf - Pointer to ring buffer
 * @return true if successful, false if error
 */
ATMO_BOOL_t ATMO_RingBuffer_Free( ATMO_RingBuffer_t *buf );

/**
 * @brief Initialize ring buffer with pre-allocated storage.
 *
 * @param buf
 * @param bufData
 * @param capacity
 * @param elementSize
 * @return true
 * @return false
 */
ATMO_BOOL_t ATMO_RingBuffer_InitWithBuf( ATMO_RingBuffer_t *buf, uint8_t *bufData, uint8_t capacity, uint32_t elementSize, ATMO_RingBuffer_Free_t freeCb );

/**
 * @brief Is the ring buffer full?
 *
 * @param buf
 * @return true if full, false if not
 */
ATMO_BOOL_t ATMO_RingBuffer_Full( ATMO_RingBuffer_t *buf );

/**
 * Is the ring buffer empty?
 *
 * @param[in] buf
 *
 * @return true if empty, false if not
 */
ATMO_BOOL_t ATMO_RingBuffer_Empty( ATMO_RingBuffer_t *buf );

/**
 * Retrieve the head of the ring buffer. Does not pop.
 *
 * @param[in] buf
 *
 * @return pointer to the head of the ring buffer. NULL if buffer is empty.
 */
void *ATMO_RingBuffer_Head( ATMO_RingBuffer_t *buf );

/**
 * Retrieve pointer to element at specific index of ring buffer.
 *
 * @param[in] buf
 * @param[in] index
 *
 * @return pointer to element. NULL if element does not exist
 */
void *ATMO_RingBuffer_Index( ATMO_RingBuffer_t *buf, uint8_t index );

/**
 * Retrieve the tail of the ring buffer. Does not pop.
 *
 * @param[in] buf
 *
 * @return pointer to the tail of the ring buffer. NULL if buffer is empty.
 */
void *ATMO_RingBuffer_Tail( ATMO_RingBuffer_t *buf );

/**
 * Pop element off of the front of the ring buffer.
 *
 * @param[in] buf
 * @return pointer to element. You must free element yourself if it was malloc'd
 */
void *ATMO_RingBuffer_Pop( ATMO_RingBuffer_t *buf );

/**
 * Push element onto back of ring buffer.
 *
 * @param[in] buf
 * @param[in] data
 *
 * @return true if pushed, false if full or error
 */
ATMO_BOOL_t ATMO_RingBuffer_Push( ATMO_RingBuffer_t *buf, void *data );

#endif
/** @}*/

/** @}*/


