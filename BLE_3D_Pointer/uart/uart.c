/**
 *
 ******************************************************************************
 * @file    uart.c
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - Core UART Driver
 ******************************************************************************
 * @attention
 *
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of Atmosphere IoT Corp. nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

#include "uart.h"
#include "../app_src/atmosphere_platform.h"

#ifdef ATMO_SLIM_STACK
	#ifndef NUMBER_OF_UART_DRIVER_INSTANCES
		#define NUMBER_OF_UART_DRIVER_INSTANCES 1
	#endif
#endif

#ifndef NUMBER_OF_UART_DRIVER_INSTANCES
#define NUMBER_OF_UART_DRIVER_INSTANCES 4
#endif

ATMO_DriverInstanceHandle_t numberOfUARTDriverInstance = 0;

const ATMO_UART_DriverInstance_t *uartInstances[NUMBER_OF_UART_DRIVER_INSTANCES];
ATMO_DriverInstanceData_t *uartInstancesData[NUMBER_OF_UART_DRIVER_INSTANCES];

#ifndef ATMO_UART_BUFFER_SIZE
#define ATMO_UART_BUFFER_SIZE 64
#endif

#ifdef ATMO_STATIC_CORE
static uint8_t ATMO_UART_Buffer[NUMBER_OF_UART_DRIVER_INSTANCES][ATMO_UART_BUFFER_SIZE];
#endif

static ATMO_UART_InternalConfig_t _ATMO_UART_InternalConfig[NUMBER_OF_UART_DRIVER_INSTANCES];

static void _ATMO_UART_InitializeDataBuf( ATMO_DriverInstanceHandle_t instance )
{
	ATMO_UART_InternalConfig_t *config = &_ATMO_UART_InternalConfig[instance];

#ifdef ATMO_STATIC_CORE
	config->buffer.dataBuf = ATMO_UART_Buffer[instance];
	config->buffer.bufCapacity = ATMO_UART_BUFFER_SIZE;
#endif

	if ( config->config.customBuffer != NULL )
	{
		config->buffer.dataBuf = config->config.customBuffer;
		config->buffer.bufCapacity = config->config.customBufferSize;
	}
	else
	{
		config->buffer.dataBuf = ATMO_Malloc( ATMO_UART_BUFFER_SIZE );
		config->buffer.bufCapacity = ATMO_UART_BUFFER_SIZE;
	}

	config->buffer.bufLen = 0;
	config->buffer.startPos = 0;
	config->buffer.endPos = 0;
}

static void _ATMO_UART_RingBufferInsertChar( ATMO_UART_Buf_t *buf, uint8_t c )
{
	buf->dataBuf[buf->endPos] = c;

	if ( buf->bufLen < buf->bufCapacity )
	{
		buf->bufLen++;
	}

	buf->endPos = ( buf->endPos + 1 ) % buf->bufCapacity;

	buf->dataBuf[buf->endPos] = 0;

	// Bumping back up against the start
	// Push start forward
	if ( buf->endPos == buf->startPos )
	{
		buf->startPos = ( buf->startPos + 1 ) % buf->bufCapacity;
	}
}

static inline void _ATMO_UART_RingBufferInsert( ATMO_UART_Buf_t *buf, uint8_t *data, uint32_t dataLen )
{
	uint32_t i;

	for ( i = 0; i < dataLen; i++ )
	{
		_ATMO_UART_RingBufferInsertChar( buf, data[i] );
	}
}

static ATMO_BOOL_t _ATMO_UART_RingBufferRead( ATMO_UART_Buf_t *buf, uint8_t *dest, uint32_t len )
{
	if ( len > buf->bufLen )
	{
		return false;
	}

	uint32_t i;

	for ( i = 0; i < len; i++ )
	{
		dest[i] = buf->dataBuf[buf->startPos];
		buf->bufLen--;
		buf->startPos = ( buf->startPos + 1 ) % buf->bufCapacity;
	}

	return true;
}

/**
 * @brief Format the buffer so the start is at index 0 of the array
 *
 * @param buf
 */
static void _ATMO_UART_RingBufferUnwrap( ATMO_UART_Buf_t *buf )
{
	if ( buf->startPos == 0 )
	{
		return;
	}

	uint8_t tmpBuf[buf->bufLen];
	_ATMO_UART_RingBufferRead( buf, tmpBuf, buf->bufLen );
	memcpy( buf->dataBuf, tmpBuf, buf->bufLen );
	buf->startPos = 0;
	buf->endPos = buf->bufLen;
	buf->dataBuf[buf->endPos] = 0;
}

ATMO_Status_t ATMO_UART_AddDriverInstance( const ATMO_UART_DriverInstance_t *driverInstance, ATMO_DriverInstanceData_t *driverInstanceData, ATMO_DriverInstanceHandle_t *instanceNumber )
{
	if ( !( numberOfUARTDriverInstance < NUMBER_OF_UART_DRIVER_INSTANCES ) )
	{
		return ATMO_Status_OutOfMemory;
	}

	uartInstances[numberOfUARTDriverInstance] = driverInstance;
	uartInstancesData[numberOfUARTDriverInstance] = driverInstanceData;
	*instanceNumber = numberOfUARTDriverInstance;

	numberOfUARTDriverInstance++;

	return ATMO_Status_Success;
}

ATMO_UART_Status_t ATMO_UART_Init( ATMO_DriverInstanceHandle_t instance )
{
	if ( instance >= NUMBER_OF_UART_DRIVER_INSTANCES )
	{
		return ATMO_UART_Status_Invalid;
	}

	_ATMO_UART_InternalConfig[instance].initialized = true;
	_ATMO_UART_InternalConfig[instance].cb = NULL;
	_ATMO_UART_InternalConfig[instance].abilityHandleRegistered = false;

	return uartInstances[instance]->Init( uartInstancesData[instance] );
}

ATMO_UART_Status_t ATMO_UART_DeInit( ATMO_DriverInstanceHandle_t instance )
{
	if ( instance >= NUMBER_OF_UART_DRIVER_INSTANCES )
	{
		return ATMO_UART_Status_Invalid;
	}

	return uartInstances[instance]->DeInit( uartInstancesData[instance] );
}

ATMO_UART_Status_t ATMO_UART_SetConfiguration( ATMO_DriverInstanceHandle_t instance, const ATMO_UART_Peripheral_t *config )
{
	if ( instance >= NUMBER_OF_UART_DRIVER_INSTANCES )
	{
		return ATMO_UART_Status_Invalid;
	}

#ifdef ATMO_UART_MIN_BAUD

	if ( config->baudRate < ATMO_UART_MIN_BAUD )
	{
		return ATMO_UART_Status_Invalid;
	}

#endif

	return uartInstances[instance]->SetConfiguration( uartInstancesData[instance], config );
}

ATMO_UART_Status_t ATMO_UART_GetConfiguration( ATMO_DriverInstanceHandle_t instance, ATMO_UART_Peripheral_t *config )
{
	if ( instance >= NUMBER_OF_UART_DRIVER_INSTANCES )
	{
		return ATMO_UART_Status_Invalid;
	}

	return uartInstances[instance]->GetConfiguration( uartInstancesData[instance], config );
}

ATMO_UART_Status_t ATMO_UART_WriteBlocking( ATMO_DriverInstanceHandle_t instance, const char *buffer, uint32_t length, uint32_t *numBytesSent, uint16_t timeoutMs )
{
	if ( instance >= NUMBER_OF_UART_DRIVER_INSTANCES )
	{
		return ATMO_UART_Status_Invalid;
	}

	return uartInstances[instance]->WriteBlocking( uartInstancesData[instance], buffer, length, numBytesSent, timeoutMs );
}

ATMO_UART_Status_t ATMO_UART_ReadBlocking( ATMO_DriverInstanceHandle_t instance, char *buffer, uint32_t length, uint32_t *numBytesReceived, uint16_t timeoutMs )
{
	if ( instance >= NUMBER_OF_UART_DRIVER_INSTANCES )
	{
		return ATMO_UART_Status_Invalid;
	}

	uint32_t numBytesReadFromBuffer = 0;

	// Copy the number of bytes available in the buffer
	if ( length <= _ATMO_UART_InternalConfig[instance].buffer.bufLen )
	{
		_ATMO_UART_RingBufferRead( &_ATMO_UART_InternalConfig[instance].buffer, ( uint8_t * )buffer, length );
		numBytesReadFromBuffer = length;
		*numBytesReceived = length;
		return ATMO_UART_Status_Success;
	}
	else
	{
		// Read all we can then wait for the rest
		_ATMO_UART_RingBufferRead( &_ATMO_UART_InternalConfig[instance].buffer, ( uint8_t * )buffer, _ATMO_UART_InternalConfig[instance].buffer.bufLen );
		numBytesReadFromBuffer = _ATMO_UART_InternalConfig[instance].buffer.bufLen;
	}

	ATMO_UART_Status_t status = uartInstances[instance]->ReadBlocking( uartInstancesData[instance], &buffer[numBytesReadFromBuffer], length - numBytesReadFromBuffer, numBytesReceived, timeoutMs );

	if ( status != ATMO_UART_Status_Success )
	{
		return status;
	}

	*numBytesReceived += numBytesReadFromBuffer;
	return ATMO_UART_Status_Success;
}

ATMO_UART_Status_t ATMO_UART_NumRxBytesAvailable( ATMO_DriverInstanceHandle_t instance, unsigned int *numBytesAvailable )
{
	if ( instance >= NUMBER_OF_UART_DRIVER_INSTANCES )
	{
		return ATMO_UART_Status_Invalid;
	}

	*numBytesAvailable = _ATMO_UART_InternalConfig[instance].buffer.bufLen;
	return ATMO_UART_Status_Success;
}

ATMO_UART_Status_t ATMO_UART_FlushTx( ATMO_DriverInstanceHandle_t instance )
{
	if ( instance >= NUMBER_OF_UART_DRIVER_INSTANCES )
	{
		return ATMO_UART_Status_Invalid;
	}

	return uartInstances[instance]->FlushTx( uartInstancesData[instance] );
}

ATMO_UART_Status_t ATMO_UART_FlushRx( ATMO_DriverInstanceHandle_t instance )
{
	if ( instance >= NUMBER_OF_UART_DRIVER_INSTANCES )
	{
		return ATMO_UART_Status_Invalid;
	}

	if ( _ATMO_UART_InternalConfig[instance].buffer.dataBuf == NULL )
	{
		_ATMO_UART_InitializeDataBuf( instance );
	}

	_ATMO_UART_InternalConfig[instance].buffer.dataBuf[0] = 0;
	_ATMO_UART_InternalConfig[instance].buffer.bufLen = 0;
	_ATMO_UART_InternalConfig[instance].buffer.startPos = 0;
	_ATMO_UART_InternalConfig[instance].buffer.endPos = 0;

	return uartInstances[instance]->FlushRx( uartInstancesData[instance] );
}

ATMO_UART_Status_t ATMO_UART_CheckForData( ATMO_DriverInstanceHandle_t instance )
{
	if ( instance >= NUMBER_OF_UART_DRIVER_INSTANCES )
	{
		return ATMO_UART_Status_Invalid;
	}

	if ( uartInstances[instance]->CheckForData == NULL )
	{
		return ATMO_UART_Status_Success;
	}

	return uartInstances[instance]->CheckForData( uartInstancesData[instance] );
}

ATMO_UART_Status_t ATMO_UART_RegisterRxAbilityHandle( ATMO_DriverInstanceHandle_t instance, unsigned int abilityHandle )
{
	if ( instance >= NUMBER_OF_UART_DRIVER_INSTANCES )
	{
		return ATMO_UART_Status_Invalid;
	}

	_ATMO_UART_InternalConfig[instance].abilityHandle = abilityHandle;
	_ATMO_UART_InternalConfig[instance].abilityHandleRegistered = true;

	return ATMO_UART_Status_Success;
}

ATMO_UART_Status_t ATMO_UART_RegisterRxCbFunc( ATMO_DriverInstanceHandle_t instance, ATMO_Callback_t cbFunc )
{
	if ( instance >= NUMBER_OF_UART_DRIVER_INSTANCES )
	{
		return ATMO_UART_Status_Invalid;
	}

	_ATMO_UART_InternalConfig[instance].cb = cbFunc;

	return ATMO_UART_Status_Success;
}

ATMO_UART_Status_t ATMO_UART_ReadAvailableData( ATMO_DriverInstanceHandle_t instance, uint8_t *buffer, unsigned int bufferLen, unsigned int *numBytesRead )
{
	unsigned int bytesAvailable = 0;
	unsigned int currentPos = 0;
	uint32_t numBytesRx = 0;

	ATMO_UART_NumRxBytesAvailable( instance, &bytesAvailable );

	while ( bytesAvailable > 0 && currentPos < bufferLen )
	{
		ATMO_UART_ReadBlocking( instance, ( char * )&buffer[currentPos], bytesAvailable, &numBytesRx, 1000 );
		currentPos += bytesAvailable;
		ATMO_UART_NumRxBytesAvailable( instance, &bytesAvailable );
	}

	*numBytesRead = currentPos;
	return ATMO_UART_Status_Success;
}

ATMO_UART_Status_t ATMO_UART_WriteStringBlocking( ATMO_DriverInstanceHandle_t instance, const char *buffer )
{
	if ( instance >= NUMBER_OF_UART_DRIVER_INSTANCES )
	{
		return ATMO_UART_Status_Invalid;
	}

	uint32_t numBytesSent = 0;
	return uartInstances[instance]->WriteBlocking( uartInstancesData[instance], buffer, strlen( buffer ), &numBytesSent, 1000 );
}

void ATMO_UART_GetDefaultConfig( ATMO_UART_Peripheral_t *config )
{
	config->baudRate = ATMO_UART_BaudRate_115200;
	config->numStopBits = ATMO_UART_NumStopBits_1;
	config->numDataBits = ATMO_UART_NumDataBits_8;
	config->parity = ATMO_UART_Parity_None;
	config->flowControl = ATMO_UART_FlowControl_None;
	config->customBuffer = NULL;
	config->customBufferSize = 0;
	config->rxBuffer = false;
}

static inline ATMO_BOOL_t ATMO_UART_IsWhiteSpace( char c )
{
	return ( c == ' ' || c == '\r' || c == '\n' );
}

ATMO_UART_Status_t ATMO_UART_ProcessRxData( ATMO_DriverInstanceHandle_t instance, uint8_t *rxData, unsigned int rxDataLen )
{
	ATMO_BOOL_t sendBuff = false;

	ATMO_UART_InternalConfig_t *config = &_ATMO_UART_InternalConfig[instance];

	if ( config->buffer.dataBuf == NULL )
	{
		_ATMO_UART_InitializeDataBuf( instance );
	}

	_ATMO_UART_RingBufferInsert( &config->buffer, rxData, rxDataLen );

	// Must be string data with rx buffer enabled
	if ( !config->config.isBinaryData && config->config.rxBuffer )
	{
		_ATMO_UART_RingBufferUnwrap( &config->buffer );
#ifndef ATMO_NO_REGEX

		// Does string match regex?
		if ( re_match( ( char * )config->config.splitRegex, ( char * )config->buffer.dataBuf ) != -1 )
		{
			sendBuff = true;
		}

#else

		if ( config->buffer.dataBuf[config->buffer.bufLen - 1] == '\r' || config->buffer.dataBuf[config->buffer.bufLen - 1] == '\n' )
		{
			sendBuff = true;
		}

#endif
	}

	if ( sendBuff )
	{
		// Trim the end of the buffer
		if ( config->config.rxTrim )
		{
			char *end = ( char * )&config->buffer.dataBuf[( config->buffer.bufLen ) - 1];

			while ( ( end != ( char * )config->buffer.dataBuf ) && ( ATMO_UART_IsWhiteSpace( *end ) ) )
			{
				*end = 0;
				config->buffer.bufLen--;
				config->buffer.endPos--;
				end--;
			}
		}

		// Null terminate buffer
		if ( !config->config.isBinaryData && config->config.rxBuffer )
		{
			config->buffer.dataBuf[config->buffer.bufLen] = 0;
		}

		ATMO_Value_t byteVal;
		ATMO_InitValue( &byteVal );
		ATMO_CreateValueString( &byteVal, ( char * )config->buffer.dataBuf );

		if ( config->cb != NULL )
		{
			ATMO_AddCallbackExecute( config->cb, &byteVal );
		}

		if ( config->abilityHandleRegistered )
		{
			ATMO_AddAbilityExecute( config->abilityHandle, &byteVal );
		}

		ATMO_FreeValue( &byteVal );

		config->buffer.bufLen = 0;
		config->buffer.startPos = 0;
		config->buffer.endPos = 0;
	}

	return ATMO_UART_Status_Success;
}

ATMO_UART_InternalConfig_t *ATMO_UART_GetInternalConfig( ATMO_DriverInstanceHandle_t instance )
{
	return &_ATMO_UART_InternalConfig[instance];
}