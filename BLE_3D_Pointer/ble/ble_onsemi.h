/**
 ******************************************************************************
 * @file    ble_onsemi.h
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - ONSEMI BLE header file
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

/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef __ATMO_ONSEMI_BLE__H
#define __ATMO_ONSEMI_BLE__H


/* Includes ------------------------------------------------------------------*/
#include "../app_src/atmosphere_platform.h"
#include "../atmo/core.h"
#include "ble.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported Constants --------------------------------------------------------*/

/* Exported Macros -----------------------------------------------------------*/

/* Exported Types ------------------------------------------------------------*/

ATMO_Status_t ATMO_ONSEMI_BLE_AddDriverInstance( ATMO_DriverInstanceHandle_t *instanceNumber );

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_PeripheralInit( ATMO_DriverInstanceData_t *instance );

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_PeripheralDeInit( ATMO_DriverInstanceData_t *instance );

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_SetEnabled( ATMO_DriverInstanceData_t *instance, bool enabled );

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_GetEnabled( ATMO_DriverInstanceData_t *instance, bool *enabled );

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_GetMacAddress( ATMO_DriverInstanceData_t *instance, ATMO_BLE_MacAddress_t *address );

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_GAPSetDeviceName( ATMO_DriverInstanceData_t *instance, const char *name );

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_GAPAdvertisingStart( ATMO_DriverInstanceData_t *instance, ATMO_BLE_AdvertisingParams_t *params );

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_GAPAdvertisingStop( ATMO_DriverInstanceData_t *instance );

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_GAPSetAdvertisedServiceUUID( ATMO_DriverInstanceData_t *instance, const char *uuid );

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_GAPAdverertisingSetManufacturerData( ATMO_DriverInstanceData_t *instance, ATMO_BLE_AdvertisingData_t *data );

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_GAPPairingCfg( ATMO_DriverInstanceData_t *instance, ATMO_BLE_PairingCfg_t *config );

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_GAPDisconnect( ATMO_DriverInstanceData_t *instance );

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_GATTSAddService( ATMO_DriverInstanceData_t *instance, ATMO_BLE_Handle_t *handle, const char *serviceUUID );

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_GATTSAddCharacteristic( ATMO_DriverInstanceData_t *instance, ATMO_BLE_Handle_t *handle, ATMO_BLE_Handle_t serviceHandle, const char *characteristicUUID, uint8_t properties, uint8_t permissions, uint32_t maxLen );

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_GATTSGetCharacteristicValue( ATMO_DriverInstanceData_t *instance, ATMO_BLE_Handle_t handle, uint8_t *valueBuf, uint32_t valueBufLen, uint32_t *valueLen );

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_GATTSRegisterCharacteristicCallback( ATMO_DriverInstanceData_t *instance, ATMO_BLE_Handle_t handle, ATMO_BLE_Characteristic_Event_t event, ATMO_Callback_t cbFunc );

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_GATTSRegisterCharacteristicAbilityHandle( ATMO_DriverInstanceData_t *instance, ATMO_BLE_Handle_t handle, ATMO_BLE_Characteristic_Event_t event, unsigned int abilityHandler );

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_GATTSSetCharacteristic( ATMO_DriverInstanceData_t *instance, ATMO_BLE_Handle_t handle, uint16_t length, uint8_t *value, ATMO_BLE_CharProperties_t *properties );

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_GATTSWriteDescriptor( ATMO_DriverInstanceData_t *instance, ATMO_BLE_Handle_t handle, uint16_t length, uint8_t *value, ATMO_BLE_CharProperties_t *properties );

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_GATTSSendIndicate( ATMO_DriverInstanceData_t *instance, ATMO_BLE_Handle_t handle, uint16_t size, uint8_t *value );

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_GATTSSendNotify( ATMO_DriverInstanceData_t *instance, ATMO_BLE_Handle_t handle, uint16_t size, uint8_t *value );

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_RegisterEventCallback( ATMO_DriverInstanceData_t *instance, ATMO_BLE_Event_t event, ATMO_Callback_t cb );

ATMO_BLE_Status_t ATMO_ONSEMI_BLE_RegisterEventAbilityHandle( ATMO_DriverInstanceData_t *instance, ATMO_BLE_Event_t event, unsigned int abilityHandle );

void ATMO_ONSEMI_BLE_SyncDb();

void ATMO_ONSEMI_BLE_DispatchEvent( ATMO_BLE_Event_t event );

ATMO_BLE_Status_t _ATMO_ONSEMI_BLE_SetInitComplete();


#ifdef __cplusplus
}
#endif

#endif /* __ATMO_ONSEMI_BLE__H */
