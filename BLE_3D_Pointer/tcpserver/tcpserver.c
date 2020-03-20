/**
 ******************************************************************************
 * @file    tcpserver.c
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - Core TCP Server Driver
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

#include "tcpserver.h"

#ifdef ATMO_SLIM_STACK
	#ifndef NUMBER_OF_TCP_SERVER_DRIVER_INSTANCES
		#define NUMBER_OF_TCP_SERVER_DRIVER_INSTANCES 1
	#endif
#endif

#ifndef NUMBER_OF_TCP_SERVER_DRIVER_INSTANCES
#define NUMBER_OF_TCP_SERVER_DRIVER_INSTANCES 4
#endif

ATMO_DriverInstanceHandle_t numTcpServerDriverInstances = 0;

const ATMO_TCP_SERVER_DriverInstance_t *tcpServerInstances[NUMBER_OF_TCP_SERVER_DRIVER_INSTANCES];
ATMO_DriverInstanceData_t *tcpServerInstancesData[NUMBER_OF_TCP_SERVER_DRIVER_INSTANCES];

ATMO_Status_t ATMO_TCP_SERVER_AddDriverInstance( const ATMO_TCP_SERVER_DriverInstance_t *driverInstance, ATMO_DriverInstanceData_t *driverInstanceData, ATMO_DriverInstanceHandle_t *instanceNumber )
{
	if ( !( numTcpServerDriverInstances < NUMBER_OF_TCP_SERVER_DRIVER_INSTANCES ) )
	{
		return ATMO_Status_OutOfMemory;
	}

	tcpServerInstances[numTcpServerDriverInstances] = driverInstance;
	tcpServerInstancesData[numTcpServerDriverInstances] = driverInstanceData;
	*instanceNumber = numTcpServerDriverInstances;

	numTcpServerDriverInstances++;

	return ATMO_Status_Success;
}

ATMO_TCP_SERVER_Status_t ATMO_TCP_SERVER_Init( ATMO_DriverInstanceHandle_t instance )
{
	if ( !( instance < numTcpServerDriverInstances ) )
	{
		return ATMO_TCP_SERVER_Status_Invalid;
	}

	return tcpServerInstances[instance]->Init( tcpServerInstancesData[instance] );
}

ATMO_TCP_SERVER_Status_t ATMO_TCP_SERVER_DeInit( ATMO_DriverInstanceHandle_t instance )
{
	if ( !( instance < numTcpServerDriverInstances ) )
	{
		return ATMO_TCP_SERVER_Status_Invalid;
	}

	return tcpServerInstances[instance]->DeInit( tcpServerInstancesData[instance] );
}


ATMO_TCP_SERVER_Status_t ATMO_TCP_SERVER_SetConfiguration( ATMO_DriverInstanceHandle_t instance, const ATMO_TCP_SERVER_Config_t *config )
{
	if ( !( instance < numTcpServerDriverInstances ) )
	{
		return ATMO_TCP_SERVER_Status_Invalid;
	}

	return tcpServerInstances[instance]->SetConfiguration( tcpServerInstancesData[instance], config );
}


ATMO_TCP_SERVER_Status_t ATMO_TCP_SERVER_Start( ATMO_DriverInstanceHandle_t instance )
{
	if ( !( instance < numTcpServerDriverInstances ) )
	{
		return ATMO_TCP_SERVER_Status_Invalid;
	}

	return tcpServerInstances[instance]->Start( tcpServerInstancesData[instance] );
}

ATMO_TCP_SERVER_Status_t ATMO_TCP_SERVER_Stop( ATMO_DriverInstanceHandle_t instance )
{
	if ( !( instance < numTcpServerDriverInstances ) )
	{
		return ATMO_TCP_SERVER_Status_Invalid;
	}

	return tcpServerInstances[instance]->Stop( tcpServerInstancesData[instance] );
}

ATMO_TCP_SERVER_Status_t ATMO_TCP_SERVER_RegisterEventCallback( ATMO_DriverInstanceHandle_t instance, ATMO_TCP_SERVER_Event_t event, ATMO_Callback_t cb )
{
	if ( !( instance < numTcpServerDriverInstances ) )
	{
		return ATMO_TCP_SERVER_Status_Invalid;
	}

	return tcpServerInstances[instance]->RegisterEventCallback( tcpServerInstancesData[instance], event, cb );
}

ATMO_TCP_SERVER_Status_t ATMO_TCP_SERVER_GetNumAvailableBytes( ATMO_DriverInstanceHandle_t instance, uint32_t connectionId, uint32_t *availableBytes )
{
	if ( !( instance < numTcpServerDriverInstances ) )
	{
		return ATMO_TCP_SERVER_Status_Invalid;
	}

	return tcpServerInstances[instance]->GetNumAvailableBytes( tcpServerInstancesData[instance], connectionId, availableBytes );
}

ATMO_TCP_SERVER_Status_t ATMO_TCP_SERVER_ReadBytes( ATMO_DriverInstanceHandle_t instance, uint32_t connectionId, uint8_t *buffer, uint32_t numBytes )
{
	if ( !( instance < numTcpServerDriverInstances ) )
	{
		return ATMO_TCP_SERVER_Status_Invalid;
	}

	return tcpServerInstances[instance]->ReadBytes( tcpServerInstancesData[instance], connectionId, buffer, numBytes );
}

ATMO_TCP_SERVER_Status_t ATMO_TCP_SERVER_WriteBytes( ATMO_DriverInstanceHandle_t instance, uint32_t connectionId, uint8_t *buffer, uint32_t numBytes )
{
	if ( !( instance < numTcpServerDriverInstances ) )
	{
		return ATMO_TCP_SERVER_Status_Invalid;
	}

	return tcpServerInstances[instance]->WriteBytes( tcpServerInstancesData[instance], connectionId, buffer, numBytes );
}

ATMO_TCP_SERVER_Status_t ATMO_TCP_SERVER_CloseConnection( ATMO_DriverInstanceHandle_t instance, uint32_t connectionId )
{
	if ( !( instance < numTcpServerDriverInstances ) )
	{
		return ATMO_TCP_SERVER_Status_Invalid;
	}

	return tcpServerInstances[instance]->CloseConnection( tcpServerInstancesData[instance], connectionId );
}






