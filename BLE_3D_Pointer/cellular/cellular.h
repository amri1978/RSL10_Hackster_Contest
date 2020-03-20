/**
 ******************************************************************************
 * @file    cellular.h
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - Cellular header file
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

/** \addtogroup Cellular
 * The cellular driver is used to interact with cellular modules.
 *  @{
 */


/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef __ATMO_CELLULAR__H
#define __ATMO_CELLULAR__H


/* Includes ------------------------------------------------------------------*/
#include "../atmo/core.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported Constants --------------------------------------------------------*/

/* Exported Macros -----------------------------------------------------------*/

/* Exported Types ------------------------------------------------------------*/

typedef enum
{
	ATMO_CELLULAR_Status_Success       = 0x00u,  // Operation was successful
	ATMO_CELLULAR_Status_Fail          = 0x01u,  // Operation failed
	ATMO_CELLULAR_Status_Initialized   = 0x02u,  // Peripheral already initialized
	ATMO_CELLULAR_Status_Invalid       = 0x03u,  // Invalid operation
	ATMO_CELLULAR_Status_NotSupported  = 0x04u,  // Feature not supported by platform
	ATMO_CELLULAR_Status_Unspecified   = 0x05u,  // Some other status not defined
} ATMO_CELLULAR_Status_t;

typedef enum
{
	ATMO_CELLULAR_ConnectionStatus_Connected,
	ATMO_CELLULAR_ConnectionStatus_Disconnected
} ATMO_CELLULAR_ConnectionStatus_t;


/* Exported Structures -------------------------------------------------------*/

// Some C gore so we can use the struct within itself
typedef struct ATMO_CELLULAR_DriverInstance_t ATMO_CELLULAR_DriverInstance_t;

struct ATMO_CELLULAR_DriverInstance_t
{
	ATMO_CELLULAR_Status_t ( *Init )( ATMO_DriverInstanceData_t *instanceData );
	ATMO_CELLULAR_Status_t ( *DeInit )( ATMO_DriverInstanceData_t *instanceData );
	ATMO_CELLULAR_Status_t ( *GetConnectionStatus )( ATMO_DriverInstanceData_t *instanceData, ATMO_CELLULAR_ConnectionStatus_t *status );
};

/* Exported Function Prototypes -----------------------------------------------*/

/**
 * This routine will add an instance of a driver for cellular into the cellular driver handler.
 *
 */
ATMO_Status_t ATMO_CELLULAR_AddDriverInstance( const ATMO_CELLULAR_DriverInstance_t *driverInstance, ATMO_DriverInstanceData_t *driverInstanceData, ATMO_DriverInstanceHandle_t *instanceNumber );

/**
 * Initialize the cellular driver.
 *
 * @param[in] instance
 *
 * @return status
 */
ATMO_CELLULAR_Status_t ATMO_CELLULAR_Init( ATMO_DriverInstanceHandle_t instance );

/**
 * This routine de-initializes the datetime driver.
 *
 * @param[in] instance
 * @return status.
 */
ATMO_CELLULAR_Status_t ATMO_CELLULAR_DeInit( ATMO_DriverInstanceHandle_t instance );

/**
 * @brief Get current cellular internet connection status.
 *
 * @param instance
 * @param status
 * @return ATMO_CELLULAR_Status_t
 */
ATMO_CELLULAR_Status_t ATMO_CELLULAR_GetConnectionStatus( ATMO_DriverInstanceHandle_t instance, ATMO_CELLULAR_ConnectionStatus_t *status );

#ifdef __cplusplus
}
#endif

#endif /* __ATMO_CELLULAR__H */
/** @}*/