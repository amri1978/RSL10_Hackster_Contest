/**
 ******************************************************************************
 * @file    gpio.h
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - DateTime header file
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

/** \addtogroup DateTime
 * The date / time driver is used for keeping track of the date and time.
 *  @{
 */


/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef __ATMO_DateTime__H
#define __ATMO_DateTime__H


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
	ATMO_DateTime_Status_Success       = 0x00u,  // Operation was successful
	ATMO_DateTime_Status_Fail          = 0x01u,  // Operation failed
	ATMO_DateTime_Status_Initialized   = 0x02u,  // Peripheral already initialized
	ATMO_DateTime_Status_Invalid       = 0x03u,  // Invalid operation
	ATMO_DateTime_Status_NotSupported  = 0x04u,  // Feature not supported by platform
	ATMO_DateTime_Status_Unspecified   = 0x05u,  // Some other status not defined
} ATMO_DateTime_Status_t;


/* Exported Structures -------------------------------------------------------*/

typedef struct
{
	uint8_t unused; // No configuration for this driver at this point
} ATMO_DateTime_Config_t;

typedef enum
{
	ATMO_DateTime_Sunday        = 0,
	ATMO_DateTime_Monday        = 1,
	ATMO_DateTime_Tuesday       = 2,
	ATMO_DateTime_Wednesday     = 3,
	ATMO_DateTime_Thursday      = 4,
	ATMO_DateTime_Friday        = 5,
	ATMO_DateTime_Saturday      = 6,
} ATMO_DateTime_Weekday_t;

typedef enum
{
	ATMO_DateTime_January       = 0x1,
	ATMO_DateTime_February      = 0x2,
	ATMO_DateTime_March         = 0x3,
	ATMO_DateTime_April         = 0x4,
	ATMO_DateTime_May           = 0x5,
	ATMO_DateTime_June          = 0x6,
	ATMO_DateTime_July          = 0x7,
	ATMO_DateTime_August        = 0x8,
	ATMO_DateTime_September     = 0x9,
	ATMO_DateTime_October       = 0x10,
	ATMO_DateTime_November      = 0x11,
	ATMO_DateTime_December      = 0x12,
} ATMO_DateTime_Month_t;


typedef struct
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours; // Assumes 24H mode
	uint8_t days;
	ATMO_DateTime_Weekday_t weekday;
	ATMO_DateTime_Month_t month;
	uint8_t years; // 2 digit year
} ATMO_DateTime_Time_t;

// Some C gore so we can use the struct within itself
typedef struct ATMO_DateTime_DriverInstance_t ATMO_DateTime_DriverInstance_t;

struct ATMO_DateTime_DriverInstance_t
{
	ATMO_DateTime_Status_t ( *Init )( ATMO_DriverInstanceData_t *instanceData );
	ATMO_DateTime_Status_t ( *DeInit )( ATMO_DriverInstanceData_t *instanceData );
	ATMO_DateTime_Status_t ( *SetConfiguration )( ATMO_DriverInstanceData_t *instanceData, const ATMO_DateTime_Config_t *config );
	ATMO_DateTime_Status_t ( *GetDateTime )( ATMO_DriverInstanceData_t *instanceData, ATMO_DateTime_Time_t *datetime );
	ATMO_DateTime_Status_t ( *GetDateTimeEpoch )( ATMO_DriverInstanceData_t *instanceData, uint32_t *datetime );
	ATMO_DateTime_Status_t ( *GetDateTimeIsoStr )( ATMO_DriverInstanceData_t *instanceData, char *datetime, uint32_t bufferLen );
	ATMO_DateTime_Status_t ( *SetDateTime )( ATMO_DriverInstanceData_t *instanceData, ATMO_DateTime_Time_t *datetime );
	ATMO_DateTime_Status_t ( *SetDateTimeEpoch )( ATMO_DriverInstanceData_t *instanceData, uint32_t datetime );
	ATMO_DateTime_Status_t ( *SetDateTimeIsoStr )( ATMO_DriverInstanceData_t *instanceData, const char *datetime );

};

/* Exported Function Prototypes -----------------------------------------------*/

/**
 * This routine will add an instance of a driver for DateTime into the DateTime driver handler.
 *
 */
ATMO_Status_t ATMO_DateTime_AddDriverInstance( const ATMO_DateTime_DriverInstance_t *driverInstance, ATMO_DriverInstanceData_t *driverInstanceData, ATMO_DriverInstanceHandle_t *instanceNumber );

/**
 * Initialize DateTime Driver
 *
 * @param[in] instance
 *
 * @return status
 */
ATMO_DateTime_Status_t ATMO_DateTime_Init( ATMO_DriverInstanceHandle_t instance );

/**
 * This routine de-initializes the datetime driver
 *
 * @param[in] instance
 * @return status.
 */
ATMO_DateTime_Status_t ATMO_DateTime_DeInit( ATMO_DriverInstanceHandle_t instance );

/**
 * Set datetime configuration
 *
 * @param[in] instance
 * @param[in] config
 * @return status
 */
ATMO_DateTime_Status_t ATMO_DateTime_SetConfiguration( ATMO_DriverInstanceHandle_t instance, const ATMO_DateTime_Config_t *config );

/**
 * Retrieve the current date and time
 *
 * @param[in] instance
 * @param[out] datetime
 * @return status
 */
ATMO_DateTime_Status_t ATMO_DateTime_GetDateTime( ATMO_DriverInstanceHandle_t instance, ATMO_DateTime_Time_t *datetime );

/**
 * Retrieve the current date and time
 *
 * @param[in] instance
 * @param[out] datetime - seconds since unix epoch
 * @return status
 */
ATMO_DateTime_Status_t ATMO_DateTime_GetDateTimeEpoch( ATMO_DriverInstanceHandle_t instance, uint32_t *datetime );

/**
 * Retrieve the current date and time
 *
 * @param[in] instance
 * @param[out] datetime - ISO 8601 String
 * @return status
 */
ATMO_DateTime_Status_t ATMO_DateTime_GetDateTimeIsoStr( ATMO_DriverInstanceHandle_t instance, char *datetime, uint32_t bufferLen );

/**
 * Set the current date and time
 *
 * @param[in] instance
 * @param[in] datetime
 * @return status
 */
ATMO_DateTime_Status_t ATMO_DateTime_SetDateTime( ATMO_DriverInstanceHandle_t instance, ATMO_DateTime_Time_t *datetime );

/**
 * Set the current date and time
 *
 * @param[in] instance
 * @param[in] datetime - seconds since unix epoch
 * @return status
 */
ATMO_DateTime_Status_t ATMO_DateTime_SetDateTimeEpoch( ATMO_DriverInstanceHandle_t instance, uint32_t datetime );

/**
 * Set the current date and time
 *
 * @param[in] instance
 * @param[in] datetime - ISO 8601 String
 * @return status
 */
ATMO_DateTime_Status_t ATMO_DateTime_SetDateTimeIsoStr( ATMO_DriverInstanceHandle_t instance, const char *datetime );

#ifdef __cplusplus
}
#endif

#endif /* __ATMO_DateTime__H */
/** @}*/