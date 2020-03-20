/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef __ATMO_FILESYSTEM_LFS__H
#define __ATMO_FILESYSTEM_LFS__H


/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stddef.h>
#include "filesystem.h"


#ifdef __cplusplus
extern "C" {
#endif

/* Exported Constants --------------------------------------------------------*/

/* Exported Macros -----------------------------------------------------------*/

/* Exported Types ------------------------------------------------------------*/

ATMO_Status_t ATMO_LFS_FILESYSTEM_AddDriverInstance( ATMO_DriverInstanceHandle_t *instanceNumber );
ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_Init( ATMO_DriverInstanceData_t *instance, ATMO_DriverInstanceHandle_t blockDriverHandle );
ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_SetConfiguration( ATMO_DriverInstanceData_t *instance, const ATMO_FILESYSTEM_Config_t *config );
ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_Wipe( ATMO_DriverInstanceData_t *instance );
ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_Mount( ATMO_DriverInstanceData_t *instance );
ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_Unmount( ATMO_DriverInstanceData_t *instance );
ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_Remove( ATMO_DriverInstanceData_t *instance, const char *path );
ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_Rename( ATMO_DriverInstanceData_t *instance, const char *oldPath, const char *newPath );
ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_Stat( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_Info_t *info, const char *path );
ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_FileOpen( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file, const char *path, int flags );
ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_FileClose( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file );
ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_FileSync( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file );
ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_FileRead( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file, void *buffer, uint32_t size );
ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_FileWrite( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file, void *buffer, uint32_t size );
ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_FileSeek( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file, uint32_t offset );
ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_FileTell( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file, uint32_t *currPos );
ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_FileSize( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file, uint32_t *size );
ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_FileRewind( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file );
ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_FileTruncate( ATMO_DriverInstanceData_t *instance, ATMO_FILESYSTEM_File_t *file, uint32_t size );
ATMO_FILESYSTEM_Status_t ATMO_LFS_FILESYSTEM_DirMk( ATMO_DriverInstanceData_t *instance, const char *path );



#ifdef __cplusplus
}
#endif

#endif /* __ATMO_FILESYSTEM_LFS__H */
