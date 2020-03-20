/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef __ATMO_ONSEMI_BLOCK__H
#define __ATMO_ONSEMI_BLOCK__H


/* Includes ------------------------------------------------------------------*/
#include "../app_src/atmosphere_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported Constants --------------------------------------------------------*/

/* Exported Macros -----------------------------------------------------------*/

/* Exported Types ------------------------------------------------------------*/

ATMO_Status_t ATMO_ONSEMI_BLOCK_AddDriverInstance( ATMO_DriverInstanceHandle_t *instanceNumber );

ATMO_BLOCK_Status_t ATMO_ONSEMI_BLOCK_Init( ATMO_DriverInstanceData_t *instance );

ATMO_BLOCK_Status_t ATMO_ONSEMI_BLOCK_Read( ATMO_DriverInstanceData_t *instance, uint32_t block, uint32_t offset, void *buffer, uint32_t size );

ATMO_BLOCK_Status_t ATMO_ONSEMI_BLOCK_Program( ATMO_DriverInstanceData_t *instance, uint32_t block, uint32_t offset, void *buffer, uint32_t size );

ATMO_BLOCK_Status_t ATMO_ONSEMI_BLOCK_Erase( ATMO_DriverInstanceData_t *instance, uint32_t block );

ATMO_BLOCK_Status_t ATMO_ONSEMI_BLOCK_Sync( ATMO_DriverInstanceData_t *instance );

ATMO_BLOCK_Status_t ATMO_ONSEMI_BLOCK_GetDeviceInfo( ATMO_DriverInstanceData_t *instance, ATMO_BLOCK_DeviceInfo_t *info );


#ifdef __cplusplus
}
#endif

#endif /* __ATMO_ONSEMI_BLOCK__H */
