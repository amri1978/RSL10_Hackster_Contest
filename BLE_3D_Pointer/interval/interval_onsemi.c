
/**
 ******************************************************************************
 * @file    interval_Default.c
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - Intel Curie Interval API Implementation
 ******************************************************************************
 * @attention
 *
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
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

#include "interval_onsemi.h"
#include "../app_src/atmosphere_platform.h"

#define ATMO_ONSEMI_INTERVAL_MAX_NUMBER_OF_TIMER 16

struct ATMO_INTERVAL_DriverInstance_t DefaultIntervalDriverInstance =
{
	ATMO_ONSEMI_INTERVAL_Init,
	ATMO_ONSEMI_INTERVAL_RemoveAbilityInterval,
	ATMO_ONSEMI_INTERVAL_AddAbilityInterval,
	ATMO_ONSEMI_INTERVAL_AddCallbackInterval
};

// TODO Nick this type should just be in the base interval driver
typedef enum
{
	ATMO_EXECUTE_ABILITY,
	ATMO_EXECUTE_CALLBACK,
} ONSEMI_INTERVAL_ExecuteType_t;

typedef struct
{
	ONSEMI_INTERVAL_ExecuteType_t type;
	ATMO_Callback_t callback;
	ATMO_AbilityHandle_t abilityHandle;
	uint64_t milliSeconds;
	ATMO_Value_t value;
	struct stimer timer;
} ONSEMI_INTERVAL_AbilityExecuteEntry;

static uint64_t lastMillis = 0;

static unsigned int currentDefaultIntervalAbilityExecuteIndex = 0;
static ONSEMI_INTERVAL_AbilityExecuteEntry currentDefaultIntervalAbilityExecuteList[ATMO_ONSEMI_INTERVAL_MAX_NUMBER_OF_TIMER];

ATMO_Status_t ATMO_ONSEMI_INTERVAL_AddDriverInstance( ATMO_DriverInstanceHandle_t *instanceNumber )
{
	static ATMO_DriverInstanceData_t driverInstanceData;

	driverInstanceData.name = "Default Interval";
	driverInstanceData.initialized = false;
	driverInstanceData.instanceNumber = 0;
	driverInstanceData.argument = NULL;

	return ATMO_INTERVAL_AddDriverInstance( &DefaultIntervalDriverInstance, &driverInstanceData, instanceNumber );
}

void ATMO_ONSEMI_INTERVAL_UpdateTimer( void *data )
{
	// Check to see if any of the events need to be fired
	for ( unsigned int i = 0; i < currentDefaultIntervalAbilityExecuteIndex; i++ )
	{
		// If something is due to be execute
		if ( stimer_is_expired(&currentDefaultIntervalAbilityExecuteList[i].timer) )
		{
			if ( currentDefaultIntervalAbilityExecuteList[i].type == ATMO_EXECUTE_ABILITY )
			{
				ATMO_AddAbilityExecute( currentDefaultIntervalAbilityExecuteList[i].abilityHandle, NULL );
			}
			else
			{
				ATMO_AddCallbackExecute( currentDefaultIntervalAbilityExecuteList[i].callback, NULL );
			}

			stimer_expire_from_now_ms(&currentDefaultIntervalAbilityExecuteList[i].timer, currentDefaultIntervalAbilityExecuteList[i].milliSeconds);
		}
	}
}

ATMO_INTERVAL_Status_t ATMO_ONSEMI_INTERVAL_Init( ATMO_DriverInstanceData_t *instance )
{
	ATMO_AddTickCallback( ATMO_ONSEMI_INTERVAL_UpdateTimer ); //We need to check the ISR for execution on each tick of the execution handler.

	return ATMO_INTERVAL_Status_Success;
}

ATMO_INTERVAL_Status_t ATMO_ONSEMI_INTERVAL_AddAbilityInterval( ATMO_DriverInstanceData_t *instance, ATMO_AbilityHandle_t abilityHandle, uint32_t interval, ATMO_INTERVAL_Handle_t *intervalHandle )
{
	if ( !( currentDefaultIntervalAbilityExecuteIndex < ATMO_ONSEMI_INTERVAL_MAX_NUMBER_OF_TIMER ) )
	{
		return ATMO_INTERVAL_Status_OutOfMemory;
	}

	currentDefaultIntervalAbilityExecuteList[currentDefaultIntervalAbilityExecuteIndex].milliSeconds = interval;
	currentDefaultIntervalAbilityExecuteList[currentDefaultIntervalAbilityExecuteIndex].abilityHandle = abilityHandle;
	currentDefaultIntervalAbilityExecuteList[currentDefaultIntervalAbilityExecuteIndex].type = ATMO_EXECUTE_ABILITY;

	stimer_init(&currentDefaultIntervalAbilityExecuteList[currentDefaultIntervalAbilityExecuteIndex].timer, Timer_GetContext());

	currentDefaultIntervalAbilityExecuteIndex++;

	return ATMO_INTERVAL_Status_Success;
}

ATMO_INTERVAL_Status_t ATMO_ONSEMI_INTERVAL_AddCallbackInterval( ATMO_DriverInstanceData_t *instance, ATMO_Callback_t cb, uint32_t interval, ATMO_INTERVAL_Handle_t *intervalHandle )
{
	if ( !( currentDefaultIntervalAbilityExecuteIndex < ATMO_ONSEMI_INTERVAL_MAX_NUMBER_OF_TIMER ) )
	{
		return ATMO_INTERVAL_Status_OutOfMemory;
	}

	currentDefaultIntervalAbilityExecuteList[currentDefaultIntervalAbilityExecuteIndex].milliSeconds = interval;
	currentDefaultIntervalAbilityExecuteList[currentDefaultIntervalAbilityExecuteIndex].callback = cb;
	currentDefaultIntervalAbilityExecuteList[currentDefaultIntervalAbilityExecuteIndex].type = ATMO_EXECUTE_CALLBACK;

	stimer_init(&currentDefaultIntervalAbilityExecuteList[currentDefaultIntervalAbilityExecuteIndex].timer, Timer_GetContext());

	currentDefaultIntervalAbilityExecuteIndex++;

	return ATMO_INTERVAL_Status_Success;
}

ATMO_INTERVAL_Status_t ATMO_ONSEMI_INTERVAL_RemoveAbilityInterval( ATMO_DriverInstanceData_t *instance, ATMO_INTERVAL_Handle_t intervalHandle )
{
	return ATMO_INTERVAL_Status_NotSupported;
}
