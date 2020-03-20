/**
 ******************************************************************************
 * @file    pwm.c
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - Core PWM Driver
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

#include "pwm.h"

#ifdef ATMO_SLIM_STACK
	#ifndef NUMBER_OF_PWM_DRIVER_INSTANCES
		#define NUMBER_OF_PWM_DRIVER_INSTANCES 1
	#endif
#endif

#ifndef NUMBER_OF_PWM_DRIVER_INSTANCES
#define NUMBER_OF_PWM_DRIVER_INSTANCES 1
#endif

ATMO_DriverInstanceHandle_t numberOfPWMDriverInstance = 0;

static const ATMO_PWM_DriverInstance_t *pwmInstances[NUMBER_OF_PWM_DRIVER_INSTANCES];
static ATMO_DriverInstanceData_t *pwmInstancesData[NUMBER_OF_PWM_DRIVER_INSTANCES];

ATMO_Status_t ATMO_PWM_AddDriverInstance( const ATMO_PWM_DriverInstance_t *driverInstance, ATMO_DriverInstanceData_t *driverInstanceData, ATMO_DriverInstanceHandle_t *instanceNumber )
{
	if ( !( numberOfPWMDriverInstance < NUMBER_OF_PWM_DRIVER_INSTANCES ) )
	{
		return ATMO_Status_OutOfMemory;
	}

	pwmInstances[numberOfPWMDriverInstance] = driverInstance;
	pwmInstancesData[numberOfPWMDriverInstance] = driverInstanceData;
	*instanceNumber = numberOfPWMDriverInstance;

	numberOfPWMDriverInstance++;

	return ATMO_Status_Success;
}

ATMO_PWM_Status_t ATMO_PWM_Init( ATMO_DriverInstanceHandle_t instance )
{
	if ( !( instance < numberOfPWMDriverInstance ) )
	{
		return ATMO_PWM_Status_Invalid;
	}

	return pwmInstances[instance]->Init( pwmInstancesData[instance] );
}

ATMO_PWM_Status_t ATMO_PWM_DeInit( ATMO_DriverInstanceHandle_t instance )
{
	if ( !( instance < numberOfPWMDriverInstance ) )
	{
		return ATMO_PWM_Status_Invalid;
	}

	return pwmInstances[instance]->DeInit( pwmInstancesData[instance] );
}

ATMO_PWM_Status_t ATMO_PWM_SetPinConfiguration( ATMO_DriverInstanceHandle_t instance, ATMO_GPIO_Device_Pin_t pin, ATMO_PWM_Config_t *config )
{
	if ( !( instance < numberOfPWMDriverInstance ) )
	{
		return ATMO_PWM_Status_Invalid;
	}

	return pwmInstances[instance]->SetPinConfiguration( pwmInstancesData[instance], pin, config );
}

ATMO_PWM_Status_t ATMO_PWM_GetPinConfiguration( ATMO_DriverInstanceHandle_t instance, ATMO_GPIO_Device_Pin_t pin, ATMO_PWM_Config_t *config )
{
	if ( !( instance < numberOfPWMDriverInstance ) )
	{
		return ATMO_PWM_Status_Invalid;
	}

	return pwmInstances[instance]->GetPinConfiguration( pwmInstancesData[instance], pin, config );
}

ATMO_PWM_Status_t ATMO_PWM_Enable( ATMO_DriverInstanceHandle_t instance, ATMO_GPIO_Device_Pin_t pin )
{
	if ( !( instance < numberOfPWMDriverInstance ) )
	{
		return ATMO_PWM_Status_Invalid;
	}

	return pwmInstances[instance]->Enable( pwmInstancesData[instance], pin );
}

ATMO_PWM_Status_t ATMO_PWM_Disable( ATMO_DriverInstanceHandle_t instance, ATMO_GPIO_Device_Pin_t pin )
{
	if ( !( instance < numberOfPWMDriverInstance ) )
	{
		return ATMO_PWM_Status_Invalid;
	}

	return pwmInstances[instance]->Disable( pwmInstancesData[instance], pin );
}

