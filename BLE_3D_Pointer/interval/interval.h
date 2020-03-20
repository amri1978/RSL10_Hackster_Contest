/**
 ******************************************************************************
 * @file    interval.h
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - Interval header file
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

/** \addtogroup Interval
 * The Interval API allows you to register abilities and callbacks to execute at a given interval of time.
 *  @{
 */

/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef __ATMO_INTERVAL__H
#define __ATMO_INTERVAL__H


/* Includes ------------------------------------------------------------------*/
#include "../atmo/core.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported Constants --------------------------------------------------------*/

typedef uint32_t ATMO_INTERVAL_Handle_t;

/* Exported Macros -----------------------------------------------------------*/

/* Exported Types ------------------------------------------------------------*/

/**
 * Interval driver function return enumerations
 */
typedef enum
{
	ATMO_INTERVAL_Status_Success              = 0x00u,  /**< Operation was successful */
	ATMO_INTERVAL_Status_Fail                 = 0x01u,  /**< Operation failed */
	ATMO_INTERVAL_Status_Initialized          = 0x02u,  /**< Peripheral already initialized */
	ATMO_INTERVAL_Status_Invalid              = 0x03u,  /**< Invalid operation or result */
	ATMO_INTERVAL_Status_NotSupported         = 0x04u,  /**< Feature not supported by platform */
	ATMO_INTERVAL_Status_Unknown              = 0x05u,  /**< Some other status not defined */
	ATMO_INTERVAL_Status_OutOfMemory          = 0x06u /**< Device out of memory */
} ATMO_INTERVAL_Status_t;

/// \cond DO_NOT_DOCUMENT
typedef struct ATMO_INTERVAL_DriverInstance_t ATMO_INTERVAL_DriverInstance_t;

struct ATMO_INTERVAL_DriverInstance_t
{
	ATMO_INTERVAL_Status_t ( *Init )( ATMO_DriverInstanceData_t *instanceData );
	ATMO_INTERVAL_Status_t ( *RemoveInterval )( ATMO_DriverInstanceData_t *instanceData, ATMO_INTERVAL_Handle_t intervalHandle );
	ATMO_INTERVAL_Status_t ( *AddAbilityInterval )( ATMO_DriverInstanceData_t *instanceData, ATMO_AbilityHandle_t abilityHandle, uint32_t interval, ATMO_INTERVAL_Handle_t *intervalHandle );
	ATMO_INTERVAL_Status_t ( *AddCallbackInterval )( ATMO_DriverInstanceData_t *instanceData, ATMO_Callback_t cb, uint32_t interval, ATMO_INTERVAL_Handle_t *intervalHandle );
};

typedef struct
{
	ATMO_AbilityHandle_t abilityHandle;
	uint64_t milliSeconds;
	ATMO_BOOL_t enabled;
	ATMO_Value_t value;
} ATMO_INTERVAL_AbilityExecuteEntry;
/// \endcond

/* Exported Structures -------------------------------------------------------*/

/* Exported Function Prototypes -----------------------------------------------*/

/**
 * This routine will add an instance of a driver for INTERVAL into the INTERVAL driver handler.
 *
 * @param[in] driverInstance
 * @param[in] driverInstanceData
 * @param[out] instanceNumber
 *
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_INTERVAL_AddDriverInstance( const ATMO_INTERVAL_DriverInstance_t *driverInstance, ATMO_DriverInstanceData_t *driverInstanceData, ATMO_DriverInstanceHandle_t *instanceNumber );

/**
 * This routine will initialize the interval driver instance.
 *
 * @param[in] instance
 */
ATMO_INTERVAL_Status_t ATMO_INTERVAL_Init( ATMO_DriverInstanceHandle_t instance );

/**
 * This routine will add an instance for the interval handler to launch
 * the said ability handler every given time by interval in milliseconds.
 *
 * @param[in] instance
 * @param[in] abilityHandle
 * @param[in] interval_ms
 * @param[out] intervalHandle - Handle to specific interval
 *
 * @return ATMO_INTERVAL_Status_t
 */
ATMO_INTERVAL_Status_t ATMO_INTERVAL_AddAbilityInterval( ATMO_DriverInstanceHandle_t instance, ATMO_AbilityHandle_t abilityHandle, uint32_t interval_ms, ATMO_INTERVAL_Handle_t *intervalHandle );

/**
 * Using the interval handler return from AddAbilityInterval this function
 * will remove the interval from execution.
 *
 * @param[in] instance
 * @param[in] intervalHandle
 *
 * @return ATMO_INTERVAL_Status_t
 */
ATMO_INTERVAL_Status_t ATMO_INTERVAL_RemoveAbilityInterval( ATMO_DriverInstanceHandle_t instance, ATMO_INTERVAL_Handle_t intervalHandle );

/**
 * This routine will add an instance for the interval handler to launch
 * the said callback every given time by interval in milliseconds.
 *
 */
ATMO_INTERVAL_Status_t ATMO_INTERVAL_AddCallbackInterval( ATMO_DriverInstanceHandle_t instance, ATMO_Callback_t cb, uint32_t interval_ms, ATMO_INTERVAL_Handle_t *intervalHandle );

#ifdef __cplusplus
}
#endif

#endif /* __ATMO_INTERVAL__H */
/** @}*/