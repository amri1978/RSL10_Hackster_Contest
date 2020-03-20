/**
 ******************************************************************************
 * @file    datetime.c
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - Core DateTime Driver
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

#include "datetime.h"

#ifdef ATMO_SLIM_STACK
	#ifndef NUMBER_OF_DATETIME_DRIVER_INSTANCES
		#define NUMBER_OF_DATETIME_DRIVER_INSTANCES 1
	#endif
#endif

#ifndef NUMBER_OF_DATETIME_DRIVER_INSTANCES
#define NUMBER_OF_DATETIME_DRIVER_INSTANCES 1
#endif

ATMO_DriverInstanceHandle_t numberOfDateTimeDriverInstance = 0;

const ATMO_DateTime_DriverInstance_t *datetimeInstances[NUMBER_OF_DATETIME_DRIVER_INSTANCES];
ATMO_DriverInstanceData_t *datetimeInstancesData[NUMBER_OF_DATETIME_DRIVER_INSTANCES];

ATMO_Status_t ATMO_DateTime_AddDriverInstance( const ATMO_DateTime_DriverInstance_t *driverInstance, ATMO_DriverInstanceData_t *driverInstanceData, ATMO_DriverInstanceHandle_t *instanceNumber )
{
	if ( !( numberOfDateTimeDriverInstance < NUMBER_OF_DATETIME_DRIVER_INSTANCES ) )
	{
		return ATMO_Status_OutOfMemory;
	}

	datetimeInstances[numberOfDateTimeDriverInstance] = driverInstance;
	datetimeInstancesData[numberOfDateTimeDriverInstance] = driverInstanceData;
	*instanceNumber = numberOfDateTimeDriverInstance;

	numberOfDateTimeDriverInstance++;

	return ATMO_Status_Success;
}

ATMO_DateTime_Status_t ATMO_DateTime_Init( ATMO_DriverInstanceHandle_t instance )
{
	if ( !( instance < numberOfDateTimeDriverInstance ) )
	{
		return ATMO_DateTime_Status_Invalid;
	}

	return datetimeInstances[instance]->Init( datetimeInstancesData[instance] );
}

ATMO_DateTime_Status_t ATMO_DateTime_DeInit( ATMO_DriverInstanceHandle_t instance )
{
	if ( !( instance < numberOfDateTimeDriverInstance ) )
	{
		return ATMO_DateTime_Status_Invalid;
	}

	return datetimeInstances[instance]->DeInit( datetimeInstancesData[instance] );
}


ATMO_DateTime_Status_t ATMO_DateTime_SetConfiguration( ATMO_DriverInstanceHandle_t instance, const ATMO_DateTime_Config_t *config )
{
	if ( !( instance < numberOfDateTimeDriverInstance ) )
	{
		return ATMO_DateTime_Status_Invalid;
	}

	return datetimeInstances[instance]->SetConfiguration( datetimeInstancesData[instance], config );
}

ATMO_DateTime_Status_t ATMO_DateTime_GetDateTime( ATMO_DriverInstanceHandle_t instance, ATMO_DateTime_Time_t *datetime )
{
	if ( !( instance < numberOfDateTimeDriverInstance ) )
	{
		return ATMO_DateTime_Status_Invalid;
	}

	return datetimeInstances[instance]->GetDateTime( datetimeInstancesData[instance], datetime );
}

ATMO_DateTime_Status_t ATMO_DateTime_GetDateTimeEpoch( ATMO_DriverInstanceHandle_t instance, uint32_t *datetime )
{
	if ( !( instance < numberOfDateTimeDriverInstance ) )
	{
		return ATMO_DateTime_Status_Invalid;
	}

	return datetimeInstances[instance]->GetDateTimeEpoch( datetimeInstancesData[instance], datetime );
}

ATMO_DateTime_Status_t ATMO_DateTime_GetDateTimeIsoStr( ATMO_DriverInstanceHandle_t instance, char *datetime, uint32_t bufferLen )
{
	if ( !( instance < numberOfDateTimeDriverInstance ) )
	{
		return ATMO_DateTime_Status_Invalid;
	}

	return datetimeInstances[instance]->GetDateTimeIsoStr( datetimeInstancesData[instance], datetime, bufferLen );
}

ATMO_DateTime_Status_t ATMO_DateTime_SetDateTime( ATMO_DriverInstanceHandle_t instance, ATMO_DateTime_Time_t *datetime )
{
	if ( !( instance < numberOfDateTimeDriverInstance ) )
	{
		return ATMO_DateTime_Status_Invalid;
	}

	return datetimeInstances[instance]->SetDateTime( datetimeInstancesData[instance], datetime );
}

ATMO_DateTime_Status_t ATMO_DateTime_SetDateTimeEpoch( ATMO_DriverInstanceHandle_t instance, uint32_t datetime )
{
	if ( !( instance < numberOfDateTimeDriverInstance ) )
	{
		return ATMO_DateTime_Status_Invalid;
	}

	return datetimeInstances[instance]->SetDateTimeEpoch( datetimeInstancesData[instance], datetime );
}

ATMO_DateTime_Status_t ATMO_DateTime_SetDateTimeIsoStr( ATMO_DriverInstanceHandle_t instance, const char *datetime )
{
	if ( !( instance < numberOfDateTimeDriverInstance ) )
	{
		return ATMO_DateTime_Status_Invalid;
	}

	return datetimeInstances[instance]->SetDateTimeIsoStr( datetimeInstancesData[instance], datetime );
}

