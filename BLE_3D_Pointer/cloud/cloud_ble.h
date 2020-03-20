/**
 ******************************************************************************
 * @file    cloud_ble.h
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - Cloud BLE header file
 *
 * The standard operation for Cloud communication over BLE is as follows.
 *
 * When the driver initializes it'll will look to it's given file system
 * for stored credentials. If credentials are not found the driver will
 * place the BLE device into provision advertising mode which will be the
 * queue to the app that this device is available for provisioning into
 * the cloud. Once provisioned the device will store those credentials
 * into the file system provided and then switch to active mode. In active
 * mode the advertising data will change to present it's self as a
 * registered device waiting for access from a user that has permission to
 * access it. If the device ever wishes to send data to the cloud as is
 * doesn't currently have any connection to a device the BLE advertising
 * will switch into looking for a gateway mode. This advertising data will
 * be picked up by any gateway device which will then connect to the device
 * and the provide an secure back haul to the cloud for which the gateway
 * will not know what data is being trasnmitted but simply routes the
 * data back and forth from the cloud.
 *
 * The BLE Cloud driver uses a fixed service UUID of
 * "bfe433cf-6b5e-4368-abab-b0a59666a402"
 *
 * and the characteristic uuid of
 * "bfe433cf-6b5f-4368-abab-b0a59666a402"
 *
 * For passing data back and forth from the device that wishes to provision
 * it and also for gateways communication. If the device is connected to a
 * BLE central and wants to send data to the cloud it can push a notification
 * from the CLOUD characteritic to request data be sent to the cloud, or
 * recieved from the cloud.
 *
 * The data in this characteristic is structured in a simple command format
 *
 * < Byte 1 Command >< Command Data >
 *
 * There are several commands that can be written to and then the cloud ble
 * driver respondes using the same command number and a notify event.
 *
 * When the BLE Cloud driver wants to provision it'll be waiting for a device
 * to connect and then write the request provisioning command into it's cloud
 * characteristic.
 *
 * Example:
 *
 * Central Writes: < Get Indentity Type Command >
 * Central Notified With: < Get Identity Type Command > < Basic Indentity >
 *
 * Central Writes: < BasicServerConfirmIdentity Command > < Project Server Token Len > < Project Server Token >
 * Central Notified With: < BasicServerConfirmIdentity Command > < Success Status >
 *
 * Central Writes: < BasicClientConfirmIdentity Command >
 * Central Notified With: < BasicClientConfirmIdentity Command > < Project Client Token Len > Project Client Token >
 *
 * Central Writes: < SetRegistrationInfo > [ DATA ]
 * Central Notified With: < SetRegistrationInfo > < Success Status >
 *
 * Command Structures:
 *
 * Get Identity Type Central: < Command (1 Byte) >
 * Get Identity Type Response: < Command (1 Byte) > < Identity Type (1 Byte) >
 *
 * Set Registration Info Central: < Command (1 Byte) > < 128-bit UUID (16 Bytes) > < Token Data Null Terminated String (x Bytes) > < URL Data Null Terminated String (x Bytes) >
 * Set Registration Info Response: < Command (1 Byte) > < Status (1 Byte) >
 *
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

/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef __ATMO_CLOUD_BLE__H
#define __ATMO_CLOUD_BLE__H


/* Includes ------------------------------------------------------------------*/
#include "../atmo/core.h"
#include "cloud.h"
#include "../ble/ble.h"


#ifdef __cplusplus
extern "C" {
#endif

/* Exported Constants --------------------------------------------------------*/
#define ATMO_CLOUD_BLE_GATEWAY_LOCAL_NAME "VYWGWR"
#define ATMO_CLOUD_BLE_PRIMARY_SERVICE_UUID "bfe433cf-6b5e-4368-abab-b0a59666a402"
#define ATMO_CLOUD_BLE_PRIMARY_CHARACTERISTIC_UUID "bfe433cf-6b5f-4368-abab-b0a59666a402"


/* Exported Macros -----------------------------------------------------------*/

/* Exported Structures -------------------------------------------------------*/

/* Exported Types ------------------------------------------------------------*/

/**
 * This will add the BLE Cloud driver instance into the CLOUD API
 * handler so that it can be used by the user application. It must
 * be noted that this will require the instance number of a valid
 * BLE driver as well as a file system driver.
 *
 * @param
 * @return
 */
ATMO_Status_t ATMO_CLOUD_BLE_AddDriverInstance( ATMO_DriverInstanceHandle_t *instanceNumber, ATMO_DriverInstanceHandle_t bleInstanceNumber );

/**
 * This will initialize the ble cloud driver. Once it starts it
 * will load from it's filesystem the credentials for ble cloud
 * registration. If it can not find any it'll place it's self
 * into provisioning mode waiting for a user to claim the device
 * if it does find registration info it will go into normal
 * advertising mode waiting for the user to communicate with it.
 *
 * @param
 * @return
 */
ATMO_CLOUD_Status_t ATMO_CLOUD_BLE_Init( ATMO_DriverInstanceData_t *instance, ATMO_BOOL_t provision );

ATMO_CLOUD_Status_t ATMO_CLOUD_BLE_SendEvent( ATMO_DriverInstanceData_t *instance, const char *eventName, ATMO_Value_t *data, uint32_t timeout );

ATMO_CLOUD_Status_t ATMO_CLOUD_BLE_PopCommand( ATMO_DriverInstanceData_t *instance, const char *commandName, ATMO_Value_t *data, uint32_t timeout );

ATMO_CLOUD_Status_t ATMO_CLOUD_BLE_SubDriverSyncRegistration( ATMO_DriverInstanceData_t *instance );

#ifdef __cplusplus
}
#endif

#endif /* __ATMO_CLOUD_BLE__H */
