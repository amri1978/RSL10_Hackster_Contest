/**
 ******************************************************************************
 * @file    block.c
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - Core BLOCK Driver
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

#include "block.h"

#ifdef ATMO_SLIM_STACK
	#ifndef NUMBER_OF_BLOCK_DRIVER_INSTANCES
		#define NUMBER_OF_BLOCK_DRIVER_INSTANCES 1
	#endif
#endif

#ifndef NUMBER_OF_BLOCK_DRIVER_INSTANCES
#define NUMBER_OF_BLOCK_DRIVER_INSTANCES 1
#endif

ATMO_DriverInstanceHandle_t numberOfBlockDriverInstance = 0;

const ATMO_BLOCK_DriverInstance_t *blockInstances[NUMBER_OF_BLOCK_DRIVER_INSTANCES];
ATMO_DriverInstanceData_t *blockInstancesData[NUMBER_OF_BLOCK_DRIVER_INSTANCES];

ATMO_Status_t ATMO_BLOCK_AddDriverInstance( const ATMO_BLOCK_DriverInstance_t *driverInstance, ATMO_DriverInstanceData_t *driverInstanceData, ATMO_DriverInstanceHandle_t *instanceNumber )
{
	if ( !( numberOfBlockDriverInstance < NUMBER_OF_BLOCK_DRIVER_INSTANCES ) )
	{
		return ATMO_Status_OutOfMemory;
	}

	blockInstances[numberOfBlockDriverInstance] = driverInstance;
	blockInstancesData[numberOfBlockDriverInstance] = driverInstanceData;
	*instanceNumber = numberOfBlockDriverInstance;

	numberOfBlockDriverInstance++;

	return ATMO_Status_Success;
}

ATMO_BLOCK_Status_t ATMO_BLOCK_Init( ATMO_DriverInstanceHandle_t instanceNumber )
{
	if ( !( instanceNumber < numberOfBlockDriverInstance ) )
	{
		return ATMO_BLOCK_Status_Invalid;
	}

	return blockInstances[instanceNumber]->Init( blockInstancesData[instanceNumber] );
}

ATMO_BLOCK_Status_t ATMO_BLOCK_Read( ATMO_DriverInstanceHandle_t instanceNumber, uint32_t block, uint32_t offset, void *buffer, uint32_t size )
{
	if ( !( instanceNumber < numberOfBlockDriverInstance ) )
	{
		return ATMO_BLOCK_Status_Invalid;
	}

	return blockInstances[instanceNumber]->Read( blockInstancesData[instanceNumber], block, offset, buffer, size );
}

ATMO_BLOCK_Status_t ATMO_BLOCK_Program( ATMO_DriverInstanceHandle_t instanceNumber, uint32_t block, uint32_t offset, void *buffer, uint32_t size )
{
	if ( !( instanceNumber < numberOfBlockDriverInstance ) )
	{
		return ATMO_BLOCK_Status_Invalid;
	}

	return blockInstances[instanceNumber]->Program( blockInstancesData[instanceNumber], block, offset, buffer, size );
}

ATMO_BLOCK_Status_t ATMO_BLOCK_Erase( ATMO_DriverInstanceHandle_t instanceNumber, uint32_t block )
{
	if ( !( instanceNumber < numberOfBlockDriverInstance ) )
	{
		return ATMO_BLOCK_Status_Invalid;
	}

	return blockInstances[instanceNumber]->Erase( blockInstancesData[instanceNumber], block );
}

ATMO_BLOCK_Status_t ATMO_BLOCK_Sync( ATMO_DriverInstanceHandle_t instanceNumber )
{
	if ( !( instanceNumber < numberOfBlockDriverInstance ) )
	{
		return ATMO_BLOCK_Status_Invalid;
	}

	return blockInstances[instanceNumber]->Sync( blockInstancesData[instanceNumber] );
}

ATMO_BLOCK_Status_t ATMO_BLOCK_GetDeviceInfo( ATMO_DriverInstanceHandle_t instanceNumber, ATMO_BLOCK_DeviceInfo_t *info )
{
	if ( !( instanceNumber < numberOfBlockDriverInstance ) )
	{
		return ATMO_BLOCK_Status_Invalid;
	}

	return blockInstances[instanceNumber]->GetDeviceInfo( blockInstancesData[instanceNumber], info );
}

