/**
 ******************************************************************************
 * @file    cloud.c
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - Core CLOUD Driver
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

#include "cloud.h"
#include "cloud_config.h"
#include "../filesystem/filesystem.h"
#include "../app_src/atmosphere_globals.h"
#include "../app_src/atmosphere_platform.h"
#include "../app_src/atmosphere_cloudcommands.h"

#ifdef ATMO_SLIM_STACK
	#ifndef NUMBER_OF_CLOUD_DRIVER_INSTANCES
		#define NUMBER_OF_CLOUD_DRIVER_INSTANCES 2
	#endif
#endif

#ifndef NUMBER_OF_CLOUD_DRIVER_INSTANCES
#define NUMBER_OF_CLOUD_DRIVER_INSTANCES 4
#endif

#define ATMO_CLOUD_CONFIG_MAX_CALLBACKS (10)

// #define ATMO_CLOUD_FORCE_URL
#define NUMBER_CLOUD_REG_CHANGED_CALLBACKS 10

ATMO_DriverInstanceHandle_t numberOfCloudDriverInstance = 0;
ATMO_DriverInstanceHandle_t _filesystemInstanceHandle = 0;

static unsigned int currentNumCloudRegChangedCallbacks = 0;
static ATMO_Callback_t cloudRegChangedCb[NUMBER_CLOUD_REG_CHANGED_CALLBACKS];

ATMO_CLOUD_RegistrationInfo_t currentRegInfo;

const ATMO_CLOUD_DriverInstance_t *cloudInstances[NUMBER_OF_CLOUD_DRIVER_INSTANCES];
ATMO_DriverInstanceData_t *cloudInstancesData[NUMBER_OF_CLOUD_DRIVER_INSTANCES];

static uint16_t _ATMO_CLOUD_ExtraSettings = 0;

typedef struct
{
	ATMO_Value_t value;
	ATMO_Callback_t cb[ATMO_CLOUD_CONFIG_MAX_CALLBACKS];
	unsigned int numCallbacks;
	ATMO_BOOL_t isSet;
} ATMO_CLOUD_Config_Priv_t;

static ATMO_CLOUD_Config_Priv_t configOptions[ATMO_CLOUD_NUM_CONFIGS];

typedef struct
{
	unsigned int abilityHandle;
	ATMO_BOOL_t abilityHandleRegistered;
	ATMO_Callback_t cb;
} ATMO_CLOUD_CommandCallback_t;

#if TOTAL_NUM_CLOUD_COMMANDS == 0
static ATMO_CLOUD_CommandCallback_t *commandCallback = NULL;
#else
static ATMO_CLOUD_CommandCallback_t commandCallback[TOTAL_NUM_CLOUD_COMMANDS];
#endif


static ATMO_BOOL_t __ATMO_CLOUD_SaveBuildUuid( unsigned int fsHandle, char *buildUuid )
{
	ATMO_FILESYSTEM_File_t buildUuidFile;

	if ( ATMO_FILESYSTEM_FileOpen( fsHandle, &buildUuidFile, "buildUuid",
	                               ATMO_WRONLY | ATMO_TRUNC | ATMO_CREAT ) != ATMO_FILESYSTEM_Status_Success )
	{
		return false;
	}

	if ( ATMO_FILESYSTEM_FileWrite( fsHandle, &buildUuidFile, buildUuid, strlen( buildUuid ) ) != ATMO_FILESYSTEM_Status_Success )
	{
		ATMO_FILESYSTEM_FileClose( fsHandle, &buildUuidFile );
		return false;
	}

	if ( ATMO_FILESYSTEM_FileClose( fsHandle, &buildUuidFile ) != ATMO_FILESYSTEM_Status_Success )
	{
		return false;
	}

	return true;
}

static ATMO_BOOL_t __ATMO_CLOUD_GetBuildUuid( unsigned int fsHandle, char *buildUuid )
{
	buildUuid[ATMO_CLOUD_UUID_STR_LENGTH] = 0;
	ATMO_FILESYSTEM_File_t buildUuidFile;

	if ( ATMO_FILESYSTEM_FileOpen( fsHandle, &buildUuidFile, "buildUuid", ATMO_RDONLY ) != ATMO_FILESYSTEM_Status_Success )
	{
		return false;
	}

	if ( ATMO_FILESYSTEM_FileRead( fsHandle, &buildUuidFile, buildUuid,
	                               ATMO_CLOUD_UUID_STR_LENGTH ) != ATMO_FILESYSTEM_Status_Success )
	{
		ATMO_FILESYSTEM_FileClose( fsHandle, &buildUuidFile );
		return false;
	}

	if ( ATMO_FILESYSTEM_FileClose( fsHandle, &buildUuidFile ) != ATMO_FILESYSTEM_Status_Success )
	{
		return false;
	}

	return true;
}

static ATMO_BOOL_t __ATMO_CLOUD_GetProjectUuid( unsigned int fsHandle, char *uuid )
{
	uuid[ATMO_CLOUD_UUID_STR_LENGTH] = 0;

	ATMO_FILESYSTEM_File_t file;

	if ( ATMO_FILESYSTEM_FileOpen( fsHandle, &file, "project_uuid",
	                               ATMO_RDONLY ) != ATMO_FILESYSTEM_Status_Success )
	{
		ATMO_PLATFORM_DebugPrint( "Error opening project_uuid file\r\n" );
		return false;
	}

	if ( ATMO_FILESYSTEM_FileRead( fsHandle, &file, uuid, ATMO_CLOUD_UUID_STR_LENGTH ) != ATMO_FILESYSTEM_Status_Success )
	{
		ATMO_PLATFORM_DebugPrint( "Error reading project uuid file\r\n" );
		ATMO_FILESYSTEM_FileClose( fsHandle, &file );
		return false;
	}

	ATMO_FILESYSTEM_FileClose( fsHandle, &file );

	return true;
}

static void __ATMO_CLOUD_SaveProjectUuid( unsigned int fsHandle, char *projectUuid )
{
	ATMO_FILESYSTEM_File_t file;

	if ( ATMO_FILESYSTEM_FileOpen( fsHandle, &file, "project_uuid",
	                               ATMO_CREAT | ATMO_WRONLY | ATMO_TRUNC ) != ATMO_FILESYSTEM_Status_Success )
	{
		return;
	}

	if ( ATMO_FILESYSTEM_FileWrite( fsHandle, &file, projectUuid, strlen( projectUuid ) ) != ATMO_FILESYSTEM_Status_Success )
	{
		ATMO_FILESYSTEM_FileClose( fsHandle, &file );
		return;
	}

	ATMO_FILESYSTEM_FileClose( fsHandle, &file );

	return;
}

static ATMO_BOOL_t __ATMO_CLOUD_IsSameProject( unsigned int fsHandle )
{
	char uuid[strlen( ATMO_GLOBALS_PROJECTUUID ) + 1];

	if ( !__ATMO_CLOUD_GetProjectUuid( fsHandle, uuid ) )
	{
		return false;
	}

	return strcmp( uuid, ATMO_GLOBALS_PROJECTUUID ) == 0;
}


static ATMO_BOOL_t __ATMO_CLOUD_IsBootAfterOta( unsigned int fsHandle )
{
	ATMO_FILESYSTEM_File_t file;
	ATMO_BOOL_t isBootAfterOta = false;

	if ( ATMO_FILESYSTEM_FileOpen( fsHandle, &file, "ota_complete", ATMO_RDONLY ) == ATMO_FILESYSTEM_Status_Success )
	{
		if ( ATMO_FILESYSTEM_FileRead( fsHandle, &file, &isBootAfterOta, sizeof( isBootAfterOta ) ) != ATMO_FILESYSTEM_Status_Success )
		{
			isBootAfterOta = false;
		}

		ATMO_FILESYSTEM_FileClose( fsHandle, &file );
	}

	return isBootAfterOta && __ATMO_CLOUD_IsSameProject( fsHandle );
}

static ATMO_CLOUD_Status_t __ATMO_CLOUD_SaveRegToFileSystem( ATMO_CLOUD_RegistrationInfo_t *registration )
{
	ATMO_FILESYSTEM_File_t file;

	if ( ATMO_FILESYSTEM_FileOpen( _filesystemInstanceHandle, &file, "registrationInfo",
	                               ATMO_RDWR | ATMO_CREAT | ATMO_TRUNC ) != ATMO_FILESYSTEM_Status_Success )
	{
		return ATMO_CLOUD_Status_Fail;
	}

	if ( ATMO_FILESYSTEM_FileWrite( _filesystemInstanceHandle, &file, registration,
	                                sizeof( ATMO_CLOUD_RegistrationInfo_t ) ) != ATMO_FILESYSTEM_Status_Success )
	{
		ATMO_FILESYSTEM_FileClose( _filesystemInstanceHandle, &file );
		return ATMO_CLOUD_Status_Fail;
	}

	ATMO_FILESYSTEM_FileClose( _filesystemInstanceHandle, &file );

	return ATMO_CLOUD_Status_Success;
}


ATMO_CLOUD_Status_t __ATMO_CLOUD_ReadRegFromFileSystem( ATMO_CLOUD_RegistrationInfo_t *registration )
{
	ATMO_CLOUD_Status_t status = ATMO_CLOUD_Status_Success;

	// Load registration if it exists
	ATMO_FILESYSTEM_File_t file;

	if ( ATMO_FILESYSTEM_FileOpen( _filesystemInstanceHandle, &file, "registrationInfo",
	                               ATMO_RDONLY ) == ATMO_FILESYSTEM_Status_Success )
	{
		if ( ATMO_FILESYSTEM_FileRead( _filesystemInstanceHandle, &file, ( uint8_t * )registration,
		                               sizeof( ATMO_CLOUD_RegistrationInfo_t ) ) != ATMO_FILESYSTEM_Status_Success )
		{
			status = ATMO_CLOUD_Status_Fail;
		}

		ATMO_FILESYSTEM_FileClose( _filesystemInstanceHandle, &file );
	}
	else
	{
		status = ATMO_CLOUD_Status_Fail;
	}

	return status;
}


ATMO_Status_t ATMO_CLOUD_AddDriverInstance( const ATMO_CLOUD_DriverInstance_t *driverInstance,
        ATMO_DriverInstanceData_t *driverInstanceData, ATMO_DriverInstanceHandle_t *instanceNumber )
{
	if ( !( numberOfCloudDriverInstance < NUMBER_OF_CLOUD_DRIVER_INSTANCES ) )
	{
		return ATMO_Status_OutOfMemory;
	}

	cloudInstances[numberOfCloudDriverInstance] = driverInstance;
	cloudInstancesData[numberOfCloudDriverInstance] = driverInstanceData;
	*instanceNumber = numberOfCloudDriverInstance;

	numberOfCloudDriverInstance++;

	return ATMO_Status_Success;
}

ATMO_CLOUD_Status_t ATMO_CLOUD_Init( ATMO_DriverInstanceHandle_t instanceNumber, ATMO_BOOL_t provision )
{
	if ( !( instanceNumber < numberOfCloudDriverInstance ) )
	{
		return ATMO_CLOUD_Status_Invalid;
	}

	return cloudInstances[instanceNumber]->Init( cloudInstancesData[instanceNumber], provision );
}

ATMO_CLOUD_Status_t ATMO_CLOUD_SetOtaFlag( ATMO_DriverInstanceHandle_t filesystemDriverHandle )
{
	ATMO_FILESYSTEM_File_t file;
	ATMO_BOOL_t data = true;

	if ( ATMO_FILESYSTEM_FileOpen( filesystemDriverHandle, &file, "ota_complete",
	                               ATMO_CREAT | ATMO_WRONLY ) != ATMO_FILESYSTEM_Status_Success )
	{
		return ATMO_CLOUD_Status_Fail;
	}

	if ( ATMO_FILESYSTEM_FileWrite( filesystemDriverHandle, &file, &data, sizeof( data ) ) != ATMO_FILESYSTEM_Status_Success )
	{
		return ATMO_CLOUD_Status_Fail;
	}

	ATMO_FILESYSTEM_FileClose( filesystemDriverHandle, &file );

	return ATMO_CLOUD_Status_Success;
}

ATMO_CLOUD_Status_t ATMO_CLOUD_SetExtraRequiredSettings( uint16_t extraSettings )
{
	_ATMO_CLOUD_ExtraSettings |= extraSettings;
	return ATMO_CLOUD_Status_Success;
}

ATMO_CLOUD_Status_t ATMO_CLOUD_ClearExtraRequiredSettings( uint16_t extraSettings )
{
	_ATMO_CLOUD_ExtraSettings &= ( ~extraSettings );
	return ATMO_CLOUD_Status_Success;
}

uint16_t ATMO_CLOUD_GetExtraRequiredSettings()
{
	return _ATMO_CLOUD_ExtraSettings;
}

ATMO_CLOUD_Status_t ATMO_CLOUD_InitFilesystemData( ATMO_DriverInstanceHandle_t filesystemDriverHandle )
{
	char storedBuildUuid[ATMO_CLOUD_UUID_STR_LENGTH + 1] = {0};
	ATMO_BOOL_t buildUuidMatch = false;
	ATMO_BOOL_t bootAfterOta = false;

	ATMO_BOOL_t wipeFlash = false;

	_filesystemInstanceHandle = filesystemDriverHandle;

	if ( !__ATMO_CLOUD_GetBuildUuid( filesystemDriverHandle, storedBuildUuid ) )
	{
#ifndef ATMO_SLIM_STACK
		ATMO_PLATFORM_DebugPrint( "Wiping Flash: Error retrieving build UUID\r\n" );
#endif
		wipeFlash = true;
	}
	else
	{
#ifndef ATMO_SLIM_STACK
		ATMO_PLATFORM_DebugPrint( "Stored Build UUID: %s\r\nCurrent Build UUID: %s\r\n", storedBuildUuid, ATMO_GLOBALS_BUILDUUID );
#endif
		buildUuidMatch = strcmp( storedBuildUuid, ATMO_GLOBALS_BUILDUUID ) == 0;
		bootAfterOta = __ATMO_CLOUD_IsBootAfterOta( filesystemDriverHandle );
#ifndef ATMO_SLIM_STACK
		ATMO_PLATFORM_DebugPrint( "Uuid Match %d Boot After OTA: %d\r\n", buildUuidMatch, bootAfterOta );
#endif

		if ( ( !buildUuidMatch ) && ( !bootAfterOta ) )
		{
#ifndef ATMO_SLIM_STACK
			ATMO_PLATFORM_DebugPrint( "Wiping Flash\r\n" );
#endif
			wipeFlash = true;
		}
	}

	char storedProjectUuid[ATMO_CLOUD_UUID_STR_LENGTH + 1];

	if ( !__ATMO_CLOUD_GetProjectUuid( filesystemDriverHandle, storedProjectUuid ) )
	{
		ATMO_PLATFORM_DebugPrint( "Error getting Project UUID" );
		wipeFlash = true;
	}

	if ( strcmp( storedProjectUuid, ATMO_GLOBALS_PROJECTUUID ) != 0 )
	{
		ATMO_PLATFORM_DebugPrint( "Project UUID Mismatch. Stored: %s Expected: %s\r\n", storedProjectUuid, ATMO_GLOBALS_PROJECTUUID );
		wipeFlash = true;
	}

	if ( wipeFlash )
	{
		ATMO_PLATFORM_DebugPrint( "Wiping Flash\r\n" );
		ATMO_FILESYSTEM_Wipe( filesystemDriverHandle );
	}
	else
	{
#ifndef ATMO_SLIM_STACK
		ATMO_PLATFORM_DebugPrint( "Not Wiping Flash\r\n" );
#endif
	}

	ATMO_PLATFORM_DebugPrint( "Initializing config opts\r\n" );

	if ( commandCallback != NULL )
	{
		memset( commandCallback, 0, TOTAL_NUM_CLOUD_COMMANDS * sizeof( ATMO_CLOUD_CommandCallback_t ) );
	}

	memset( configOptions, 0, sizeof( configOptions ) );

	int i;

	for ( i = 0; i < ATMO_CLOUD_NUM_CONFIGS; i++ )
	{
		ATMO_InitValue( &configOptions[i].value );
	}

	ATMO_PLATFORM_DebugPrint( "Saving project UUID\r\n" );
	__ATMO_CLOUD_SaveProjectUuid( filesystemDriverHandle, ATMO_GLOBALS_PROJECTUUID );

	ATMO_PLATFORM_DebugPrint( "Saving build UUID\r\n" );
	__ATMO_CLOUD_SaveBuildUuid( filesystemDriverHandle, ATMO_GLOBALS_BUILDUUID );

	ATMO_PLATFORM_DebugPrint( "Saved build UUID project UUID\r\n" );

	ATMO_CLOUD_InitRegistrationInfo( &currentRegInfo );

	ATMO_CLOUD_RegistrationInfo_t regInfo;
	ATMO_CLOUD_InitRegistrationInfo( &regInfo );

	ATMO_CLOUD_Status_t regReadStatus = __ATMO_CLOUD_ReadRegFromFileSystem( &regInfo );

	if ( regReadStatus != ATMO_CLOUD_Status_Success )
	{
#ifndef ATMO_SLIM_STACK
		ATMO_PLATFORM_DebugPrint( "Error reading registration from FS. Clearning.\r\n" );
#endif
		ATMO_CLOUD_ClearRegistration();
	}

#ifndef ATMO_SLIM_STACK
	ATMO_PLATFORM_DebugPrint( "Device Registered: %d\r\n", regInfo.registered );
#endif

	ATMO_CLOUD_SetRegistration( &regInfo );


	return ATMO_CLOUD_Status_Success;
}

static void __ATMO_CLOUD_CopyRegistrationInfo( ATMO_CLOUD_RegistrationInfo_t *from, ATMO_CLOUD_RegistrationInfo_t *to )
{
	if ( to == from )
	{
		return;
	}

	memcpy( to, from, sizeof( ATMO_CLOUD_RegistrationInfo_t ) );
}

ATMO_CLOUD_Status_t ATMO_CLOUD_InitRegistrationInfo( ATMO_CLOUD_RegistrationInfo_t *info )
{
	memset( info, 0, sizeof( ATMO_CLOUD_RegistrationInfo_t ) );
	info->registered = false;

	return ATMO_CLOUD_Status_Success;
}

ATMO_CLOUD_Status_t ATMO_CLOUD_SetRegistration( ATMO_CLOUD_RegistrationInfo_t *info )
{
#ifdef ATMO_CLOUD_FORCE_URL
	strcpy( info->url, "http://192.168.1.126:1338" );
#endif

	// If info is null, then the structure has been changed underneath us
	// Just need to save it and sync sub drivers
	if ( info != NULL )
	{
		__ATMO_CLOUD_CopyRegistrationInfo( info, &currentRegInfo );
	}

	unsigned int i;

	for ( i = 0; i < currentNumCloudRegChangedCallbacks; i++ )
	{
		ATMO_Value_t value;
		ATMO_InitValue( &value );
		ATMO_AddCallbackExecute( cloudRegChangedCb[i], &value );
	}

	if ( info != NULL )
	{
		return __ATMO_CLOUD_SaveRegToFileSystem( info );
	}
	else
	{
		return __ATMO_CLOUD_SaveRegToFileSystem( &currentRegInfo );
	}
}

ATMO_CLOUD_Status_t ATMO_CLOUD_ClearRegistration()
{
	ATMO_FILESYSTEM_Remove( _filesystemInstanceHandle, "registrationInfo" );
	ATMO_FILESYSTEM_Remove( _filesystemInstanceHandle, "cloudUrl" );
	ATMO_CLOUD_InitRegistrationInfo( &currentRegInfo );
	return ATMO_CLOUD_Status_Success;
}

ATMO_CLOUD_RegistrationInfo_t *ATMO_CLOUD_GetRegistration( void )
{
	return &currentRegInfo;
}

ATMO_CLOUD_Status_t ATMO_CLOUD_SendEvent( ATMO_DriverInstanceHandle_t instanceNumber, const char *eventName, ATMO_Value_t *data,
        uint32_t timeout )
{
	if ( !( instanceNumber < numberOfCloudDriverInstance ) )
	{
		return ATMO_CLOUD_Status_Invalid;
	}

	return cloudInstances[instanceNumber]->SendEvent( cloudInstancesData[instanceNumber], eventName, data, timeout );
}

ATMO_CLOUD_Status_t ATMO_CLOUD_PopCommand( ATMO_DriverInstanceHandle_t instanceNumber, const char *commandName,
        ATMO_Value_t *data, uint32_t timeout )
{
	if ( !( instanceNumber < numberOfCloudDriverInstance ) )
	{
		return ATMO_CLOUD_Status_Invalid;
	}

	return cloudInstances[instanceNumber]->PopCommand( cloudInstancesData[instanceNumber], commandName, data, timeout );
}

ATMO_CLOUD_Status_t ATMO_CLOUD_RegisterRegistrationSetCallback( ATMO_Callback_t cb )
{
	if ( currentNumCloudRegChangedCallbacks >= NUMBER_CLOUD_REG_CHANGED_CALLBACKS )
	{
		return ATMO_CLOUD_Status_Fail;
	}

	cloudRegChangedCb[currentNumCloudRegChangedCallbacks++] = cb;

	return ATMO_CLOUD_Status_Success;
}

ATMO_CLOUD_Status_t ATMO_CLOUD_GetRegistrationInfoUuid( ATMO_CLOUD_RegistrationInfo_t *info, char *buffer, size_t size )
{
	if ( info == NULL || buffer == NULL || size < 37 )
	{
		return ATMO_CLOUD_Status_Invalid;
	}

	ATMO_UUID_t uuid;
	memcpy( uuid.data, info->uuid, 16 );
	uuid.endianness = ATMO_ENDIAN_Type_Big;
	uuid.type = ATMO_UUID_Type_128_Bit;

	if ( ATMO_UuidToString( &uuid, buffer, size ) != ATMO_Status_Success )
	{
		return ATMO_CLOUD_Status_Fail;
	}

	return ATMO_CLOUD_Status_Success;
}

ATMO_CLOUD_Status_t ATMO_CLOUD_SetRegistrationInfoToken( ATMO_CLOUD_RegistrationInfo_t *info, const char *token )
{

	if ( info == NULL || token == NULL )
	{
		return ATMO_CLOUD_Status_Invalid;
	}

	strncpy( info->token, token, ATMO_CLOUD_TOKEN_LENGTH );

	return ATMO_CLOUD_Status_Success;

}

ATMO_CLOUD_Status_t ATMO_CLOUD_GetRegistrationInfoToken( ATMO_CLOUD_RegistrationInfo_t *info, char *buffer, size_t size )
{
	if ( info == NULL || buffer == NULL )
	{
		return ATMO_CLOUD_Status_Invalid;
	}

	strncpy( buffer, info->token, size );

	return ATMO_CLOUD_Status_Success;
}

ATMO_CLOUD_Status_t ATMO_CLOUD_SetRegistrationInfoUrl( ATMO_CLOUD_RegistrationInfo_t *info, const char *url )
{
	if ( info == NULL || url == NULL )
	{
		return ATMO_CLOUD_Status_Invalid;
	}

	size_t len = strlen( url );

	if ( len >= ATMO_CLOUD_MAX_URL_LENGTH )
	{
		return ATMO_CLOUD_Status_Invalid;
	}

	strncpy( info->url, url, ATMO_CLOUD_MAX_URL_LENGTH );

	return ATMO_CLOUD_Status_Success;
}

ATMO_CLOUD_Status_t ATMO_CLOUD_GetRegistrationInfoUrl( ATMO_CLOUD_RegistrationInfo_t *info, char *buffer, size_t size )
{
	if ( info == NULL || buffer == NULL )
	{
		return ATMO_CLOUD_Status_Invalid;
	}

	strncpy( buffer, info->url, size );

	return ATMO_CLOUD_Status_Success;
}

ATMO_CLOUD_CommandCallback_t *__ATMO_CLOUD_GetCloudCommandByName( const char *commandName )
{
	unsigned int i;

	for ( i = 0; i < TOTAL_NUM_CLOUD_COMMANDS; i++ )
	{
		if ( strcmp( commandName, cloudCommandsList[i] ) == 0 )
		{
			return &commandCallback[i];
		}
	}

	return NULL;
}

ATMO_CLOUD_Status_t ATMO_CLOUD_RegisterCommandRxAbilityHandle( const char *commandName, unsigned int abilityHandle )
{
	ATMO_CLOUD_CommandCallback_t *commandCbStruct = __ATMO_CLOUD_GetCloudCommandByName( commandName );

	if ( commandCbStruct == NULL )
	{
		return ATMO_CLOUD_Status_Fail;
	}

	commandCbStruct->abilityHandle = abilityHandle;
	commandCbStruct->abilityHandleRegistered = true;

	return ATMO_CLOUD_Status_Success;
}

ATMO_CLOUD_Status_t ATMO_CLOUD_RegisterCommandRxCallback( const char *commandName, ATMO_Callback_t cb )
{
	ATMO_CLOUD_CommandCallback_t *commandCbStruct = __ATMO_CLOUD_GetCloudCommandByName( commandName );

	if ( commandCbStruct == NULL )
	{
		return ATMO_CLOUD_Status_Fail;
	}

	commandCbStruct->cb = cb;

	return ATMO_CLOUD_Status_Success;
}

ATMO_CLOUD_Status_t ATMO_CLOUD_DispatchCommandResult( const char *command, ATMO_Value_t *result )
{
	ATMO_CLOUD_CommandCallback_t *cb = __ATMO_CLOUD_GetCloudCommandByName( command );

	if ( cb != NULL )
	{
		if ( cb->abilityHandleRegistered )
		{
			ATMO_AddAbilityExecute( cb->abilityHandle, result );
		}
		else if ( cb->cb != NULL )
		{
			ATMO_AddCallbackExecute( cb->cb, result );
		}
	}

	return ATMO_CLOUD_Status_Success;
}

/**
* Register for a notification when a config item changes
*
* @param[in] configHandle - Configuration handle
* @param[in] callback - Callback to be executed when config item changes. Config handle will be passed as an arg to the callback.
*/
ATMO_CLOUD_Status_t ATMO_CLOUD_RegisterNotifyConfigChange( unsigned int configHandle, ATMO_Callback_t callback )
{
	if ( configHandle >= ATMO_CLOUD_NUM_CONFIGS )
	{
		return ATMO_CLOUD_Status_Fail;
	}

	if ( configOptions[configHandle].numCallbacks >= ATMO_CLOUD_CONFIG_MAX_CALLBACKS )
	{
		return ATMO_CLOUD_Status_Fail;
	}

	configOptions[configHandle].cb[configOptions[configHandle].numCallbacks++] = callback;

	return ATMO_CLOUD_Status_Success;
}

ATMO_CLOUD_Status_t ATMO_CLOUD_SetConfig( unsigned int configHandle, ATMO_Value_t *data )
{
	if ( configHandle >= ATMO_CLOUD_NUM_CONFIGS )
	{
		return ATMO_CLOUD_Status_Fail;
	}

	ATMO_FreeValue( &configOptions[configHandle].value );

	ATMO_CreateValueCopy( &configOptions[configHandle].value, data );
	configOptions[configHandle].isSet = true;

	unsigned int i;

	// Execute any waiting callbacks
	for ( i = 0; i < configOptions[configHandle].numCallbacks; i++ )
	{
		ATMO_Value_t value;
		ATMO_InitValue( &value );
		ATMO_CreateValueUnsignedInt( &value, configHandle );
		ATMO_AddCallbackExecute( configOptions[configHandle].cb[i], &value );
		ATMO_FreeValue( &value );
	}

	return ATMO_CLOUD_Status_Success;
}

ATMO_CLOUD_Status_t ATMO_CLOUD_GetConfig( unsigned int configHandle, ATMO_Value_t *data )
{
	if ( configHandle >= ATMO_CLOUD_NUM_CONFIGS || !configOptions[configHandle].isSet )
	{
		return ATMO_CLOUD_Status_Fail;
	}

	ATMO_CreateValueCopy( data, &configOptions[configHandle].value );

	return ATMO_CLOUD_Status_Success;
}
