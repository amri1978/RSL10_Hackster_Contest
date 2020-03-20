/**
 ******************************************************************************
 * @file    cloud_provisioner.h
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - Cloud Provisioner header file
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

#ifndef _ATMO_CLOUD_PROVISIONER_H_
#define _ATMO_CLOUD_PROVISIONER_H_

#include "../atmo/core.h"

typedef enum
{
	ATMO_CLOUD_PROVISIONER_IdentityType_Basic = 0x01,
	ATMO_CLOUD_PROVISIONER_IdentityType_x509Cert = 0x02
} ATMO_CLOUD_PROVISIONER_IdentityType_t;

typedef enum
{
	ATMO_CLOUD_PROVISIONER_CommandType_GetIndentityType = 0x01,
	ATMO_CLOUD_PROVISIONER_CommandType_BasicCentralConfirmIdentity = 0x02,
	ATMO_CLOUD_PROVISIONER_CommandType_BasicClientConfirmIdentity = 0x03,
	ATMO_CLOUD_PROVISIONER_CommandType_LockRegistrationInfo = 0x04,
	ATMO_CLOUD_PROVISIONER_CommandType_GetRegistrationInfo = 0x05,
	ATMO_CLOUD_PROVISIONER_CommandType_SetRegistrationInfoUrl = 0x06,
	ATMO_CLOUD_PROVISIONER_CommandType_SetRegistrationInfoUuid = 0x07,
	ATMO_CLOUD_PROVISIONER_CommandType_SetRegistrationInfoToken = 0x08,
	ATMO_CLOUD_PROVISIONER_CommandType_ClearRegistrationInfo = 0x09,
	ATMO_CLOUD_PROVISIONER_CommandType_SetDateTimeIsoStr = 0x0A, // Expects an ISO 8601 String
	ATMO_CLOUD_PROVISIONER_CommandType_UnlockRegistrationInfo = 0x0B,
	ATMO_CLOUD_PROVISIONER_CommandType_SetDateTimeEpochInt = 0x0C, // 32 bit little endian unsigned int
	ATMO_CLOUD_PROVISIONER_CommandType_ExtraSettingsWifi = 0x0D,
	ATMO_CLOUD_PROVISIONER_CommandType_ExtraSettingsSigfox = 0x0E,
	ATMO_CLOUD_PROVISIONER_CommandType_ExtraSettingsLoRa = 0x0F,
	ATMO_CLOUD_PROVISIONER_CommandType_Max,
} ATMO_CLOUD_PROVISIONER_CommandType_t;

typedef enum
{
	ATMO_CLOUD_PROVISIONER_SubCommand_WifiExtraSettings_GetStatus = 0x0,
	ATMO_CLOUD_PROVISIONER_SubCommand_WifiExtraSettings_SetSSID = 0x1,
	ATMO_CLOUD_PROVISIONER_SubCommand_WifiExtraSettings_SetPassword = 0x2,
	ATMO_CLOUD_PROVISIONER_SubCommand_WifiExtraSettings_Max
} ATMO_CLOUD_PROVISIONER_SubCommand_Wifi_t;

typedef enum
{
	ATMO_CLOUD_PROVISIONER_SubCommand_SigfoxExtraSettings_GetDeviceId = 0x0,
	ATMO_CLOUD_PROVISIONER_SubCommand_SigfoxExtraSettings_Max
} ATMO_CLOUD_PROVISIONER_SubCommand_Sigfox_t;

typedef enum
{
	ATMO_CLOUD_PROVISIONER_SubCommand_LoRaExtraSettings_GetDeviceId = 0x0,
	ATMO_CLOUD_PROVISIONER_SubCommand_LoRaExtraSettings_Max
} ATMO_CLOUD_PROVISIONER_SubCommand_LoRa_t;

/**
 * @brief Handle a provisioning command
 *
 * @param command - Command buffer
 * @param commandLength - Size in bytes of command
 * @param respBuffer - Response buffer
 * @param respBufferLength  - Size in bytes of response buffer
 * @return int - Length of response, or -1 if error
 */
int ATMO_CLOUD_PROVISIONER_HandleCommand( uint8_t *command, uint32_t commandLength, uint8_t *respBuffer, uint32_t respBufferLength, ATMO_BOOL_t *connectionVerified );

#endif