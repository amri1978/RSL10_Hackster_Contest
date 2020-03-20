#include "filesystem_crastfs.h"
#include "../app_src/atmosphere_platform.h"
#include "../block/block.h"
#include "../cloud/cloud.h"

/**
 * CrastFS
 *
 * OVERVIEW
 * ========
 * The CRASTFS is designed to hold exactly the amount of information that Nimbus needs to run.
 */

static ATMO_BLOCK_DeviceInfo_t deviceInfo;
static ATMO_DriverInstanceHandle_t blockHandle = 0;
static ATMO_BOOL_t _ATMO_CRASTFS_EnoughRoom = true;
#define ATMO_CRASTFS_MAGICWORD (0xABBC4369)

typedef struct
{
	uint32_t offset;
	uint32_t size;
} ATMO_CRASTFS_FileInfo_t;

#define ATMO_CRASTFS_NUM_FILES (6)
static ATMO_CRASTFS_FileInfo_t _ATMO_CRASTFS_FileInfo[ATMO_CRASTFS_NUM_FILES] =
{
	{
		ATMO_CRASTFS_REGISTERED_OFFSET,
		ATMO_CRASTFS_REGISTERED_SIZE
	},
	{
		ATMO_CRASTFS_PROJECT_UUID_OFFSET,
		ATMO_CRASTFS_PROJECT_UUID_SIZE
	},
	{
		ATMO_CRASTFS_BUILD_UUID_OFFSET,
		ATMO_CRASTFS_BUILD_UUID_SIZE
	},
	{
		ATMO_CRASTFS_OTA_COMPLETE_OFFSET,
		ATMO_CRASTFS_OTA_COMPLETE_SIZE
	},
	{
		ATMO_CRASTFS_EXTRA1_OFFSET,
		ATMO_CRASTFS_EXTRA1_SIZE
	},
	{
		ATMO_CRASTFS_EXTRA2_OFFSET,
		ATMO_CRASTFS_EXTRA2_SIZE
	}
};

ATMO_FILESYSTEM_DriverInstance_t CRASTFSFilesystemDriverInstance =
{
	ATMO_CRASTFS_FILESYSTEM_Init,
	ATMO_CRASTFS_FILESYSTEM_SetConfiguration,
	ATMO_CRASTFS_FILESYSTEM_Wipe,
	ATMO_CRASTFS_FILESYSTEM_Mount,
	ATMO_CRASTFS_FILESYSTEM_Unmount,
	ATMO_CRASTFS_FILESYSTEM_Remove,
	ATMO_CRASTFS_FILESYSTEM_Rename,
	ATMO_CRASTFS_FILESYSTEM_Stat,
	ATMO_CRASTFS_FILESYSTEM_FileOpen,
	ATMO_CRASTFS_FILESYSTEM_FileClose,
	ATMO_CRASTFS_FILESYSTEM_FileSync,
	ATMO_CRASTFS_FILESYSTEM_FileRead,
	ATMO_CRASTFS_FILESYSTEM_FileWrite,
	ATMO_CRASTFS_FILESYSTEM_FileSeek,
	ATMO_CRASTFS_FILESYSTEM_FileTell,
	ATMO_CRASTFS_FILESYSTEM_FileSize,
	ATMO_CRASTFS_FILESYSTEM_FileRewind,
	ATMO_CRASTFS_FILESYSTEM_FileTruncate,
	ATMO_CRASTFS_FILESYSTEM_DirMk
};

static const char *buildUuidPathStr = "buildUuid";
static const char *projectUuidPathStr = "project_uuid";
static const char *regInfoPathStr = "registrationInfo";
static const char *otaCompletePathStr = "ota_complete";
static const char *extra1PathStr = "extra1";
static const char *extra2PathStr = "extra2";

static ATMO_DriverInstanceData_t crastFsDriverInstanceData;

ATMO_Status_t ATMO_CRASTFS_FILESYSTEM_AddDriverInstance( ATMO_DriverInstanceHandle_t *instanceNumber )
{

	crastFsDriverInstanceData.name = "";
	crastFsDriverInstanceData.initialized = false;
	crastFsDriverInstanceData.instanceNumber = *instanceNumber;
	crastFsDriverInstanceData.argument = NULL;
	return ATMO_FILESYSTEM_AddDriverInstance( &CRASTFSFilesystemDriverInstance, &crastFsDriverInstanceData, instanceNumber );
}
ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_Init( ATMO_DriverInstanceData_t *instance, ATMO_DriverInstanceHandle_t blockDriverHandle )
{
	blockHandle = blockDriverHandle;
	ATMO_BLOCK_GetDeviceInfo( blockHandle, &deviceInfo );

	uint32_t totalNumBytes = deviceInfo.blockSize * deviceInfo.blockCount;

	if ( totalNumBytes < ATMO_CRASTFS_SIZE )
	{
		_ATMO_CRASTFS_EnoughRoom = false;
		return ATMO_FILESYSTEM_Status_Fail;
	}

	// Write metadata
	uint32_t magicWord = 0;

	if ( ATMO_BLOCK_Read( blockHandle, 0, ATMO_CRASTFS_MAGICWORD_OFFSET, &magicWord, sizeof( magicWord ) ) != ATMO_BLOCK_Status_Success )
	{
		return ATMO_FILESYSTEM_Status_Fail;
	}

	if ( magicWord != ATMO_CRASTFS_MAGICWORD )
	{
#ifndef ATMO_SLIM_STACK
		ATMO_PLATFORM_DebugPrint( "Filesystem version incorrect, wiping...\r\n" );
#endif
		ATMO_CRASTFS_FILESYSTEM_Wipe( instance );
	}

	return ATMO_FILESYSTEM_Status_Success;
}

ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_SetConfiguration( ATMO_DriverInstanceData_t *instance, const ATMO_FILESYSTEM_Config_t *config )
{
	return ATMO_FILESYSTEM_Status_Success;
}

ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_Wipe( ATMO_DriverInstanceData_t *instance )
{
	unsigned int i;

	for ( i = 0; i < deviceInfo.blockCount; i++ )
	{
		ATMO_BLOCK_Erase( blockHandle, i );
	}

	// Write metadata
	uint32_t magicWord = ATMO_CRASTFS_MAGICWORD;

	if ( ATMO_BLOCK_Program( blockHandle, 0, ATMO_CRASTFS_MAGICWORD_OFFSET, &magicWord, sizeof( magicWord ) ) != ATMO_BLOCK_Status_Success )
	{
		return ATMO_FILESYSTEM_Status_Fail;
	}

	return ATMO_FILESYSTEM_Status_Success;
}

ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_Mount( ATMO_DriverInstanceData_t *instance )
{
	return ATMO_FILESYSTEM_Status_Success;
}

ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_Unmount( ATMO_DriverInstanceData_t *instance )
{
	return ATMO_FILESYSTEM_Status_Success;
}

static int _ATMO_CRASTFS_FILESYSTEM_GetFileInfoIndex( const char *path )
{
	if ( strcmp( path, buildUuidPathStr ) == 0 )
	{
		return 2;
	}
	else if ( strcmp( path, projectUuidPathStr ) == 0 )
	{
		return 1;
	}
	else if ( strcmp( path, regInfoPathStr ) == 0 )
	{
		return 0;
	}
	else if ( strcmp( path, otaCompletePathStr ) == 0 )
	{
		return 3;
	}
	else if ( strcmp( path, extra1PathStr ) == 0 )
	{
		return 4;
	}
	else if ( strcmp( path, extra2PathStr ) == 0 )
	{
		return 5;
	}
	else
	{
		return -1;
	}
}

static ATMO_FILESYSTEM_Status_t _ATMO_CRASTFS_SimpleRead( uint32_t offset, void *buffer, uint32_t bufferSize, uint32_t readSize )
{
	if ( bufferSize < readSize )
	{
		return ATMO_FILESYSTEM_Status_Fail;
	}

	return ( ATMO_BLOCK_Read( blockHandle, 0, offset, buffer, readSize ) == ATMO_BLOCK_Status_Success ) ? ATMO_FILESYSTEM_Status_Success : ATMO_FILESYSTEM_Status_Fail;
}

static ATMO_FILESYSTEM_Status_t _ATMO_CRASTFS_SimpleWrite( uint32_t offset, void *buffer, uint32_t writeSize, uint32_t maxFileSize )
{
	if ( writeSize > maxFileSize )
	{
		return ATMO_FILESYSTEM_Status_Fail;
	}

	return ( ATMO_BLOCK_Program( blockHandle, 0, offset, buffer, writeSize ) == ATMO_BLOCK_Status_Success ) ? ATMO_FILESYSTEM_Status_Success : ATMO_FILESYSTEM_Status_Fail;
}

static ATMO_FILESYSTEM_Status_t _ATMO_CRASTFS_ReadRegistrationData( void *buffer, uint32_t bufferSize )
{
	if ( bufferSize < sizeof( ATMO_CLOUD_RegistrationInfo_t ) )
	{
		return ATMO_FILESYSTEM_Status_Fail;
	}

	ATMO_CLOUD_RegistrationInfo_t *regInfo = ( ATMO_CLOUD_RegistrationInfo_t * )buffer;

	// Read registered
	uint8_t registered = 0;

	if ( _ATMO_CRASTFS_SimpleRead( ATMO_CRASTFS_REGISTERED_OFFSET, &registered, 1, ATMO_CRASTFS_REGISTERED_SIZE ) != ATMO_FILESYSTEM_Status_Success )
	{
		return ATMO_FILESYSTEM_Status_Fail;
	}

	regInfo->registered = ( registered == 1 ) ? true : false;

	// Read device uuid
	if ( _ATMO_CRASTFS_SimpleRead( ATMO_CRASTFS_DEVICE_UUID_OFFSET, regInfo->uuid, ATMO_CLOUD_UUID_LENGTH, ATMO_CRASTFS_DEVICE_UUID_SIZE ) != ATMO_FILESYSTEM_Status_Success )
	{
		return ATMO_FILESYSTEM_Status_Fail;
	}

	// Read token
	if ( _ATMO_CRASTFS_SimpleRead( ATMO_CRASTFS_TOKEN_OFFSET, regInfo->token, ATMO_CLOUD_TOKEN_LENGTH, ATMO_CRASTFS_TOKEN_SIZE ) != ATMO_FILESYSTEM_Status_Success )
	{
		return ATMO_FILESYSTEM_Status_Fail;
	}

	// Read url
	if ( _ATMO_CRASTFS_SimpleRead( ATMO_CRASTFS_URL_OFFSET, regInfo->url, ATMO_CLOUD_MAX_URL_LENGTH, ATMO_CRASTFS_URL_SIZE ) != ATMO_FILESYSTEM_Status_Success )
	{
		return ATMO_FILESYSTEM_Status_Fail;
	}

	return ATMO_FILESYSTEM_Status_Success;
}

static ATMO_FILESYSTEM_Status_t _ATMO_CRASTFS_ReadOtaComplete( void *buffer, uint32_t bufferSize )
{
	uint8_t otaCompleteByte = 0;

	if ( _ATMO_CRASTFS_SimpleRead( ATMO_CRASTFS_OTA_COMPLETE_OFFSET, &otaCompleteByte, 1, ATMO_CRASTFS_OTA_COMPLETE_SIZE ) != ATMO_FILESYSTEM_Status_Success )
	{
		return ATMO_FILESYSTEM_Status_Fail;
	}

	ATMO_BOOL_t *otaComplete = ( ATMO_BOOL_t * )buffer;
	( *otaComplete ) = ( otaCompleteByte == 1 ) ? true : false;

	return ATMO_FILESYSTEM_Status_Success;
}

static ATMO_FILESYSTEM_Status_t _ATMO_CRASTFS_WriteRegistrationData( void *buffer, uint32_t bufferSize )
{
	ATMO_CLOUD_RegistrationInfo_t *regInfo = ( ATMO_CLOUD_RegistrationInfo_t * )buffer;

	// Write registered
	uint8_t registered = regInfo->registered ? 1 : 0;

	if ( _ATMO_CRASTFS_SimpleWrite( ATMO_CRASTFS_REGISTERED_OFFSET, &registered, 1, ATMO_CRASTFS_REGISTERED_SIZE ) != ATMO_FILESYSTEM_Status_Success )
	{
		return ATMO_FILESYSTEM_Status_Fail;
	}

	// Write device uuid
	if ( _ATMO_CRASTFS_SimpleWrite( ATMO_CRASTFS_DEVICE_UUID_OFFSET, regInfo->uuid, ATMO_CLOUD_UUID_LENGTH, ATMO_CRASTFS_DEVICE_UUID_SIZE ) != ATMO_FILESYSTEM_Status_Success )
	{
		return ATMO_FILESYSTEM_Status_Fail;
	}

	// Write token
	if ( _ATMO_CRASTFS_SimpleWrite( ATMO_CRASTFS_TOKEN_OFFSET, regInfo->token, ATMO_CLOUD_TOKEN_LENGTH, ATMO_CRASTFS_TOKEN_SIZE ) != ATMO_FILESYSTEM_Status_Success )
	{
		return ATMO_FILESYSTEM_Status_Fail;
	}

	// Write url
	if ( _ATMO_CRASTFS_SimpleWrite( ATMO_CRASTFS_URL_OFFSET, regInfo->url, ATMO_CLOUD_MAX_URL_LENGTH, ATMO_CRASTFS_URL_SIZE ) != ATMO_FILESYSTEM_Status_Success )
	{
		return ATMO_FILESYSTEM_Status_Fail;
	}

	return ATMO_FILESYSTEM_Status_Success;
}

static ATMO_FILESYSTEM_Status_t _ATMO_CRASTFS_WriteOtaComplete( void *buffer, uint32_t bufferSize )
{

	ATMO_BOOL_t *otaComplete = ( ATMO_BOOL_t * )buffer;
	uint8_t otaCompleteByte = ( *otaComplete ) ? 1 : 0;

	if ( _ATMO_CRASTFS_SimpleWrite( ATMO_CRASTFS_OTA_COMPLETE_OFFSET, &otaCompleteByte, 1, ATMO_CRASTFS_OTA_COMPLETE_SIZE ) != ATMO_FILESYSTEM_Status_Success )
	{
		return ATMO_FILESYSTEM_Status_Fail;
	}

	return ATMO_FILESYSTEM_Status_Success;
}

ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_Remove( ATMO_DriverInstanceData_t *instance, const char *path )
{
	// Unavailable for crastfs, just overwrite it!
	return ATMO_FILESYSTEM_Status_Success;
}

ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_Rename( ATMO_DriverInstanceData_t *instance, const char *oldPath, const char *newPath )
{
	// Unavailable for crastfs
	return ATMO_FILESYSTEM_Status_Success;
}

ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_Stat( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_Info_t *info, const char *path )
{
	int index = _ATMO_CRASTFS_FILESYSTEM_GetFileInfoIndex( path );

	if ( index < 0 )
	{
		return ATMO_FILESYSTEM_Status_Fail;
	}

	info->size = _ATMO_CRASTFS_FileInfo[index].size;
	info->type = ATMO_FILESYSTEM_Type_File;

	return ATMO_FILESYSTEM_Status_Success;
}

ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_FileOpen( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file, const char *path, int flags )
{
	int index = _ATMO_CRASTFS_FILESYSTEM_GetFileInfoIndex( path );

	if ( index < 0 )
	{
		return ATMO_FILESYSTEM_Status_Fail;
	}

#ifdef ATMO_CRASTFS_DEBUG
	ATMO_PLATFORM_DebugPrint( "Opened file %s. Offset: %d Size %d\r\n", path, offset, size );
#endif

	file->data = &_ATMO_CRASTFS_FileInfo[index];

	return ATMO_FILESYSTEM_Status_Success;
}

ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_FileClose( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file )
{
	return ATMO_FILESYSTEM_Status_Success;
}

ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_FileSync( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file )
{
	return ATMO_FILESYSTEM_Status_Success;
}

ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_FileRead( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file, void *buffer, uint32_t size )
{
	ATMO_CRASTFS_FileInfo_t *info = ( ATMO_CRASTFS_FileInfo_t * )file->data;

	switch ( info->offset )
	{
		case ATMO_CRASTFS_REGISTERED_OFFSET:
		{
			return _ATMO_CRASTFS_ReadRegistrationData( buffer, size );
		}

		case ATMO_CRASTFS_BUILD_UUID_OFFSET:
		{
			return _ATMO_CRASTFS_SimpleRead( ATMO_CRASTFS_BUILD_UUID_OFFSET, buffer, size, ATMO_CRASTFS_BUILD_UUID_SIZE );
		}

		case ATMO_CRASTFS_PROJECT_UUID_OFFSET:
		{
			return _ATMO_CRASTFS_SimpleRead( ATMO_CRASTFS_PROJECT_UUID_OFFSET, buffer, size, ATMO_CRASTFS_PROJECT_UUID_SIZE );
		}

		case ATMO_CRASTFS_OTA_COMPLETE_OFFSET:
		{
			return _ATMO_CRASTFS_ReadOtaComplete( buffer, size );
		}

		case ATMO_CRASTFS_EXTRA1_OFFSET:
		{
			return _ATMO_CRASTFS_SimpleRead( ATMO_CRASTFS_EXTRA1_OFFSET, buffer, size, ATMO_CRASTFS_EXTRA1_SIZE );
		}

		case ATMO_CRASTFS_EXTRA2_OFFSET:
		{
			return _ATMO_CRASTFS_SimpleRead( ATMO_CRASTFS_EXTRA2_OFFSET, buffer, size, ATMO_CRASTFS_EXTRA2_SIZE );
		}

		default:
		{
			return ATMO_FILESYSTEM_Status_Fail;
		}
	}

	return ATMO_FILESYSTEM_Status_Success;
}

ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_FileWrite( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file, void *buffer, uint32_t size )
{

	ATMO_CRASTFS_FileInfo_t *info = ( ATMO_CRASTFS_FileInfo_t * )file->data;

#ifdef ATMO_CRASTFS_DEBUG
	ATMO_PLATFORM_DebugPrint( "Writing Offset %d\r\n", info->offset );
#endif

	switch ( info->offset )
	{
		case ATMO_CRASTFS_REGISTERED_OFFSET:
		{
			return _ATMO_CRASTFS_WriteRegistrationData( buffer, size );
		}

		case ATMO_CRASTFS_BUILD_UUID_OFFSET:
		{
			return _ATMO_CRASTFS_SimpleWrite( ATMO_CRASTFS_BUILD_UUID_OFFSET, buffer, size, ATMO_CRASTFS_BUILD_UUID_SIZE );
		}

		case ATMO_CRASTFS_PROJECT_UUID_OFFSET:
		{
			return _ATMO_CRASTFS_SimpleWrite( ATMO_CRASTFS_PROJECT_UUID_OFFSET, buffer, size, ATMO_CRASTFS_PROJECT_UUID_SIZE );
		}

		case ATMO_CRASTFS_OTA_COMPLETE_OFFSET:
		{
			return _ATMO_CRASTFS_ReadOtaComplete( buffer, size );
		}

		case ATMO_CRASTFS_EXTRA1_OFFSET:
		{
			return _ATMO_CRASTFS_SimpleWrite( ATMO_CRASTFS_EXTRA1_OFFSET, buffer, size, ATMO_CRASTFS_EXTRA1_SIZE );
		}

		case ATMO_CRASTFS_EXTRA2_OFFSET:
		{
			return _ATMO_CRASTFS_SimpleWrite( ATMO_CRASTFS_EXTRA2_OFFSET, buffer, size, ATMO_CRASTFS_EXTRA2_SIZE );
		}

		default:
		{
			return ATMO_FILESYSTEM_Status_Fail;
		}
	}

	return ATMO_FILESYSTEM_Status_Success;
}

ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_FileSeek( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file, uint32_t offset )
{
	return ATMO_FILESYSTEM_Status_NotSupported;
}
ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_FileTell( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file, uint32_t *currPos )
{
	return ATMO_FILESYSTEM_Status_NotSupported;
}

ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_FileSize( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file, uint32_t *size )
{
	ATMO_CRASTFS_FileInfo_t *info = ( ATMO_CRASTFS_FileInfo_t * )file->data;
	*size = info->size;
	return ATMO_FILESYSTEM_Status_Success;
}

ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_FileRewind( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file )
{
	return ATMO_FILESYSTEM_Status_NotSupported;
}

ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_FileTruncate( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file, uint32_t size )
{
	return ATMO_FILESYSTEM_Status_NotSupported;
}

ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_DirMk( ATMO_DriverInstanceData_t *instance, const char *path )
{
	return ATMO_FILESYSTEM_Status_NotSupported;
}
