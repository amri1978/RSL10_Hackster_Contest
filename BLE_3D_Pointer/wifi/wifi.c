/**
 ******************************************************************************
 * @file    wifi.c
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - Core WIFI Driver
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

#include "wifi.h"
#include "../cloud/cloud.h"

#ifdef ATMO_SLIM_STACK
	#ifndef NUMBER_OF_WIFI_DRIVER_INSTANCES
		#define NUMBER_OF_WIFI_DRIVER_INSTANCES 1
	#endif
#endif

#ifndef NUMBER_OF_WIFI_DRIVER_INSTANCES
#define NUMBER_OF_WIFI_DRIVER_INSTANCES 1
#endif

ATMO_DriverInstanceHandle_t numberOfWIFIDriverInstance = 0;

const ATMO_WIFI_DriverInstance_t *wifiInstances[NUMBER_OF_WIFI_DRIVER_INSTANCES];
ATMO_DriverInstanceData_t *wifiInstancesData[NUMBER_OF_WIFI_DRIVER_INSTANCES];
static ATMO_BOOL_t wifiDriverEnabled[NUMBER_OF_WIFI_DRIVER_INSTANCES] = {true};

ATMO_Status_t ATMO_WIFI_AddDriverInstance( const ATMO_WIFI_DriverInstance_t *driverInstance, ATMO_DriverInstanceData_t *driverInstanceData, ATMO_DriverInstanceHandle_t *instanceNumber )
{
	if ( !( numberOfWIFIDriverInstance < NUMBER_OF_WIFI_DRIVER_INSTANCES ) )
	{
		return ATMO_Status_OutOfMemory;
	}

	wifiInstances[numberOfWIFIDriverInstance] = driverInstance;
	wifiInstancesData[numberOfWIFIDriverInstance] = driverInstanceData;
	*instanceNumber = numberOfWIFIDriverInstance;

	numberOfWIFIDriverInstance++;

	return ATMO_Status_Success;
}

ATMO_WIFI_Status_t ATMO_WIFI_Init( ATMO_DriverInstanceHandle_t instance )
{
	if ( instance >= numberOfWIFIDriverInstance )
	{
		return ATMO_WIFI_Status_Invalid;
	}

	return wifiInstances[instance]->Init( wifiInstancesData[instance] );
}

ATMO_WIFI_Status_t ATMO_WIFI_DeInit( ATMO_DriverInstanceHandle_t instance )
{
	if ( instance >= numberOfWIFIDriverInstance || !wifiDriverEnabled[instance] )
	{
		return ATMO_WIFI_Status_Invalid;
	}

	return wifiInstances[instance]->DeInit( wifiInstancesData[instance] );
}

ATMO_WIFI_Status_t ATMO_WIFI_SetEnabled( ATMO_DriverInstanceHandle_t instance, ATMO_BOOL_t enabled )
{
	if ( instance >= numberOfWIFIDriverInstance )
	{
		return ATMO_WIFI_Status_Invalid;
	}

	wifiDriverEnabled[instance] = enabled;

	ATMO_CLOUD_ClearExtraRequiredSettings( ATMO_CLOUD_ExtraSettings_WIFI );

	if ( enabled )
	{
		ATMO_CLOUD_SetExtraRequiredSettings( ATMO_CLOUD_ExtraSettings_WIFI );
	}

	return ATMO_WIFI_Status_Success;
}

ATMO_WIFI_Status_t ATMO_WIFI_GetEnabled( ATMO_DriverInstanceHandle_t instance, ATMO_BOOL_t *enabled )
{
	if ( instance >= numberOfWIFIDriverInstance )
	{
		return ATMO_WIFI_Status_Invalid;
	}

	*enabled = wifiDriverEnabled[instance];

	return ATMO_WIFI_Status_Success;
}

ATMO_WIFI_Status_t ATMO_WIFI_SetConfiguration( ATMO_DriverInstanceHandle_t instance, const ATMO_WIFI_Config_t *config )
{
	if ( instance >= numberOfWIFIDriverInstance || !wifiDriverEnabled[instance] )
	{
		return ATMO_WIFI_Status_Invalid;
	}

	return wifiInstances[instance]->SetConfiguration( wifiInstancesData[instance], config );
}

ATMO_WIFI_Status_t ATMO_WIFI_Start( ATMO_DriverInstanceHandle_t instance )
{
	if ( instance >= numberOfWIFIDriverInstance || !wifiDriverEnabled[instance] )
	{
		return ATMO_WIFI_Status_Invalid;
	}

	return wifiInstances[instance]->Start( wifiInstancesData[instance] );
}

ATMO_WIFI_Status_t ATMO_WIFI_Stop( ATMO_DriverInstanceHandle_t instance )
{
	if ( instance >= numberOfWIFIDriverInstance || !wifiDriverEnabled[instance] )
	{
		return ATMO_WIFI_Status_Invalid;
	}

	return wifiInstances[instance]->Stop( wifiInstancesData[instance] );
}


ATMO_WIFI_Status_t ATMO_WIFI_GetIpAddress( ATMO_DriverInstanceHandle_t instance, char *ipBuf, uint32_t bufLen )
{
	if ( instance >= numberOfWIFIDriverInstance || !wifiDriverEnabled[instance] )
	{
		return ATMO_WIFI_Status_Invalid;
	}

	return wifiInstances[instance]->GetIpAddress( wifiInstancesData[instance], ipBuf, bufLen );
}


ATMO_WIFI_Status_t ATMO_WIFI_RegisterCallback( ATMO_DriverInstanceHandle_t instance, ATMO_WIFI_Event_t event, ATMO_Callback_t cb )
{
	if ( instance >= numberOfWIFIDriverInstance || !wifiDriverEnabled[instance] )
	{
		return ATMO_WIFI_Status_Invalid;
	}

	return wifiInstances[instance]->RegisterCallback( wifiInstancesData[instance], event, cb );
}

ATMO_WIFI_Status_t ATMO_WIFI_RegisterAbility( ATMO_DriverInstanceHandle_t instance, ATMO_WIFI_Event_t event, unsigned int abilityHandle )
{
	if ( instance >= numberOfWIFIDriverInstance || !wifiDriverEnabled[instance] )
	{
		return ATMO_WIFI_Status_Invalid;
	}

	return wifiInstances[instance]->RegisterAbility( wifiInstancesData[instance], event, abilityHandle );
}

ATMO_WIFI_Status_t ATMO_WIFI_GetStatus( ATMO_DriverInstanceHandle_t instance, ATMO_WIFI_ConnectionStatus_t *status )
{
	if ( instance >= numberOfWIFIDriverInstance || !wifiDriverEnabled[instance] )
	{
		return ATMO_WIFI_Status_Invalid;
	}

	return wifiInstances[instance]->GetStatus( wifiInstancesData[instance], status );
}


