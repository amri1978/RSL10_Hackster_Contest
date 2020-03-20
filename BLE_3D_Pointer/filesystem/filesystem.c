/**
 ******************************************************************************
 * @file    filesystem.c
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - Core Filesystem Driver
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

#include "filesystem.h"
#include "../app_src/atmosphere_platform.h"

#ifdef ATMO_SLIM_STACK
	#ifndef NUMBER_OF_FILESYSTEM_DRIVER_INSTANCES
		#define NUMBER_OF_FILESYSTEM_DRIVER_INSTANCES 1
	#endif
#endif

#ifndef NUMBER_OF_FILESYSTEM_DRIVER_INSTANCES
#define NUMBER_OF_FILESYSTEM_DRIVER_INSTANCES 1
#endif

static ATMO_FILESYSTEM_Config_t filesystemDefaultConfig =
{
	.numRetries = 1,
	.retryDelayMs = 50,
};

ATMO_DriverInstanceHandle_t numberOfFilesystemDriverInstance = 0;

const ATMO_FILESYSTEM_DriverInstance_t *filesystemInstances[NUMBER_OF_FILESYSTEM_DRIVER_INSTANCES];
ATMO_DriverInstanceData_t *filesystemInstancesData[NUMBER_OF_FILESYSTEM_DRIVER_INSTANCES];
ATMO_FILESYSTEM_Config_t filesystemConfiguration[NUMBER_OF_FILESYSTEM_DRIVER_INSTANCES];


ATMO_Status_t ATMO_FILESYSTEM_AddDriverInstance( const ATMO_FILESYSTEM_DriverInstance_t *driverInstance, ATMO_DriverInstanceData_t *driverInstanceData, ATMO_DriverInstanceHandle_t *instanceNumber )
{
	if ( !( numberOfFilesystemDriverInstance < NUMBER_OF_FILESYSTEM_DRIVER_INSTANCES ) )
	{
		return ATMO_Status_OutOfMemory;
	}

	filesystemInstances[numberOfFilesystemDriverInstance] = driverInstance;
	filesystemInstancesData[numberOfFilesystemDriverInstance] = driverInstanceData;
	*instanceNumber = numberOfFilesystemDriverInstance;

	numberOfFilesystemDriverInstance++;

	return ATMO_Status_Success;
}

ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_Init( ATMO_DriverInstanceHandle_t instanceNumber, ATMO_DriverInstanceHandle_t blockDriverHandle )
{
	if ( !( instanceNumber < numberOfFilesystemDriverInstance ) )
	{
		return ATMO_FILESYSTEM_Status_Invalid;
	}

	// Init configuration
	filesystemConfiguration[instanceNumber] = filesystemDefaultConfig;

	return filesystemInstances[instanceNumber]->Init( filesystemInstancesData[instanceNumber], blockDriverHandle );
}

ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_SetConfiguration( ATMO_DriverInstanceHandle_t instanceNumber, const ATMO_FILESYSTEM_Config_t *config )
{
	if ( !( instanceNumber < numberOfFilesystemDriverInstance ) )
	{
		return ATMO_FILESYSTEM_Status_Invalid;
	}

	memcpy( &filesystemConfiguration[instanceNumber], config, sizeof( ATMO_FILESYSTEM_Config_t ) );

	return filesystemInstances[instanceNumber]->SetConfiguration( filesystemInstancesData[instanceNumber], config );
}

ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_Wipe( ATMO_DriverInstanceHandle_t instanceNumber )
{
	if ( !( instanceNumber < numberOfFilesystemDriverInstance ) )
	{
		return ATMO_FILESYSTEM_Status_Invalid;
	}

	ATMO_FILESYSTEM_Status_t status = ATMO_FILESYSTEM_Status_Fail;

	unsigned int i;

	for ( i = 0; i < filesystemConfiguration[instanceNumber].numRetries && status != ATMO_FILESYSTEM_Status_Success; i++ )
	{
		status = filesystemInstances[instanceNumber]->Wipe( filesystemInstancesData[instanceNumber] );

		if ( status != ATMO_FILESYSTEM_Status_Success )
		{
			ATMO_PLATFORM_DelayMilliseconds( filesystemConfiguration[instanceNumber].retryDelayMs );
		}
	}

	return status;
}


ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_Mount( ATMO_DriverInstanceHandle_t instanceNumber )
{
	if ( !( instanceNumber < numberOfFilesystemDriverInstance ) )
	{
		return ATMO_FILESYSTEM_Status_Invalid;
	}

	ATMO_FILESYSTEM_Status_t status = ATMO_FILESYSTEM_Status_Fail;

	unsigned int i;

	for ( i = 0; i < filesystemConfiguration[instanceNumber].numRetries && status != ATMO_FILESYSTEM_Status_Success; i++ )
	{
		status = filesystemInstances[instanceNumber]->Mount( filesystemInstancesData[instanceNumber] );

		if ( status != ATMO_FILESYSTEM_Status_Success )
		{
			ATMO_PLATFORM_DelayMilliseconds( filesystemConfiguration[instanceNumber].retryDelayMs );
		}
	}

	return status;
}

ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_Unmount( ATMO_DriverInstanceHandle_t instanceNumber )
{
	if ( !( instanceNumber < numberOfFilesystemDriverInstance ) )
	{
		return ATMO_FILESYSTEM_Status_Invalid;
	}

	ATMO_FILESYSTEM_Status_t status = ATMO_FILESYSTEM_Status_Fail;

	unsigned int i;

	for ( i = 0; i < filesystemConfiguration[instanceNumber].numRetries && status != ATMO_FILESYSTEM_Status_Success; i++ )
	{
		status = filesystemInstances[instanceNumber]->Unmount( filesystemInstancesData[instanceNumber] );

		if ( status != ATMO_FILESYSTEM_Status_Success )
		{
			ATMO_PLATFORM_DelayMilliseconds( filesystemConfiguration[instanceNumber].retryDelayMs );
		}
	}

	return status;
}

ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_Remove( ATMO_DriverInstanceHandle_t instanceNumber, const char *path )
{
	if ( !( instanceNumber < numberOfFilesystemDriverInstance ) )
	{
		return ATMO_FILESYSTEM_Status_Invalid;
	}

	unsigned int i;

	ATMO_FILESYSTEM_Status_t status = ATMO_FILESYSTEM_Status_Fail;

	for ( i = 0; i < filesystemConfiguration[instanceNumber].numRetries && status != ATMO_FILESYSTEM_Status_Success; i++ )
	{
		status = filesystemInstances[instanceNumber]->Remove( filesystemInstancesData[instanceNumber], path );

		if ( status != ATMO_FILESYSTEM_Status_Success )
		{
			ATMO_PLATFORM_DelayMilliseconds( filesystemConfiguration[instanceNumber].retryDelayMs );
		}
	}

	return status;
}

ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_Rename( ATMO_DriverInstanceHandle_t instanceNumber, const char *oldPath, const char *newPath )
{
	if ( !( instanceNumber < numberOfFilesystemDriverInstance ) )
	{
		return ATMO_FILESYSTEM_Status_Invalid;
	}

	ATMO_FILESYSTEM_Status_t status = ATMO_FILESYSTEM_Status_Fail;

	unsigned int i;

	for ( i = 0; i < filesystemConfiguration[instanceNumber].numRetries && status != ATMO_FILESYSTEM_Status_Success; i++ )
	{
		status = filesystemInstances[instanceNumber]->Rename( filesystemInstancesData[instanceNumber], oldPath, newPath );

		if ( status != ATMO_FILESYSTEM_Status_Success )
		{
			ATMO_PLATFORM_DelayMilliseconds( filesystemConfiguration[instanceNumber].retryDelayMs );
		}
	}

	return status;
}

ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_Stat( ATMO_DriverInstanceHandle_t instanceNumber, ATMO_FILESYSTEM_Info_t *info, const char *path )
{
	if ( !( instanceNumber < numberOfFilesystemDriverInstance ) )
	{
		return ATMO_FILESYSTEM_Status_Invalid;
	}

	ATMO_FILESYSTEM_Status_t status = ATMO_FILESYSTEM_Status_Fail;

	unsigned int i;

	for ( i = 0; i < filesystemConfiguration[instanceNumber].numRetries && status != ATMO_FILESYSTEM_Status_Success; i++ )
	{
		status = filesystemInstances[instanceNumber]->Stat( filesystemInstancesData[instanceNumber], info, path );

		if ( status != ATMO_FILESYSTEM_Status_Success )
		{
			ATMO_PLATFORM_DelayMilliseconds( filesystemConfiguration[instanceNumber].retryDelayMs );
		}
	}

	return status;
}

ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_FileOpen( ATMO_DriverInstanceHandle_t instanceNumber, ATMO_FILESYSTEM_File_t *file, const char *path, int flags )
{
	if ( !( instanceNumber < numberOfFilesystemDriverInstance ) )
	{
		return ATMO_FILESYSTEM_Status_Invalid;
	}

	ATMO_FILESYSTEM_Status_t status = ATMO_FILESYSTEM_Status_Fail;

	unsigned int i;

	for ( i = 0; i < filesystemConfiguration[instanceNumber].numRetries && status != ATMO_FILESYSTEM_Status_Success; i++ )
	{
		status = filesystemInstances[instanceNumber]->FileOpen( filesystemInstancesData[instanceNumber], file, path, flags );

		if ( flags & ATMO_NORETRY )
		{
			break;
		}

		if ( status != ATMO_FILESYSTEM_Status_Success )
		{
#ifndef ATMO_SLIM_STACK
			ATMO_PLATFORM_DebugPrint( "Error opening file %s\r\n", path );
#endif
			ATMO_PLATFORM_DelayMilliseconds( filesystemConfiguration[instanceNumber].retryDelayMs );
		}
	}

	return status;
}

ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_FileClose( ATMO_DriverInstanceHandle_t instanceNumber, ATMO_FILESYSTEM_File_t *file )
{
	if ( !( instanceNumber < numberOfFilesystemDriverInstance ) )
	{
		return ATMO_FILESYSTEM_Status_Invalid;
	}

	ATMO_FILESYSTEM_Status_t status = ATMO_FILESYSTEM_Status_Fail;

	unsigned int i;

	for ( i = 0; i < filesystemConfiguration[instanceNumber].numRetries && status != ATMO_FILESYSTEM_Status_Success; i++ )
	{
		status = filesystemInstances[instanceNumber]->FileClose( filesystemInstancesData[instanceNumber], file );

		if ( status != ATMO_FILESYSTEM_Status_Success )
		{
			ATMO_PLATFORM_DelayMilliseconds( filesystemConfiguration[instanceNumber].retryDelayMs );
		}
	}

	return status;
}

ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_FileSync( ATMO_DriverInstanceHandle_t instanceNumber, ATMO_FILESYSTEM_File_t *file )
{
	if ( !( instanceNumber < numberOfFilesystemDriverInstance ) )
	{
		return ATMO_FILESYSTEM_Status_Invalid;
	}

	ATMO_FILESYSTEM_Status_t status = ATMO_FILESYSTEM_Status_Fail;

	unsigned int i;

	for ( i = 0; i < filesystemConfiguration[instanceNumber].numRetries && status != ATMO_FILESYSTEM_Status_Success; i++ )
	{
		status = filesystemInstances[instanceNumber]->FileSync( filesystemInstancesData[instanceNumber], file );

		if ( status != ATMO_FILESYSTEM_Status_Success )
		{
			ATMO_PLATFORM_DelayMilliseconds( filesystemConfiguration[instanceNumber].retryDelayMs );
		}
	}

	return status;
}

ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_FileRead( ATMO_DriverInstanceHandle_t instanceNumber, ATMO_FILESYSTEM_File_t *file, void *buffer, uint32_t size )
{
	if ( !( instanceNumber < numberOfFilesystemDriverInstance ) )
	{
		return ATMO_FILESYSTEM_Status_Invalid;
	}

	ATMO_FILESYSTEM_Status_t status = ATMO_FILESYSTEM_Status_Fail;

	unsigned int i;

	for ( i = 0; i < filesystemConfiguration[instanceNumber].numRetries && status != ATMO_FILESYSTEM_Status_Success; i++ )
	{
		status = filesystemInstances[instanceNumber]->FileRead( filesystemInstancesData[instanceNumber], file, buffer, size );

		if ( status != ATMO_FILESYSTEM_Status_Success )
		{
			ATMO_PLATFORM_DelayMilliseconds( filesystemConfiguration[instanceNumber].retryDelayMs );
		}
	}

	return status;
}

ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_FileWrite( ATMO_DriverInstanceHandle_t instanceNumber, ATMO_FILESYSTEM_File_t *file, void *buffer, uint32_t size )
{
	if ( !( instanceNumber < numberOfFilesystemDriverInstance ) )
	{
		return ATMO_FILESYSTEM_Status_Invalid;
	}

	ATMO_FILESYSTEM_Status_t status = ATMO_FILESYSTEM_Status_Fail;

	unsigned int i;

	for ( i = 0; i < filesystemConfiguration[instanceNumber].numRetries && status != ATMO_FILESYSTEM_Status_Success; i++ )
	{
		status = filesystemInstances[instanceNumber]->FileWrite( filesystemInstancesData[instanceNumber], file, buffer, size );

		if ( status != ATMO_FILESYSTEM_Status_Success )
		{
			ATMO_PLATFORM_DelayMilliseconds( filesystemConfiguration[instanceNumber].retryDelayMs );
		}
	}

	return status;
}

ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_FileSeek( ATMO_DriverInstanceHandle_t instanceNumber, ATMO_FILESYSTEM_File_t *file, uint32_t offset )
{
	if ( !( instanceNumber < numberOfFilesystemDriverInstance ) )
	{
		return ATMO_FILESYSTEM_Status_Invalid;
	}

	ATMO_FILESYSTEM_Status_t status = ATMO_FILESYSTEM_Status_Fail;

	unsigned int i;

	for ( i = 0; i < filesystemConfiguration[instanceNumber].numRetries && status != ATMO_FILESYSTEM_Status_Success; i++ )
	{
		status = filesystemInstances[instanceNumber]->FileSeek( filesystemInstancesData[instanceNumber], file, offset );

		if ( status != ATMO_FILESYSTEM_Status_Success )
		{
			ATMO_PLATFORM_DelayMilliseconds( filesystemConfiguration[instanceNumber].retryDelayMs );
		}
	}

	return status;
}

ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_FileTell( ATMO_DriverInstanceHandle_t instanceNumber, ATMO_FILESYSTEM_File_t *file, uint32_t *currPos )
{
	if ( !( instanceNumber < numberOfFilesystemDriverInstance ) )
	{
		return ATMO_FILESYSTEM_Status_Invalid;
	}

	ATMO_FILESYSTEM_Status_t status = ATMO_FILESYSTEM_Status_Fail;

	unsigned int i;

	for ( i = 0; i < filesystemConfiguration[instanceNumber].numRetries && status != ATMO_FILESYSTEM_Status_Success; i++ )
	{
		status = filesystemInstances[instanceNumber]->FileTell( filesystemInstancesData[instanceNumber], file, currPos );

		if ( status != ATMO_FILESYSTEM_Status_Success )
		{
			ATMO_PLATFORM_DelayMilliseconds( filesystemConfiguration[instanceNumber].retryDelayMs );
		}
	}

	return status;
}

ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_FileSize( ATMO_DriverInstanceHandle_t instanceNumber, ATMO_FILESYSTEM_File_t *file, uint32_t *size )
{
	if ( !( instanceNumber < numberOfFilesystemDriverInstance ) )
	{
		return ATMO_FILESYSTEM_Status_Invalid;
	}

	ATMO_FILESYSTEM_Status_t status = ATMO_FILESYSTEM_Status_Fail;

	unsigned int i;

	for ( i = 0; i < filesystemConfiguration[instanceNumber].numRetries && status != ATMO_FILESYSTEM_Status_Success; i++ )
	{
		status = filesystemInstances[instanceNumber]->FileSize( filesystemInstancesData[instanceNumber], file, size );

		if ( status != ATMO_FILESYSTEM_Status_Success )
		{
			ATMO_PLATFORM_DelayMilliseconds( filesystemConfiguration[instanceNumber].retryDelayMs );
		}
	}

	return status;
}

ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_FileRewind( ATMO_DriverInstanceHandle_t instanceNumber, ATMO_FILESYSTEM_File_t *file )
{
	if ( !( instanceNumber < numberOfFilesystemDriverInstance ) )
	{
		return ATMO_FILESYSTEM_Status_Invalid;
	}

	ATMO_FILESYSTEM_Status_t status = ATMO_FILESYSTEM_Status_Fail;

	unsigned int i;

	for ( i = 0; i < filesystemConfiguration[instanceNumber].numRetries && status != ATMO_FILESYSTEM_Status_Success; i++ )
	{
		status = filesystemInstances[instanceNumber]->FileRewind( filesystemInstancesData[instanceNumber], file );

		if ( status != ATMO_FILESYSTEM_Status_Success )
		{
			ATMO_PLATFORM_DelayMilliseconds( filesystemConfiguration[instanceNumber].retryDelayMs );
		}
	}

	return status;
}

ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_FileTruncate( ATMO_DriverInstanceHandle_t instanceNumber, ATMO_FILESYSTEM_File_t *file, uint32_t size )
{
	if ( !( instanceNumber < numberOfFilesystemDriverInstance ) )
	{
		return ATMO_FILESYSTEM_Status_Invalid;
	}

	ATMO_FILESYSTEM_Status_t status = ATMO_FILESYSTEM_Status_Fail;

	unsigned int i;

	for ( i = 0; i < filesystemConfiguration[instanceNumber].numRetries && status != ATMO_FILESYSTEM_Status_Success; i++ )
	{
		status = filesystemInstances[instanceNumber]->FileTruncate( filesystemInstancesData[instanceNumber], file, size );

		if ( status != ATMO_FILESYSTEM_Status_Success )
		{
			ATMO_PLATFORM_DelayMilliseconds( filesystemConfiguration[instanceNumber].retryDelayMs );
		}
	}

	return status;
}

ATMO_FILESYSTEM_Status_t ATMO_FILESYSTEM_DirMk( ATMO_DriverInstanceHandle_t instanceNumber, const char *path )
{
	if ( !( instanceNumber < numberOfFilesystemDriverInstance ) )
	{
		return ATMO_FILESYSTEM_Status_Invalid;
	}

	ATMO_FILESYSTEM_Status_t status = ATMO_FILESYSTEM_Status_Fail;

	unsigned int i;

	for ( i = 0; i < filesystemConfiguration[instanceNumber].numRetries && status != ATMO_FILESYSTEM_Status_Success; i++ )
	{
		status = filesystemInstances[instanceNumber]->DirMk( filesystemInstancesData[instanceNumber], path );

		if ( status != ATMO_FILESYSTEM_Status_Success )
		{
			ATMO_PLATFORM_DelayMilliseconds( filesystemConfiguration[instanceNumber].retryDelayMs );
		}
	}

	return status;
}


