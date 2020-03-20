/**
 ******************************************************************************
 * @file    gpio_ONSEMI.c
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - ONSEMI GPIO API Implementation
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
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
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

#include "gpio_onsemi.h"
#include <BDK.h>
#include <EventCallback.h>

#define GPIO_NUM_MAX 15

ATMO_GPIO_DriverInstance_t ONSEMIGPIODriverInstance =
{
	ATMO_ONSEMI_GPIO_Init,
	ATMO_ONSEMI_GPIO_DeInit,
	ATMO_ONSEMI_GPIO_SetPinConfiguration,
	ATMO_ONSEMI_GPIO_GetPinConfiguration,
	ATMO_ONSEMI_GPIO_RegisterInterruptAbilityHandle,
	ATMO_ONSEMI_GPIO_RegisterInterruptCallback,
	ATMO_ONSEMI_GPIO_SetPinState,
	ATMO_ONSEMI_GPIO_GetPinState,
	ATMO_ONSEMI_GPIO_Read,
	ATMO_ONSEMI_GPIO_Toggle
};

typedef enum
{
	_ATMO_ONSENSE_GPIO_BTN0,
	_ATMO_ONSENSE_GPIO_BTN1,
	_ATMO_ONSENSE_GPIO_GIO,
	_ATMO_ONSENSE_GPIO_BHI160,
	_ATMO_ONSENSE_GPIO_NUM_INTSOURCES
} _ATMO_ONSEMI_GPIO_IntSources_t;

typedef struct
{
	ATMO_Callback_t cb;
	ATMO_AbilityHandle_t abilityHandle;
	bool isCallback;
	ATMO_GPIO_Device_Pin_t pin;
	uint8_t irqn;
	uint8_t intSource;
	EventCallback_Type event;
	bool avilable;
} _ATMO_ONSEMI_GPIO_IntConfig_t;

static _ATMO_ONSEMI_GPIO_IntConfig_t _ATMO_ONSEMI_GPIO_IntConfig[_ATMO_ONSENSE_GPIO_NUM_INTSOURCES] = {0};


ATMO_Status_t ATMO_ONSEMI_GPIO_AddDriverInstance( ATMO_DriverInstanceHandle_t *instanceNumber )
{
	static ATMO_DriverInstanceData_t driverInstanceData;

	driverInstanceData.name = "";
	driverInstanceData.initialized = false;
	driverInstanceData.instanceNumber = 0;
	driverInstanceData.argument = NULL;

	return ATMO_GPIO_AddDriverInstance( &ONSEMIGPIODriverInstance, &driverInstanceData, instanceNumber );
}

void DIO0_IRQHandler( void )
{
	EventCallback_Call( 15 );

}

void DIO1_IRQHandler( void )
{
	EventCallback_Call( 12 );
}

void DIO2_IRQHandler( void )
{
	EventCallback_Call( 3 );
}

// BHI160
void DIO3_IRQHandler( void )
{
	ATMO_PLATFORM_DebugPrint("BHI160 ISR\r\n");
	EventCallback_Call( 9 );
}

static void _ATMO_ONSEMI_GPIO_EventHandler( void *data )
{
	_ATMO_ONSEMI_GPIO_IntConfig_t *intConfig = ( _ATMO_ONSEMI_GPIO_IntConfig_t * )data;

	ATMO_Value_t value;
	ATMO_InitValue( &value );
	ATMO_CreateValueUnsignedInt( &value, ATMO_ONSEMI_GPIO_Read( NULL, intConfig->pin ) );

	if ( intConfig->isCallback )
	{
		ATMO_AddCallbackExecute( intConfig->cb, &value );
	}
	else
	{
		ATMO_AddAbilityExecute( intConfig->abilityHandle, &value );
	}

	ATMO_FreeValue( &value );
}

ATMO_GPIO_Status_t ATMO_ONSEMI_GPIO_Init( ATMO_DriverInstanceData_t *instance )
{
	_ATMO_ONSEMI_GPIO_IntConfig[_ATMO_ONSENSE_GPIO_BTN0].pin = 15;
	_ATMO_ONSEMI_GPIO_IntConfig[_ATMO_ONSENSE_GPIO_BTN0].irqn = DIO0_IRQn;
	_ATMO_ONSEMI_GPIO_IntConfig[_ATMO_ONSENSE_GPIO_BTN0].intSource = 0;

	_ATMO_ONSEMI_GPIO_IntConfig[_ATMO_ONSENSE_GPIO_BTN1].pin = 12;
	_ATMO_ONSEMI_GPIO_IntConfig[_ATMO_ONSENSE_GPIO_BTN1].irqn = DIO1_IRQn;
	_ATMO_ONSEMI_GPIO_IntConfig[_ATMO_ONSENSE_GPIO_BTN1].intSource = 1;

	_ATMO_ONSEMI_GPIO_IntConfig[_ATMO_ONSENSE_GPIO_GIO].pin = 3;
	_ATMO_ONSEMI_GPIO_IntConfig[_ATMO_ONSENSE_GPIO_GIO].irqn = DIO2_IRQn;
	_ATMO_ONSEMI_GPIO_IntConfig[_ATMO_ONSENSE_GPIO_GIO].intSource = 2;

	_ATMO_ONSEMI_GPIO_IntConfig[_ATMO_ONSENSE_GPIO_BHI160].pin = 9;
	_ATMO_ONSEMI_GPIO_IntConfig[_ATMO_ONSENSE_GPIO_BHI160].irqn = DIO3_IRQn;
	_ATMO_ONSEMI_GPIO_IntConfig[_ATMO_ONSENSE_GPIO_BHI160].intSource = 3;
	return ATMO_GPIO_Status_Success;
}

ATMO_GPIO_Status_t ATMO_ONSEMI_GPIO_DeInit( ATMO_DriverInstanceData_t *instance )
{
	return ATMO_GPIO_Status_Success;
}

static inline bool _ATMO_ONSEMI_GPIO_IS_VALID( ATMO_GPIO_Device_Pin_t pin )
{
	return pin >= 0 && pin <= 15;
}

ATMO_GPIO_Status_t ATMO_ONSEMI_GPIO_SetPinConfiguration( ATMO_DriverInstanceData_t *instance, ATMO_GPIO_Device_Pin_t pin, const ATMO_GPIO_Config_t *config )
{
	if ( !_ATMO_ONSEMI_GPIO_IS_VALID( pin ) )
	{
		return ATMO_GPIO_Status_Fail;
	}

	uint32_t mask = 0;

	switch ( config->pinMode )
	{
		case ATMO_GPIO_PinMode_Disabled:
		{
			mask |= DIO_MODE_DISABLE;
			break;
		}

		case ATMO_GPIO_PinMode_Input_HighImpedance:
		{
			mask |= DIO_MODE_GPIO_IN_0 | DIO_NO_PULL;
			break;
		}

		case ATMO_GPIO_PinMode_Input_PullUp:
		{
			mask |= DIO_MODE_GPIO_IN_0 | DIO_WEAK_PULL_UP;
			break;
		}

		case ATMO_GPIO_PinMode_Input_PullDown:
		{
			mask |= DIO_MODE_GPIO_IN_0 | DIO_WEAK_PULL_DOWN;
			break;
		}

		case ATMO_GPIO_PinMode_Output_PushPull:
		{
			mask |= ( config->initialState == ATMO_GPIO_PinState_High ) ? DIO_MODE_GPIO_OUT_1 : DIO_MODE_GPIO_OUT_0;
			break;
		}

		default:
		{
			return ATMO_GPIO_Status_Invalid;
		}
	}

	Sys_DIO_Config( pin, mask );
	return ATMO_GPIO_Status_Success;
}

ATMO_GPIO_Status_t ATMO_ONSEMI_GPIO_GetPinConfiguration( ATMO_DriverInstanceData_t *instance, ATMO_GPIO_Device_Pin_t pin, ATMO_GPIO_Config_t *config )
{
	return ATMO_GPIO_Status_NotSupported;
}

static uint32_t _ATMO_ONSEMI_GPIO_GetIntMask( uint8_t atmoTrigger )
{
	ATMO_GPIO_InterruptTrigger_t unmasked = ATMO_GPIO_GetInterruptTrigger( atmoTrigger );

	uint32_t onsemiMask = 0;

	switch ( unmasked )
	{
		case ATMO_GPIO_InterruptTrigger_RisingEdge:
		{
			onsemiMask |= DIO_EVENT_RISING_EDGE;
			break;
		}

		case ATMO_GPIO_InterruptTrigger_FallingEdge:
		{
			onsemiMask |= DIO_EVENT_FALLING_EDGE;
			break;
		}

		case ATMO_GPIO_InterruptTrigger_BothEdges:
		{
			onsemiMask |= DIO_EVENT_TRANSITION;
			break;
		}
	}

	return onsemiMask;
}

/**
 * @brief Given a pin, get the interrupt source data structure
 *
 * @param pin
 * @return _ATMO_ONSEMI_GPIO_IntConfig_t*
 */
static _ATMO_ONSEMI_GPIO_IntConfig_t *_ATMO_ONSEMI_GPIO_GetIntEntry( ATMO_GPIO_Device_Pin_t pin )
{
	for ( unsigned int i = 0; i < _ATMO_ONSENSE_GPIO_NUM_INTSOURCES; i++ )
	{
		if ( _ATMO_ONSEMI_GPIO_IntConfig[i].pin == pin )
		{
			return &_ATMO_ONSEMI_GPIO_IntConfig[i];
		}
	}

	return NULL;
}

static ATMO_GPIO_Status_t _ATMO_ONSEMI_GPIO_RegisterInterrupt( ATMO_GPIO_Device_Pin_t pin, uint8_t trigger, _ATMO_ONSEMI_GPIO_IntConfig_t *intEntry )
{
	// Register interrupt
	uint32_t mask = _ATMO_ONSEMI_GPIO_GetIntMask( trigger );

	NVIC_DisableIRQ( intEntry->irqn );

	if ( mask != 0 )
	{
		// // Attach interrupt
		EventCallback_Remove( &intEntry->event );

		ATMO_PLATFORM_DebugPrint( "Registering int source %d pin %d irqn %d\r\n", intEntry->intSource, intEntry->pin, intEntry->irqn );

		Sys_DIO_IntConfig( intEntry->intSource, pin | mask | DIO_DEBOUNCE_DISABLE, DIO_DEBOUNCE_SLOWCLK_DIV32, 1 );

		// Event ID just needs to be unique
		EventCallback_Init( &intEntry->event, pin,
		                    ( EventCallback_Prototype ) &_ATMO_ONSEMI_GPIO_EventHandler, intEntry );
		EventCallback_Register( &intEntry->event );

		NVIC_ClearPendingIRQ( intEntry->irqn );
		NVIC_EnableIRQ( intEntry->irqn );
	}

	return ATMO_GPIO_Status_Success;
}


ATMO_GPIO_Status_t ATMO_ONSEMI_GPIO_RegisterInterruptAbilityHandle( ATMO_DriverInstanceData_t *instance, ATMO_GPIO_Device_Pin_t pin, uint8_t trigger, unsigned int abilityHandle )
{
	if ( !_ATMO_ONSEMI_GPIO_IS_VALID( pin ) )
	{
		return ATMO_GPIO_Status_Fail;
	}

	_ATMO_ONSEMI_GPIO_IntConfig_t *intEntry = _ATMO_ONSEMI_GPIO_GetIntEntry( pin );

	if ( intEntry == NULL )
	{
		return ATMO_GPIO_Status_Invalid;
	}

	intEntry->abilityHandle = abilityHandle;
	intEntry->isCallback = false;

	_ATMO_ONSEMI_GPIO_RegisterInterrupt( pin, trigger, intEntry );
	return ATMO_GPIO_Status_Success;
}

ATMO_GPIO_Status_t ATMO_ONSEMI_GPIO_RegisterInterruptCallback( ATMO_DriverInstanceData_t *instance, ATMO_GPIO_Device_Pin_t pin, uint8_t trigger, ATMO_Callback_t callback )
{
	if ( !_ATMO_ONSEMI_GPIO_IS_VALID( pin ) )
	{
		return ATMO_GPIO_Status_Fail;
	}

	_ATMO_ONSEMI_GPIO_IntConfig_t *intEntry = _ATMO_ONSEMI_GPIO_GetIntEntry( pin );

	if ( intEntry == NULL )
	{
		return ATMO_GPIO_Status_Invalid;
	}

	intEntry->cb = callback;
	intEntry->isCallback = true;

	_ATMO_ONSEMI_GPIO_RegisterInterrupt( pin, trigger, intEntry );
	return ATMO_GPIO_Status_Success;
}


ATMO_GPIO_Status_t ATMO_ONSEMI_GPIO_SetPinState( ATMO_DriverInstanceData_t *instance, ATMO_GPIO_Device_Pin_t pin, ATMO_GPIO_PinState_t state )
{
	if ( !_ATMO_ONSEMI_GPIO_IS_VALID( pin ) )
	{
		return ATMO_GPIO_Status_Fail;
	}

	Sys_DIO_Config( pin, ( state == ATMO_GPIO_PinState_High ) ? DIO_MODE_GPIO_OUT_1 : DIO_MODE_GPIO_OUT_0 );
	return ATMO_GPIO_Status_Success;
}

ATMO_GPIO_Status_t ATMO_ONSEMI_GPIO_GetPinState( ATMO_DriverInstanceData_t *instance, ATMO_GPIO_Device_Pin_t pin, ATMO_GPIO_PinState_t *state )
{
	if ( !_ATMO_ONSEMI_GPIO_IS_VALID( pin ) )
	{
		return ATMO_GPIO_Status_Fail;
	}

	*state = DIO_DATA->ALIAS[pin] ? ATMO_GPIO_PinState_High : ATMO_GPIO_PinState_Low;

	return ATMO_GPIO_Status_Success;
}

ATMO_GPIO_PinState_t ATMO_ONSEMI_GPIO_Read( ATMO_DriverInstanceData_t *instance, ATMO_GPIO_Device_Pin_t pin )
{
	ATMO_GPIO_PinState_t state;

	if ( ATMO_ONSEMI_GPIO_GetPinState( instance, pin, &state ) != ATMO_GPIO_Status_Success )
	{
		return ATMO_GPIO_PinState_Error;
	}

	return state;
}

ATMO_GPIO_Status_t ATMO_ONSEMI_GPIO_Toggle( ATMO_DriverInstanceData_t *instance, ATMO_GPIO_Device_Pin_t pin )
{
	ATMO_GPIO_PinState_t state;

	if ( ATMO_ONSEMI_GPIO_GetPinState( instance, pin, &state ) != ATMO_GPIO_Status_Success )
	{
		return ATMO_GPIO_PinState_Error;
	}

	if ( state == ATMO_GPIO_PinState_High )
	{
		return ATMO_ONSEMI_GPIO_SetPinState( instance, pin, ATMO_GPIO_PinState_Low );
	}

	else
	{
		return ATMO_ONSEMI_GPIO_SetPinState( instance, pin, ATMO_GPIO_PinState_High );
	}
}
