
/**
 ******************************************************************************
 * @file    interval.h
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - INTERVAL header file
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
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
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

/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef __ATMO_ONSEMI_INTERVAL__H
#define __ATMO_ONSEMI_INTERVAL__H

/* Includes ------------------------------------------------------------------*/
#include "../atmo/core.h"
#include "interval.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported Constants --------------------------------------------------------*/

/* Exported Macros -----------------------------------------------------------*/

/* Exported Types ------------------------------------------------------------*/

/* Exported Structures -------------------------------------------------------*/

ATMO_Status_t ATMO_ONSEMI_INTERVAL_AddDriverInstance( ATMO_DriverInstanceHandle_t *instanceNumber );

ATMO_INTERVAL_Status_t ATMO_ONSEMI_INTERVAL_Init( ATMO_DriverInstanceData_t *instance );

ATMO_INTERVAL_Status_t ATMO_ONSEMI_INTERVAL_AddAbilityInterval( ATMO_DriverInstanceData_t *instance, ATMO_AbilityHandle_t abilityHandle, uint32_t interval, ATMO_INTERVAL_Handle_t *intervalHandle );

ATMO_INTERVAL_Status_t ATMO_ONSEMI_INTERVAL_AddCallbackInterval( ATMO_DriverInstanceData_t *instance, ATMO_Callback_t cb, uint32_t interval, ATMO_INTERVAL_Handle_t *intervalHandle );

ATMO_INTERVAL_Status_t ATMO_ONSEMI_INTERVAL_RemoveAbilityInterval( ATMO_DriverInstanceData_t *instance, ATMO_INTERVAL_Handle_t intervalHandle );

#ifdef __cplusplus
}
#endif

#endif
