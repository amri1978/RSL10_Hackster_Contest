#include "cloud_provisioner.h"
#include "cloud_config.h"
#include "cloud.h"
#include "../app_src/atmosphere_platform.h"
#include "../wifi/wifi.h"
#include "../datetime/datetime.h"

static int __ATMO_CLOUD_PROVISIONER_BuildGenericSuccess( ATMO_CLOUD_PROVISIONER_CommandType_t command, uint8_t *respBuffer, uint32_t respBufferLength )
{
	if ( respBufferLength < 2 )
	{
		return -1;
	}

	respBuffer[0] = command;
	respBuffer[1] = ATMO_CLOUD_Status_Success;
	return 2;
}

static int __ATMO_CLOUD_PROVISIONER_BuildGenericFail( ATMO_CLOUD_PROVISIONER_CommandType_t command, uint8_t *respBuffer, uint32_t respBufferLength )
{
	if ( respBufferLength < 2 )
	{
		return -1;
	}

	respBuffer[0] = command;
	respBuffer[1] = ATMO_CLOUD_Status_Fail;
	return 2;
}

static int __ATMO_CLOUD_PROVISIONER_HandleSigfoxSubCommand( ATMO_CLOUD_PROVISIONER_SubCommand_Sigfox_t subCommand, uint8_t *data, uint32_t dataLength, uint8_t *respBuffer, uint32_t respBufferLength )
{
	switch ( subCommand )
	{
		case ATMO_CLOUD_PROVISIONER_SubCommand_SigfoxExtraSettings_GetDeviceId:
		{
			if ( respBufferLength < 7 )
			{
				return -1;
			}

			ATMO_Value_t deviceId;
			ATMO_InitValue( &deviceId );

			if ( ATMO_CLOUD_GetConfig( ATMO_CLOUD_CONFIG( SIGFOX, DEVICE_ID ), &deviceId ) != ATMO_CLOUD_Status_Success )
			{
				ATMO_FreeValue( &deviceId );
				return __ATMO_CLOUD_PROVISIONER_BuildGenericFail( ATMO_CLOUD_PROVISIONER_CommandType_ExtraSettingsSigfox, respBuffer, respBufferLength );
			}
			else
			{
				unsigned int deviceIdVal;
				ATMO_GetUnsignedInt( &deviceId, &deviceIdVal );
				ATMO_FreeValue( &deviceId );

				// Need to do this because some compilers get picky if you put a uint32_t into ATMO_GetUnsignedInt
				uint32_t deviceIdVal32Bit = ( uint32_t )( deviceIdVal );

				uint8_t response[7] =
				{
					ATMO_CLOUD_PROVISIONER_CommandType_ExtraSettingsSigfox,
					ATMO_CLOUD_PROVISIONER_SubCommand_SigfoxExtraSettings_GetDeviceId,
					ATMO_CLOUD_Status_Success,
					deviceIdVal32Bit & 0xFF,
					( deviceIdVal32Bit >> 8 ) & 0xFF,
					( deviceIdVal32Bit >> 16 ) & 0xFF,
					( deviceIdVal32Bit >> 24 ) & 0xFF
				};

				memcpy( respBuffer, response, 7 );
				return 7;
			}

			break;
		}

		default:
		{
			return -1;
		}
	}

	return -1;
}


static int __ATMO_CLOUD_PROVISIONER_HandleLoRaSubCommand( ATMO_CLOUD_PROVISIONER_SubCommand_LoRa_t subCommand, uint8_t *data, uint32_t dataLength, uint8_t *respBuffer, uint32_t respBufferLength )
{
	switch ( subCommand )
	{
		case ATMO_CLOUD_PROVISIONER_SubCommand_LoRaExtraSettings_GetDeviceId:
		{
			uint32_t respLen = ATMO_LORA_DEV_EUI_LEN + 3;

			if ( respBufferLength < respLen )
			{
				return -1;
			}

			ATMO_Value_t deviceId;
			ATMO_InitValue( &deviceId );

			if ( ATMO_CLOUD_GetConfig( ATMO_CLOUD_CONFIG( LORA, DEVICE_ID ), &deviceId ) != ATMO_CLOUD_Status_Success )
			{
				ATMO_FreeValue( &deviceId );
				return __ATMO_CLOUD_PROVISIONER_BuildGenericFail( ATMO_CLOUD_PROVISIONER_CommandType_ExtraSettingsLoRa, respBuffer, respBufferLength );
			}
			else
			{
				if ( deviceId.size < ATMO_LORA_DEV_EUI_LEN )
				{
					ATMO_FreeValue( &deviceId );
					return __ATMO_CLOUD_PROVISIONER_BuildGenericFail( ATMO_CLOUD_PROVISIONER_CommandType_ExtraSettingsLoRa, respBuffer, respBufferLength );
				}


				uint8_t response[respLen];
				response[0] = ATMO_CLOUD_PROVISIONER_CommandType_ExtraSettingsLoRa;
				response[1] = ATMO_CLOUD_PROVISIONER_SubCommand_LoRaExtraSettings_GetDeviceId;
				response[2] = ATMO_CLOUD_Status_Success;
				memcpy( &response[3], deviceId.data, ATMO_LORA_DEV_EUI_LEN );

				ATMO_FreeValue( &deviceId );

				memcpy( respBuffer, response, respLen );
				return respLen;
			}

			break;
		}

		default:
		{
			return -1;
		}
	}

	return -1;
}

static int __ATMO_CLOUD_PROVISIONER_HandleWifiSubCommand( ATMO_CLOUD_PROVISIONER_SubCommand_Wifi_t subCommand, uint8_t *data, uint32_t dataLength, uint8_t *respBuffer, uint32_t respBufferLength )
{
	switch ( subCommand )
	{
		case ATMO_CLOUD_PROVISIONER_SubCommand_WifiExtraSettings_GetStatus:
		{
			ATMO_WIFI_ConnectionStatus_t wifiStatus;
			ATMO_WIFI_GetStatus( 0, &wifiStatus );

			if ( respBufferLength < 22 )
			{
				return -1;
			}

			uint32_t responseLen = 0;
			respBuffer[responseLen++] = ATMO_CLOUD_PROVISIONER_CommandType_ExtraSettingsWifi;
			respBuffer[responseLen++] = ATMO_CLOUD_PROVISIONER_SubCommand_WifiExtraSettings_GetStatus;
			respBuffer[responseLen++] = ATMO_CLOUD_Status_Success;
			respBuffer[responseLen++] = ( uint8_t )wifiStatus.connectivity;
			respBuffer[responseLen++] = ( uint8_t )wifiStatus.disconnectReason;
			respBuffer[responseLen++] = ( uint8_t )wifiStatus.ipType;

			// IP Address
			memcpy( &respBuffer[responseLen], wifiStatus.ipAddress, 16 );
			responseLen += 16;
			return responseLen;
		}

		case ATMO_CLOUD_PROVISIONER_SubCommand_WifiExtraSettings_SetSSID:
		{
			ATMO_Value_t atmoValue;
			ATMO_InitValue( &atmoValue );
			ATMO_CreateValueString( &atmoValue, ( char * )data );
			ATMO_CLOUD_SetConfig( ATMO_CLOUD_CONFIG( WIFI, SSID ), &atmoValue );
			ATMO_FreeValue( &atmoValue );

			if ( respBufferLength < 3 )
			{
				return -1;
			}

			uint8_t response[3] = {ATMO_CLOUD_PROVISIONER_CommandType_ExtraSettingsWifi, ATMO_CLOUD_PROVISIONER_SubCommand_WifiExtraSettings_SetSSID, ATMO_CLOUD_Status_Success};
			memcpy( respBuffer, response, 3 );
			return 3;
		}

		case ATMO_CLOUD_PROVISIONER_SubCommand_WifiExtraSettings_SetPassword:
		{
			ATMO_Value_t atmoValue;
			ATMO_InitValue( &atmoValue );
			ATMO_CreateValueString( &atmoValue, ( char * )data );
			ATMO_CLOUD_SetConfig( ATMO_CLOUD_CONFIG( WIFI, PASS ), &atmoValue );
			ATMO_FreeValue( &atmoValue );

			if ( respBufferLength < 3 )
			{
				return -1;
			}

			uint8_t response[3] = {ATMO_CLOUD_PROVISIONER_CommandType_ExtraSettingsWifi, ATMO_CLOUD_PROVISIONER_SubCommand_WifiExtraSettings_SetPassword, ATMO_CLOUD_Status_Success};
			memcpy( respBuffer, response, 3 );
			return 3;
		}

		default:
		{
			return -1;
		}
	}

	return -1;
}

int ATMO_CLOUD_PROVISIONER_HandleCommand( uint8_t *command, uint32_t commandLength, uint8_t *respBuffer, uint32_t respBufferLength, ATMO_BOOL_t *connectionVerified )
{
#ifndef ATMO_SLIM_STACK
	ATMO_PLATFORM_DebugPrint( "\r\n[Cloud Provision] CMD %02X\r\n", command[0] );
	unsigned int i;

	for ( i = 1; i < commandLength; i++ )
	{
		if ( command[i] >= ' ' && command[i] <= '~' )
		{
			ATMO_PLATFORM_DebugPrint( "[Cloud Provision] Data[%02d]: %02X (%c)\r\n", i, command[i], command[i] );
		}
		else
		{
			ATMO_PLATFORM_DebugPrint( "[Cloud Provision] Data[%02d]: %02X\r\n", i, command[i] );
		}


	}

#endif

	// Minimum size
	if ( respBufferLength < 2 )
	{
		return -1;
	}

	switch ( ( ATMO_CLOUD_PROVISIONER_CommandType_t )command[0] )
	{
		case ATMO_CLOUD_PROVISIONER_CommandType_GetIndentityType:
		{
			if ( respBufferLength < 4 )
			{
				return -1;
			}

			uint16_t extraSettings = ATMO_CLOUD_GetExtraRequiredSettings();

			uint8_t response[4] = {ATMO_CLOUD_PROVISIONER_CommandType_GetIndentityType, ATMO_CLOUD_PROVISIONER_IdentityType_Basic,
			                       extraSettings & 0xFF, ( extraSettings >> 8 ) & 0xFF
			                      };
			memcpy( respBuffer, response, 4 );
			return 4;
		}

		case ATMO_CLOUD_PROVISIONER_CommandType_BasicCentralConfirmIdentity:
		{
			//FIXME: We need to actually use the basic certification method.
			//SECURITY: This feature must be complete before launch.
			*connectionVerified = true;

			return __ATMO_CLOUD_PROVISIONER_BuildGenericSuccess( ATMO_CLOUD_PROVISIONER_CommandType_BasicCentralConfirmIdentity, respBuffer, respBufferLength );
		}

		case ATMO_CLOUD_PROVISIONER_CommandType_BasicClientConfirmIdentity:
			break;

		case ATMO_CLOUD_PROVISIONER_CommandType_LockRegistrationInfo:
		{
			if ( ATMO_CLOUD_GetRegistration()->registered == true || *connectionVerified == false )
			{
				return __ATMO_CLOUD_PROVISIONER_BuildGenericFail( ATMO_CLOUD_PROVISIONER_CommandType_LockRegistrationInfo, respBuffer, respBufferLength );
			}

			ATMO_CLOUD_GetRegistration()->registered = true;

			ATMO_CLOUD_Status_t saveStatus = ATMO_CLOUD_SetRegistration( NULL );

			uint8_t response[2] = {ATMO_CLOUD_PROVISIONER_CommandType_LockRegistrationInfo, saveStatus};
			memcpy( respBuffer, response, 2 );
			return 2;
		}

		case ATMO_CLOUD_PROVISIONER_CommandType_GetRegistrationInfo:
		{
			return __ATMO_CLOUD_PROVISIONER_BuildGenericFail( ATMO_CLOUD_PROVISIONER_CommandType_GetRegistrationInfo, respBuffer, respBufferLength );
		}

		case ATMO_CLOUD_PROVISIONER_CommandType_SetRegistrationInfoUrl:
		{

			if ( ATMO_CLOUD_GetRegistration()->registered == true || *connectionVerified == false )
			{
				return __ATMO_CLOUD_PROVISIONER_BuildGenericFail( ATMO_CLOUD_PROVISIONER_CommandType_SetRegistrationInfoUrl, respBuffer, respBufferLength );
			}

			char *url = ( char * )&command[1];

			ATMO_CLOUD_SetRegistrationInfoUrl(
			    ATMO_CLOUD_GetRegistration(),
			    url );

			return __ATMO_CLOUD_PROVISIONER_BuildGenericSuccess( ATMO_CLOUD_PROVISIONER_CommandType_SetRegistrationInfoUrl, respBuffer, respBufferLength );
		}

		case ATMO_CLOUD_PROVISIONER_CommandType_SetRegistrationInfoUuid:
		{

			if ( ATMO_CLOUD_GetRegistration()->registered == true || *connectionVerified == false )
			{
				return __ATMO_CLOUD_PROVISIONER_BuildGenericFail( ATMO_CLOUD_PROVISIONER_CommandType_SetRegistrationInfoUuid, respBuffer, respBufferLength );
			}

			uint8_t *uuidData = &command[1];

			memcpy( ATMO_CLOUD_GetRegistration()->uuid, uuidData, 16 );

			return __ATMO_CLOUD_PROVISIONER_BuildGenericSuccess( ATMO_CLOUD_PROVISIONER_CommandType_SetRegistrationInfoUuid, respBuffer, respBufferLength );
		}

		case ATMO_CLOUD_PROVISIONER_CommandType_SetRegistrationInfoToken:
		{

			if ( ATMO_CLOUD_GetRegistration()->registered == true || *connectionVerified == false )
			{
				return __ATMO_CLOUD_PROVISIONER_BuildGenericFail( ATMO_CLOUD_PROVISIONER_CommandType_SetRegistrationInfoToken, respBuffer, respBufferLength );
			}

			char *token = ( char * )&command[1];

			ATMO_CLOUD_SetRegistrationInfoToken(
			    ATMO_CLOUD_GetRegistration(),
			    token );

			return __ATMO_CLOUD_PROVISIONER_BuildGenericSuccess( ATMO_CLOUD_PROVISIONER_CommandType_SetRegistrationInfoToken, respBuffer, respBufferLength );
		}

		case ATMO_CLOUD_PROVISIONER_CommandType_ClearRegistrationInfo:
		{
			ATMO_CLOUD_ClearRegistration();

			return __ATMO_CLOUD_PROVISIONER_BuildGenericSuccess( ATMO_CLOUD_PROVISIONER_CommandType_ClearRegistrationInfo, respBuffer, respBufferLength );
		}

		case ATMO_CLOUD_PROVISIONER_CommandType_SetDateTimeIsoStr:
		{
			if ( commandLength == 0 )
			{
				return __ATMO_CLOUD_PROVISIONER_BuildGenericFail( ATMO_CLOUD_PROVISIONER_CommandType_SetDateTimeIsoStr, respBuffer, respBufferLength );
			}
			else
			{
				//FIXME: We should have configuration for which ISO Date instance a CLOUD Driver is using
				char *dateTime = ( char * )&command[1];

				ATMO_DateTime_SetDateTimeIsoStr( 0, dateTime );

				return __ATMO_CLOUD_PROVISIONER_BuildGenericSuccess( ATMO_CLOUD_PROVISIONER_CommandType_SetDateTimeIsoStr, respBuffer, respBufferLength );
			}

			break;
		}

		case ATMO_CLOUD_PROVISIONER_CommandType_UnlockRegistrationInfo:
		{
			if ( ATMO_CLOUD_GetRegistration()->registered == false )
			{
				return __ATMO_CLOUD_PROVISIONER_BuildGenericFail( ATMO_CLOUD_PROVISIONER_CommandType_UnlockRegistrationInfo, respBuffer, respBufferLength );
			}

			char savedToken[ATMO_CLOUD_TOKEN_LENGTH + 1];

			ATMO_CLOUD_GetRegistrationInfoToken( ATMO_CLOUD_GetRegistration(), savedToken, ATMO_CLOUD_TOKEN_LENGTH + 1 );

			char *token = ( char * )&command[1];

			if ( memcmp( token, savedToken, ATMO_CLOUD_TOKEN_LENGTH ) == 0 )
			{
				ATMO_CLOUD_GetRegistration()->registered = false;
				return __ATMO_CLOUD_PROVISIONER_BuildGenericSuccess( ATMO_CLOUD_PROVISIONER_CommandType_UnlockRegistrationInfo, respBuffer, respBufferLength );
			}

			else
			{
				return __ATMO_CLOUD_PROVISIONER_BuildGenericFail( ATMO_CLOUD_PROVISIONER_CommandType_UnlockRegistrationInfo, respBuffer, respBufferLength );
			}

			break;
		}

		case ATMO_CLOUD_PROVISIONER_CommandType_SetDateTimeEpochInt:
		{
			if ( commandLength != 5 )
			{
				return __ATMO_CLOUD_PROVISIONER_BuildGenericFail( ATMO_CLOUD_PROVISIONER_CommandType_SetDateTimeEpochInt, respBuffer, respBufferLength );
			}
			else
			{
				uint32_t dateEpoch = 0;
				memcpy( &dateEpoch, &command[1], sizeof( uint32_t ) );
				ATMO_DateTime_SetDateTimeEpoch( 0, dateEpoch );

				return __ATMO_CLOUD_PROVISIONER_BuildGenericSuccess( ATMO_CLOUD_PROVISIONER_CommandType_SetDateTimeEpochInt, respBuffer, respBufferLength );
			}

			break;
		}

		case ATMO_CLOUD_PROVISIONER_CommandType_ExtraSettingsWifi:
		{
			if ( commandLength < 2 )
			{
				return __ATMO_CLOUD_PROVISIONER_BuildGenericFail( ATMO_CLOUD_PROVISIONER_CommandType_ExtraSettingsWifi, respBuffer, respBufferLength );
			}

			uint8_t *wifiCommandData = ( commandLength > 2 ) ? &command[2] : NULL;
			unsigned int wifiCommandDataSize = ( commandLength > 2 ) ? commandLength - 2 : 0;
			return __ATMO_CLOUD_PROVISIONER_HandleWifiSubCommand( ( ATMO_CLOUD_PROVISIONER_SubCommand_Wifi_t )command[1], wifiCommandData, wifiCommandDataSize, respBuffer, respBufferLength );
		}

		case ATMO_CLOUD_PROVISIONER_CommandType_ExtraSettingsSigfox:
		{
			if ( commandLength < 2 )
			{
				return __ATMO_CLOUD_PROVISIONER_BuildGenericFail( ATMO_CLOUD_PROVISIONER_CommandType_ExtraSettingsSigfox, respBuffer, respBufferLength );
			}

			uint8_t *sfCommandData = ( commandLength > 2 ) ? &command[2] : NULL;
			unsigned int sfCommandDataSize = ( commandLength > 2 ) ? commandLength - 2 : 0;
			return __ATMO_CLOUD_PROVISIONER_HandleSigfoxSubCommand( ( ATMO_CLOUD_PROVISIONER_SubCommand_Sigfox_t )command[1], sfCommandData, sfCommandDataSize, respBuffer, respBufferLength );
		}

		case ATMO_CLOUD_PROVISIONER_CommandType_ExtraSettingsLoRa:
		{
			if ( commandLength < 2 )
			{
				return __ATMO_CLOUD_PROVISIONER_BuildGenericFail( ATMO_CLOUD_PROVISIONER_CommandType_ExtraSettingsLoRa, respBuffer, respBufferLength );
			}

			uint8_t *loRaCommandData = ( commandLength > 2 ) ? &command[2] : NULL;
			unsigned int loRaCommandDataSize = ( commandLength > 2 ) ? commandLength - 2 : 0;
			return __ATMO_CLOUD_PROVISIONER_HandleLoRaSubCommand( ( ATMO_CLOUD_PROVISIONER_SubCommand_LoRa_t )command[1], loRaCommandData, loRaCommandDataSize, respBuffer, respBufferLength );
		}

		default:
			break;
	};

	return -1;
}