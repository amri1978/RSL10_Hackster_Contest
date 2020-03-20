#include "filesystem_lfs.h"
#include "lfs.h"
#include "../app_src/atmosphere_platform.h"
#include "../block/block.h"

#define NUMBER_OF_LITTLEFS_FILESYSTEM_DRIVER_INSTANCES (4)

static struct lfs_config configs[NUMBER_OF_LITTLEFS_FILESYSTEM_DRIVER_INSTANCES];
static lfs_t filesystems[NUMBER_OF_LITTLEFS_FILESYSTEM_DRIVER_INSTANCES];

static uint32_t currentNumFileSystems = 0;

ATMO_FILESYSTEM_DriverInstance_t lfsFilesystemDriverInstance =
{
	ATMO_LFS_FILESYSTEM_Init,
	ATMO_LFS_FILESYSTEM_SetConfiguration,
	ATMO_LFS_FILESYSTEM_Wipe,
	ATMO_LFS_FILESYSTEM_Mount,
	ATMO_LFS_FILESYSTEM_Unmount,
	ATMO_LFS_FILESYSTEM_Remove,
	ATMO_LFS_FILESYSTEM_Rename,
	ATMO_LFS_FILESYSTEM_Stat,
	ATMO_LFS_FILESYSTEM_FileOpen,
	ATMO_LFS_FILESYSTEM_FileClose,
	ATMO_LFS_FILESYSTEM_FileSync,
	ATMO_LFS_FILESYSTEM_FileRead,
	ATMO_LFS_FILESYSTEM_FileWrite,
	ATMO_LFS_FILESYSTEM_FileSeek,
	ATMO_LFS_FILESYSTEM_FileTell,
	ATMO_LFS_FILESYSTEM_FileSize,
	ATMO_LFS_FILESYSTEM_FileRewind,
	ATMO_LFS_FILESYSTEM_FileTruncate,
	ATMO_LFS_FILESYSTEM_DirMk
};

int lfs_read( const struct lfs_config *c, lfs_block_t block,
              lfs_off_t off, void *buffer, lfs_size_t size )
{
	uint32_t instance = *( ( uint32_t * )c->context );

	if ( ATMO_BLOCK_Read( instance, block, off, buffer, size ) != ATMO_BLOCK_Status_Success )
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

int lfs_prog( const struct lfs_config *c, lfs_block_t block,
              lfs_off_t off, const void *buffer, lfs_size_t size )
{
	uint32_t instance = *( ( uint32_t * )c->context );

	if ( ATMO_BLOCK_Program( instance, block, off, ( void * )buffer, size ) != ATMO_BLOCK_Status_Success )
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

int lfs_erase( const struct lfs_config *c, lfs_block_t block )
{
	uint32_t instance = *( ( uint32_t * )c->context );

	if ( ATMO_BLOCK_Erase( instance, block ) != ATMO_BLOCK_Status_Success )
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

int lfs_sync( const struct lfs_config *c )
{
	uint32_t instance = *( ( uint32_t * )c->context );

	if ( ATMO_BLOCK_Sync( instance ) != ATMO_BLOCK_Status_Success )
	{
		return -1;
	}
	else
	{
		return 0;
	}
}


ATMO_Status_t ATMO_LFS_FILESYSTEM_AddDriverInstance( ATMO_DriverInstanceHandle_t *instanceNumber )
{
	ATMO_DriverInstanceData_t *driver = ( ATMO_DriverInstanceData_t * )ATMO_Malloc( sizeof( ATMO_DriverInstanceData_t ) );
	driver->name = "Filesystem";
	driver->initialized = false;
	driver->instanceNumber = *instanceNumber;
	driver->argument = ATMO_Malloc( sizeof( uint32_t ) );
	*( ( uint32_t * )driver->argument ) = currentNumFileSystems++;

	return ATMO_FILESYSTEM_AddDriverInstance( &lfsFilesystemDriverInstance, driver, instanceNumber );
}
ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_Init( ATMO_DriverInstanceData_t *instance, ATMO_DriverInstanceHandle_t blockDriverHandle )
{
	uint32_t fsNum = *( ( uint32_t * )instance->argument );

	if ( fsNum >= NUMBER_OF_LITTLEFS_FILESYSTEM_DRIVER_INSTANCES )
	{
		return ATMO_FILESYSTEM_Status_Invalid;
	}

	ATMO_BLOCK_DeviceInfo_t devInfo;
	ATMO_BLOCK_GetDeviceInfo( blockDriverHandle, &devInfo );

	configs[fsNum].context = malloc( sizeof( uint32_t ) );
	*( ( uint32_t * )configs[fsNum].context ) = blockDriverHandle;
	configs[fsNum].block_count = devInfo.blockCount;
	configs[fsNum].block_size = devInfo.blockSize;
	configs[fsNum].read_size = devInfo.readSize;
	configs[fsNum].prog_size = devInfo.progSize;
	configs[fsNum].lookahead = 128;
	configs[fsNum].erase = lfs_erase;
	configs[fsNum].prog = lfs_prog;
	configs[fsNum].read = lfs_read;
	configs[fsNum].sync = lfs_sync;
	configs[fsNum].file_buffer = NULL;
	configs[fsNum].lookahead_buffer = NULL;
	configs[fsNum].prog_buffer = NULL;
	configs[fsNum].read_buffer = NULL;

	return ATMO_FILESYSTEM_Status_Success;
}

ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_SetConfiguration( ATMO_DriverInstanceData_t *instance, const ATMO_FILESYSTEM_Config_t *config )
{
	return ATMO_FILESYSTEM_Status_Success;
}

ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_Wipe( ATMO_DriverInstanceData_t *instance )
{
	uint32_t fsNum = *( ( uint32_t * )instance->argument );

	// Try to unmount
	ATMO_LFS_FILESYSTEM_Unmount( instance );

	lfs_format( &filesystems[fsNum], &configs[fsNum] );

	return ATMO_LFS_FILESYSTEM_Mount( instance );
}

ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_Mount( ATMO_DriverInstanceData_t *instance )
{
	uint32_t fsNum = *( ( uint32_t * )instance->argument );

	if ( fsNum >= NUMBER_OF_LITTLEFS_FILESYSTEM_DRIVER_INSTANCES )
	{
		return ATMO_FILESYSTEM_Status_Invalid;
	}

	int err = lfs_mount( &filesystems[fsNum], &configs[fsNum] );

	// This will happen the first time
	if ( err )
	{
		lfs_format( &filesystems[fsNum], &configs[fsNum] );

		if ( lfs_mount( &filesystems[fsNum], &configs[fsNum] ) != 0 )
		{
			return ATMO_FILESYSTEM_Status_Fail;
		}
	}

	return ATMO_FILESYSTEM_Status_Success;
}

ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_Unmount( ATMO_DriverInstanceData_t *instance )
{
	uint32_t fsNum = *( ( uint32_t * )instance->argument );

	if ( fsNum >= NUMBER_OF_LITTLEFS_FILESYSTEM_DRIVER_INSTANCES )
	{
		return ATMO_FILESYSTEM_Status_Invalid;
	}

	if ( lfs_unmount( &filesystems[fsNum] ) != 0 )
	{
		return ATMO_FILESYSTEM_Status_Fail;
	}

	return ATMO_FILESYSTEM_Status_Success;
}

ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_Remove( ATMO_DriverInstanceData_t *instance, const char *path )
{
	uint32_t fsNum = *( ( uint32_t * )instance->argument );

	if ( fsNum >= NUMBER_OF_LITTLEFS_FILESYSTEM_DRIVER_INSTANCES )
	{
		return ATMO_FILESYSTEM_Status_Invalid;
	}

	if ( lfs_remove( &filesystems[fsNum], path ) != 0 )
	{
		return ATMO_FILESYSTEM_Status_Fail;
	}

	return ATMO_FILESYSTEM_Status_Success;
}

ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_Rename( ATMO_DriverInstanceData_t *instance, const char *oldPath, const char *newPath )
{
	uint32_t fsNum = *( ( uint32_t * )instance->argument );

	if ( fsNum >= NUMBER_OF_LITTLEFS_FILESYSTEM_DRIVER_INSTANCES )
	{
		return ATMO_FILESYSTEM_Status_Invalid;
	}

	if ( lfs_rename( &filesystems[fsNum], oldPath, newPath ) != 0 )
	{
		return ATMO_FILESYSTEM_Status_Fail;
	}

	return ATMO_FILESYSTEM_Status_Success;
}

ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_Stat( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_Info_t *info, const char *path )
{
	uint32_t fsNum = *( ( uint32_t * )instance->argument );

	if ( fsNum >= NUMBER_OF_LITTLEFS_FILESYSTEM_DRIVER_INSTANCES )
	{
		return ATMO_FILESYSTEM_Status_Invalid;
	}

	struct lfs_info lfsInfo;

	if ( lfs_stat( &filesystems[fsNum], path, &lfsInfo ) != 0 )
	{
		return ATMO_FILESYSTEM_Status_Fail;
	}

	info->type = ( ATMO_FILESYSTEM_Type_t )lfsInfo.type;
	info->size = lfsInfo.size;
	strcpy( info->name, lfsInfo.name );

	return ATMO_FILESYSTEM_Status_Success;
}

ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_FileOpen( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file, const char *path, int flags )
{
	uint32_t fsNum = *( ( uint32_t * )instance->argument );

	if ( fsNum >= NUMBER_OF_LITTLEFS_FILESYSTEM_DRIVER_INSTANCES )
	{
		return ATMO_FILESYSTEM_Status_Invalid;
	}

	file->data = ATMO_Malloc( sizeof( struct lfs_file ) );

	if ( lfs_file_open( &filesystems[fsNum], ( lfs_file_t * )file->data, path, flags ) != 0 )
	{
		ATMO_Free( file->data );
		return ATMO_FILESYSTEM_Status_Fail;
	}

	return ATMO_FILESYSTEM_Status_Success;
}

ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_FileClose( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file )
{
	uint32_t fsNum = *( ( uint32_t * )instance->argument );

	if ( fsNum >= NUMBER_OF_LITTLEFS_FILESYSTEM_DRIVER_INSTANCES )
	{
		return ATMO_FILESYSTEM_Status_Invalid;
	}

	if ( lfs_file_close( &filesystems[fsNum], ( lfs_file_t * )file->data ) != 0 )
	{
		ATMO_Free( file->data );
		return ATMO_FILESYSTEM_Status_Fail;
	}

	ATMO_Free( file->data );

	return ATMO_FILESYSTEM_Status_Success;
}

ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_FileSync( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file )
{
	uint32_t fsNum = *( ( uint32_t * )instance->argument );

	if ( fsNum >= NUMBER_OF_LITTLEFS_FILESYSTEM_DRIVER_INSTANCES )
	{
		return ATMO_FILESYSTEM_Status_Invalid;
	}

	if ( lfs_file_sync( &filesystems[fsNum], ( lfs_file_t * )file->data ) != 0 )
	{
		return ATMO_FILESYSTEM_Status_Fail;
	}

	return ATMO_FILESYSTEM_Status_Success;
}

ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_FileRead( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file, void *buffer, uint32_t size )
{
	uint32_t fsNum = *( ( uint32_t * )instance->argument );

	if ( fsNum >= NUMBER_OF_LITTLEFS_FILESYSTEM_DRIVER_INSTANCES )
	{
		return ATMO_FILESYSTEM_Status_Invalid;
	}

	if ( lfs_file_read( &filesystems[fsNum], ( lfs_file_t * )file->data, buffer, size ) != size )
	{
		return ATMO_FILESYSTEM_Status_Fail;
	}

	return ATMO_FILESYSTEM_Status_Success;
}

ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_FileWrite( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file, void *buffer, uint32_t size )
{
	uint32_t fsNum = *( ( uint32_t * )instance->argument );

	if ( fsNum >= NUMBER_OF_LITTLEFS_FILESYSTEM_DRIVER_INSTANCES )
	{
		return ATMO_FILESYSTEM_Status_Invalid;
	}

	if ( lfs_file_write( &filesystems[fsNum], ( lfs_file_t * )file->data, buffer, size ) != size )
	{
		return ATMO_FILESYSTEM_Status_Fail;
	}

	return ATMO_FILESYSTEM_Status_Success;
}

ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_FileSeek( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file, uint32_t offset )
{
	uint32_t fsNum = *( ( uint32_t * )instance->argument );

	if ( fsNum >= NUMBER_OF_LITTLEFS_FILESYSTEM_DRIVER_INSTANCES )
	{
		return ATMO_FILESYSTEM_Status_Invalid;
	}

	if ( lfs_file_seek( &filesystems[fsNum], ( lfs_file_t * )file->data, offset, LFS_SEEK_SET ) != 0 )
	{
		return ATMO_FILESYSTEM_Status_Fail;
	}

	return ATMO_FILESYSTEM_Status_Success;
}
ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_FileTell( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file, uint32_t *currPos )
{
	uint32_t fsNum = *( ( uint32_t * )instance->argument );

	if ( fsNum >= NUMBER_OF_LITTLEFS_FILESYSTEM_DRIVER_INSTANCES )
	{
		return ATMO_FILESYSTEM_Status_Invalid;
	}

	*currPos = lfs_file_tell( &filesystems[fsNum], ( lfs_file_t * )file->data );

	return ATMO_FILESYSTEM_Status_Success;
}

ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_FileSize( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file, uint32_t *size )
{
	uint32_t fsNum = *( ( uint32_t * )instance->argument );

	if ( fsNum >= NUMBER_OF_LITTLEFS_FILESYSTEM_DRIVER_INSTANCES )
	{
		return ATMO_FILESYSTEM_Status_Invalid;
	}

	*size = lfs_file_size( &filesystems[fsNum], ( lfs_file_t * )file->data );

	return ATMO_FILESYSTEM_Status_Success;
}

ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_FileRewind( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file )
{
	uint32_t fsNum = *( ( uint32_t * )instance->argument );

	if ( fsNum >= NUMBER_OF_LITTLEFS_FILESYSTEM_DRIVER_INSTANCES )
	{
		return ATMO_FILESYSTEM_Status_Invalid;
	}

	lfs_file_rewind( &filesystems[fsNum], ( lfs_file_t * )file->data );

	return ATMO_FILESYSTEM_Status_Success;
}

ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_FileTruncate( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file, uint32_t size )
{
	uint32_t fsNum = *( ( uint32_t * )instance->argument );

	if ( fsNum >= NUMBER_OF_LITTLEFS_FILESYSTEM_DRIVER_INSTANCES )
	{
		return ATMO_FILESYSTEM_Status_Invalid;
	}

	if ( lfs_file_truncate( &filesystems[fsNum], ( lfs_file_t * )file->data, size ) != 0 )
	{
		return ATMO_FILESYSTEM_Status_Fail;
	}

	return ATMO_FILESYSTEM_Status_Success;
}

ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_DirMk( ATMO_DriverInstanceData_t *instance, const char *path )
{
	uint32_t fsNum = *( ( uint32_t * )instance->argument );

	if ( fsNum >= NUMBER_OF_LITTLEFS_FILESYSTEM_DRIVER_INSTANCES )
	{
		return ATMO_FILESYSTEM_Status_Invalid;
	}

	if ( lfs_mkdir( &filesystems[fsNum], path ) != 0 )
	{
		return ATMO_FILESYSTEM_Status_Fail;
	}

	return ATMO_FILESYSTEM_Status_Success;
}
