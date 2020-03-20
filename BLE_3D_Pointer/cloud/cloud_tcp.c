/**
 ******************************************************************************
 * @file    cloud_tcp.c
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - CLOUD over TCP Driver
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

#include "cloud_tcp.h"
#include "cloud_config.h"
#include "../app_src/atmosphere_platform.h"
#include "../app_src/atmosphere_globals.h"
#include "../datetime/datetime.h"
#include "../wifi/wifi.h"
#include "cloud_provisioner.h"
#include "../base64/atmo_base64.h"
#include "../atmo/core.h"
#include "../tcpserver/tcpserver.h"
#include "../http/http.h"
#include "../interval/interval.h"
#include "../ringbuffer/atmosphere_ringbuffer.h"
#include "../app_src/atmosphere_cloudcommands.h"
#include "../cellular/cellular.h"
#include "../app_src/atmo_config.h"

#define ATMO_MAX_NUM_CLOUD_TCP_INSTANCES (2)

static const ATMO_CLOUD_DriverInstance_t cloudTcpDriverInstance =
{
	ATMO_CLOUD_TCP_Init,
	ATMO_CLOUD_TCP_SendEvent,
	ATMO_CLOUD_TCP_PopCommand
};

typedef struct
{
	ATMO_DriverInstanceData_t instance;
	ATMO_BOOL_t inTransaction;
	ATMO_DriverInstanceHandle_t networkInstance;
	ATMO_DriverInstanceHandle_t httpInstance;
	ATMO_DriverInstanceHandle_t tcpServerInstance;
	ATMO_DriverInstanceHandle_t intervalInstance;
	ATMO_DriverType_t networkType;
	ATMO_RingBuffer_t eventQueue;
	uint32_t instanceNum;
} ATMO_CLOUD_TCP_Priv_t;

static ATMO_CLOUD_TCP_Priv_t _ATMO_CLOUD_TCP_PrivData[ATMO_MAX_NUM_CLOUD_TCP_INSTANCES];
static uint32_t _ATMO_CLOUD_TCP_CurrentNumInstances = 0;
static ATMO_BOOL_t _ATMO_CLOUD_TCP_IntervalCreated = false;
static ATMO_BOOL_t _ATMO_CLOUD_TCP_ConnectionVerified = false;
static ATMO_BOOL_t _ATMO_CLOUD_TCP_ProvisionComplete = false;
static ATMO_BOOL_t _ATMO_CLOUD_TCP_Connected = false;

static ATMO_DriverInstanceHandle_t _ATMO_CLOUD_TCP_TcpServerInstance = 0; // There should only be one TCP server running on a system at a time

char rfc3986[256] = {0};

static ATMO_CLOUD_Status_t _ATMO_CLOUD_TCP_SendEvent( ATMO_DriverInstanceData_t *instance, const char *eventName, ATMO_Value_t *data, uint32_t timeout );
static ATMO_CLOUD_Status_t _ATMO_CLOUD_TCP_PopCommand( ATMO_DriverInstanceData_t *instance, const char *commandName, ATMO_Value_t *data, uint32_t timeout );

typedef enum
{
	ATMO_CLOUD_TCP_PROVISIONING,
	ATMO_CLOUD_TCP_PROVISIONED,
	ATMO_CLOUD_TCP_NUM_PROVISION_STATUS,
} ATMO_CLOUD_TCP_ProvisionStatus_t;

#define ATMO_CLOUD_MAX_NAME_LEN (32)

typedef struct
{
	ATMO_BOOL_t isEvent;
	char name[ATMO_CLOUD_MAX_NAME_LEN];
	ATMO_Value_t value;
} ATMO_CLOUD_Entry_t;

#ifndef ATMO_CLOUD_TCP_QUEUE_SIZE
#define ATMO_CLOUD_TCP_QUEUE_SIZE (10)
#endif

#ifndef ATMO_CLOUD_COMMAND_CHECK_INTERVAL_MS
#define ATMO_CLOUD_COMMAND_CHECK_INTERVAL_MS (5000)
#endif

static uint8_t _ATMO_CLOUD_TCP_QueueBuf[ATMO_CLOUD_TCP_QUEUE_SIZE * sizeof( ATMO_CLOUD_Entry_t )];

static ATMO_RingBuffer_t _ATMO_CLOUD_TCP_CmdEvtQueue;

static void _ATMO_CLOUD_TCP_FreeCloudEntry( void *entry )
{
	ATMO_CLOUD_Entry_t *cloudEntry = ( ATMO_CLOUD_Entry_t * )entry;
	ATMO_FreeValue( &cloudEntry->value );
}

ATMO_Status_t ATMO_CLOUD_TCP_AddDriverInstance( ATMO_DriverInstanceHandle_t *instanceNumber, ATMO_DriverInstanceHandle_t wifiInstanceNumber, ATMO_DriverInstanceHandle_t httpClientInstanceNumber, ATMO_DriverInstanceHandle_t tcpServerInstanceNumber, ATMO_DriverInstanceHandle_t intervalInstanceNumber )
{
	return ATMO_CLOUD_TCP_AddDriverInstanceByType( instanceNumber, wifiInstanceNumber, ATMO_DRIVERTYPE_WIFI, httpClientInstanceNumber, tcpServerInstanceNumber, intervalInstanceNumber );
}

ATMO_Status_t ATMO_CLOUD_TCP_AddDriverInstanceByType( ATMO_DriverInstanceHandle_t *instanceNumber, ATMO_DriverInstanceHandle_t networkInstanceNumber, ATMO_DriverType_t networkType, ATMO_DriverInstanceHandle_t httpClientInstanceNumber, ATMO_DriverInstanceHandle_t tcpServerInstanceNumber, ATMO_DriverInstanceHandle_t intervalInstanceNumber )
{
	if ( _ATMO_CLOUD_TCP_CurrentNumInstances >= ATMO_MAX_NUM_CLOUD_TCP_INSTANCES )
	{
		return ATMO_Status_Fail;
	}

	ATMO_CLOUD_TCP_Priv_t *privData = &_ATMO_CLOUD_TCP_PrivData[_ATMO_CLOUD_TCP_CurrentNumInstances];

	memset( privData, 0, sizeof( ATMO_CLOUD_TCP_Priv_t ) );
	privData->networkInstance = networkInstanceNumber;
	privData->networkType = networkType;
	privData->httpInstance = httpClientInstanceNumber;
	privData->tcpServerInstance = tcpServerInstanceNumber;
	privData->intervalInstance = intervalInstanceNumber;
	privData->instanceNum = _ATMO_CLOUD_TCP_CurrentNumInstances;

	privData->instance.name = "Cloud TCP";
	privData->instance.initialized = false;
	privData->instance.instanceNumber = 0;
	privData->instance.argument = ( void * )&privData->instanceNum;

	_ATMO_CLOUD_TCP_CurrentNumInstances++;

	ATMO_Status_t status = ATMO_CLOUD_AddDriverInstance( &cloudTcpDriverInstance, &privData->instance, instanceNumber );
	return status;
}

static void __ATMO_CLOUD_HTTP_UrlEncodeTableInit()
{
	int i;

	for ( i = 0; i < 256; i++ )
	{
		rfc3986[i] = isalnum( i ) || i == '~' || i == '-' || i == '.' || i == '_'
		             ? i
		             : 0;
	}
}

static int __ATMO_CLOUD_HTTP_UrlEncode( char *s, char *enc )
{
	unsigned int len = strlen( s );
	unsigned int currEncPos = 0;
	unsigned int i;

	for ( i = 0; i < len; i++ )
	{
		if ( rfc3986[( int )s[i]] )
		{
			// Just use the regular char
			if ( enc != NULL )
			{
				sprintf( &enc[currEncPos], "%c", s[i] );
			}

			currEncPos++;
		}
		else
		{
			if ( enc != NULL )
			{
				sprintf( &enc[currEncPos], "%%%02X", s[i] );
			}

			currEncPos += 3;
		}
	}

	return currEncPos + 1;
}

static ATMO_CLOUD_Status_t _ATMO_CLOUD_TCP_BuildSSIDFromUUID( uint8_t *mac, ATMO_UUID_t *uuid, char *ssid )
{
	uint8_t rawData[22] = {0};
	memcpy( rawData, mac, 6 );
	memcpy( &rawData[6], uuid->data, 16 );

#ifndef ATMO_SLIM_STACK
	unsigned int i;

	for ( i = 0; i < 22; i++ )
	{
		ATMO_PLATFORM_DebugPrint( "SSID[%02d]: %02X\r\n", i, rawData[i] );
	}

#endif

	// encode data
	unsigned int outLen = ATMO_BASE64_GetEncodeOutputSize( rawData, 22 );
	char out[outLen];
	ATMO_BASE64_Encode( rawData, 22, out, outLen );
	memcpy( ssid, out, 30 );

	return ATMO_CLOUD_Status_Success;
}

static void _ATMO_CLOUD_TCP_DataAvailable( void *data )
{
	// Get data
	unsigned int connectionId = 0;
	ATMO_GetUnsignedInt( ( ATMO_Value_t * )data, &connectionId );

	uint32_t numBytes = 0;
	ATMO_TCP_SERVER_GetNumAvailableBytes( _ATMO_CLOUD_TCP_TcpServerInstance, connectionId, &numBytes );

	uint8_t cmdBuffer[numBytes];
	ATMO_TCP_SERVER_ReadBytes( _ATMO_CLOUD_TCP_TcpServerInstance, connectionId, cmdBuffer, numBytes );

	uint8_t response[22] = {0};
	int respLen = ATMO_CLOUD_PROVISIONER_HandleCommand( cmdBuffer, numBytes, response, sizeof( response ), &_ATMO_CLOUD_TCP_ConnectionVerified );

	if ( respLen < 0 )
	{
		ATMO_TCP_SERVER_WriteBytes( _ATMO_CLOUD_TCP_TcpServerInstance, connectionId, response, 0 );
		return;
	}

	// TODO nick have the wifi provisioning send some sort of command indicating that provisioning is done
	if ( response[0] == ATMO_CLOUD_PROVISIONER_CommandType_ExtraSettingsWifi && response[1] == ATMO_CLOUD_PROVISIONER_SubCommand_WifiExtraSettings_SetPassword )
	{
		_ATMO_CLOUD_TCP_ProvisionComplete = true;
	}
	else
	{
		_ATMO_CLOUD_TCP_ProvisionComplete = false;
	}

	ATMO_TCP_SERVER_WriteBytes( _ATMO_CLOUD_TCP_TcpServerInstance, connectionId, response, respLen );
}

static void _ATMO_CLOUD_TCP_DataSent( void *data )
{
	if ( _ATMO_CLOUD_TCP_ProvisionComplete )
	{
		ATMO_PLATFORM_DebugPrint( "[CLOUD TCP] Starting WiFi\r\n" );
		ATMO_TCP_SERVER_Stop( _ATMO_CLOUD_TCP_TcpServerInstance );
	}
}

static void _ATMO_CLOUD_TCP_WifiConnected( void *data )
{
	_ATMO_CLOUD_TCP_Connected = true;
}

static void _ATMO_CLOUD_TCP_WifiDisconnected( void *data )
{
	_ATMO_CLOUD_TCP_Connected = false;
}

static void _ATMO_CLOUD_TCP_CheckBuffer( void *data )
{
	static ATMO_BOOL_t currentlyChecking = false;
	static uint64_t lastCloudCommandCheck = 0;

	if ( currentlyChecking )
	{
		return;
	}

	currentlyChecking = true;

	unsigned int i;

	for ( i = 0; i < _ATMO_CLOUD_TCP_CurrentNumInstances; i++ )
	{
		if ( _ATMO_CLOUD_TCP_PrivData[i].inTransaction )
		{
			currentlyChecking = false;
			return;
		}

		if ( !ATMO_RingBuffer_Empty( &_ATMO_CLOUD_TCP_PrivData[i].eventQueue ) )
		{
			ATMO_CLOUD_Entry_t *pEntry = ATMO_RingBuffer_Pop( &_ATMO_CLOUD_TCP_PrivData[i].eventQueue );
			ATMO_CLOUD_Entry_t entry;
			memcpy(&entry, pEntry, sizeof(entry));

			if ( entry.isEvent )
			{
//				uint32_t timeSec = ATMO_PLATFORM_UptimeMs() / 1000;
//				ATMO_PLATFORM_DebugPrint("%lu: Sending event %s\r\n", timeSec, (char *)entry->name);
				_ATMO_CLOUD_TCP_SendEvent( &_ATMO_CLOUD_TCP_PrivData[i].instance, entry.name, &entry.value, 10000 );
				ATMO_FreeValue( &entry.value );
			}
		}

		// Check cloud commands
		if ( ATMO_PLATFORM_UptimeMs() - lastCloudCommandCheck >= ATMO_CLOUD_COMMAND_CHECK_INTERVAL_MS)
		{
			lastCloudCommandCheck = ATMO_PLATFORM_UptimeMs();

			unsigned int j;

			for ( j = 0; j < TOTAL_NUM_CLOUD_COMMANDS; j++ )
			{
				ATMO_Value_t value;
				ATMO_InitValue( &value );

				if ( _ATMO_CLOUD_TCP_PopCommand( &_ATMO_CLOUD_TCP_PrivData[i].instance, cloudCommandsList[j], &value, 10000 ) == ATMO_CLOUD_Status_Success )
				{
					ATMO_CLOUD_DispatchCommandResult( cloudCommandsList[j], &value );
				}

				ATMO_FreeValue( &value );
			}
		}
	}

	currentlyChecking = false;
}

static uint32_t _ATMO_CLOUD_TCP_GetInstanceNum( ATMO_DriverInstanceData_t *instance )
{
	uint32_t *instanceNum = ( uint32_t * )instance->argument;
	return *instanceNum;
}

ATMO_CLOUD_Status_t ATMO_CLOUD_TCP_Init( ATMO_DriverInstanceData_t *instance, ATMO_BOOL_t provision )
{
	uint32_t instanceNum = _ATMO_CLOUD_TCP_GetInstanceNum( instance );

	if ( instanceNum >= _ATMO_CLOUD_TCP_CurrentNumInstances )
	{
		return ATMO_CLOUD_Status_Fail;
	}

	// Each instance get its own ring buffer to handle commands and events
	ATMO_RingBuffer_InitWithBuf( &_ATMO_CLOUD_TCP_PrivData[instanceNum].eventQueue, _ATMO_CLOUD_TCP_QueueBuf, ATMO_CLOUD_TCP_QUEUE_SIZE, sizeof( ATMO_CLOUD_Entry_t ), _ATMO_CLOUD_TCP_FreeCloudEntry );

	// All instances use the same interval to check
	if ( !_ATMO_CLOUD_TCP_IntervalCreated )
	{
		ATMO_AddTickCallback( _ATMO_CLOUD_TCP_CheckBuffer );
		__ATMO_CLOUD_HTTP_UrlEncodeTableInit();
		_ATMO_CLOUD_TCP_IntervalCreated = true;
	}

	// WIFI driver will start a TCP Server if it needs to
	if ( _ATMO_CLOUD_TCP_PrivData[instanceNum].networkType == ATMO_DRIVERTYPE_WIFI )
	{
		_ATMO_CLOUD_TCP_TcpServerInstance = _ATMO_CLOUD_TCP_PrivData[instanceNum].tcpServerInstance;

		ATMO_WIFI_ConnectionStatus_t wifiStatus;

		if ( ATMO_WIFI_GetStatus( _ATMO_CLOUD_TCP_PrivData[instanceNum].networkInstance, &wifiStatus ) != ATMO_WIFI_Status_Success )
		{
			return ATMO_CLOUD_Status_Fail;
		}

		if ( ATMO_CLOUD_GetRegistration()->registered || !provision )
		{
			ATMO_WIFI_RegisterCallback( _ATMO_CLOUD_TCP_PrivData[instanceNum].networkInstance, ATMO_WIFI_CONNECTED, _ATMO_CLOUD_TCP_WifiConnected );
			ATMO_WIFI_RegisterCallback( _ATMO_CLOUD_TCP_PrivData[instanceNum].networkInstance, ATMO_WIFI_DISCONNECTED, _ATMO_CLOUD_TCP_WifiDisconnected );
			return ATMO_CLOUD_Status_Success;
		}

		ATMO_UUID_t projUuidBin;
		ATMO_StringToUuid( ATMO_GLOBALS_PROJECTUUID, &projUuidBin, ATMO_ENDIAN_Type_Big );

		char ssid[32] = {0};
		_ATMO_CLOUD_TCP_BuildSSIDFromUUID( wifiStatus.macAddress, &projUuidBin, ssid );

#ifndef ATMO_SLIM_STACK
		ATMO_PLATFORM_DebugPrint( "SSID: %s\r\n", ssid );
#endif

		// Start advertising AP
		ATMO_WIFI_Config_t wifiConf;
		wifiConf.mode = ATMO_WIFI_MODE_AP;
		wifiConf.ssid = ssid;
		wifiConf.password = NULL;
		ATMO_WIFI_SetConfiguration( _ATMO_CLOUD_TCP_PrivData[instanceNum].networkInstance, &wifiConf );
		ATMO_WIFI_Start( _ATMO_CLOUD_TCP_PrivData[instanceNum].networkInstance );
		ATMO_WIFI_RegisterCallback( _ATMO_CLOUD_TCP_PrivData[instanceNum].networkInstance, ATMO_WIFI_CONNECTED, _ATMO_CLOUD_TCP_WifiConnected );
		ATMO_WIFI_RegisterCallback( _ATMO_CLOUD_TCP_PrivData[instanceNum].networkInstance, ATMO_WIFI_DISCONNECTED, _ATMO_CLOUD_TCP_WifiDisconnected );

#ifndef ATMO_SLIM_STACK
		ATMO_PLATFORM_DebugPrint( "WiFi Started\r\n" );
#endif

		// Start a TCP Server
		ATMO_TCP_SERVER_Config_t serverConfig;
		serverConfig.port = 80;
		ATMO_TCP_SERVER_SetConfiguration( _ATMO_CLOUD_TCP_PrivData[instanceNum].tcpServerInstance, &serverConfig );
		ATMO_TCP_SERVER_RegisterEventCallback( _ATMO_CLOUD_TCP_PrivData[instanceNum].tcpServerInstance, ATMO_TCP_SERVER_DataAvailable, _ATMO_CLOUD_TCP_DataAvailable );
		ATMO_TCP_SERVER_RegisterEventCallback( _ATMO_CLOUD_TCP_PrivData[instanceNum].tcpServerInstance, ATMO_TCP_SERVER_DataSent, _ATMO_CLOUD_TCP_DataSent );
		ATMO_TCP_SERVER_Start( _ATMO_CLOUD_TCP_PrivData[instanceNum].tcpServerInstance );

#ifndef ATMO_SLIM_STACK
		ATMO_PLATFORM_DebugPrint( "TCP Server Started\r\n" );
#endif
	}

	return ATMO_CLOUD_Status_Success;
}

static ATMO_BOOL_t _ATMO_CLOUD_TCP_TransportAvailable( ATMO_DriverInstanceData_t *instance )
{
	uint32_t instanceNum = _ATMO_CLOUD_TCP_GetInstanceNum( instance );

	if ( instanceNum >= _ATMO_CLOUD_TCP_CurrentNumInstances )
	{
		return false;
	}

	if(_ATMO_CLOUD_TCP_PrivData[instanceNum].networkType == ATMO_DRIVERTYPE_ETHERNET)
	{
		return true;
	}
	else if ( _ATMO_CLOUD_TCP_PrivData[instanceNum].networkType == ATMO_DRIVERTYPE_WIFI )
	{
		ATMO_WIFI_ConnectionStatus_t wifiStatus;

		if ( ATMO_WIFI_GetStatus( _ATMO_CLOUD_TCP_PrivData[instanceNum].networkInstance, &wifiStatus ) != ATMO_WIFI_Status_Success )
		{
			return false;
		}

		if ( !ATMO_CLOUD_GetRegistration()->registered || ( wifiStatus.connectivity != ATMO_WIFI_CONNECTIVITY_CONNECTED ) )
		{
			return false;
		}
	}
	else if ( _ATMO_CLOUD_TCP_PrivData[instanceNum].networkType == ATMO_DRIVERTYPE_CELLULAR )
	{
		ATMO_CELLULAR_ConnectionStatus_t cellStatus = ATMO_CELLULAR_ConnectionStatus_Disconnected;

		if ( ATMO_CELLULAR_GetConnectionStatus( _ATMO_CLOUD_TCP_PrivData[instanceNum].networkInstance, &cellStatus ) != ATMO_CELLULAR_Status_Success )
		{
			return false;
		}

		if ( ( !ATMO_CLOUD_GetRegistration()->registered ) || ( cellStatus != ATMO_CELLULAR_ConnectionStatus_Connected ) )
		{
			return false;
		}

	}

	return true;
}

ATMO_CLOUD_Status_t ATMO_CLOUD_TCP_SendEvent( ATMO_DriverInstanceData_t *instance, const char *eventName, ATMO_Value_t *data, uint32_t timeout )
{
	uint32_t instanceNum = _ATMO_CLOUD_TCP_GetInstanceNum( instance );

	// Don't send events if there's no connection
	if(!_ATMO_CLOUD_TCP_TransportAvailable(instance))
	{
		return ATMO_CLOUD_Status_Fail;
	}

	if ( ATMO_RingBuffer_Full( &_ATMO_CLOUD_TCP_PrivData[instanceNum].eventQueue ) )
	{
		return ATMO_CLOUD_Status_Fail;
	}

	ATMO_CLOUD_Entry_t entry;
	entry.isEvent = true;
	strncpy( entry.name, eventName, ATMO_CLOUD_MAX_NAME_LEN );

	ATMO_InitValue( &entry.value );
	ATMO_CreateValueCopy( &entry.value, data );
	ATMO_RingBuffer_Push( &_ATMO_CLOUD_TCP_PrivData[instanceNum].eventQueue, &entry );
	return ATMO_CLOUD_Status_Success;
}

ATMO_CLOUD_Status_t _ATMO_CLOUD_TCP_SendEvent( ATMO_DriverInstanceData_t *instance, const char *eventName, ATMO_Value_t *data, uint32_t timeout )
{
	if ( !_ATMO_CLOUD_TCP_TransportAvailable( instance ) )
	{
		return ATMO_CLOUD_Status_Fail;
	}

	uint32_t instanceNum = _ATMO_CLOUD_TCP_GetInstanceNum( instance );

	if ( _ATMO_CLOUD_TCP_PrivData[instanceNum].inTransaction )
	{
		return ATMO_CLOUD_Status_Fail;
	}

	_ATMO_CLOUD_TCP_PrivData[instanceNum].inTransaction = true;

	// Get data as a string
	ATMO_Value_t dataStr;
	ATMO_InitValue( &dataStr );
	ATMO_CreateValueConverted( &dataStr, ATMO_DATATYPE_STRING, data );
	unsigned int encodedDataLen = 0;

	if ( strlen( ( char * )dataStr.data ) == 0 )
	{
		encodedDataLen = strlen( "null" ) + 1;
	}
	else
	{
		encodedDataLen = strlen( ( char * )dataStr.data ) * 3;
	}

	char dataEncoded[encodedDataLen + 1];
	memset( dataEncoded, 0, encodedDataLen + 1 );

	if ( strlen( ( char * )dataStr.data ) == 0 )
	{
		strcpy( dataEncoded, "null" );
	}
	else
	{
		__ATMO_CLOUD_HTTP_UrlEncode( ( char * )dataStr.data, dataEncoded );
	}

	ATMO_FreeValue( &dataStr );

	// Get UUID str
	char uuid[40];
	ATMO_CLOUD_GetRegistrationInfoUuid( ATMO_CLOUD_GetRegistration(), uuid, sizeof( uuid ) );

	// Print everything binary
	const char *urlFormat = "%s/thing/%s/event/%s/%s";
	unsigned int urlSize = strlen( ATMO_CLOUD_GetRegistration()->url ) + strlen( uuid ) + strlen( eventName ) + strlen( dataEncoded ) + 16;

	char url[urlSize + 1];
	memset( url, 0, urlSize + 1 );
	sprintf( url, urlFormat, ATMO_CLOUD_GetRegistration()->url, uuid, eventName,
	         dataEncoded );

	ATMO_HTTP_Header_t header;
	header.headerKey = "cloud";
	header.headerValue = ATMO_CLOUD_GetRegistration()->token;

	ATMO_HTTP_Transaction_t trans;
	trans.url = url;
	trans.method = ATMO_HTTP_GET;
	trans.data = NULL;
	trans.dataLen = 0;
	trans.headerOverlay = &header;
	trans.headerOverlayLen = 1;

	unsigned int respCode = 0;
	unsigned int respLen = 0;

	if ( ATMO_HTTP_Perform( _ATMO_CLOUD_TCP_PrivData[instanceNum].httpInstance, &trans, &respCode, &respLen, 5000 ) != ATMO_HTTP_Status_Success )
	{
		_ATMO_CLOUD_TCP_PrivData[instanceNum].inTransaction = false;
		return ATMO_CLOUD_Status_Fail;
	}

	if ( respCode != 200 )
	{
		_ATMO_CLOUD_TCP_PrivData[instanceNum].inTransaction = false;
		return ATMO_CLOUD_Status_Fail;
	}

	_ATMO_CLOUD_TCP_PrivData[instanceNum].inTransaction = false;
	return ATMO_CLOUD_Status_Success;
}

ATMO_CLOUD_Status_t ATMO_CLOUD_TCP_PopCommand( ATMO_DriverInstanceData_t *instance, const char *commandName, ATMO_Value_t *data, uint32_t timeout )
{
	if ( !_ATMO_CLOUD_TCP_TransportAvailable( instance ) )
	{
		return ATMO_CLOUD_Status_Fail;
	}

	uint32_t instanceNum = _ATMO_CLOUD_TCP_GetInstanceNum( instance );

	if ( ATMO_RingBuffer_Full( &_ATMO_CLOUD_TCP_PrivData[instanceNum].eventQueue ) )
	{
		return ATMO_CLOUD_Status_Fail;
	}

	// Is this command already in the buffer?
	// For a slow connection (i.e. cellular) it might take a few seconds to send requests
	// If we don't do this, the cloud command requests will start piling up, and cloud events will get starved out
	uint8_t i;

	for ( i = 0; i < _ATMO_CLOUD_TCP_PrivData[instanceNum].eventQueue.count; i++ )
	{
		ATMO_CLOUD_Entry_t *pEntry = ( ATMO_CLOUD_Entry_t * )ATMO_RingBuffer_Index( &_ATMO_CLOUD_TCP_PrivData[instanceNum].eventQueue, i );

		if ( pEntry->isEvent == false && strcmp( pEntry->name, commandName ) == 0 )
		{
			return ATMO_CLOUD_Status_Fail;
		}
	}

	ATMO_CLOUD_Entry_t entry;
	entry.isEvent = false;
	strncpy( entry.name, commandName, ATMO_CLOUD_MAX_NAME_LEN );
	ATMO_InitValue( &entry.value );
	ATMO_RingBuffer_Push( &_ATMO_CLOUD_TCP_PrivData[instanceNum].eventQueue, &entry );
	return ATMO_CLOUD_Status_Success;
}

ATMO_CLOUD_Status_t _ATMO_CLOUD_TCP_PopCommand( ATMO_DriverInstanceData_t *instance, const char *commandName, ATMO_Value_t *data, uint32_t timeout )
{
	if ( !_ATMO_CLOUD_TCP_TransportAvailable( instance ) )
	{
		return ATMO_CLOUD_Status_Fail;
	}

	uint32_t instanceNum = _ATMO_CLOUD_TCP_GetInstanceNum( instance );

	if ( _ATMO_CLOUD_TCP_PrivData[instanceNum].inTransaction )
	{
		return ATMO_CLOUD_Status_Fail;
	}

	_ATMO_CLOUD_TCP_PrivData[instanceNum].inTransaction = true;

	// Get UUID str
	char uuid[40];
	ATMO_CLOUD_GetRegistrationInfoUuid( ATMO_CLOUD_GetRegistration(), uuid, sizeof( uuid ) );

	const char *urlFormat = "%s/thing/%s/command/%s/pop";
	unsigned int urlSize = strlen( ATMO_CLOUD_GetRegistration()->url ) + strlen( uuid ) + strlen( commandName ) + 22;
	char url[urlSize + 1];
	memset( url, 0, urlSize + 1 );
	sprintf( url, urlFormat, ATMO_CLOUD_GetRegistration()->url, uuid, commandName );

// #ifdef ATMO_HTTP_DEBUG
// 	ATMO_PLATFORM_DebugPrint("Sending to url: %s\r\n", url);
// #endif

	ATMO_HTTP_Header_t header;
	header.headerKey = "cloud";
	header.headerValue = ATMO_CLOUD_GetRegistration()->token;

	ATMO_HTTP_Transaction_t trans;
	trans.url = url;
	trans.method = ATMO_HTTP_GET;
	trans.data = NULL;
	trans.dataLen = 0;
	trans.headerOverlay = &header;
	trans.headerOverlayLen = 1;

	unsigned int respCode = 0;
	unsigned int respLen = 0;

	if ( ATMO_HTTP_Perform( _ATMO_CLOUD_TCP_PrivData[instanceNum].httpInstance, &trans, &respCode, &respLen, 5000 ) != ATMO_HTTP_Status_Success )
	{
		_ATMO_CLOUD_TCP_PrivData[instanceNum].inTransaction = false;
		return ATMO_CLOUD_Status_Fail;
	}

	if ( respCode != 200 || respLen == 0 )
	{
		_ATMO_CLOUD_TCP_PrivData[instanceNum].inTransaction = false;
		return ATMO_CLOUD_Status_Fail;
	}

	uint8_t respData[respLen + 1];
	memset( respData, 0, respLen + 1 );

	if ( ATMO_HTTP_GetRespData( _ATMO_CLOUD_TCP_PrivData[instanceNum].httpInstance, respData, respLen ) != ATMO_HTTP_Status_Success )
	{
		_ATMO_CLOUD_TCP_PrivData[instanceNum].inTransaction = false;
		return ATMO_CLOUD_Status_Fail;
	}

	respData[respLen] = 0;

// #ifdef ATMO_HTTP_DEBUG
// 	ATMO_PLATFORM_DebugPrint("Resp %s\r\n\r\n", (char *)respData);
// #endif

	ATMO_InitValue( data );

	// Simple JSON parsing (should incorporate robust json lib at some point)
	char *start = strstr( ( char * )respData, "command\":" );

	if ( start )
	{
		start += strlen( "command\":" );

		// Trim if string
		if ( *start == '\"' )
		{
			start++;
		}

		char *end = ( char * )&respData[strlen( ( char * )respData ) - 1];

		if ( *end == '}' )
		{
			end--;

			if ( *end == '\"' )
			{
				end--;
			}

			unsigned int len = ( unsigned int )( end - start + 1 );
			char cmdDataStr[len + 1];
			memset( cmdDataStr, 0, len + 1 );
			cmdDataStr[len] = 0;
			memcpy( cmdDataStr, start, len );
			ATMO_CreateValueString( data, cmdDataStr );
// #ifdef ATMO_HTTP_DEBUG
// 			ATMO_PLATFORM_DebugPrint("Final Data: %s\r\n", data->data);
// #endif
		}
		else
		{
			_ATMO_CLOUD_TCP_PrivData[instanceNum].inTransaction = false;
			return ATMO_CLOUD_Status_Fail;
		}
	}
	else
	{
		if ( strcmp( ( char * )respData, "null" ) == 0 )
		{
			_ATMO_CLOUD_TCP_PrivData[instanceNum].inTransaction = false;
			return ATMO_CLOUD_Status_NoData;
		}
		else
		{
			_ATMO_CLOUD_TCP_PrivData[instanceNum].inTransaction = false;
			return ATMO_CLOUD_Status_Fail;
		}
	}

	_ATMO_CLOUD_TCP_PrivData[instanceNum].inTransaction = false;
	return ATMO_CLOUD_Status_Success;
}
