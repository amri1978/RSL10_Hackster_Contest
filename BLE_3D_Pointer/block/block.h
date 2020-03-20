/**
 ******************************************************************************
 * @file    block.h
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - Block header file
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

/** \addtogroup Block
 *  @{
 * The Block driver provides low level access to non-volatile storage such as
 * EEPROM or SPI Flash. Most users will benefit more from usage of the higher level
 * Filesystem driver.
 */

/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef __ATMO_BLOCK__H
#define __ATMO_BLOCK__H


/* Includes ------------------------------------------------------------------*/
#include "../atmo/core.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported Constants --------------------------------------------------------*/

/* Exported Macros -----------------------------------------------------------*/

/* Exported Types ------------------------------------------------------------*/

/**
 * Block driver return configuration
 */
typedef enum
{
	ATMO_BLOCK_Status_Success              = 0x00u,  /**< Common - Operation was successful */
	ATMO_BLOCK_Status_Fail                 = 0x01u,  /**< Common - Operation failed */
	ATMO_BLOCK_Status_Initialized          = 0x02u,  /**< Common - Peripheral already initialized */
	ATMO_BLOCK_Status_Invalid              = 0x03u,  /**< Common - Invalid operation or result */
	ATMO_BLOCK_Status_NotSupported         = 0x04u,  /**< Common - Feature not supported by platform */
	ATMO_BLOCK_Status_Unknown              = 0x05u,  /**< Common - Some other status not defined */
} ATMO_BLOCK_Status_t;

/* Exported Structures -------------------------------------------------------*/

typedef struct
{
	uint32_t readSize;
	uint32_t progSize;
	uint32_t blockSize;
	uint32_t blockCount;
} ATMO_BLOCK_DeviceInfo_t;

typedef struct ATMO_BLOCK_DriverInstance_t ATMO_BLOCK_DriverInstance_t;

struct ATMO_BLOCK_DriverInstance_t
{
	ATMO_BLOCK_Status_t ( *Init )( ATMO_DriverInstanceData_t *instance );
	ATMO_BLOCK_Status_t ( *Read )( ATMO_DriverInstanceData_t *instance, uint32_t block, uint32_t offset, void *buffer, uint32_t size );
	ATMO_BLOCK_Status_t ( *Program )( ATMO_DriverInstanceData_t *instance, uint32_t block, uint32_t offset, void *buffer, uint32_t size );
	ATMO_BLOCK_Status_t ( *Erase )( ATMO_DriverInstanceData_t *instance, uint32_t block );
	ATMO_BLOCK_Status_t ( *Sync )( ATMO_DriverInstanceData_t *instance );
	ATMO_BLOCK_Status_t ( *GetDeviceInfo )( ATMO_DriverInstanceData_t *instance, ATMO_BLOCK_DeviceInfo_t *info );
};

/* Exported Function Prototypes -----------------------------------------------*/

/**
 * Register instance of block driver with underlying driver framework
 *
 * @param[in] driverInstance
 * @param[in] driverInstanceData
 * @param[out] instanceNumber
 *
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_BLOCK_AddDriverInstance( const ATMO_BLOCK_DriverInstance_t *driverInstance, ATMO_DriverInstanceData_t *driverInstanceData, ATMO_DriverInstanceHandle_t *instanceNumber );

/**
 * Initialize core block driver
 *
 * @param[in] instanceNumber
 *
 * @return ATMO_BLOCK_Status_t
 */
ATMO_BLOCK_Status_t ATMO_BLOCK_Init( ATMO_DriverInstanceHandle_t instanceNumber );

/**
 * Read a block from NVM
 *
 * @param[in] instanceNumber
 * @param[in] block - Block number (based on block size of platform)
 * @param[in] offset - Offset in bytes within block
 * @param[in/out] buffer - Buffer to be filled with data
 * @param[in] size - Number of bytes to read
 *
 * @return ATMO_BLOCK_Status_t
 */
ATMO_BLOCK_Status_t ATMO_BLOCK_Read( ATMO_DriverInstanceHandle_t instanceNumber, uint32_t block, uint32_t offset, void *buffer, uint32_t size );

/**
 * Write a block to NVM
 *
 * @param[in] instanceNumber
 * @param[in] block - Block number (based on block size of platform)
 * @param[in] offset - Offset in bytes within block
 * @param[in/out] buffer - Data to write
 * @param[in] size - Number of bytes to write
 *
 * @return ATMO_BLOCK_Status_t
 */
ATMO_BLOCK_Status_t ATMO_BLOCK_Program( ATMO_DriverInstanceHandle_t instanceNumber, uint32_t block, uint32_t offset, void *buffer, uint32_t size );

/**
 * Erase a block in NVM
 *
 * @param[in] instanceNumber
 *
 * @return ATMO_BLOCK_Status_t
 */
ATMO_BLOCK_Status_t ATMO_BLOCK_Erase( ATMO_DriverInstanceHandle_t instanceNumber, uint32_t block );

/**
 * Synchronize NVM
 *
 * May not be necessary for all platforms. Check platform specific notes.
 *
 * @param[in] instanceNumber
 *
 * @return ATMO_BLOCK_Status_t
 */
ATMO_BLOCK_Status_t ATMO_BLOCK_Sync( ATMO_DriverInstanceHandle_t instanceNumber );

/**
 * Retrieve information about underlying NVM
 *
 * @param[in] instanceNumber
 * @param[out] info
 *
 * @return ATMO_BLOCK_Status_t
 */
ATMO_BLOCK_Status_t ATMO_BLOCK_GetDeviceInfo( ATMO_DriverInstanceHandle_t instanceNumber, ATMO_BLOCK_DeviceInfo_t *info );

#ifdef __cplusplus
}
#endif

#endif /* __ATMO_BLOCK__H */
/** @}*/