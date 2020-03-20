/**
 ******************************************************************************
 * @file    http.c
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - Core HTTP Driver
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

#include "http.h"
#include "picohttpparser.h"
#include "../atmo/core.h"
#include "../app_src/atmosphere_platform.h"
#include "../tcpclient/tcpclient.h"

#define ATMO_HTTP_NUM_INSTANCES (2)

static void _ATMO_HTTP_Tcp1RxCb( void *rawData );
static void _ATMO_HTTP_Tcp2RxCb( void *rawData );

typedef struct
{
	ATMO_DriverInstanceHandle_t tcpClientInstance;
	uint8_t *pktBuf;
	uint32_t pktBufSize;
	ATMO_BOOL_t rxDataAvailable;
	uint8_t *lastRespData;
	uint32_t lastRespDataLen;
} ATMO_HTTP_PrivData_t;

static ATMO_HTTP_PrivData_t _ATMO_HTTP_PrivData[2];
static ATMO_Callback_t _ATMO_HTTP_TcpCallbacks[2] = {_ATMO_HTTP_Tcp1RxCb, _ATMO_HTTP_Tcp2RxCb};
static uint8_t _ATMO_HTTP_NumInstances = 0;

#define ATMO_HTTP_POST_TEMPLATE "POST %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\nContent-Type: %s\r\nContent-Length: %d\r\n"
#define ATMO_HTTP_GET_TEMPLATE "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n"

static void _ATMO_HTTP_Tcp1RxCb( void *rawData )
{
	_ATMO_HTTP_PrivData[0].rxDataAvailable = true;
}

static void _ATMO_HTTP_Tcp2RxCb( void *rawData )
{
	_ATMO_HTTP_PrivData[1].rxDataAvailable = true;
}

ATMO_HTTP_Status_t ATMO_HTTP_Init( ATMO_DriverInstanceHandle_t tcpClientDriverInstance, uint8_t *packetBuf, uint32_t packetBufSize, ATMO_DriverInstanceHandle_t *httpInstance )
{
	if ( _ATMO_HTTP_NumInstances >= ATMO_HTTP_NUM_INSTANCES )
	{
		return ATMO_HTTP_Status_Fail;
	}

	_ATMO_HTTP_PrivData[_ATMO_HTTP_NumInstances].tcpClientInstance = tcpClientDriverInstance;
	_ATMO_HTTP_PrivData[_ATMO_HTTP_NumInstances].pktBuf = packetBuf;
	_ATMO_HTTP_PrivData[_ATMO_HTTP_NumInstances].pktBufSize = packetBufSize;
	_ATMO_HTTP_PrivData[_ATMO_HTTP_NumInstances].rxDataAvailable = false;
	_ATMO_HTTP_PrivData[_ATMO_HTTP_NumInstances].lastRespData = NULL;
	_ATMO_HTTP_PrivData[_ATMO_HTTP_NumInstances].lastRespDataLen = 0;
	ATMO_TCP_CLIENT_SetReceiveCallback( tcpClientDriverInstance, _ATMO_HTTP_TcpCallbacks[_ATMO_HTTP_NumInstances] );
	*httpInstance = _ATMO_HTTP_NumInstances++;

	return ATMO_HTTP_Status_Success;
}

ATMO_HTTP_Status_t ATMO_HTTP_DeInit( ATMO_DriverInstanceHandle_t instance )
{
	return ATMO_HTTP_Status_Success;
}


ATMO_HTTP_Status_t ATMO_HTTP_SetConfiguration( ATMO_DriverInstanceHandle_t instance, const ATMO_HTTP_Config_t *config )
{
	return ATMO_HTTP_Status_Success;
}

static void _ATMO_HTTP_ParseUrl( const char *url, ATMO_BOOL_t *isHttps, unsigned int *port, const char **host, unsigned int *hostLen, char **path, unsigned int *pathLen )
{
	*port = 80;

	// Find the protocol
	if ( strstr( url, "https" ) == url )
	{
		*isHttps = true;
		*port = 443;
	}

	*host = strstr( url, "://" );

	if ( ( *host ) != NULL )
	{
		( *host ) += strlen( "://" );
	}
	else
	{
		( *host ) = url;
	}


	// Check if the user supplied a port
	char *portStrStart = strstr( *host, ":" );

	if ( portStrStart != NULL )
	{
		*port = ( unsigned int )strtol( portStrStart + 1, NULL, 0 );
	}

	char *pathStart = strstr( *host, "/" );

	// No path
	if ( pathStart == NULL )
	{
		*path = NULL;
		*pathLen = 0;
	}
	else
	{
		*hostLen = pathStart - ( *host );
		*pathLen = strlen( pathStart );
		*path = pathStart;
	}

	return;
}

/**
 * @brief Build an HTTP request string
 *
 * @param transaction
 * @param isHttps
 * @param port
 * @param host - The host string, NULL terminated. This MUST BE FREED by the caller.
 * @return char* - The entire request string. NULL terminated. This MUST BE FREED by the caller.
 */
static char *_ATMO_HTTP_BuildRequest( ATMO_DriverInstanceHandle_t instance, ATMO_HTTP_Transaction_t *transaction, ATMO_BOOL_t *isHttps, unsigned int *port, const char **host, unsigned int *hostLen )
{
	char *path = NULL;
	unsigned int pathLen = 0;

	_ATMO_HTTP_ParseUrl( transaction->url, isHttps, port, host, hostLen, &path, &pathLen );

	// Save the host in its own string
	char hostStr[*hostLen + 1];
	memset( hostStr, 0, ( *hostLen ) + 1 );
	strncpy( hostStr, *host, *hostLen );

	char pathStr[pathLen + 1];
	memset( pathStr, 0, pathLen + 1 );
	strncpy( pathStr, path, pathLen );

	// Could do this one of two ways:
	// Allocate a bigger buffer than we'll ever need OR
	// Precalculate the size beforehand and allocate a buffer of the perfect size
	// Allocating a nice big buffer is faster and simpler
	// Still need to make sure that the buffer doesn't get overrun
	unsigned int currentSize = 0;

	if ( transaction->method == ATMO_HTTP_POST )
	{
		currentSize += sprintf( ( char * )_ATMO_HTTP_PrivData[instance].pktBuf, ATMO_HTTP_POST_TEMPLATE, pathStr, hostStr, transaction->contentType == NULL ? "application/json" : transaction->contentType,
		                        transaction->dataLen );
	}
	else
	{
		currentSize += sprintf( ( char * )_ATMO_HTTP_PrivData[instance].pktBuf, ATMO_HTTP_GET_TEMPLATE, pathStr, hostStr );
	}

	// Add additional headers
	unsigned int i;

	for ( i = 0; i < transaction->headerOverlayLen; i++ )
	{
		int headerSize = snprintf( NULL, 0, "%s: %s\r\n", transaction->headerOverlay[i].headerKey, transaction->headerOverlay[i].headerValue );

		if ( currentSize + headerSize >= _ATMO_HTTP_PrivData[instance].pktBufSize )
		{
			return NULL;
		}

		currentSize += sprintf( ( char * )&_ATMO_HTTP_PrivData[instance].pktBuf[currentSize], "%s: %s\r\n", transaction->headerOverlay[i].headerKey, transaction->headerOverlay[i].headerValue );
	}

	// Add additonal \r\n before data (or end)
	if ( currentSize + 2 >= _ATMO_HTTP_PrivData[instance].pktBufSize )
	{
		return NULL;
	}

	currentSize += sprintf( ( char * )&_ATMO_HTTP_PrivData[instance].pktBuf[currentSize], "\r\n" );

	if ( transaction->method == ATMO_HTTP_POST )
	{
		int size = transaction->dataLen + 4; // Data length and \r\n\r\n

		if ( currentSize + size >= _ATMO_HTTP_PrivData[instance].pktBufSize )
		{
			return NULL;
		}

		sprintf( ( char * )&_ATMO_HTTP_PrivData[instance].pktBuf[currentSize], "%s\r\n\r\n", transaction->data );
	}

	return ( char * )_ATMO_HTTP_PrivData[instance].pktBuf;
}

static ATMO_BOOL_t _ATMO_HTTP_ParseResponse( ATMO_DriverInstanceHandle_t instance, uint8_t *dataPtr, unsigned int *respCode, unsigned int *respDataLen )
{
	_ATMO_HTTP_PrivData[instance].lastRespDataLen = 0;
	*respDataLen = 0;

	// Find HTTP version
	if ( strstr( ( char * )dataPtr, "HTTP/" ) != ( char * )dataPtr )
	{

		return false;
	}

	// Find the first space
	char *token = strchr( ( char * )dataPtr, ' ' );

	if ( token == NULL )
	{
		return false;
	}

	token++;

	*respCode = strtoul( token, NULL, 10 );

	if ( *respCode != 200 )
	{
		// It's OK, but there's no meaningful data to return
		return true;
	}

	char *dataStart = strstr( ( char * )dataPtr, "\r\n\r\n" );

	if ( dataStart == NULL )
	{
		return true;
	}

	dataStart += 4;
	*respDataLen = strlen( dataStart ) + 1;

	_ATMO_HTTP_PrivData[instance].lastRespData = ( uint8_t * )dataStart;
	_ATMO_HTTP_PrivData[instance].lastRespDataLen = *respDataLen;


	return true;
}

ATMO_HTTP_Status_t ATMO_HTTP_Perform( ATMO_DriverInstanceHandle_t instance, ATMO_HTTP_Transaction_t *transaction, unsigned int *respCode, unsigned int *respDataLen, unsigned int timeoutMs )
{
	unsigned int port = 80;
	ATMO_BOOL_t isHttps = false;
	const char *host = NULL;
	unsigned int hostLen = 0;
	_ATMO_HTTP_PrivData[instance].lastRespDataLen = 0;

	// Build request string
	char *reqString = _ATMO_HTTP_BuildRequest( instance, transaction, &isHttps, &port, &host, &hostLen );

	char hostStr[hostLen + 1];
	memset( hostStr, 0, hostLen + 1 );
	memcpy( hostStr, host, hostLen );

	// If the host string has a port in it
	char *portStart = strchr( hostStr, ':' );

	if ( portStart != NULL )
	{
		*portStart = 0;
	}

	// ATMO_PLATFORM_DebugPrint("HTTP Req Str: %s\r\n", reqString);

	ATMO_TCP_CLIENT_Status_t tcpStatus = ATMO_TCP_CLIENT_Connect( _ATMO_HTTP_PrivData[instance].tcpClientInstance, hostStr, port, isHttps );

	if ( tcpStatus != ATMO_TCP_CLIENT_Status_Success )
	{
		return ATMO_HTTP_Status_Fail;
	}

	// Send data
	// Wait for response
	_ATMO_HTTP_PrivData[instance].rxDataAvailable = false;
	tcpStatus = ATMO_TCP_CLIENT_WriteBytes( _ATMO_HTTP_PrivData[instance].tcpClientInstance, ( uint8_t * )reqString, strlen( reqString ) );

	if ( tcpStatus != ATMO_TCP_CLIENT_Status_Success )
	{
		ATMO_TCP_CLIENT_Disconnect( _ATMO_HTTP_PrivData[instance].tcpClientInstance );
		return ATMO_HTTP_Status_Fail;
	}

	uint64_t startTime = ATMO_PLATFORM_UptimeMs();
	ATMO_BOOL_t isTimeout = false;

	while ( ( ATMO_PLATFORM_UptimeMs() - startTime ) < timeoutMs )
	{
		if ( _ATMO_HTTP_PrivData[instance].rxDataAvailable )
		{
			uint32_t numBytes = 0;
			uint8_t *dataPtr = NULL;
			ATMO_TCP_CLIENT_GetNumAvailableBytes( _ATMO_HTTP_PrivData[instance].tcpClientInstance, &numBytes, &dataPtr );
			dataPtr[numBytes] = 0;
			_ATMO_HTTP_ParseResponse( instance, dataPtr, respCode, respDataLen );
			break;
		}

		ATMO_PLATFORM_DelayMilliseconds( 1 );
		ATMO_Tick();
	}

	if ( ( ATMO_PLATFORM_UptimeMs() - startTime ) > timeoutMs )
	{
		isTimeout = true;
	}

	// No matter what, disconnect here
	ATMO_TCP_CLIENT_Disconnect( _ATMO_HTTP_PrivData[instance].tcpClientInstance );

	if ( isTimeout )
	{
		ATMO_PLATFORM_DebugPrint( "HTTP timeout %d\r\n", timeoutMs );
		return ATMO_HTTP_Status_Fail;
	}

	return ATMO_HTTP_Status_Success;
}

ATMO_HTTP_Status_t ATMO_HTTP_GetRespData( ATMO_DriverInstanceHandle_t instance, uint8_t *buf, unsigned int bufLen )
{
	if ( _ATMO_HTTP_PrivData[instance].lastRespDataLen > bufLen || _ATMO_HTTP_PrivData[instance].lastRespDataLen == 0 )
	{
		return ATMO_HTTP_Status_Fail;
	}

	memset( buf, 0, bufLen );
	memcpy( buf, _ATMO_HTTP_PrivData[instance].lastRespData, _ATMO_HTTP_PrivData[instance].lastRespDataLen );
	_ATMO_HTTP_PrivData[instance].lastRespDataLen = 0;
	return ATMO_HTTP_Status_Success;
}


