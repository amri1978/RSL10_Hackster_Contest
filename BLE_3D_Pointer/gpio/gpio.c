/**
 ******************************************************************************
 * @file    gpio.c
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - Core GPIO Driver
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

#include "../app_src/atmosphere_platform.h"
#include "gpio.h"

#ifdef ATMO_SLIM_STACK
	#ifndef NUMBER_OF_GPIO_DRIVER_INSTANCES
		#define NUMBER_OF_GPIO_DRIVER_INSTANCES 1
	#endif
#endif

#ifndef NUMBER_OF_GPIO_DRIVER_INSTANCES
#define NUMBER_OF_GPIO_DRIVER_INSTANCES 1
#endif

ATMO_DriverInstanceHandle_t numberOfGPIODriverInstance = 0;

const ATMO_GPIO_DriverInstance_t *gpioInstances[NUMBER_OF_GPIO_DRIVER_INSTANCES];
ATMO_DriverInstanceData_t *gpioInstancesData[NUMBER_OF_GPIO_DRIVER_INSTANCES];

ATMO_Status_t ATMO_GPIO_AddDriverInstance( const ATMO_GPIO_DriverInstance_t *driverInstance, ATMO_DriverInstanceData_t *driverInstanceData, ATMO_DriverInstanceHandle_t *instanceNumber )
{
	if ( !( numberOfGPIODriverInstance < NUMBER_OF_GPIO_DRIVER_INSTANCES ) )
	{
		return ATMO_Status_OutOfMemory;
	}

	gpioInstances[numberOfGPIODriverInstance] = driverInstance;
	gpioInstancesData[numberOfGPIODriverInstance] = driverInstanceData;
	*instanceNumber = numberOfGPIODriverInstance;

	numberOfGPIODriverInstance++;

	return ATMO_Status_Success;
}

ATMO_GPIO_Status_t ATMO_GPIO_Init( ATMO_DriverInstanceHandle_t instance )
{
	if ( !( instance < numberOfGPIODriverInstance ) )
	{
		return ATMO_GPIO_Status_Invalid;
	}

	return gpioInstances[instance]->Init( gpioInstancesData[instance] );
}

ATMO_GPIO_Status_t ATMO_GPIO_DeInit( ATMO_DriverInstanceHandle_t instance )
{
	if ( !( instance < numberOfGPIODriverInstance ) )
	{
		return ATMO_GPIO_Status_Invalid;
	}

	return gpioInstances[instance]->DeInit( gpioInstancesData[instance] );
}

ATMO_GPIO_Status_t ATMO_GPIO_SetPinConfiguration( ATMO_DriverInstanceHandle_t instance, ATMO_GPIO_Device_Pin_t pin,
        const ATMO_GPIO_Config_t *config )
{
	if ( !( instance < numberOfGPIODriverInstance ) )
	{
		return ATMO_GPIO_Status_Invalid;
	}

	return gpioInstances[instance]->SetPinConfiguration( gpioInstancesData[instance], pin, config );
}

ATMO_GPIO_Status_t ATMO_GPIO_GetPinConfiguration( ATMO_DriverInstanceHandle_t instance, ATMO_GPIO_Device_Pin_t pin,
        ATMO_GPIO_Config_t *config )
{
	if ( !( instance < numberOfGPIODriverInstance ) )
	{
		return ATMO_GPIO_Status_Invalid;
	}

	return gpioInstances[instance]->GetPinConfiguration( gpioInstancesData[instance], pin, config );
}

ATMO_GPIO_Status_t ATMO_GPIO_RegisterInterruptAbilityHandle( ATMO_DriverInstanceHandle_t instance, ATMO_GPIO_Device_Pin_t pin,
        uint8_t trigger, ATMO_AbilityHandle_t abilityHandle )
{
	if ( !( instance < numberOfGPIODriverInstance ) )
	{
		return ATMO_GPIO_Status_Invalid;
	}

	return gpioInstances[instance]->RegisterInterruptAbilityHandle( gpioInstancesData[instance], pin, trigger, abilityHandle );
}

ATMO_GPIO_Status_t ATMO_GPIO_RegisterInterruptCallback( ATMO_DriverInstanceHandle_t instance, ATMO_GPIO_Device_Pin_t pin,
        uint8_t trigger, ATMO_Callback_t callback )
{
	if ( !( instance < numberOfGPIODriverInstance ) )
	{
		return ATMO_GPIO_Status_Invalid;
	}

	return gpioInstances[instance]->RegisterInterruptCallback( gpioInstancesData[instance], pin, trigger, callback );
}

ATMO_GPIO_Status_t ATMO_GPIO_SetPinState( ATMO_DriverInstanceHandle_t instance, ATMO_GPIO_Device_Pin_t pin,
        ATMO_GPIO_PinState_t state )
{
	if ( !( instance < numberOfGPIODriverInstance ) )
	{
		return ATMO_GPIO_Status_Invalid;
	}

	return gpioInstances[instance]->SetPinState( gpioInstancesData[instance], pin, state );
}

ATMO_GPIO_Status_t ATMO_GPIO_GetPinState( ATMO_DriverInstanceHandle_t instance, ATMO_GPIO_Device_Pin_t pin,
        ATMO_GPIO_PinState_t *state )
{
	if ( !( instance < numberOfGPIODriverInstance ) )
	{
		return ATMO_GPIO_Status_Invalid;
	}

	return gpioInstances[instance]->GetPinState( gpioInstancesData[instance], pin, state );
}

ATMO_GPIO_PinState_t ATMO_GPIO_Read( ATMO_DriverInstanceHandle_t instance, ATMO_GPIO_Device_Pin_t pin )
{
	if ( !( instance < numberOfGPIODriverInstance ) )
	{
		return ATMO_GPIO_PinState_Error;
	}

	return gpioInstances[instance]->Read( gpioInstancesData[instance], pin );
}

ATMO_GPIO_Status_t ATMO_GPIO_Toggle( ATMO_DriverInstanceHandle_t instance, ATMO_GPIO_Device_Pin_t pin )
{
	if ( !( instance < numberOfGPIODriverInstance ) )
	{
		return ATMO_GPIO_Status_Invalid;
	}

	return gpioInstances[instance]->Toggle( gpioInstancesData[instance], pin );
}

/**
 * @brief Given an interrupt trigger mask (as passed into the registration functions), remove the Direct Interrupt Bit and just get the trigger
 *
 * @param maskedTrigger
 * @return ATMO_GPIO_InterruptTrigger_t
 */
ATMO_GPIO_InterruptTrigger_t ATMO_GPIO_GetInterruptTrigger( uint8_t maskedTrigger )
{
	return ( ATMO_GPIO_InterruptTrigger_t ) maskedTrigger & 0x0F;
}

/**
 * @brief Given an interrupt trigger mask, get the bit indicating whether or not it's a direct interrupt
 *
 * @param maskedTrigger
 * @return true
 * @return false
 */
ATMO_BOOL_t ATMO_GPIO_IsDirectInterrupt( uint8_t maskedTrigger )
{
	return ( maskedTrigger & ATMO_GPIO_InterruptTrigger_DirectCallback ) == ATMO_GPIO_InterruptTrigger_DirectCallback;
}