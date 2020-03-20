/**
 ******************************************************************************
 * @file    tcpclient.c
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - Core TCP Client Driver
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

#include "tcpclient.h"

#ifdef ATMO_SLIM_STACK
	#ifndef NUMBER_OF_TCP_CLIENT_DRIVER_INSTANCES
		#define NUMBER_OF_TCP_CLIENT_DRIVER_INSTANCES 1
	#endif
#endif

#ifndef NUMBER_OF_TCP_CLIENT_DRIVER_INSTANCES
#define NUMBER_OF_TCP_CLIENT_DRIVER_INSTANCES 2
#endif

ATMO_DriverInstanceHandle_t numTcpClientDriverInstances = 0;

const ATMO_TCP_CLIENT_DriverInstance_t *tcpClientInstances[NUMBER_OF_TCP_CLIENT_DRIVER_INSTANCES];
ATMO_DriverInstanceData_t *tcpClientInstancesData[NUMBER_OF_TCP_CLIENT_DRIVER_INSTANCES];

ATMO_Status_t ATMO_TCP_CLIENT_AddDriverInstance( const ATMO_TCP_CLIENT_DriverInstance_t *driverInstance, ATMO_DriverInstanceData_t *driverInstanceData, ATMO_DriverInstanceHandle_t *instanceNumber )
{
	if ( !( numTcpClientDriverInstances < NUMBER_OF_TCP_CLIENT_DRIVER_INSTANCES ) )
	{
		return ATMO_Status_OutOfMemory;
	}

	tcpClientInstances[numTcpClientDriverInstances] = driverInstance;
	tcpClientInstancesData[numTcpClientDriverInstances] = driverInstanceData;
	*instanceNumber = numTcpClientDriverInstances;

	numTcpClientDriverInstances++;

	return ATMO_Status_Success;
}

ATMO_TCP_CLIENT_Status_t ATMO_TCP_CLIENT_Init( ATMO_DriverInstanceHandle_t instance )
{
	if ( !( instance < numTcpClientDriverInstances ) )
	{
		return ATMO_TCP_CLIENT_Status_Invalid;
	}

	return tcpClientInstances[instance]->Init( tcpClientInstancesData[instance] );
}

ATMO_TCP_CLIENT_Status_t ATMO_TCP_CLIENT_DeInit( ATMO_DriverInstanceHandle_t instance )
{
	if ( !( instance < numTcpClientDriverInstances ) )
	{
		return ATMO_TCP_CLIENT_Status_Invalid;
	}

	return tcpClientInstances[instance]->DeInit( tcpClientInstancesData[instance] );
}


ATMO_TCP_CLIENT_Status_t ATMO_TCP_CLIENT_SetConfiguration( ATMO_DriverInstanceHandle_t instance, const ATMO_TCP_CLIENT_Config_t *config )
{
	if ( !( instance < numTcpClientDriverInstances ) )
	{
		return ATMO_TCP_CLIENT_Status_Invalid;
	}

	return tcpClientInstances[instance]->SetConfiguration( tcpClientInstancesData[instance], config );
}


ATMO_TCP_CLIENT_Status_t ATMO_TCP_CLIENT_Connect( ATMO_DriverInstanceHandle_t instance, const char *host, unsigned int port, ATMO_BOOL_t useSSL )
{
	if ( !( instance < numTcpClientDriverInstances ) )
	{
		return ATMO_TCP_CLIENT_Status_Invalid;
	}

	return tcpClientInstances[instance]->Connect( tcpClientInstancesData[instance], host, port, useSSL );
}

ATMO_TCP_CLIENT_Status_t ATMO_TCP_CLIENT_Disconnect( ATMO_DriverInstanceHandle_t instance )
{
	if ( !( instance < numTcpClientDriverInstances ) )
	{
		return ATMO_TCP_CLIENT_Status_Invalid;
	}

	return tcpClientInstances[instance]->Disconnect( tcpClientInstancesData[instance] );
}

ATMO_TCP_CLIENT_Status_t ATMO_TCP_CLIENT_GetConnectionStatus( ATMO_DriverInstanceHandle_t instance, ATMO_TCP_CLIENT_ConnectionStatus_t *status )
{
	if ( !( instance < numTcpClientDriverInstances ) )
	{
		return ATMO_TCP_CLIENT_Status_Invalid;
	}

	return tcpClientInstances[instance]->GetConnectionStatus( tcpClientInstancesData[instance], status );
}

ATMO_TCP_CLIENT_Status_t ATMO_TCP_CLIENT_WriteBytes( ATMO_DriverInstanceHandle_t instance, uint8_t *data, unsigned int dataLen )
{
	if ( !( instance < numTcpClientDriverInstances ) )
	{
		return ATMO_TCP_CLIENT_Status_Invalid;
	}

	return tcpClientInstances[instance]->WriteBytes( tcpClientInstancesData[instance], data, dataLen );
}

ATMO_TCP_CLIENT_Status_t ATMO_TCP_CLIENT_SetReceiveCallback( ATMO_DriverInstanceHandle_t instance, ATMO_Callback_t cb )
{
	if ( !( instance < numTcpClientDriverInstances ) )
	{
		return ATMO_TCP_CLIENT_Status_Invalid;
	}

	return tcpClientInstances[instance]->SetReceiveCallback( tcpClientInstancesData[instance], cb );
}

ATMO_TCP_CLIENT_Status_t ATMO_TCP_CLIENT_GetNumAvailableBytes( ATMO_DriverInstanceHandle_t instance, uint32_t *numBytes, uint8_t **bytePtr )
{
	if ( !( instance < numTcpClientDriverInstances ) )
	{
		return ATMO_TCP_CLIENT_Status_Invalid;
	}

	return tcpClientInstances[instance]->GetNumAvailableBytes( tcpClientInstancesData[instance], numBytes, bytePtr );
}
