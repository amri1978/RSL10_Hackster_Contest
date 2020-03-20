/**
 ******************************************************************************
 * @file    cloud_uart.c
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - CLOUD over UART Driver
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

#include "cloud_uart.h"
#include "cloud_config.h"
#include "../app_src/atmosphere_platform.h"
#include "../app_src/atmosphere_globals.h"
#include "../uart/uart.h"
#include "cloud_provisioner.h"
#include "../atmo/core.h"
#include "../wifi/wifi.h"

static const ATMO_CLOUD_DriverInstance_t cloudUartDriverInstance =
{
	ATMO_CLOUD_UART_Init,
	ATMO_CLOUD_UART_SendEvent,
	ATMO_CLOUD_UART_PopCommand
};

static ATMO_DriverInstanceData_t _ATMO_CLOUD_UART_DriverInstanceData;
static ATMO_DriverInstanceHandle_t _ATMO_CLOUD_UART_UartInstance;

#define ATMO_CLOUD_UART_SYNCBYTE 0xAB
#define ATMO_CLOUD_UART_TX_RX_BUFLEN (64)

static uint8_t _ATMO_CLOUD_UART_Buf[ATMO_CLOUD_UART_TX_RX_BUFLEN] = {0};

ATMO_Status_t ATMO_CLOUD_UART_AddDriverInstance( ATMO_DriverInstanceHandle_t *instanceNumber, ATMO_DriverInstanceHandle_t uartInstanceNumber )
{
	_ATMO_CLOUD_UART_UartInstance = uartInstanceNumber;

	_ATMO_CLOUD_UART_DriverInstanceData.name = "Cloud UART";
	_ATMO_CLOUD_UART_DriverInstanceData.initialized = false;
	_ATMO_CLOUD_UART_DriverInstanceData.instanceNumber = 0;
	_ATMO_CLOUD_UART_DriverInstanceData.argument = NULL;

	return ATMO_CLOUD_AddDriverInstance( &cloudUartDriverInstance, &_ATMO_CLOUD_UART_DriverInstanceData, instanceNumber );
}

void _ATMO_CLOUD_UART_SkipWhiteSpace( char **data )
{
	while ( *( *data ) == ' ' )
	{
		( *data )++;
	}
}

void _ATMO_CLOUD_UART_AtSendOk()
{
	ATMO_UART_WriteStringBlocking( _ATMO_CLOUD_UART_UartInstance, "\r\nOK\r\n" );
}

void _ATMO_CLOUD_UART_AtSendErr()
{
	ATMO_UART_WriteStringBlocking( _ATMO_CLOUD_UART_UartInstance, "\r\nERROR\r\n" );
}

char *_ATMO_CLOUD_UART_AtGetNextParameter( char *currentCmdPos )
{
	// If this is the first call, find =
	char *delPos = strchr( currentCmdPos, '=' );

	if ( delPos != NULL )
	{
		return delPos + 1;
	}

	// Find next comma
	delPos = strchr( currentCmdPos, ',' );

	if ( delPos != NULL )
	{
		return delPos + 1;
	}

	return NULL;
}

void _ATMO_CLOUD_UART_ProcessAt()
{
	_ATMO_CLOUD_UART_AtSendOk();
}

void _ATMO_CLOUD_UART_ProcessAtManufacturerInformation()
{
	ATMO_UART_WriteStringBlocking( _ATMO_CLOUD_UART_UartInstance, "\r\nAtmosphereIoT\r\nOK\r\n" );
}

void _ATMO_CLOUD_UART_ProcessAtModelInformation()
{
	// Project UUID
	if ( !ATMO_CLOUD_GetRegistration()->registered )
	{
		ATMO_UART_WriteStringBlocking( _ATMO_CLOUD_UART_UartInstance, "\r\n" ATMO_GLOBALS_PROJECTUUID "\r\nOK\r\n" );
	}
	else
	{
		ATMO_UUID_t uuidBin;
		memcpy( &uuidBin.data, ATMO_CLOUD_GetRegistration()->uuid, 16 );
		uuidBin.type = ATMO_UUID_Type_128_Bit;
		uuidBin.endianness = ATMO_UUID_ENDIAN_DEFAULT;

		uint8_t uuidStrBuf[ATMO_UUID_STR_LEN] = {0};
		ATMO_UuidToString( &uuidBin, ( char * )uuidStrBuf, ATMO_UUID_STR_LEN );

		uint8_t respBuffer[49] = {0}; // \r\nUUID\r\nOK\r\n + NULL
		sprintf( ( char * )respBuffer, "\r\n%s\r\nOK\r\n", ( char * )uuidStrBuf );
		ATMO_UART_WriteStringBlocking( _ATMO_CLOUD_UART_UartInstance, ( char * )respBuffer );
	}


}

void _ATMO_CLOUD_UART_ProcessAtRevisionInformation()
{
	// Build UUID
	ATMO_UART_WriteStringBlocking( _ATMO_CLOUD_UART_UartInstance, "\r\n" ATMO_GLOBALS_BUILDUUID "\r\nOK\r\n" );
}

void _ATMO_CLOUD_UART_ProcessAtUnlock( char *data )
{
	if ( !ATMO_CLOUD_GetRegistration()->registered )
	{
		ATMO_PLATFORM_DebugPrint( "Not registered\r\n" );
		_ATMO_CLOUD_UART_AtSendErr();
		return;
	}
	else
	{
		// Make sure token matches
		char *tokenStart = _ATMO_CLOUD_UART_AtGetNextParameter( data );

		if ( strlen( tokenStart ) != strlen( ATMO_CLOUD_GetRegistration()->token ) )
		{
			_ATMO_CLOUD_UART_AtSendErr();
			return;
		}

		if ( strcmp( tokenStart, ATMO_CLOUD_GetRegistration()->token ) == 0 )
		{
			ATMO_CLOUD_GetRegistration()->registered = false;
			_ATMO_CLOUD_UART_AtSendOk();
		}
		else
		{
			_ATMO_CLOUD_UART_AtSendErr();
		}
	}
}

void _ATMO_CLOUD_UART_ProcessAtLock( char *data )
{
	if ( ATMO_CLOUD_GetRegistration()->registered == true )
	{
		_ATMO_CLOUD_UART_AtSendErr();
		return;
	}

	ATMO_CLOUD_GetRegistration()->registered = true;
	ATMO_CLOUD_SetRegistration( NULL );


	_ATMO_CLOUD_UART_AtSendOk();
}

void _ATMO_CLOUD_UART_ProcessAtGetExtraSettings( char *data )
{
	if ( ATMO_CLOUD_GetRegistration()->registered == true )
	{
		_ATMO_CLOUD_UART_AtSendErr();
		return;
	}

	char extraSettingsStr[32] = {0};
	sprintf( extraSettingsStr, "\r\n0x%04X\r\nOK\r\n", ATMO_CLOUD_GetExtraRequiredSettings() );
	ATMO_UART_WriteStringBlocking( _ATMO_CLOUD_UART_UartInstance, extraSettingsStr );
}

static ATMO_BOOL_t _ATMO_CLOUD_UART_ProcessSetExtraSetting( uint16_t subType, uint16_t id, char *value )
{
	if ( subType == ATMO_CLOUD_PROVISIONER_CommandType_ExtraSettingsWifi )
	{
		if ( id == ATMO_CLOUD_PROVISIONER_SubCommand_WifiExtraSettings_SetSSID )
		{
			ATMO_Value_t atmoValue;
			ATMO_InitValue( &atmoValue );
			ATMO_CreateValueString( &atmoValue, ( char * )value );
			ATMO_CLOUD_SetConfig( ATMO_CLOUD_CONFIG( WIFI, SSID ), &atmoValue );
			ATMO_FreeValue( &atmoValue );
		}
		else if ( id == ATMO_CLOUD_PROVISIONER_SubCommand_WifiExtraSettings_SetPassword )
		{
			ATMO_Value_t atmoValue;
			ATMO_InitValue( &atmoValue );
			ATMO_CreateValueString( &atmoValue, ( char * )value );
			ATMO_CLOUD_SetConfig( ATMO_CLOUD_CONFIG( WIFI, PASS ), &atmoValue );
			ATMO_FreeValue( &atmoValue );
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}

static ATMO_BOOL_t _ATMO_CLOUD_UART_ProcessGetExtraSetting( uint16_t subType, uint16_t id )
{
	if ( subType == ATMO_CLOUD_PROVISIONER_CommandType_ExtraSettingsWifi )
	{
		if ( id == ATMO_CLOUD_PROVISIONER_SubCommand_WifiExtraSettings_GetStatus )
		{
			ATMO_WIFI_ConnectionStatus_t wifiStatus;

			if ( ATMO_WIFI_GetStatus( 0, &wifiStatus ) != ATMO_WIFI_Status_Success )
			{
				return false;
			}

			char resp[128] = {0};
			sprintf( resp, "\r\nConnectivity: 0x%X", wifiStatus.connectivity );

			if ( wifiStatus.connectivity == ATMO_WIFI_CONNECTIVITY_CONNECTED )
			{
				if ( wifiStatus.ipType == ATMO_WIFI_IPV4 )
				{
					sprintf( resp + strlen( resp ), "\r\nIP: %d.%d.%d.%d", wifiStatus.ipAddress[0], wifiStatus.ipAddress[1], wifiStatus.ipAddress[2], wifiStatus.ipAddress[3] );
				}
				else
				{
					sprintf( resp + strlen( resp ), "\r\nIP: %02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X,",
					         wifiStatus.ipAddress[0],
					         wifiStatus.ipAddress[1],
					         wifiStatus.ipAddress[2],
					         wifiStatus.ipAddress[3],
					         wifiStatus.ipAddress[4],
					         wifiStatus.ipAddress[5],
					         wifiStatus.ipAddress[6],
					         wifiStatus.ipAddress[7],
					         wifiStatus.ipAddress[8],
					         wifiStatus.ipAddress[9],
					         wifiStatus.ipAddress[10],
					         wifiStatus.ipAddress[11],
					         wifiStatus.ipAddress[12],
					         wifiStatus.ipAddress[13],
					         wifiStatus.ipAddress[14],
					         wifiStatus.ipAddress[15] );
				}

			}
			else if ( wifiStatus.connectivity == ATMO_WIFI_CONNECTIVITY_DISCONNECTED )
			{
				sprintf( resp + strlen( resp ), "\r\nDisconnect Reason: 0x%X", wifiStatus.disconnectReason );
			}

			sprintf( resp + strlen( resp ), "\r\n" );
			ATMO_UART_WriteStringBlocking( _ATMO_CLOUD_UART_UartInstance, resp );
		}
		else
		{
			return false;
		}
	}
	else if ( subType == ATMO_CLOUD_PROVISIONER_CommandType_ExtraSettingsSigfox )
	{
		if ( id == ATMO_CLOUD_PROVISIONER_SubCommand_SigfoxExtraSettings_GetDeviceId )
		{
			ATMO_Value_t deviceId;
			ATMO_InitValue( &deviceId );

			if ( ATMO_CLOUD_GetConfig( ATMO_CLOUD_CONFIG( SIGFOX, DEVICE_ID ), &deviceId ) != ATMO_CLOUD_Status_Success )
			{
				return false;
			}
			else
			{
				ATMO_UART_WriteStringBlocking( _ATMO_CLOUD_UART_UartInstance, (char *)deviceId.data );
				ATMO_FreeValue( &deviceId );
			}
		}
		else
		{
			return false;
		}

	}
	else if ( subType == ATMO_CLOUD_PROVISIONER_CommandType_ExtraSettingsLoRa )
	{
		if ( id == ATMO_CLOUD_PROVISIONER_SubCommand_LoRaExtraSettings_GetDeviceId )
		{
			ATMO_Value_t deviceId;
			ATMO_InitValue( &deviceId );

			if ( ATMO_CLOUD_GetConfig( ATMO_CLOUD_CONFIG( LORA, DEVICE_ID ), &deviceId ) != ATMO_CLOUD_Status_Success )
			{
				return false;
			}
			else
			{
				ATMO_UART_WriteStringBlocking( _ATMO_CLOUD_UART_UartInstance, (char *)deviceId.data );
				ATMO_FreeValue( &deviceId );
			}
		}
		else
		{
			return false;
		}

	}
	else
	{
		return false;
	}

	return true;
}

void _ATMO_CLOUD_UART_ProcessAtSetExtraSetting( char *data )
{
	char *settingSubtype = _ATMO_CLOUD_UART_AtGetNextParameter( data );

	if ( settingSubtype == NULL )
	{
		_ATMO_CLOUD_UART_AtSendErr();
		return;
	}

	data = settingSubtype;
	char *settingId = _ATMO_CLOUD_UART_AtGetNextParameter( data );

	if ( settingId == NULL )
	{
		_ATMO_CLOUD_UART_AtSendErr();
		return;
	}

	data = settingId;
	char *settingVal = _ATMO_CLOUD_UART_AtGetNextParameter( data );

	if ( settingVal == NULL )
	{
		_ATMO_CLOUD_UART_AtSendErr();
		return;
	}

	if ( _ATMO_CLOUD_UART_ProcessSetExtraSetting( strtol( settingSubtype, NULL, 16 ), strtol( settingId, NULL, 16 ), settingVal ) )
	{
		_ATMO_CLOUD_UART_AtSendOk();
	}
	else
	{
		_ATMO_CLOUD_UART_AtSendErr();
	}
}

void _ATMO_CLOUD_UART_ProcessAtGetExtraSetting( char *data )
{
	char *settingSubtype = _ATMO_CLOUD_UART_AtGetNextParameter( data );

	if ( settingSubtype == NULL )
	{
		_ATMO_CLOUD_UART_AtSendErr();
		return;
	}

	data = settingSubtype;
	char *settingId = _ATMO_CLOUD_UART_AtGetNextParameter( data );

	if ( settingId == NULL )
	{
		_ATMO_CLOUD_UART_AtSendErr();
		return;
	}

	data = settingId;

	if ( _ATMO_CLOUD_UART_ProcessGetExtraSetting( strtol( settingSubtype, NULL, 16 ), strtol( settingId, NULL, 16 ) ) )
	{
		_ATMO_CLOUD_UART_AtSendOk();
	}
	else
	{
		_ATMO_CLOUD_UART_AtSendErr();
	}
}

void _ATMO_CLOUD_UART_ProcessAtSetUrl( char *data )
{
	if ( ATMO_CLOUD_GetRegistration()->registered == true )
	{
		_ATMO_CLOUD_UART_AtSendErr();
		return;
	}

	char *url = _ATMO_CLOUD_UART_AtGetNextParameter( data );

	if ( url == NULL )
	{
		_ATMO_CLOUD_UART_AtSendErr();
		return;
	}

	ATMO_CLOUD_SetRegistrationInfoUrl( ATMO_CLOUD_GetRegistration(), url );
	_ATMO_CLOUD_UART_AtSendOk();
}

void _ATMO_CLOUD_UART_ProcessAtSetToken( char *data )
{
	if ( ATMO_CLOUD_GetRegistration()->registered == true )
	{
		_ATMO_CLOUD_UART_AtSendErr();
		return;
	}

	char *token = _ATMO_CLOUD_UART_AtGetNextParameter( data );

	if ( token == NULL )
	{
		_ATMO_CLOUD_UART_AtSendErr();
		return;
	}

	ATMO_CLOUD_SetRegistrationInfoToken( ATMO_CLOUD_GetRegistration(), token );
	_ATMO_CLOUD_UART_AtSendOk();
}

void _ATMO_CLOUD_UART_ProcessAtSetUuid( char *data )
{
	if ( ATMO_CLOUD_GetRegistration()->registered == true )
	{
		_ATMO_CLOUD_UART_AtSendErr();
		return;
	}

	char *uuid = _ATMO_CLOUD_UART_AtGetNextParameter( data );

	if ( uuid == NULL )
	{
		_ATMO_CLOUD_UART_AtSendErr();
		return;
	}

	ATMO_UUID_t uuidBin;
	ATMO_StringToUuid( uuid, &uuidBin, ATMO_UUID_ENDIAN_DEFAULT );

	memcpy( ATMO_CLOUD_GetRegistration()->uuid, &uuidBin.data, 16 );
	_ATMO_CLOUD_UART_AtSendOk();
}

static void _ATMO_CLOUD_UART_TrimString( char *strBuf )
{
	char *lastChar = &strBuf[strlen( strBuf ) - 1];

	while ( *lastChar == '\n' || *lastChar == '\r' )
	{
		*lastChar = 0;
		lastChar--;
	}
}

void _ATMO_CLOUD_UART_RxCb( void *data )
{
	ATMO_Value_t *value = ( ATMO_Value_t * )data;

	// Echo cmd
	ATMO_UART_WriteStringBlocking( _ATMO_CLOUD_UART_UartInstance, ( char * )value->data );


	if ( ( strncmp( ( char * )value->data, "AT\r\n", strlen( "AT\r\n" ) ) ) == 0 )
	{
		_ATMO_CLOUD_UART_ProcessAt();
	}
	else
	{
		// Trim end of string
		if ( strlen( ( char * )value->data ) > 0 )
		{
			_ATMO_CLOUD_UART_TrimString( ( char * )value->data );
		}

		if ( strncmp( ( char * )value->data, "AT+GMI", strlen( "AT+GMI" ) ) == 0 )
		{
			_ATMO_CLOUD_UART_ProcessAtManufacturerInformation();
		}
		else if ( strncmp( ( char * )value->data, "AT+GMM", strlen( "AT+GMM" ) ) == 0 )
		{
			_ATMO_CLOUD_UART_ProcessAtModelInformation();
		}
		else if ( strncmp( ( char * )value->data, "AT+GMR", strlen( "AT+GMR" ) ) == 0 )
		{
			_ATMO_CLOUD_UART_ProcessAtRevisionInformation();
		}
		else if ( strncmp( ( char * )value->data, "AT#NIMUNLOCK", strlen( "AT#NIMUNLOCK" ) ) == 0 )
		{
			_ATMO_CLOUD_UART_ProcessAtUnlock( ( char * )value->data );
		}
		else if ( strncmp( ( char * )value->data, "AT#NIMLOCK", strlen( "AT#NIMLOCK" ) ) == 0 )
		{
			_ATMO_CLOUD_UART_ProcessAtLock( ( char * )value->data );
		}
		else if ( strncmp( ( char * )value->data, "AT#NIMGETEXTRASETTINGS", strlen( "AT#NIMGETEXTRASETTINGS" ) ) == 0 )
		{
			_ATMO_CLOUD_UART_ProcessAtGetExtraSettings( ( char * )value->data );
		}
		else if ( strncmp( ( char * )value->data, "AT#NIMSETEXTRASETTING", strlen( "AT#NIMSETEXTRASETTING" ) ) == 0 )
		{
			_ATMO_CLOUD_UART_ProcessAtSetExtraSetting( ( char * )value->data );
		}
		else if ( strncmp( ( char * )value->data, "AT#NIMGETEXTRASETTING", strlen( "AT#NIMGETEXTRASETTING" ) ) == 0 )
		{
			_ATMO_CLOUD_UART_ProcessAtGetExtraSetting( ( char * )value->data );
		}
		else if ( strncmp( ( char * )value->data, "AT#NIMSETURL", strlen( "AT#NIMSETURL" ) ) == 0 )
		{
			_ATMO_CLOUD_UART_ProcessAtSetUrl( ( char * )value->data );
		}
		else if ( strncmp( ( char * )value->data, "AT#NIMSETTOKEN", strlen( "AT#NIMSETTOKEN" ) ) == 0 )
		{
			_ATMO_CLOUD_UART_ProcessAtSetToken( ( char * )value->data );
		}
		else if ( strncmp( ( char * )value->data, "AT#NIMSETUUID", strlen( "AT#NIMSETUUID" ) ) == 0 )
		{
			_ATMO_CLOUD_UART_ProcessAtSetUuid( ( char * )value->data );
		}
	}

}

ATMO_CLOUD_Status_t ATMO_CLOUD_UART_Init( ATMO_DriverInstanceData_t *instance, ATMO_BOOL_t provision )
{
	ATMO_UART_Peripheral_t uartConfig;
	ATMO_UART_GetDefaultConfig( &uartConfig );
	uartConfig.isBinaryData = false;
	uartConfig.rxBuffer = true;
	uartConfig.rxTrim = false;
	uartConfig.customBuffer = _ATMO_CLOUD_UART_Buf;
	uartConfig.customBufferSize = ATMO_CLOUD_UART_TX_RX_BUFLEN;
	strcpy( uartConfig.splitRegex, ".*\r\n" );
	ATMO_UART_RegisterRxCbFunc( _ATMO_CLOUD_UART_UartInstance, _ATMO_CLOUD_UART_RxCb );
	ATMO_UART_SetConfiguration( _ATMO_CLOUD_UART_UartInstance, &uartConfig );
	return ATMO_CLOUD_Status_Success;
}

ATMO_CLOUD_Status_t ATMO_CLOUD_UART_SendEvent( ATMO_DriverInstanceData_t *instance, const char *eventName, ATMO_Value_t *data, uint32_t timeout )
{
	return ATMO_CLOUD_Status_NotSupported;
}

ATMO_CLOUD_Status_t ATMO_CLOUD_UART_PopCommand( ATMO_DriverInstanceData_t *instance, const char *commandName, ATMO_Value_t *data, uint32_t timeout )
{
	return ATMO_CLOUD_Status_NotSupported;
}
