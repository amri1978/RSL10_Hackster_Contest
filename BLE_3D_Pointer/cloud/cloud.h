/**
 ******************************************************************************
 * @file    cloud.h
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - Cloud header file
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

/** \addtogroup Cloud
 *  @{
 * The cloud driver is used to enable access to Atmosphere through cloud event and cloud command elements.
 * The base cloud driver only provides filesystem initialization and shared functionality, while
 * all transport is handled by the cloud sub-drivers (eg. cloud_tcp and cloud_ble).
 */


/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef __ATMO_CLOUD__H
#define __ATMO_CLOUD__H


/* Includes ------------------------------------------------------------------*/
#include "../atmo/core.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported Constants --------------------------------------------------------*/

#define ATMO_CLOUD_UUID_LENGTH 16
#define ATMO_CLOUD_UUID_STR_LENGTH 36
#define ATMO_CLOUD_TOKEN_LENGTH 16
#define ATMO_CLOUD_MAX_URL_LENGTH 64
#define ATMO_CLOUD_MAX_PROVISION_INFO_NAME_LEN 32

/* Exported Macros -----------------------------------------------------------*/

/* Exported Types ------------------------------------------------------------*/

/**
 * Atmosphere Cloud Driver Return Enumerations
 */
typedef enum
{
	ATMO_CLOUD_Status_Success              = 0x00u,  // Common - Operation was successful
	ATMO_CLOUD_Status_Fail                 = 0x01u,  // Common - Operation failed
	ATMO_CLOUD_Status_Initialized          = 0x02u,  // Common - Peripheral already initialized
	ATMO_CLOUD_Status_Invalid              = 0x03u,  // Common - Invalid operation or result
	ATMO_CLOUD_Status_NotSupported         = 0x04u,  // Common - Feature not supported by platform
	ATMO_CLOUD_Status_Unknown              = 0x05u,  // Common - Some other status not defined
	ATMO_CLOUD_Status_Timeout              = 0x06u,  // The request timed out before completing
	ATMO_CLOUD_Status_NoData			   = 0x07u, // Cloud command did not return any data
} ATMO_CLOUD_Status_t;

typedef enum
{
	ATMO_CLOUD_ExtraSettings_BLE = 0x1,
	ATMO_CLOUD_ExtraSettings_WIFI = 0x02,
	ATMO_CLOUD_ExtraSettings_LORA = 0x04,
	ATMO_CLOUD_ExtraSettings_CELLULAR = 0x08,
	ATMO_CLOUD_ExtraSettings_THREAD = 0x10,
	ATMO_CLOUD_ExtraSettings_6LOWPAN = 0x20,
	ATMO_CLOUD_ExtraSettings_ETHERNET = 0x40,
	ATMO_CLOUD_ExtraSettings_ZIGBEE = 0x80,
	ATMO_CLOUD_ExtraSettings_ZWAVE = 0x100,
	ATMO_CLOUD_ExtraSettings_SIGFOX = 0x200,
	ATMO_CLOUD_ExtraSettings_NEUL = 0x400
} ATMO_CLOUD_ExtraSettings_t; // Extra settings needed for provisioning

/* Exported Structures -------------------------------------------------------*/

/// \cond DO_NOT_DOCUMENT
typedef struct
{
	ATMO_BOOL_t registered;
	uint8_t uuid[ATMO_CLOUD_UUID_LENGTH];	//Device UUID on the cloud
	char token[ATMO_CLOUD_TOKEN_LENGTH];	//Device token used for basic authentication
	char url[ATMO_CLOUD_MAX_URL_LENGTH];			//URL to the cloud server
} ATMO_CLOUD_RegistrationInfo_t;

typedef struct ATMO_CLOUD_DriverInstance_t ATMO_CLOUD_DriverInstance_t;

struct ATMO_CLOUD_DriverInstance_t
{
	ATMO_CLOUD_Status_t ( *Init )( ATMO_DriverInstanceData_t *instanceData, ATMO_BOOL_t provision );
	ATMO_CLOUD_Status_t ( *SendEvent )( ATMO_DriverInstanceData_t *instanceData, const char *eventName, ATMO_Value_t *data, uint32_t timeout );
	ATMO_CLOUD_Status_t ( *PopCommand )( ATMO_DriverInstanceData_t *instanceData, const char *commandName, ATMO_Value_t *data, uint32_t timeout );
};
/// \endcond

/* Exported Function Prototypes -----------------------------------------------*/

/**
 * Register Cloud driver instance with underlying driver framework
 *
 * @param[in] driverInstance
 * @param[in] driverInstanceData
 * @param[out] instanceNumber
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_CLOUD_AddDriverInstance( const ATMO_CLOUD_DriverInstance_t *driverInstance, ATMO_DriverInstanceData_t *driverInstanceData, ATMO_DriverInstanceHandle_t *instanceNumber );

/**
 * This will initialize the cloud driver instance. Normally this would
 * include loading registration from non-volitile memory and then storing
 * into RAM. from there the driver would then begin it's usual advertising
 * methods to communicate with the app and cloud.
 *
 * @param[in] instanceNumber
 * @param[in] provision - Set as a provisioning interface
 * @return ATMO_CLOUD_Status_t
 */
ATMO_CLOUD_Status_t ATMO_CLOUD_Init( ATMO_DriverInstanceHandle_t instanceNumber, ATMO_BOOL_t provision );

/**
 * @brief Set extra required provisioning settings for specific hardware platform
 *
 * @param extraSettings - OR of ATMO_CLOUD_ExtraSettings_t types
 * @return ATMO_CLOUD_Status_t
 */
ATMO_CLOUD_Status_t ATMO_CLOUD_SetExtraRequiredSettings( uint16_t extraSettings );

/**
 * @brief Clear extra required provisioning settings for specific hardware platform
 *
 * @param extraSettings - OR of ATMO_CLOUD_ExtraSettings_t types
 * @return ATMO_CLOUD_Status_t
 */
ATMO_CLOUD_Status_t ATMO_CLOUD_ClearExtraRequiredSettings( uint16_t extraSettings );

/**
 * @brief Get extra required provisioning settings for specific hardware platform
 *
 * @return uint16_t
 */
uint16_t ATMO_CLOUD_GetExtraRequiredSettings();

/**
 * Initialize any saved data in the filesystem
 *
 * @param[in] filesystemDriverHandle
 * @return ATMO_CLOUD_Status_t
 */
ATMO_CLOUD_Status_t ATMO_CLOUD_InitFilesystemData( ATMO_DriverInstanceHandle_t filesystemDriverHandle );

/**
 * Write flag to the FS indicating that an OTA upgrade just occured
 *
 * @param[in] filesystemDriverHandle
 * @return ATMO_CLOUD_Status_t
 */
ATMO_CLOUD_Status_t ATMO_CLOUD_SetOtaFlag( ATMO_DriverInstanceHandle_t filesystemDriverHandle );

/**
 * Initialize registration info structure
 * @param[in] info
 * @return ATMO_CLOUD_Status_t
 */
ATMO_CLOUD_Status_t ATMO_CLOUD_InitRegistrationInfo( ATMO_CLOUD_RegistrationInfo_t *info );

/**
 * This will set the registration info for the driver instance.
 *
 * @param[in] instanceNumber
 * @param[in] info
 * @return ATMO_CLOUD_Status_t
 */
ATMO_CLOUD_Status_t ATMO_CLOUD_SetRegistration( ATMO_CLOUD_RegistrationInfo_t *info );

/**
 * This will get a pointer to current registration info from the driver instance
 *
 * @return pointer to registration info
 */
ATMO_CLOUD_RegistrationInfo_t *ATMO_CLOUD_GetRegistration( void );

/**
 * This will clear the registration data
 *
 * @param
 * @return
 */
ATMO_CLOUD_Status_t ATMO_CLOUD_ClearRegistration();

/**
* Register for a notification when a config item changes
*
* @param[in] configHandle - Configuration handle
* @param[in] callback - Callback to be executed when config item changes. Config handle will be passed as an arg to the callback.
*/
ATMO_CLOUD_Status_t ATMO_CLOUD_RegisterNotifyConfigChange( unsigned int configHandle, ATMO_Callback_t callback );

/**
* Set configuration item
*
* @param[in] configHandle
* @param[in] data
*/
ATMO_CLOUD_Status_t ATMO_CLOUD_SetConfig( unsigned int configHandle, ATMO_Value_t *data );

/**
* Get configuration item
*
* @param[in] configHandle
* @param[out] data
*/
ATMO_CLOUD_Status_t ATMO_CLOUD_GetConfig( unsigned int configHandle, ATMO_Value_t *data );

/**
 * Register to receive a notification when the registration changes
 */
ATMO_CLOUD_Status_t ATMO_CLOUD_RegisterRegistrationSetCallback( ATMO_Callback_t cb );

/**
 * This will request the driver to attempt to send the value to the
 * cloud.
 *
 * @param[in] instanceNumber
 * @param[in] eventName - Name of event
 * @param[in] data - Event data
 * @param[in] timeout - timeout in ms
 * @return ATMO_CLOUD_Status_t
 */
ATMO_CLOUD_Status_t ATMO_CLOUD_SendEvent( ATMO_DriverInstanceHandle_t instanceNumber, const char *eventName, ATMO_Value_t *data, uint32_t timeout );

/**
 * This will request the driver to go pop the next command off the
 * cloud's command buffer with the provided command name.
 *
 * @param[in] instanceNumber
 * @param[in] commandName - Name of command
 * @param[out] data - Command data
 * @param[in] timeout - timeout in ms
 * @return ATMO_CLOUD_Status_t
 */
ATMO_CLOUD_Status_t ATMO_CLOUD_PopCommand( ATMO_DriverInstanceHandle_t instanceNumber, const char *commandName, ATMO_Value_t *data, uint32_t timeout );

/**
 * Register ability handle to be executed when Cloud Command is received
 *
 * @param[in] commandName
 * @param[in] abilityHandle
 */
ATMO_CLOUD_Status_t ATMO_CLOUD_RegisterCommandRxAbilityHandle( const char *commandName, unsigned int abilityHandle );

/**
 * Register callback to be executed when Cloud Command is received
 *
 * @param[in] commandName
 * @param[in] abilityHandle
 */
ATMO_CLOUD_Status_t ATMO_CLOUD_RegisterCommandRxCallback( const char *commandName, ATMO_Callback_t cb );

// Given a cloud command, dispatch it to appropriate callbacks
ATMO_CLOUD_Status_t ATMO_CLOUD_DispatchCommandResult( const char *command, ATMO_Value_t *result );

/**
 * This function will return the UUIDv4 value formatted in standard lower
 * case UUIDv4 string format in the buffer. Buffer size must be at least
 * 37 bytes to contain the string.
 *
 * Example: b05a625b-a6b6-4f1d-820a-13ff296ad94f
 *
 * @param[in] info
 * @param[out] buffer - buffer to hold UUID data
 * @param[in] size - buffer size
 * @return ATMO_CLOUD_Status_t
 */
ATMO_CLOUD_Status_t ATMO_CLOUD_GetRegistrationInfoUuid( ATMO_CLOUD_RegistrationInfo_t *info, char *buffer, size_t size );

/**
 * Set token in registration info structure
 * @param[in] info
 * @param[in] token
 * @return ATMO_CLOUD_Status_t
 */
ATMO_CLOUD_Status_t ATMO_CLOUD_SetRegistrationInfoToken( ATMO_CLOUD_RegistrationInfo_t *info, const char *token );

/**
 * This will return the registration token as a string contained in buffer
 * that the registration info is using.
 *
 * @param[in] info
 * @param[out] buffer
 * @param[in] size
 * @return ATMO_CLOUD_Status_t
 */
ATMO_CLOUD_Status_t ATMO_CLOUD_GetRegistrationInfoToken( ATMO_CLOUD_RegistrationInfo_t *info, char *buffer, size_t size );

/**
 * Set URL in registration info structure
 * @param[in] info
 * @param[in] url
 * @return ATMO_CLOUD_Status_t
 */
ATMO_CLOUD_Status_t ATMO_CLOUD_SetRegistrationInfoUrl( ATMO_CLOUD_RegistrationInfo_t *info, const char *url );

/**
 * This function will return the url as a string that the registration info
 * is set to communicate with.
 *
 * @param[in] info
 * @param[out] buffer
 * @param[in] size
 * @return ATMO_CLOUD_Status_t
 */
ATMO_CLOUD_Status_t ATMO_CLOUD_GetRegistrationInfoUrl( ATMO_CLOUD_RegistrationInfo_t *info, char *buffer, size_t size );

#ifdef __cplusplus
}
#endif

#endif /* __ATMO_CLOUD__H */

/** @}*/