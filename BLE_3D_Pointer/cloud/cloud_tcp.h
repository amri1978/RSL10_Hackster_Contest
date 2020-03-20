/**
 ******************************************************************************
 * @file    cloud_TCP.h
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - Cloud TCP header file
 *
 * The standard operation for Cloud communication over TCP is as follows.
 *
 * The cloud TCP driver is used for provisioning. One provisioned, it will communicate with
 * the cloud via HTTP (or MQTT in the future).
 *
 * The provisioning packets and flow are identical to the cloud_ble driver.
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
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIATCP
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
#ifndef __ATMO_CLOUD_TCP__H
#define __ATMO_CLOUD_TCP__H


/* Includes ------------------------------------------------------------------*/
#include "../atmo/core.h"
#include "cloud.h"
#include "../tcpserver/tcpserver.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported Constants --------------------------------------------------------*/

/* Exported Macros -----------------------------------------------------------*/

/* Exported Structures -------------------------------------------------------*/

/* Exported Types ------------------------------------------------------------*/

/**
 * @brief This will register the TCP driver with the cloud driver
 *
 * @param instanceNumber
 * @param networkInstanceNumber- Instance of the Network Driver. Can be Cellular or WiFi
 * @param networkType - ATMO_DriverType_t WIFI or Cellular
 * @param httpClientInstanceNumber - Instance of the HTTP Client driver. This will be used at the transport for cloud events and cloud commands.
 * @param tcpServerInstanceNumber - Instance of the TCP Server driver. THis will be used for provisioning. Ignored for Cellular.
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_CLOUD_TCP_AddDriverInstanceByType( ATMO_DriverInstanceHandle_t *instanceNumber, ATMO_DriverInstanceHandle_t networkInstanceNumber, ATMO_DriverType_t networkType, ATMO_DriverInstanceHandle_t httpClientInstanceNumber, ATMO_DriverInstanceHandle_t tcpServerInstanceNumber, ATMO_DriverInstanceHandle_t intervalInstanceNumber );

/**
 * @brief This will register the TCP driver with the cloud driver with the default network type (WiFi)
 *
 * @param instanceNumber
 * @param wifiInstanceNumber- Instance of the WiFi Driver.
 * @param httpClientInstanceNumber - Instance of the HTTP Client driver. This will be used at the transport for cloud events and cloud commands.
 * @param tcpServerInstanceNumber - Instance of the TCP Server driver. THis will be used for provisioning. Ignored for Cellular.
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_CLOUD_TCP_AddDriverInstance( ATMO_DriverInstanceHandle_t *instanceNumber, ATMO_DriverInstanceHandle_t wifiInstanceNumber, ATMO_DriverInstanceHandle_t httpClientInstanceNumber, ATMO_DriverInstanceHandle_t tcpServerInstanceNumber, ATMO_DriverInstanceHandle_t intervalInstanceNumber );

/**
 * Initialize the TCP driver. If the device isn't registered, a TCP server will be created for provisioning.
 *
 * @param
 * @return
 */
ATMO_CLOUD_Status_t ATMO_CLOUD_TCP_Init( ATMO_DriverInstanceData_t *instance, ATMO_BOOL_t provision );

ATMO_CLOUD_Status_t ATMO_CLOUD_TCP_SendEvent( ATMO_DriverInstanceData_t *instance, const char *eventName, ATMO_Value_t *data, uint32_t timeout );

ATMO_CLOUD_Status_t ATMO_CLOUD_TCP_PopCommand( ATMO_DriverInstanceData_t *instance, const char *commandName, ATMO_Value_t *data, uint32_t timeout );

#ifdef __cplusplus
}
#endif

#endif /* __ATMO_CLOUD_TCP__H */
