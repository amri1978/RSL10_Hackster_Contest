/**
 ******************************************************************************
 * @file    cloud_ble.c
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - CLOUD over BLE Driver
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

#include "cloud_ble.h"
#include "cloud_config.h"
#include "../app_src/atmosphere_platform.h"
#include "../app_src/atmosphere_globals.h"
#include "../datetime/datetime.h"
#include "../wifi/wifi.h"
#include "cloud_provisioner.h"

const ATMO_CLOUD_DriverInstance_t cloudBleDriverInstance =
{
	ATMO_CLOUD_BLE_Init,
	ATMO_CLOUD_BLE_SendEvent,
	ATMO_CLOUD_BLE_PopCommand
};

typedef struct
{
	ATMO_BOOL_t connectionVerified;
	ATMO_DriverInstanceHandle_t bleInstanceNumber;
	ATMO_BLE_Handle_t bleServiceHandle;
	ATMO_BLE_Handle_t bleCharacteristicHandle;
} __ATMO_CLOUD_BLE_DriverInstanceConfig_t;

typedef enum
{
	ATMO_CLOUD_BLE_PROVISIONING,
	ATMO_CLOUD_BLE_PROVISIONED,
	ATMO_CLOUD_BLE_WAITING_FOR_GATEWAY,
	ATMO_CLOUD_BLE_NUM_PROVISION_STATUS,
} ATMO_CLOUD_BLE_ProvisionStatus_t;

char provisioningStatusChar[ATMO_CLOUD_BLE_NUM_PROVISION_STATUS] = {'P', 'D', 'G'};

static __ATMO_CLOUD_BLE_DriverInstanceConfig_t *currentBleDriverInstanceConfig = NULL;

static void __ATMO_CLOUD_BLE_GetAdvName( ATMO_CLOUD_BLE_ProvisionStatus_t status, char *name )
{
	ATMO_BLE_MacAddress_t macAddress;

	if ( ATMO_BLE_GetMacAddress( currentBleDriverInstanceConfig->bleInstanceNumber, &macAddress ) == ATMO_BLE_Status_Success )
	{
		sprintf( name, "%c%02X%02X", provisioningStatusChar[status], macAddress.data[4], macAddress.data[5] );
	}
	else
	{
		sprintf( name, "%cFFFF", provisioningStatusChar[status] );
	}
}

ATMO_CLOUD_Status_t __ATMO_CLOUD_BLE_StartAdvertisingForProvisioning( __ATMO_CLOUD_BLE_DriverInstanceConfig_t *config )
{
	char deviceName[32];
	__ATMO_CLOUD_BLE_GetAdvName( ATMO_CLOUD_BLE_PROVISIONING, deviceName );

#ifndef ATMO_SLIM_STACK
	ATMO_PLATFORM_DebugPrint( "Setting device name: %s\r\n", deviceName );
#endif
	ATMO_BLE_GAPSetDeviceName( config->bleInstanceNumber, deviceName );

#ifndef ATMO_SLIM_STACK
	ATMO_PLATFORM_DebugPrint( "Setting service uuid %s\r\n", ATMO_GLOBALS_PROJECTUUID);
#endif
	ATMO_BLE_GAPSetAdvertisedServiceUUID( config->bleInstanceNumber, ATMO_GLOBALS_PROJECTUUID );

	ATMO_BLE_AdvertisingParams_t advParams;
	advParams.type = ATMO_BLE_ADV_TYPE_INDIRECT;
	advParams.advertisingInterval = 1000;
#ifndef ATMO_SLIM_STACK
	ATMO_PLATFORM_DebugPrint( "Starting advertising\r\n" );
#endif
	ATMO_BLE_GAPAdvertisingStart( config->bleInstanceNumber, &advParams );

	return ATMO_CLOUD_Status_Success;
}

ATMO_CLOUD_Status_t __ATMO_CLOUD_BLE_StartAdvertisingForRegistered( __ATMO_CLOUD_BLE_DriverInstanceConfig_t *config )
{
	char deviceName[32];
	__ATMO_CLOUD_BLE_GetAdvName( ATMO_CLOUD_BLE_PROVISIONED, deviceName );

	ATMO_BLE_GAPSetDeviceName( config->bleInstanceNumber, deviceName );

	if ( ATMO_CLOUD_GetRegistration()->registered == false )
	{
		return ATMO_CLOUD_Status_Invalid;
	}

	char uuidBuffer[40];

	ATMO_CLOUD_GetRegistrationInfoUuid( ATMO_CLOUD_GetRegistration(), uuidBuffer, 40 );

	ATMO_BLE_GAPSetAdvertisedServiceUUID( config->bleInstanceNumber, uuidBuffer );

	ATMO_BLE_AdvertisingParams_t advParams;
	advParams.type = ATMO_BLE_ADV_TYPE_INDIRECT;
	advParams.advertisingInterval = 1000;
	ATMO_BLE_GAPAdvertisingStart( config->bleInstanceNumber, &advParams );

	return ATMO_CLOUD_Status_Success;
}

ATMO_CLOUD_Status_t __ATMO_CLOUD_BLE_StartAdvertising( __ATMO_CLOUD_BLE_DriverInstanceConfig_t *config )
{
	ATMO_BLE_GAPAdvertisingStop( config->bleInstanceNumber );

	if ( ATMO_CLOUD_GetRegistration()->registered == true )
	{
		return __ATMO_CLOUD_BLE_StartAdvertisingForRegistered( config );
	}

	else
	{
		return __ATMO_CLOUD_BLE_StartAdvertisingForProvisioning( config );
	}
}



void __ATMO_CLOUD_BLE_ConnectedCallback( void *value )
{
	currentBleDriverInstanceConfig->connectionVerified = false;
	ATMO_BLE_GAPAdvertisingStop( currentBleDriverInstanceConfig->bleInstanceNumber );
}

void __ATMO_CLOUD_BLE_DisconnectedCallback( void *value )
{
	currentBleDriverInstanceConfig->connectionVerified = false;
	__ATMO_CLOUD_BLE_StartAdvertising( currentBleDriverInstanceConfig );
}

ATMO_CLOUD_Status_t __ATMO_CLOUD_BLE_StartAdvertisingForGateway( __ATMO_CLOUD_BLE_DriverInstanceConfig_t *config )
{
	return ATMO_CLOUD_Status_NotSupported;
}

static void __ATMO_CLOUD_BLE_SendCommandResponse( uint8_t *data, unsigned int dataLen )
{
	ATMO_BLE_GATTSSetCharacteristic(
	    currentBleDriverInstanceConfig->bleInstanceNumber,
	    currentBleDriverInstanceConfig->bleCharacteristicHandle,
	    dataLen,
	    data,
	    NULL );
}

static void __ATMO_CLOUD_BLE_RegistrationSetCb( void *data )
{
	ATMO_BLE_GAPAdvertisingStop( currentBleDriverInstanceConfig->bleInstanceNumber );
	__ATMO_CLOUD_BLE_StartAdvertising( currentBleDriverInstanceConfig );
}

void __ATMO_CLOUD_BLE_CharacteristicWrittenCallback( void *value )
{
	//We don't have a config to operatate on.
	if ( currentBleDriverInstanceConfig == NULL )
	{
		return;
	}

	//The BLE Driver returns the current value
	//written as an ATMO_Value_t object.
	ATMO_Value_t *currentValue = ( ATMO_Value_t * )value;

	//We expect a binary value.
	if ( currentValue->type != ATMO_DATATYPE_BINARY || currentValue->size < 1 )
	{
		uint8_t response[3] = {0xFF, currentValue->type, 0};
		__ATMO_CLOUD_BLE_SendCommandResponse( response, 3 );
		return;
	}

	uint8_t *data = ( uint8_t * ) currentValue->data;
	uint8_t respBuffer[22] = {0};
	int cmdLen = ATMO_CLOUD_PROVISIONER_HandleCommand( data, currentValue->size, respBuffer, sizeof( respBuffer ), &currentBleDriverInstanceConfig->connectionVerified );

	if ( cmdLen < 0 )
	{
#ifndef ATMO_SLIM_STACK
		ATMO_PLATFORM_DebugPrint( "[CLOUD BLE] Error building response\r\n" );
#endif
		return;
	}

	__ATMO_CLOUD_BLE_SendCommandResponse( respBuffer, cmdLen );
}

ATMO_Status_t ATMO_CLOUD_BLE_AddDriverInstance( ATMO_DriverInstanceHandle_t *instanceNumber, ATMO_DriverInstanceHandle_t bleInstanceNumber )
{

	static __ATMO_CLOUD_BLE_DriverInstanceConfig_t config;

	config.connectionVerified = false;
	config.bleInstanceNumber = bleInstanceNumber;

	static ATMO_DriverInstanceData_t cloudDriverInstanceData;

	cloudDriverInstanceData.name = "Cloud BLE";
	cloudDriverInstanceData.initialized = false;
	cloudDriverInstanceData.instanceNumber = 0;
	cloudDriverInstanceData.argument = ( void * )&config;

	return ATMO_CLOUD_AddDriverInstance( &cloudBleDriverInstance, &cloudDriverInstanceData, instanceNumber );
}

ATMO_CLOUD_Status_t ATMO_CLOUD_BLE_Init( ATMO_DriverInstanceData_t *instance, ATMO_BOOL_t provision )
{
	__ATMO_CLOUD_BLE_DriverInstanceConfig_t *config = ( __ATMO_CLOUD_BLE_DriverInstanceConfig_t * )instance->argument;

	//FIXME: Really need to make it so that there can be
	// multiple Cloud BLE driver instances which this
	// kind of prevents.
	currentBleDriverInstanceConfig = config;

	ATMO_BLE_PeripheralInit( config->bleInstanceNumber );
	ATMO_BLE_RegisterEventCallback( config->bleInstanceNumber, ATMO_BLE_EVENT_Connected, __ATMO_CLOUD_BLE_ConnectedCallback );
	ATMO_BLE_RegisterEventCallback( config->bleInstanceNumber, ATMO_BLE_EVENT_Disconnected, __ATMO_CLOUD_BLE_DisconnectedCallback );

	if ( provision )
	{
		ATMO_BLE_GATTSAddService(
		    config->bleInstanceNumber,
		    &config->bleServiceHandle,
		    ATMO_CLOUD_BLE_PRIMARY_SERVICE_UUID );

		ATMO_BLE_GATTSAddCharacteristic(
		    config->bleInstanceNumber,
		    &config->bleCharacteristicHandle,
		    config->bleServiceHandle,
		    ATMO_CLOUD_BLE_PRIMARY_CHARACTERISTIC_UUID,
		    ATMO_BLE_Property_Read | ATMO_BLE_Property_Write | ATMO_BLE_Property_Notify, ATMO_BLE_Permission_Read | ATMO_BLE_Permission_Write, 64 );

		instance->initialized = true;

		ATMO_BLE_GATTSRegisterCharacteristicCallback(
		    config->bleInstanceNumber,
		    config->bleCharacteristicHandle,
		    ATMO_BLE_Characteristic_Written,
		    __ATMO_CLOUD_BLE_CharacteristicWrittenCallback );
	}

	__ATMO_CLOUD_BLE_StartAdvertising( config );

	ATMO_CLOUD_RegisterRegistrationSetCallback( __ATMO_CLOUD_BLE_RegistrationSetCb );

	return ATMO_CLOUD_Status_Success;
}

ATMO_CLOUD_Status_t ATMO_CLOUD_BLE_SendEvent( ATMO_DriverInstanceData_t *instance, const char *eventName, ATMO_Value_t *data, uint32_t timeout )
{
	return ATMO_CLOUD_Status_NotSupported;
}

ATMO_CLOUD_Status_t ATMO_CLOUD_BLE_PopCommand( ATMO_DriverInstanceData_t *instance, const char *commandName, ATMO_Value_t *data, uint32_t timeout )
{
	return ATMO_CLOUD_Status_NotSupported;
}

