/**
 ******************************************************************************
 * @file    ble.c
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - Core BLE Driver
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

#include "ble.h"

static void _ATMO_BLE_ConnectedHandler( ATMO_DriverInstanceHandle_t instance );

static void _ATMO_BLE0_ConnectedHandler( void *data )
{
	_ATMO_BLE_ConnectedHandler( 0 );
}

typedef struct
{
	ATMO_Callback_t connectedCb;
	const ATMO_BLE_DriverInstance_t *instance;
	ATMO_DriverInstanceData_t *instanceData;
	ATMO_BLE_Handle_t gattServiceHandle;
	ATMO_BLE_Handle_t serviceChangedCharHandle;
} ATMO_BLE_Instance_t;

#ifndef ATMO_SLIM_STACK
#define NUMBER_OF_BLE_DRIVER_INSTANCES 4
static void _ATMO_BLE1_ConnectedHandler( void *data )
{
	_ATMO_BLE_ConnectedHandler( 1 );
}

static void _ATMO_BLE2_ConnectedHandler( void *data )
{
	_ATMO_BLE_ConnectedHandler( 2 );
}

static void _ATMO_BLE3_ConnectedHandler( void *data )
{
	_ATMO_BLE_ConnectedHandler( 3 );
}

static ATMO_Callback_t _ATMO_BLE_ConnectedHandlers[NUMBER_OF_BLE_DRIVER_INSTANCES] = {_ATMO_BLE0_ConnectedHandler, _ATMO_BLE1_ConnectedHandler, _ATMO_BLE2_ConnectedHandler, _ATMO_BLE3_ConnectedHandler};
#else
#define NUMBER_OF_BLE_DRIVER_INSTANCES 1
static ATMO_Callback_t _ATMO_BLE_ConnectedHandlers[NUMBER_OF_BLE_DRIVER_INSTANCES] = {_ATMO_BLE0_ConnectedHandler};
#endif

static ATMO_DriverInstanceHandle_t _ATMO_BLE_NumInstances = 0;

ATMO_BLE_Instance_t _ATMO_BLE_Instances[NUMBER_OF_BLE_DRIVER_INSTANCES];

static void _ATMO_BLE_ConnectedHandler( ATMO_DriverInstanceHandle_t instance )
{
#ifndef ATMO_BLE_NO_SERVICE_CHANGED
	uint8_t handles[4] = {0x01, 0x00, 0xFF, 0xFF};
	ATMO_BLE_GATTSSetCharacteristic( instance, _ATMO_BLE_Instances[instance].serviceChangedCharHandle, 4, handles, NULL );
#endif
}

static ATMO_BLE_Status_t _ATMO_BLE_AddGattServiceChanged( ATMO_DriverInstanceHandle_t instance )
{
	static ATMO_BOOL_t serviceChangedAdded = false;

#ifndef ATMO_BLE_NO_SERVICE_CHANGED
	if ( !serviceChangedAdded )
	{
		ATMO_BLE_Status_t status = ATMO_BLE_GATTSAddService( instance, &_ATMO_BLE_Instances[instance].gattServiceHandle, "1801" );

		if ( status == ATMO_BLE_Status_Success )
		{
			status = ATMO_BLE_GATTSAddCharacteristic( instance, &_ATMO_BLE_Instances[instance].serviceChangedCharHandle, _ATMO_BLE_Instances[instance].gattServiceHandle, "2A05",
			         ATMO_BLE_Property_Read | ATMO_BLE_Property_Notify, ATMO_BLE_Permission_Read | ATMO_BLE_Permission_Write, 4 );
		}

		// Some platforms won't let us set this during early init, and that's OK
		// That's the reason there's no error code check here
		ATMO_BLE_SetServicesChanged( instance );

		serviceChangedAdded = true;
		return status;
	}
#endif

	return ATMO_BLE_Status_Success;
}

ATMO_Status_t ATMO_BLE_AddDriverInstance( const ATMO_BLE_DriverInstance_t *driverInstance, ATMO_DriverInstanceData_t *driverInstanceData, ATMO_DriverInstanceHandle_t *instanceNumber )
{
	if ( !( _ATMO_BLE_NumInstances < NUMBER_OF_BLE_DRIVER_INSTANCES ) )
	{
		return ATMO_Status_OutOfMemory;
	}

	_ATMO_BLE_Instances[_ATMO_BLE_NumInstances].instance = driverInstance;
	_ATMO_BLE_Instances[_ATMO_BLE_NumInstances].instanceData = driverInstanceData;
	_ATMO_BLE_Instances[_ATMO_BLE_NumInstances].connectedCb = _ATMO_BLE_ConnectedHandlers[_ATMO_BLE_NumInstances];
	*instanceNumber = _ATMO_BLE_NumInstances;

	_ATMO_BLE_NumInstances++;

	return ATMO_Status_Success;
}

ATMO_BLE_Status_t ATMO_BLE_PeripheralInit( ATMO_DriverInstanceHandle_t instance )
{
	if ( !( instance < _ATMO_BLE_NumInstances ) )
	{
		return ATMO_BLE_Status_Invalid;
	}

	ATMO_BLE_Status_t status = _ATMO_BLE_Instances[instance].instance->PeripheralInit( _ATMO_BLE_Instances[instance].instanceData );

	if ( status != ATMO_BLE_Status_Success )
	{
		return status;
	}

	status = _ATMO_BLE_AddGattServiceChanged( instance );

	if ( status != ATMO_BLE_Status_Success )
	{
		return status;
	}

	status = ATMO_BLE_RegisterEventCallback( instance, ATMO_BLE_EVENT_Connected, _ATMO_BLE_Instances[instance].connectedCb );

	return status;
}

ATMO_BLE_Status_t ATMO_BLE_SetEnabled( ATMO_DriverInstanceHandle_t instance, ATMO_BOOL_t enabled )
{
	if ( !( instance < _ATMO_BLE_NumInstances ) )
	{
		return ATMO_BLE_Status_Invalid;
	}

	return _ATMO_BLE_Instances[instance].instance->SetEnabled( _ATMO_BLE_Instances[instance].instanceData, enabled );
}

ATMO_BLE_Status_t ATMO_BLE_GetEnabled( ATMO_DriverInstanceHandle_t instance, ATMO_BOOL_t *enabled )
{
	if ( !( instance < _ATMO_BLE_NumInstances ) )
	{
		return ATMO_BLE_Status_Invalid;
	}

	return _ATMO_BLE_Instances[instance].instance->GetEnabled( _ATMO_BLE_Instances[instance].instanceData, enabled );
}

ATMO_BLE_Status_t ATMO_BLE_PeripheralDeInit( ATMO_DriverInstanceHandle_t instance )
{
	if ( !( instance < _ATMO_BLE_NumInstances ) )
	{
		return ATMO_BLE_Status_Invalid;
	}

	return _ATMO_BLE_Instances[instance].instance->PeripheralDeInit( _ATMO_BLE_Instances[instance].instanceData );
}

ATMO_BLE_Status_t ATMO_BLE_GetMacAddress( ATMO_DriverInstanceHandle_t instance, ATMO_BLE_MacAddress_t *address )
{
	if ( !( instance < _ATMO_BLE_NumInstances ) )
	{
		return ATMO_BLE_Status_Invalid;
	}

	return _ATMO_BLE_Instances[instance].instance->GetMacAddress( _ATMO_BLE_Instances[instance].instanceData, address );
}

ATMO_BLE_Status_t ATMO_BLE_GAPSetDeviceName( ATMO_DriverInstanceHandle_t instance, const char *name )
{
	if ( !( instance < _ATMO_BLE_NumInstances ) )
	{
		return ATMO_BLE_Status_Invalid;
	}

	return _ATMO_BLE_Instances[instance].instance->GAPSetDeviceName( _ATMO_BLE_Instances[instance].instanceData, name );
}

ATMO_BLE_Status_t ATMO_BLE_GAPAdvertisingStart( ATMO_DriverInstanceHandle_t instance, ATMO_BLE_AdvertisingParams_t *params )
{
	if ( !( instance < _ATMO_BLE_NumInstances ) )
	{
		return ATMO_BLE_Status_Invalid;
	}

	return _ATMO_BLE_Instances[instance].instance->GAPAdvertisingStart( _ATMO_BLE_Instances[instance].instanceData, params );
}

ATMO_BLE_Status_t ATMO_BLE_GAPAdvertisingStop( ATMO_DriverInstanceHandle_t instance )
{
	if ( !( instance < _ATMO_BLE_NumInstances ) )
	{
		return ATMO_BLE_Status_Invalid;
	}

	return _ATMO_BLE_Instances[instance].instance->GAPAdvertisingStop( _ATMO_BLE_Instances[instance].instanceData );
}

ATMO_BLE_Status_t ATMO_BLE_GAPSetAdvertisedServiceUUID( ATMO_DriverInstanceHandle_t instance, const char *uuid )
{
	if ( !( instance < _ATMO_BLE_NumInstances ) )
	{
		return ATMO_BLE_Status_Invalid;
	}

	return _ATMO_BLE_Instances[instance].instance->GAPSetAdvertisedServiceUUID( _ATMO_BLE_Instances[instance].instanceData, uuid );
}

ATMO_BLE_Status_t ATMO_BLE_GAPAdverertisingSetManufacturerData( ATMO_DriverInstanceHandle_t instance, ATMO_BLE_AdvertisingData_t *data )
{
	if ( !( instance < _ATMO_BLE_NumInstances ) )
	{
		return ATMO_BLE_Status_Invalid;
	}

	return _ATMO_BLE_Instances[instance].instance->GAPAdverertisingSetManufacturerData( _ATMO_BLE_Instances[instance].instanceData, data );
}

ATMO_BLE_Status_t ATMO_BLE_GAPPairingCfg( ATMO_DriverInstanceHandle_t instance, ATMO_BLE_PairingCfg_t *config )
{
	if ( !( instance < _ATMO_BLE_NumInstances ) )
	{
		return ATMO_BLE_Status_Invalid;
	}

	return _ATMO_BLE_Instances[instance].instance->GAPPairingCfg( _ATMO_BLE_Instances[instance].instanceData, config );
}

ATMO_BLE_Status_t ATMO_BLE_GAPDisconnect( ATMO_DriverInstanceHandle_t instance )
{
	if ( !( instance < _ATMO_BLE_NumInstances ) )
	{
		return ATMO_BLE_Status_Invalid;
	}

	return _ATMO_BLE_Instances[instance].instance->GAPDisconnect( _ATMO_BLE_Instances[instance].instanceData );
}

ATMO_BLE_Status_t ATMO_BLE_GATTSAddService( ATMO_DriverInstanceHandle_t instance, ATMO_BLE_Handle_t *handle,
        const char *serviceUUID )
{
	if ( !( instance < _ATMO_BLE_NumInstances ) )
	{
		return ATMO_BLE_Status_Invalid;
	}

	return _ATMO_BLE_Instances[instance].instance->GATTSAddService( _ATMO_BLE_Instances[instance].instanceData, handle, serviceUUID );
}

ATMO_BLE_Status_t ATMO_BLE_GATTSAddCharacteristic( ATMO_DriverInstanceHandle_t instance, ATMO_BLE_Handle_t *handle,
        ATMO_BLE_Handle_t serviceHandle, const char *characteristicUUID, uint8_t properties, uint8_t permissions, uint32_t maxLen )
{
	if ( !( instance < _ATMO_BLE_NumInstances ) )
	{
		return ATMO_BLE_Status_Invalid;
	}

	if ( maxLen == 0 )
	{
		maxLen = 1;
	}

	return _ATMO_BLE_Instances[instance].instance->GATTSAddCharacteristic( _ATMO_BLE_Instances[instance].instanceData, handle, serviceHandle, characteristicUUID, properties, permissions, maxLen );
}

ATMO_BLE_Status_t ATMO_BLE_GATTSGetCharacteristicValue( ATMO_DriverInstanceHandle_t instance, ATMO_BLE_Handle_t handle,
        uint8_t *valueBuf, uint32_t valueBufLen, uint32_t *valueLen )
{
	if ( !( instance < _ATMO_BLE_NumInstances ) )
	{
		return ATMO_BLE_Status_Invalid;
	}

	return _ATMO_BLE_Instances[instance].instance->GATTSGetCharacteristicValue( _ATMO_BLE_Instances[instance].instanceData, handle, valueBuf, valueBufLen, valueLen );
}

ATMO_BLE_Status_t ATMO_BLE_GATTSRegisterCharacteristicCallback( ATMO_DriverInstanceHandle_t instance, ATMO_BLE_Handle_t handle, ATMO_BLE_Characteristic_Event_t event, ATMO_Callback_t cbFunc )
{
	if ( !( instance < _ATMO_BLE_NumInstances ) )
	{
		return ATMO_BLE_Status_Invalid;
	}

	return _ATMO_BLE_Instances[instance].instance->GATTSRegisterCharacteristicCallback( _ATMO_BLE_Instances[instance].instanceData, handle, event, cbFunc );
}

ATMO_BLE_Status_t ATMO_BLE_GATTSRegisterCharacteristicAbilityHandle( ATMO_DriverInstanceHandle_t instance, ATMO_BLE_Handle_t handle,
        ATMO_BLE_Characteristic_Event_t event, unsigned int abilityHandler )
{
	if ( !( instance < _ATMO_BLE_NumInstances ) )
	{
		return ATMO_BLE_Status_Invalid;
	}

	return _ATMO_BLE_Instances[instance].instance->GATTSRegisterCharacteristicAbilityHandle( _ATMO_BLE_Instances[instance].instanceData, handle, event, abilityHandler );
}

ATMO_BLE_Status_t ATMO_BLE_GATTSSetCharacteristic( ATMO_DriverInstanceHandle_t instance, ATMO_BLE_Handle_t handle,
        uint16_t length, uint8_t *value, ATMO_BLE_CharProperties_t *properties )
{
	if ( !( instance < _ATMO_BLE_NumInstances ) )
	{
		return ATMO_BLE_Status_Invalid;
	}

	return _ATMO_BLE_Instances[instance].instance->GATTSSetCharacteristic( _ATMO_BLE_Instances[instance].instanceData, handle, length, value, properties );
}

ATMO_BLE_Status_t ATMO_BLE_GATTSWriteDescriptor( ATMO_DriverInstanceHandle_t instance, ATMO_BLE_Handle_t handle,
        uint16_t length, uint8_t *value, ATMO_BLE_CharProperties_t *properties )
{
	if ( !( instance < _ATMO_BLE_NumInstances ) )
	{
		return ATMO_BLE_Status_Invalid;
	}

	return _ATMO_BLE_Instances[instance].instance->GATTSWriteDescriptor( _ATMO_BLE_Instances[instance].instanceData, handle, length, value, properties );
}

ATMO_BLE_Status_t ATMO_BLE_GATTSSendIndicate( ATMO_DriverInstanceHandle_t instance, ATMO_BLE_Handle_t handle, uint16_t size,
        uint8_t *value )
{
	if ( !( instance < _ATMO_BLE_NumInstances ) )
	{
		return ATMO_BLE_Status_Invalid;
	}

	return _ATMO_BLE_Instances[instance].instance->GATTSSendIndicate( _ATMO_BLE_Instances[instance].instanceData, handle, size, value );
}

ATMO_BLE_Status_t ATMO_BLE_GATTSSendNotify( ATMO_DriverInstanceHandle_t instance, ATMO_BLE_Handle_t handle, uint16_t size, uint8_t *value )
{
	if ( !( instance < _ATMO_BLE_NumInstances ) )
	{
		return ATMO_BLE_Status_Invalid;
	}

	return _ATMO_BLE_Instances[instance].instance->GATTSSendNotify( _ATMO_BLE_Instances[instance].instanceData, handle, size, value );
}

ATMO_BLE_Status_t ATMO_BLE_SetServicesChanged( ATMO_DriverInstanceHandle_t instance )
{
	if ( !( instance < _ATMO_BLE_NumInstances ) )
	{
		return ATMO_BLE_Status_Invalid;
	}
#ifndef ATMO_BLE_NO_SERVICE_CHANGED
	// All possible services
	uint8_t handles[4] = {0x01, 0x00, 0xFF, 0xFF};

	// This should automatically send a notification since they are enabled for this characteristic
	return ATMO_BLE_GATTSSetCharacteristic( instance, _ATMO_BLE_Instances[instance].serviceChangedCharHandle, 4, handles, NULL );
#else
	return ATMO_BLE_Status_Success;
#endif
}

ATMO_BLE_Status_t ATMO_BLE_RegisterEventCallback( ATMO_DriverInstanceHandle_t instance, ATMO_BLE_Event_t event, ATMO_Callback_t cb )
{
	if ( !( instance < _ATMO_BLE_NumInstances ) || ( event >= ATMO_BLE_EVENT_NumEvents ) )
	{
		return ATMO_BLE_Status_Invalid;
	}

	return _ATMO_BLE_Instances[instance].instance->RegisterEventCallback( _ATMO_BLE_Instances[instance].instanceData, event, cb );
}

ATMO_BLE_Status_t ATMO_BLE_RegisterEventAbilityHandle( ATMO_DriverInstanceHandle_t instance, ATMO_BLE_Event_t event, unsigned int abilityHandle )
{
	if ( !( instance < _ATMO_BLE_NumInstances ) || ( event >= ATMO_BLE_EVENT_NumEvents ) )
	{
		return ATMO_BLE_Status_Invalid;
	}

	return _ATMO_BLE_Instances[instance].instance->RegisterEventAbilityHandle( _ATMO_BLE_Instances[instance].instanceData, event, abilityHandle );
}
