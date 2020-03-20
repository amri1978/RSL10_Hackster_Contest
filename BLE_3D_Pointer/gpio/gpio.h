/**
 ******************************************************************************
 * @file    gpio.h
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - GPIO header file
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

/** \addtogroup GPIO
 * The GPIO driver is used for access and control of an end device’s basic general purpose input/output (GPIO) functionality.
 * This includes reading and writing GPIO pins and configuring a GPIO pin as an input or output.
 *  @{
 */


/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef __ATMO_GPIO__H
#define __ATMO_GPIO__H


/* Includes ------------------------------------------------------------------*/
#include "../atmo/core.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported Constants --------------------------------------------------------*/

/* Exported Macros -----------------------------------------------------------*/

/* Exported Types ------------------------------------------------------------*/

/**
 * GPIO Pin Mode
 */
typedef enum
{
	ATMO_GPIO_PinMode_Disabled                            = 0x0000u,
	ATMO_GPIO_PinMode_Input_HighImpedance                 = 0x0001u,
	ATMO_GPIO_PinMode_Input_PullUp                        = 0x0101u,
	ATMO_GPIO_PinMode_Input_PullDown                      = 0x0201u,
	ATMO_GPIO_PinMode_Output_PushPull                     = 0x0002u,
	ATMO_GPIO_PinMode_Output_OpenDrain                    = 0x0003u,
	ATMO_GPIO_PinMode_Output_OpenDrainPullUp              = 0x0103u,
} ATMO_GPIO_PinMode_t;

/**
 * GPIO Pin State
 */
typedef enum
{
	ATMO_GPIO_PinState_Low   = 0x00u,
	ATMO_GPIO_PinState_High  = 0x01u,
	ATMO_GPIO_PinState_Error = 0xFFu,
} ATMO_GPIO_PinState_t;

/**
 * GPIO Interrupt Trigger
 */
typedef enum
{
	ATMO_GPIO_InterruptTrigger_None         = 0x00u,
	ATMO_GPIO_InterruptTrigger_RisingEdge   = 0x01u,
	ATMO_GPIO_InterruptTrigger_FallingEdge  = 0x02u,
	ATMO_GPIO_InterruptTrigger_BothEdges    = 0x03u,
	ATMO_GPIO_InterruptTrigger_LogicZero    = 0x04u,
	ATMO_GPIO_InterruptTrigger_LogicOne     = 0x05u,
	ATMO_GPIO_InterruptTrigger_DirectCallback = 0x80u, // OR this with the trigger to have the callback called directly, not going through the ATMO framework
} ATMO_GPIO_InterruptTrigger_t;

/**
 * GPIO Driver Return Enumerations
 */
typedef enum
{
	ATMO_GPIO_Status_Success       = 0x00u,  /**< Operation was successful */
	ATMO_GPIO_Status_Fail          = 0x01u,  /**< Operation failed */
	ATMO_GPIO_Status_Initialized   = 0x02u,  /**< Peripheral already initialized */
	ATMO_GPIO_Status_Invalid       = 0x03u,  /**< Invalid operation */
	ATMO_GPIO_Status_NotSupported  = 0x04u,  /**< Feature not supported by platform */
	ATMO_GPIO_Status_Unspecified   = 0x05u,  /**< Some other status not defined */
} ATMO_GPIO_Status_t;

/**
 * Generic typedef for a device pin
 */
typedef uint32_t ATMO_GPIO_Device_Pin_t;

#define ATMO_GPIO_NO_PIN 0xFFFFFFFF


/* Exported Structures -------------------------------------------------------*/

/**
 * GPIO Pin Configuration Structure
 */
typedef struct
{
	ATMO_GPIO_PinMode_t pinMode; /**< Pin Mode */
	ATMO_GPIO_PinState_t initialState; /**< Initial pin state. Only relevant for output modes. */
} ATMO_GPIO_Config_t;

// Some C gore so we can use the struct within itself
typedef struct ATMO_GPIO_DriverInstance_t ATMO_GPIO_DriverInstance_t;

struct ATMO_GPIO_DriverInstance_t
{
	ATMO_GPIO_Status_t ( *Init )( ATMO_DriverInstanceData_t *instanceData );
	ATMO_GPIO_Status_t ( *DeInit )( ATMO_DriverInstanceData_t *instanceData );
	ATMO_GPIO_Status_t ( *SetPinConfiguration )( ATMO_DriverInstanceData_t *instanceData, ATMO_GPIO_Device_Pin_t pin, const ATMO_GPIO_Config_t *config );
	ATMO_GPIO_Status_t ( *GetPinConfiguration )( ATMO_DriverInstanceData_t *instanceData, ATMO_GPIO_Device_Pin_t pin, ATMO_GPIO_Config_t *config );
	ATMO_GPIO_Status_t ( *RegisterInterruptAbilityHandle )( ATMO_DriverInstanceData_t *instanceData, ATMO_GPIO_Device_Pin_t pin, uint8_t trigger, ATMO_AbilityHandle_t abilityHandle );
	ATMO_GPIO_Status_t ( *RegisterInterruptCallback )( ATMO_DriverInstanceData_t *instanceData, ATMO_GPIO_Device_Pin_t pin, uint8_t trigger, ATMO_Callback_t cb );
	ATMO_GPIO_Status_t ( *SetPinState )( ATMO_DriverInstanceData_t *instanceData, ATMO_GPIO_Device_Pin_t pin, ATMO_GPIO_PinState_t state );
	ATMO_GPIO_Status_t ( *GetPinState )( ATMO_DriverInstanceData_t *instanceData, ATMO_GPIO_Device_Pin_t pin, ATMO_GPIO_PinState_t *state );
	ATMO_GPIO_PinState_t ( *Read )( ATMO_DriverInstanceData_t *instanceData, ATMO_GPIO_Device_Pin_t pin );
	ATMO_GPIO_Status_t ( *Toggle )( ATMO_DriverInstanceData_t *instanceData, ATMO_GPIO_Device_Pin_t pin );
};

/* Exported Function Prototypes -----------------------------------------------*/

/**
 * This routine will add an instance of a driver for GPIO into the GPIO driver handler.
 *
 */
ATMO_Status_t ATMO_GPIO_AddDriverInstance( const ATMO_GPIO_DriverInstance_t *driverInstance, ATMO_DriverInstanceData_t *driverInstanceData, ATMO_DriverInstanceHandle_t *instanceNumber );

/**
 * Initialize GPIO Driver
 *
 * @param[in] instance
 *
 * @return status
 */
ATMO_GPIO_Status_t ATMO_GPIO_Init( ATMO_DriverInstanceHandle_t instance );

/**
 * This routine de-initializes a pin by setting its configuration back to the default after reset.
 *
 * @param pin :The pin to be de-initialized.
 * @return status.
 */
ATMO_GPIO_Status_t ATMO_GPIO_DeInit( ATMO_DriverInstanceHandle_t instance );

/**
 * This routine configures a GPIO pin.
 *
 * @param pin :The pin to be initialized.
 * @param config :The pointer to the pin configuration structure.
 * @return status.
 */
ATMO_GPIO_Status_t ATMO_GPIO_SetPinConfiguration( ATMO_DriverInstanceHandle_t instance, ATMO_GPIO_Device_Pin_t pin, const ATMO_GPIO_Config_t *config );

/**
 * This routine gets a GPIO pin configuration.
 *
 * @param pin :The pin from which to retrieve configuration information.
 * @param config :The pointer to the pin configuration structure.
 * @return status.
 */
ATMO_GPIO_Status_t ATMO_GPIO_GetPinConfiguration( ATMO_DriverInstanceHandle_t instance, ATMO_GPIO_Device_Pin_t pin, ATMO_GPIO_Config_t *config );

/**
 * This routine will register or unregister a user-defined callback function for the specified pin.
 *
 * @param pin :The pin to be assigned the callback function.
 * @param trigger :The interrupt trigger type.
 * @param abilityHandle: abilityHandle to register
 * @return status.
 */
ATMO_GPIO_Status_t ATMO_GPIO_RegisterInterruptAbilityHandle( ATMO_DriverInstanceHandle_t instance, ATMO_GPIO_Device_Pin_t pin, uint8_t trigger, ATMO_AbilityHandle_t abilityHandle );

/**
 * This routine will register or unregister a user-defined callback function for the specified pin.
 *
 * @param pin :The pin to be assigned the callback function.
 * @param trigger :The interrupt trigger type.
 * @param cb :The pointer to the callback function. Set to NULL to unregister a previously registered callback function.
 * @param direct: Call the ISR directly without going through the atmosphere stack
 * @return status.
 */
ATMO_GPIO_Status_t ATMO_GPIO_RegisterInterruptCallback( ATMO_DriverInstanceHandle_t instance, ATMO_GPIO_Device_Pin_t pin, uint8_t trigger, ATMO_Callback_t cb );


/**
 * This routine sets the output value of a pin.
 *
 * @param pin :The pin to be controlled.
 * @param state :The state which the pin will be set to.
 * @return status.
 */
ATMO_GPIO_Status_t ATMO_GPIO_SetPinState( ATMO_DriverInstanceHandle_t instance, ATMO_GPIO_Device_Pin_t pin, ATMO_GPIO_PinState_t state );

/**
 * This routine gets the current value of a pin.
 *
 * @param pin :The pin to be evaluated.
 * @param state :The current pin state.
 * @return status.
 */
ATMO_GPIO_Status_t ATMO_GPIO_GetPinState( ATMO_DriverInstanceHandle_t instance, ATMO_GPIO_Device_Pin_t pin, ATMO_GPIO_PinState_t *state );

/**
 * This routine gets the current value of a pin.
 *
 * @param pin :The pin to be evaluated.
 * @return The current pin state.
 */
ATMO_GPIO_PinState_t ATMO_GPIO_Read( ATMO_DriverInstanceHandle_t instance, ATMO_GPIO_Device_Pin_t pin );

/**
 * This routine toggles the output value of a pin.
 *
 * @param pin :The pin to be toggled.
 * @return status
 */
ATMO_GPIO_Status_t ATMO_GPIO_Toggle( ATMO_DriverInstanceHandle_t instance, ATMO_GPIO_Device_Pin_t pin );

/**
 * @brief Given an interrupt trigger mask (as passed into the registration functions), remove the Direct Interrupt Bit and just get the trigger
 *
 * @param maskedTrigger
 * @return ATMO_GPIO_InterruptTrigger_t
 */
ATMO_GPIO_InterruptTrigger_t ATMO_GPIO_GetInterruptTrigger( uint8_t maskedTrigger );

/**
 * @brief Given an interrupt trigger mask, get the bit indicating whether or not it's a direct interrupt
 *
 * @param maskedTrigger
 * @return true
 * @return false
 */
ATMO_BOOL_t ATMO_GPIO_IsDirectInterrupt( uint8_t maskedTrigger );


#ifdef __cplusplus
}
#endif

#endif /* __ATMO_GPIO__H */
/** @}*/