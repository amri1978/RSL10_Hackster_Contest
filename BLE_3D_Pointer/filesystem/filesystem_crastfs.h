/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef __ATMO_FILESYSTEM_CRASTFS__H
#define __ATMO_FILESYSTEM_CRASTFS__H


/* Includes ------------------------------------------------------------------*/
#include "filesystem.h"


#ifdef __cplusplus
extern "C" {
#endif

#define ATMO_CRASTFS_MAGICWORD_OFFSET (0)
#define ATMO_CRASTFS_MAGICWORD_SIZE (4)

#define ATMO_CRASTFS_REGISTERED_OFFSET (ATMO_CRASTFS_MAGICWORD_OFFSET + ATMO_CRASTFS_MAGICWORD_SIZE)
#define ATMO_CRASTFS_REGISTERED_SIZE (1)

#define ATMO_CRASTFS_TOKEN_OFFSET (ATMO_CRASTFS_REGISTERED_OFFSET + ATMO_CRASTFS_REGISTERED_SIZE)
#define ATMO_CRASTFS_TOKEN_SIZE (ATMO_CLOUD_TOKEN_LENGTH)

#define ATMO_CRASTFS_DEVICE_UUID_OFFSET (ATMO_CRASTFS_TOKEN_OFFSET + ATMO_CRASTFS_TOKEN_SIZE)
#define ATMO_CRASTFS_DEVICE_UUID_SIZE (ATMO_CLOUD_UUID_LENGTH)

#define ATMO_CRASTFS_URL_OFFSET (ATMO_CRASTFS_DEVICE_UUID_OFFSET + ATMO_CRASTFS_DEVICE_UUID_SIZE)
#define ATMO_CRASTFS_URL_SIZE (ATMO_CLOUD_MAX_URL_LENGTH)

// Unfortunately, these were stored as strings
// Need to maintain backwards compatibility. Whoops!
#define ATMO_CRASTFS_PROJECT_UUID_OFFSET (ATMO_CRASTFS_URL_OFFSET + ATMO_CRASTFS_URL_SIZE)
#define ATMO_CRASTFS_PROJECT_UUID_SIZE (36)

#define ATMO_CRASTFS_BUILD_UUID_OFFSET (ATMO_CRASTFS_PROJECT_UUID_OFFSET + ATMO_CRASTFS_PROJECT_UUID_SIZE)
#define ATMO_CRASTFS_BUILD_UUID_SIZE (36)

#define ATMO_CRASTFS_OTA_COMPLETE_OFFSET (ATMO_CRASTFS_BUILD_UUID_OFFSET + ATMO_CRASTFS_BUILD_UUID_SIZE)
#define ATMO_CRASTFS_OTA_COMPLETE_SIZE (1)

// Two slots for any other information (SSID/PASS for example)
#define ATMO_CRASTFS_EXTRA1_OFFSET (ATMO_CRASTFS_OTA_COMPLETE_OFFSET + ATMO_CRASTFS_OTA_COMPLETE_SIZE)
#define ATMO_CRASTFS_EXTRA1_SIZE (33)

#define ATMO_CRASTFS_EXTRA2_OFFSET (ATMO_CRASTFS_EXTRA1_OFFSET + ATMO_CRASTFS_EXTRA1_SIZE)
#define ATMO_CRASTFS_EXTRA2_SIZE (33)

#define ATMO_CRASTFS_SIZE (ATMO_CRASTFS_EXTRA2_OFFSET + ATMO_CRASTFS_EXTRA2_SIZE)

/* Exported Constants --------------------------------------------------------*/

/* Exported Macros -----------------------------------------------------------*/

/* Exported Types ------------------------------------------------------------*/

ATMO_Status_t ATMO_CRASTFS_FILESYSTEM_AddDriverInstance( ATMO_DriverInstanceHandle_t *instanceNumber );
ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_Init( ATMO_DriverInstanceData_t *instance, ATMO_DriverInstanceHandle_t blockDriverHandle );
ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_SetConfiguration( ATMO_DriverInstanceData_t *instance, const ATMO_FILESYSTEM_Config_t *config );
ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_Wipe( ATMO_DriverInstanceData_t *instance );
ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_Mount( ATMO_DriverInstanceData_t *instance );
ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_Unmount( ATMO_DriverInstanceData_t *instance );
ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_Remove( ATMO_DriverInstanceData_t *instance, const char *path );
ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_Rename( ATMO_DriverInstanceData_t *instance, const char *oldPath, const char *newPath );
ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_Stat( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_Info_t *info, const char *path );
ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_FileOpen( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file, const char *path, int flags );
ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_FileClose( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file );
ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_FileSync( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file );
ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_FileRead( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file, void *buffer, uint32_t size );
ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_FileWrite( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file, void *buffer, uint32_t size );
ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_FileSeek( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file, uint32_t offset );
ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_FileTell( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file, uint32_t *currPos );
ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_FileSize( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file, uint32_t *size );
ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_FileRewind( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file );
ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_FileTruncate( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file, uint32_t size );
ATMO_FILESYSTEM_Status_t ATMO_CRASTFS_FILESYSTEM_DirMk( ATMO_DriverInstanceData_t *instance, const char *path );



#ifdef __cplusplus
}
#endif

#endif /* __ATMO_FILESYSTEM_CRASTFS__H */
