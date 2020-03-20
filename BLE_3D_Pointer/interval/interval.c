/**
 ******************************************************************************
 * @file    interval.c
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - Core INTERVAL Driver
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

#include "interval.h"

#ifdef ATMO_SLIM_STACK
	#ifndef NUMBER_OF_INTERVAL_DRIVER_INSTANCES
		#define NUMBER_OF_INTERVAL_DRIVER_INSTANCES 1
	#endif
#endif

#ifndef NUMBER_OF_INTERVAL_DRIVER_INSTANCES
#define NUMBER_OF_INTERVAL_DRIVER_INSTANCES 1
#endif
ATMO_DriverInstanceHandle_t numberOfIntervalDriverInstance = 0;

const ATMO_INTERVAL_DriverInstance_t *intervalInstances[NUMBER_OF_INTERVAL_DRIVER_INSTANCES];
ATMO_DriverInstanceData_t *intervalInstancesData[NUMBER_OF_INTERVAL_DRIVER_INSTANCES];

ATMO_Status_t ATMO_INTERVAL_AddDriverInstance( const ATMO_INTERVAL_DriverInstance_t *driverInstance, ATMO_DriverInstanceData_t *driverInstanceData, ATMO_DriverInstanceHandle_t *instanceNumber )
{
	if ( !( numberOfIntervalDriverInstance < NUMBER_OF_INTERVAL_DRIVER_INSTANCES ) )
	{
		return ATMO_Status_OutOfMemory;
	}

	intervalInstances[numberOfIntervalDriverInstance] = driverInstance;
	intervalInstancesData[numberOfIntervalDriverInstance] = driverInstanceData;
	*instanceNumber = numberOfIntervalDriverInstance;

	numberOfIntervalDriverInstance++;

	return ATMO_Status_Success;
}

ATMO_INTERVAL_Status_t ATMO_INTERVAL_Init( ATMO_DriverInstanceHandle_t instance )
{
	if ( !( instance < numberOfIntervalDriverInstance ) )
	{
		return ATMO_INTERVAL_Status_Invalid;
	}

	return intervalInstances[instance]->Init( intervalInstancesData[instance] );
}

ATMO_INTERVAL_Status_t ATMO_INTERVAL_AddAbilityInterval( ATMO_DriverInstanceHandle_t instance, ATMO_AbilityHandle_t abilityHandle, uint32_t interval_ms, ATMO_INTERVAL_Handle_t *intervalHandle )
{
	if ( !( instance < numberOfIntervalDriverInstance ) )
	{
		return ATMO_INTERVAL_Status_Invalid;
	}

	return intervalInstances[instance]->AddAbilityInterval( intervalInstancesData[instance], abilityHandle, interval_ms, intervalHandle );
}

ATMO_INTERVAL_Status_t ATMO_INTERVAL_AddCallbackInterval( ATMO_DriverInstanceHandle_t instance, ATMO_Callback_t cb, uint32_t interval_ms, ATMO_INTERVAL_Handle_t *intervalHandle )
{
	if ( !( instance < numberOfIntervalDriverInstance ) )
	{
		return ATMO_INTERVAL_Status_Invalid;
	}

	return intervalInstances[instance]->AddCallbackInterval( intervalInstancesData[instance], cb, interval_ms, intervalHandle );
}

ATMO_INTERVAL_Status_t ATMO_INTERVAL_RemoveInterval( ATMO_DriverInstanceHandle_t instance, ATMO_INTERVAL_Handle_t intervalHandle )
{
	if ( !( instance < numberOfIntervalDriverInstance ) )
	{
		return ATMO_INTERVAL_Status_Invalid;
	}

	return intervalInstances[instance]->RemoveInterval( intervalInstancesData[instance], intervalHandle );
}
