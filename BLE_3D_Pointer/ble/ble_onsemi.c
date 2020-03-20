#include "ble_onsemi.h"
#include "ble_onsemi_db.h"
#include <BDK_Task.h>
#include <HAL_error.h>
#include <app_trace.h>
#include "HAL.h"

const ATMO_BLE_DriverInstance_t onsemiBleDriverInstance =
{
	ATMO_ONSEMI_BLE_PeripheralInit,
	ATMO_ONSEMI_BLE_PeripheralDeInit,
	ATMO_ONSEMI_BLE_SetEnabled,
	ATMO_ONSEMI_BLE_GetEnabled,
	ATMO_ONSEMI_BLE_GetMacAddress,
	ATMO_ONSEMI_BLE_GAPSetDeviceName,
	ATMO_ONSEMI_BLE_GAPAdvertisingStart,
	ATMO_ONSEMI_BLE_GAPAdvertisingStop,
	ATMO_ONSEMI_BLE_GAPSetAdvertisedServiceUUID,
	ATMO_ONSEMI_BLE_GAPAdverertisingSetManufacturerData,
	ATMO_ONSEMI_BLE_GAPPairingCfg,
	ATMO_ONSEMI_BLE_GAPDisconnect,
	ATMO_ONSEMI_BLE_GATTSAddService,
	ATMO_ONSEMI_BLE_GATTSAddCharacteristic,
	ATMO_ONSEMI_BLE_GATTSGetCharacteristicValue,
	ATMO_ONSEMI_BLE_GATTSRegisterCharacteristicCallback,
	ATMO_ONSEMI_BLE_GATTSRegisterCharacteristicAbilityHandle,
	ATMO_ONSEMI_BLE_GATTSSetCharacteristic,
	ATMO_ONSEMI_BLE_GATTSWriteDescriptor,
	ATMO_ONSEMI_BLE_GATTSSendIndicate,
	ATMO_ONSEMI_BLE_GATTSSendNotify,
	ATMO_ONSEMI_BLE_RegisterEventCallback,
	ATMO_ONSEMI_BLE_RegisterEventAbilityHandle
};

static ATMO_Callback_t _ATMO_ONSEMI_BLE_EventCallbacks[ATMO_BLE_EVENT_NumEvents][ATMO_ONSEMI_BLE_MAX_ABILITIES_PER_EVENT];
static uint8_t _ATMO_ONSEMI_BLE_NumEventCallbacks[ATMO_BLE_EVENT_NumEvents] = {0};

static ATMO_AbilityHandle_t _ATMO_ONSEMI_BLE_EventAbilities[ATMO_BLE_EVENT_NumEvents][ATMO_ONSEMI_BLE_MAX_ABILITIES_PER_EVENT];
static uint8_t _ATMO_ONSEMI_BLE_NumEventAbilities[ATMO_BLE_EVENT_NumEvents] = {0};

static char _ATMO_ONSEMI_BLE_LocalName[6] = {0};
static ATMO_UUID_t _ATMO_ONSEMI_BLE_PrimaryUuid;

static bool _ATMO_ONSEMI_BLE_Advertising = false;
static bool _ATMO_ONSEMI_BLE_InitComplete = false;

static bool _ATMO_ONSEMI_Connected = false;

ATMO_Status_t ATMO_ONSEMI_BLE_AddDriverInstance( ATMO_DriverInstanceHandle_t *instanceNumber )
{
	static ATMO_DriverInstanceData_t driverInstanceData;

	driverInstanceData.name = "ONSEMI BLE";
	driverInstanceData.initialized = false;
	driverInstanceData.instanceNumber = 0;
	driverInstanceData.argument = NULL;

	return ATMO_BLE_AddDriverInstance( &onsemiBleDriverInstance, &driverInstanceData, instanceNumber );
}

static void _ATMO_BLE_ONSEMI_ServiceAdd( void )
{
	static unsigned int currentService = 0;

	if ( currentService == _ATMO_ONSEMI_BLE_NumServices )
	{
		BDK_BLE_ProfileAddedInd();
		return;
	}

	ATMO_PLATFORM_DebugPrint( "Adding service %d with %d characteristics\r\n", currentService, _ATMO_ONSEMI_BLE_Services[currentService].numCharacteristics );

	struct gattm_add_svc_req *req;


	req = KE_MSG_ALLOC_DYN( GATTM_ADD_SVC_REQ, TASK_GATTM, TASK_APP,
	                        gattm_add_svc_req, _ATMO_ONSEMI_BLE_Services[currentService].numCharacteristics * 3 * sizeof( struct gattm_att_desc ) );
	req->svc_desc.start_hdl = _ATMO_ONSEMI_BLE_Services[currentService].startHandle;
	req->svc_desc.task_id = TASK_APP;
	req->svc_desc.perm = PERM( SVC_UUID_LEN, UUID_128 );
	req->svc_desc.nb_att =  _ATMO_ONSEMI_BLE_Services[currentService].numCharacteristics * 3;
	memcpy( req->svc_desc.uuid,  _ATMO_ONSEMI_BLE_Services[currentService].uuid.data, ATT_UUID_128_LEN );
	memcpy( req->svc_desc.atts, _ATMO_ONSEMI_BLE_Services[currentService].characteristics,
	        _ATMO_ONSEMI_BLE_Services[currentService].numCharacteristics * 3 * sizeof( struct gattm_att_desc ) );

	ke_msg_send( req );

	currentService++;

}

static void _ATMO_BLE_ONSEMI_AddServiceResp( ke_msg_id_t const msg_id,
        struct gattm_add_svc_rsp const *param, ke_task_id_t const dest_id,
        ke_task_id_t const src_id )
{
	ATMO_PLATFORM_DebugPrint( "Add response: %d\r\n", param->status );

	// Add the next service
	_ATMO_BLE_ONSEMI_ServiceAdd();
}

static void _ATMO_BLE_ONSEMI_Enable( uint8_t conidx )
{

}

static _ATMO_ONSEMI_BLE_Characteristic_t *_ATMO_BLE_ONSEMI_GetCharFromHandle( uint32_t handle )
{
	// Find characteristic in table
	for ( unsigned int i = 0; i < _ATMO_ONSEMI_BLE_NumServices; i++ )
	{
		for ( unsigned int j = 0; j < _ATMO_ONSEMI_BLE_Services[i].numCharacteristics; j++ )
		{
			if ( _ATMO_ONSEMI_BLE_Services[i].characteristicDesc[j].handle == handle )
			{
				return &_ATMO_ONSEMI_BLE_Services[i].characteristicDesc[j];
			}
		}
	}

	return NULL;
}

static void _ATMO_BLE_ONSEMI_DispatchCharEvent( ATMO_BLE_Characteristic_Event_t event, _ATMO_ONSEMI_BLE_Characteristic_t *characteristic, ATMO_Value_t *data )
{
	for ( unsigned int i = 0; i < characteristic->numAbilities[event]; i++ )
	{
		ATMO_AddAbilityExecute( characteristic->ability[event][i], data );
	}

	for ( unsigned int i = 0; i < characteristic->numCallbacks[event]; i++ )
	{
		ATMO_AddCallbackExecute( characteristic->callbacks[event][i], data );
	}
}

static int _ATMO_BLE_ONSEMI_ReadReqInd( ke_msg_id_t const msg_id,
                                        struct gattc_read_req_ind const *param, ke_task_id_t const dest_id,
                                        ke_task_id_t const src_id )
{
	uint8_t status = GAP_ERR_NO_ERROR;
	uint8_t *val_ptr = NULL;
	uint8_t val_len = 0;
	uint16_t att_num = 0;
	struct gattc_read_cfm *cfm;

	int conidx = BDK_BLE_GetConIdx();

	if ( conidx == INVALID_DEV_IDX )
	{
		return KE_MSG_CONSUMED;
	}

	att_num = param->handle - 1;

	_ATMO_ONSEMI_BLE_Characteristic_t *characteristic = _ATMO_BLE_ONSEMI_GetCharFromHandle( att_num );

	if ( characteristic == NULL )
	{
		status = ATT_ERR_INVALID_HANDLE;
	}

	if ( status == GAP_ERR_NO_ERROR )
	{
		val_ptr = characteristic->data;
		val_len = characteristic->currentLength;
	}

	cfm = KE_MSG_ALLOC_DYN( GATTC_READ_CFM, KE_BUILD_ID( TASK_GATTC, conidx ),
	                        TASK_APP, gattc_read_cfm, val_len )
	      ;

	if ( val_ptr != NULL )
	{
		memcpy( cfm->value, val_ptr, val_len );
	}

	cfm->handle = param->handle;
	cfm->length = val_len;
	cfm->status = status;

	ke_msg_send( cfm );

	return KE_MSG_CONSUMED;
}

static int _ATMO_BLE_ONSEMI_WriteReqInd( ke_msg_id_t const msg_id,
        struct gattc_write_req_ind const *param, ke_task_id_t const dest_id,
        ke_task_id_t const src_id )
{
	uint8_t status = GAP_ERR_NO_ERROR;
	uint16_t att_num = 0;
	int conidx = BDK_BLE_GetConIdx();
	struct gattc_write_cfm *cfm;

	/* Check if connection is valid. */
	if ( conidx == INVALID_DEV_IDX )
	{
		return KE_MSG_CONSUMED;
	}

	/* Check that offset is valid */
	if ( param->offset != 0 )
	{
		status = ATT_ERR_INVALID_OFFSET;
	}

	att_num = param->handle - 1;

	ATMO_PLATFORM_DebugPrint( "Write request for handle %d\r\n", att_num );

	_ATMO_ONSEMI_BLE_Characteristic_t *characteristic = _ATMO_BLE_ONSEMI_GetCharFromHandle( att_num );

	if ( characteristic == NULL )
	{
		ATMO_PLATFORM_DebugPrint("Unable to find characteristic matching handle %d\r\n", att_num);
		status = ATT_ERR_INVALID_HANDLE;
	}

	if ( status == GAP_ERR_NO_ERROR )
	{
		if ( param->length > characteristic->maxLength )
		{
			status = ATT_ERR_INVALID_ATTRIBUTE_VAL_LEN;
		}
		else
		{
			ATMO_PLATFORM_DebugPrint("Dispatching write event length %d...\r\n", param->length);
			memcpy( characteristic->data, param->value, param->length );
			characteristic->currentLength = param->length;

			ATMO_Value_t atmoVal;
			ATMO_InitValue( &atmoVal );
			ATMO_CreateValueBinary( &atmoVal, param->value, param->length );
			_ATMO_BLE_ONSEMI_DispatchCharEvent( ATMO_BLE_Characteristic_Written, characteristic, &atmoVal );
			ATMO_FreeValue( &atmoVal );
		}
	}


	cfm = KE_MSG_ALLOC( GATTC_WRITE_CFM, KE_BUILD_ID( TASK_GATTC, conidx ),
	                    TASK_APP, gattc_write_cfm );

	cfm->handle = param->handle;
	cfm->status = status;

	ke_msg_send( cfm );

	return KE_MSG_CONSUMED;
}

static int _ATMO_BLE_ONSEMI_AttInfoReqInd( ke_msg_id_t const msg_id,
        struct gattc_read_req_ind const *param, ke_task_id_t const dest_id,
        ke_task_id_t const src_id )
{

}

static int _ATMO_BLE_ONSEMI_CmpEvt( ke_msg_id_t const msg_id,
                                    struct gattc_cmp_evt const *param, ke_task_id_t const dest_id,
                                    ke_task_id_t const src_id )
{
	return KE_MSG_CONSUMED;
}

void ATMO_ONSEMI_BLE_SyncDb()
{
	BDK_TaskAddMsgHandler( GATTM_ADD_SVC_RSP,
	                       ( ke_msg_func_t ) &_ATMO_BLE_ONSEMI_AddServiceResp );
	BDK_TaskAddMsgHandler( GATTC_READ_REQ_IND,
	                       ( ke_msg_func_t ) &_ATMO_BLE_ONSEMI_ReadReqInd );
	BDK_TaskAddMsgHandler( GATTC_WRITE_REQ_IND,
	                       ( ke_msg_func_t ) &_ATMO_BLE_ONSEMI_WriteReqInd );
	BDK_TaskAddMsgHandler( GATTC_ATT_INFO_REQ_IND,
	                       ( ke_msg_func_t ) &_ATMO_BLE_ONSEMI_AttInfoReqInd );
	BDK_TaskAddMsgHandler( GATTC_CMP_EVT,
	                       ( ke_msg_func_t ) &_ATMO_BLE_ONSEMI_CmpEvt );

	BDK_BLE_AddService( &_ATMO_BLE_ONSEMI_ServiceAdd, &_ATMO_BLE_ONSEMI_Enable );
}

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_PeripheralInit( ATMO_DriverInstanceData_t *instance )
{
	return ATMO_BLE_Status_Success;
}

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_SetEnabled( ATMO_DriverInstanceData_t *instance, bool enabled )
{
	return ATMO_BLE_Status_NotSupported;
}

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_GetEnabled( ATMO_DriverInstanceData_t *instance, bool *enabled )
{
	return ATMO_BLE_Status_NotSupported;
}

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_PeripheralDeInit( ATMO_DriverInstanceData_t *instance )
{
	return ATMO_BLE_Status_Success;
}

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_GetMacAddress( ATMO_DriverInstanceData_t *instance, ATMO_BLE_MacAddress_t *address )
{
	Device_Param_Read( PARAM_ID_PUBLIC_BLE_ADDRESS, address->data );
	return ATMO_BLE_Status_Success;
}

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_GAPSetDeviceName( ATMO_DriverInstanceData_t *instance, const char *name )
{
	strncpy( _ATMO_ONSEMI_BLE_LocalName, name, sizeof( _ATMO_ONSEMI_BLE_LocalName ) );
	return ATMO_BLE_Status_Success;
}

static ATMO_BLE_Status_t _ATMO_ONSEMI_BLE_MakeAdvPackage( uint8_t *buf, uint8_t *length )
{
	unsigned int i;
	*length = 0;

	buf[( *length )++] = 0x02;
	buf[( *length )++] = ATMO_BLE_AD_TYPE_FLAG;
	buf[( *length )++] = 0x06;


	buf[( *length )++] = 0x11;
	buf[( *length )++] = ATMO_BLE_AD_TYPE_128SRV_PART;

	for ( i = 0; i < 16; i++ )
	{
		buf[( *length )++] = _ATMO_ONSEMI_BLE_PrimaryUuid.data[i];
	}

	size_t nameLen = strlen( _ATMO_ONSEMI_BLE_LocalName );

	buf[( *length )++] = nameLen + 1;
	buf[( *length )++] = ATMO_BLE_AD_TYPE_NAME_SHORT;

	for ( i = 0; i < nameLen; i++ )
	{
		buf[( *length )++] = _ATMO_ONSEMI_BLE_LocalName[i];
	}

	return ATMO_BLE_Status_Success;
}

ATMO_BLE_Status_t _ATMO_ONSEMI_BLE_SetInitComplete()
{
	if(_ATMO_ONSEMI_BLE_InitComplete == true)
	{
		return ATMO_BLE_Status_Success;
	}

	_ATMO_ONSEMI_BLE_InitComplete = true;
	ATMO_ONSEMI_BLE_GAPAdvertisingStart(NULL, NULL);
	return ATMO_BLE_Status_Success;
}

ATMO_BLE_Status_t _ATMO_ONSEMI_BLE_GAPAdvertisingStartPriv( ATMO_BLE_AdvertisingParams_t *params )
{
	ATMO_PLATFORM_DebugPrint( "STARTING ADV\r\n" );

	/* Prepare the GAPM_START_ADVERTISE_CMD message */
	struct gapm_start_advertise_cmd *cmd;

	/* Prepare the start advertisement command message */
	cmd = KE_MSG_ALLOC( GAPM_START_ADVERTISE_CMD, TASK_GAPM, TASK_APP,
	                    gapm_start_advertise_cmd );
	cmd->op.addr_src = GAPM_STATIC_ADDR;
	cmd->channel_map = ADV_ALL_CHNLS_EN;

	cmd->intv_min = 64;
	cmd->intv_max = 64;

	cmd->op.code = GAPM_ADV_UNDIRECT;
	cmd->op.state = 0;
	cmd->info.host.mode = GAP_GEN_DISCOVERABLE;
	cmd->info.host.adv_filt_policy = ADV_ALLOW_SCAN_ANY_CON_ANY;

	/* Set advertisement packet data (Complete Local Name). */
	uint32_t currentIndex = 0;
	cmd->info.host.adv_data[currentIndex++] = 1 + strlen( _ATMO_ONSEMI_BLE_LocalName );
	cmd->info.host.adv_data[currentIndex++] = GAP_AD_TYPE_COMPLETE_NAME;
	memcpy( &cmd->info.host.adv_data[currentIndex], _ATMO_ONSEMI_BLE_LocalName, strlen( _ATMO_ONSEMI_BLE_LocalName ) );
	currentIndex += strlen( _ATMO_ONSEMI_BLE_LocalName );

	cmd->info.host.adv_data[currentIndex++] = 17;
	cmd->info.host.adv_data[currentIndex++] = GAP_AD_TYPE_MORE_128_BIT_UUID;
	memcpy( &cmd->info.host.adv_data[currentIndex], _ATMO_ONSEMI_BLE_PrimaryUuid.data, 16 );
	currentIndex += 16;
	cmd->info.host.adv_data_len = currentIndex;

	/* Set Scan Response packet data (Manufacturer Specific Data). */
	cmd->info.host.scan_rsp_data_len = 0;

	/* Send the message */
	ke_msg_send( cmd );
	return ATMO_BLE_Status_Success;
}

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_GAPAdvertisingStart( ATMO_DriverInstanceData_t *instance, ATMO_BLE_AdvertisingParams_t *params )
{
	if(!_ATMO_ONSEMI_BLE_InitComplete)
	{
		return ATMO_BLE_Status_Success;
	}

	if ( _ATMO_ONSEMI_BLE_Advertising )
	{
		// Stop first
		ATMO_ONSEMI_BLE_GAPAdvertisingStop(NULL);
		return ATMO_BLE_Status_Success;
	}

	_ATMO_ONSEMI_BLE_GAPAdvertisingStartPriv( params );

	_ATMO_ONSEMI_BLE_Advertising = true;
	return ATMO_BLE_Status_Success;
}

ATMO_BLE_Status_t _ATMO_ONSEMI_BLE_GAPAdvertisingStopPriv( )
{
	struct gapm_cancel_cmd *cmd;

	cmd = KE_MSG_ALLOC( GAPM_CANCEL_CMD, TASK_GAPM, TASK_APP, gapm_cancel_cmd );

	cmd->operation = GAPM_CANCEL;

	ke_msg_send( cmd );
	return ATMO_BLE_Status_Success;
}

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_GAPAdvertisingStop( ATMO_DriverInstanceData_t *instance )
{
	_ATMO_ONSEMI_BLE_Advertising = false;
	return ATMO_BLE_Status_Success;

	if(!_ATMO_ONSEMI_BLE_Advertising)
	{
		return ATMO_BLE_Status_Success; 
	}

	BDK_BLE_AdvertisingStop();
	_ATMO_ONSEMI_BLE_Advertising = false;
	return ATMO_BLE_Status_Success;
}

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_GAPSetAdvertisedServiceUUID( ATMO_DriverInstanceData_t *instance, const char *uuid )
{
	ATMO_PLATFORM_DebugPrint( "Converting stringto uuid\r\n" );
	ATMO_StringToUuid( uuid, &_ATMO_ONSEMI_BLE_PrimaryUuid, ATMO_ENDIAN_Type_Little );
	return ATMO_BLE_Status_Success;
}

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_GAPAdverertisingSetManufacturerData( ATMO_DriverInstanceData_t *instance, ATMO_BLE_AdvertisingData_t *data )
{
	return ATMO_BLE_Status_NotSupported;
}

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_GAPPairingCfg( ATMO_DriverInstanceData_t *instance, ATMO_BLE_PairingCfg_t *config )
{
	return ATMO_BLE_Status_Success;
}

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_GAPDisconnect( ATMO_DriverInstanceData_t *instance )
{
	return ATMO_BLE_Status_NotSupported;
}


ATMO_BLE_Status_t ATMO_ONSEMI_BLE_GATTSAddService( ATMO_DriverInstanceData_t *instance, ATMO_BLE_Handle_t *handle, const char *serviceUUID )
{
	ATMO_PLATFORM_DebugPrint( "Adding service %s\r\n", serviceUUID );
	ATMO_UUID_t uuid;
	ATMO_StringToUuid( serviceUUID, &uuid, ATMO_ENDIAN_Type_Little );

	// Find the service in the table
	for ( unsigned int i = 0; i < _ATMO_ONSEMI_BLE_NumServices; i++ )
	{
		if ( memcmp( &_ATMO_ONSEMI_BLE_Services[i].uuid, &uuid, sizeof( uuid ) ) == 0 )
		{
			*handle = _ATMO_ONSEMI_BLE_Services[i].startHandle;
			ATMO_PLATFORM_DebugPrint( "Found service. Handle: %d\r\n", *handle );
			return ATMO_BLE_Status_Success;
		}
	}

	return ATMO_BLE_Status_Fail;
}

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_GATTSAddCharacteristic( ATMO_DriverInstanceData_t *instance, ATMO_BLE_Handle_t *handle, ATMO_BLE_Handle_t serviceHandle, const char *characteristicUUID, uint8_t properties, uint8_t permissions, uint32_t maxLen )
{
	ATMO_PLATFORM_DebugPrint( "Adding characteristic %s\r\n", characteristicUUID );
	ATMO_UUID_t uuid;
	ATMO_StringToUuid( characteristicUUID, &uuid, ATMO_ENDIAN_Type_Little );

	// Find characteristic in table
	for ( unsigned int i = 0; i < _ATMO_ONSEMI_BLE_NumServices; i++ )
	{
		for ( unsigned int j = 0; j < _ATMO_ONSEMI_BLE_Services[i].numCharacteristics; j++ )
		{
			ATMO_PLATFORM_DebugPrint( "Checking service %d char %d\r\n", i, j );

			// Every characteristic is 3 entries -> declaration, characteristic, ccd
			// Need to index to the characteristic entry for every j
			uint8_t index = ( j * 3 ) + 1;
			struct gattm_att_desc *characteristic = &_ATMO_ONSEMI_BLE_Services[i].characteristics[index];

			if ( memcmp( characteristic->uuid, uuid.data, 16 ) == 0 )
			{
				*handle = serviceHandle + index;
				ATMO_PLATFORM_DebugPrint( "Found Characteristic. Handle: %d\r\n", *handle );
				return ATMO_BLE_Status_Success;
			}
		}
	}

	ATMO_PLATFORM_DebugPrint( "Did not find characteristic %s\r\n", characteristicUUID );
	return ATMO_BLE_Status_Fail;
}

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_GATTSGetCharacteristicValue( ATMO_DriverInstanceData_t *instance, ATMO_BLE_Handle_t handle, uint8_t *valueBuf, uint32_t valueBufLen, uint32_t *valueLen )
{
	return ATMO_BLE_Status_NotSupported;
}

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_GATTSRegisterCharacteristicCallback( ATMO_DriverInstanceData_t *instance,  ATMO_BLE_Handle_t handle, ATMO_BLE_Characteristic_Event_t event, ATMO_Callback_t cbFunc )
{
	_ATMO_ONSEMI_BLE_Characteristic_t *characteristic = _ATMO_BLE_ONSEMI_GetCharFromHandle( handle );

	if ( characteristic == NULL )
	{
		return ATMO_BLE_Status_Fail;
	}

	if ( characteristic->numCallbacks[event] >= ATMO_ONSEMI_BLE_MAX_ABILITIES_PER_EVENT )
	{
		return ATMO_BLE_Status_Fail;
	}

	characteristic->callbacks[event][characteristic->numCallbacks[event]++] = cbFunc;
	return ATMO_BLE_Status_Success;
}

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_GATTSRegisterCharacteristicAbilityHandle( ATMO_DriverInstanceData_t *instance, ATMO_BLE_Handle_t handle, ATMO_BLE_Characteristic_Event_t event, unsigned int abilityHandler )
{
	_ATMO_ONSEMI_BLE_Characteristic_t *characteristic = _ATMO_BLE_ONSEMI_GetCharFromHandle( handle );

	if ( characteristic == NULL )
	{
		return ATMO_BLE_Status_Fail;
	}

	if ( characteristic->numAbilities[event] >= ATMO_ONSEMI_BLE_MAX_ABILITIES_PER_EVENT )
	{
		return ATMO_BLE_Status_Fail;
	}

	characteristic->ability[event][characteristic->numAbilities[event]++] = abilityHandler;
	return ATMO_BLE_Status_Success;
}

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_GATTSSetCharacteristic( ATMO_DriverInstanceData_t *instance, ATMO_BLE_Handle_t handle, uint16_t length, uint8_t *value, ATMO_BLE_CharProperties_t *properties )
{
	_ATMO_ONSEMI_BLE_Characteristic_t *characteristic = _ATMO_BLE_ONSEMI_GetCharFromHandle( handle );

	if ( characteristic == NULL || length > characteristic->maxLength )
	{
		return ATMO_BLE_Status_Fail;
	}

	memcpy( characteristic->data, value, length );
	characteristic->currentLength = length;
	return ATMO_BLE_Status_Success;
}

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_GATTSWriteDescriptor( ATMO_DriverInstanceData_t *instance, ATMO_BLE_Handle_t handle, uint16_t length, uint8_t *value, ATMO_BLE_CharProperties_t *properties )
{
	return ATMO_BLE_Status_NotSupported;
}

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_GATTSSendIndicate( ATMO_DriverInstanceData_t *instance, ATMO_BLE_Handle_t handle, uint16_t size, uint8_t *value )
{
	return ATMO_BLE_Status_NotSupported;
}

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_GATTSSendNotify( ATMO_DriverInstanceData_t *instance, ATMO_BLE_Handle_t handle, uint16_t size, uint8_t *value )
{
	return ATMO_BLE_Status_Success;
}

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_RegisterEventCallback( ATMO_DriverInstanceData_t *instance, ATMO_BLE_Event_t event, ATMO_Callback_t cb )
{
	if ( _ATMO_ONSEMI_BLE_NumEventCallbacks[event] >= ATMO_ONSEMI_BLE_MAX_ABILITIES_PER_EVENT )
	{
		return ATMO_BLE_Status_Invalid;
	}

	ATMO_PLATFORM_DebugPrint( "Registering BLE callback event %d\r\n", event );

	_ATMO_ONSEMI_BLE_EventCallbacks[event][_ATMO_ONSEMI_BLE_NumEventCallbacks[event]++] = cb;
	return ATMO_BLE_Status_Success;
}

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_RegisterEventAbilityHandle( ATMO_DriverInstanceData_t *instance, ATMO_BLE_Event_t event, unsigned int abilityHandle )
{
	if ( _ATMO_ONSEMI_BLE_NumEventAbilities[event] >= ATMO_ONSEMI_BLE_MAX_ABILITIES_PER_EVENT )
	{
		return ATMO_BLE_Status_Invalid;
	}

	_ATMO_ONSEMI_BLE_EventAbilities[event][_ATMO_ONSEMI_BLE_NumEventAbilities[event]++] = abilityHandle;
	return ATMO_BLE_Status_Success;
}

void ATMO_ONSEMI_BLE_DispatchEvent( ATMO_BLE_Event_t event )
{
	if(event == ATMO_BLE_EVENT_Connected)
	{
		_ATMO_ONSEMI_Connected = true;
	}
	else if(event == ATMO_BLE_EVENT_Disconnected)
	{
		_ATMO_ONSEMI_Connected = false;
	}

	for ( unsigned int i = 0; i < _ATMO_ONSEMI_BLE_NumEventAbilities[event]; i++ )
	{
		ATMO_PLATFORM_DebugPrint( "Dispatching ability %02X\r\n", _ATMO_ONSEMI_BLE_EventAbilities[event][i] );
		ATMO_AddAbilityExecute( _ATMO_ONSEMI_BLE_EventAbilities[event][i], NULL );
	}

	for ( unsigned int i = 0; i < _ATMO_ONSEMI_BLE_NumEventCallbacks[event]; i++ )
	{
		ATMO_PLATFORM_DebugPrint( "Dispatching callback %d\r\n", event );
		ATMO_AddCallbackExecute( _ATMO_ONSEMI_BLE_EventCallbacks[event][i], NULL );
	}
}
