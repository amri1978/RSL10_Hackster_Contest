/**
 ******************************************************************************
 * @file    cellular.c
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - Core Cellular Driver
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

#include "cellular.h"

#ifndef ATMO_SLIM_STACK
#define NUMBER_OF_CELLULAR_DRIVER_INSTANCES 4
#else
#define NUMBER_OF_CELLULAR_DRIVER_INSTANCES 1
#endif

ATMO_DriverInstanceHandle_t numberOfCellularDriverInstances = 0;

const ATMO_CELLULAR_DriverInstance_t *cellularInstances[NUMBER_OF_CELLULAR_DRIVER_INSTANCES];
ATMO_DriverInstanceData_t *cellularInstancesData[NUMBER_OF_CELLULAR_DRIVER_INSTANCES];

ATMO_Status_t ATMO_CELLULAR_AddDriverInstance( const ATMO_CELLULAR_DriverInstance_t *driverInstance, ATMO_DriverInstanceData_t *driverInstanceData, ATMO_DriverInstanceHandle_t *instanceNumber )
{
	if ( !( numberOfCellularDriverInstances < NUMBER_OF_CELLULAR_DRIVER_INSTANCES ) )
	{
		return ATMO_Status_OutOfMemory;
	}

	cellularInstances[numberOfCellularDriverInstances] = driverInstance;
	cellularInstancesData[numberOfCellularDriverInstances] = driverInstanceData;
	*instanceNumber = numberOfCellularDriverInstances;

	numberOfCellularDriverInstances++;

	return ATMO_Status_Success;
}

ATMO_CELLULAR_Status_t ATMO_CELLULAR_Init( ATMO_DriverInstanceHandle_t instance )
{
	if ( !( instance < numberOfCellularDriverInstances ) )
	{
		return ATMO_CELLULAR_Status_Invalid;
	}

	return cellularInstances[instance]->Init( cellularInstancesData[instance] );
}

ATMO_CELLULAR_Status_t ATMO_CELLULAR_DeInit( ATMO_DriverInstanceHandle_t instance )
{
	if ( !( instance < numberOfCellularDriverInstances ) )
	{
		return ATMO_CELLULAR_Status_Invalid;
	}

	return cellularInstances[instance]->DeInit( cellularInstancesData[instance] );
}

ATMO_CELLULAR_Status_t ATMO_CELLULAR_GetConnectionStatus( ATMO_DriverInstanceHandle_t instance, ATMO_CELLULAR_ConnectionStatus_t *status )
{
	if ( !( instance < numberOfCellularDriverInstances ) )
	{
		return ATMO_CELLULAR_Status_Invalid;
	}

	return cellularInstances[instance]->GetConnectionStatus( cellularInstancesData[instance], status );
}

