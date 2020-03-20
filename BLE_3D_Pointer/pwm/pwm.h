/**
 ******************************************************************************
 * @file    pwm.h
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - PWM header file
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

/** \addtogroup PWM
 * The PWM driver is used for configuring pulse-width modulation (PWM) on an end device’s pin.
 *  @{
 */

#ifndef __ATMO_PWM_H_
#define __ATMO_PWM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "../atmo/core.h"
#include "../gpio/gpio.h"

/**
 * PWM Driver Return Enumerations
 */
typedef enum
{
	ATMO_PWM_Status_Success       = 0x00u,  /**< Operation was successful */
	ATMO_PWM_Status_Fail          = 0x01u,  /**< Operation failed */
	ATMO_PWM_Status_Initialized   = 0x02u,  /**< Peripheral already initialized */
	ATMO_PWM_Status_Invalid       = 0x03u,  /**< Invalid operation */
	ATMO_PWM_Status_NotSupported  = 0x04u,  /**< Feature not supported by platform */
	ATMO_PWM_Status_Unknown       = 0x05u,  /**< Some other status not defined */
	ATMO_PWM_Status_Busy          = 0x20u,  /**< Conversion in progress */
	ATMO_PWM_Status_Timeout       = 0x21u,  /**< Conversion timed out */
} ATMO_PWM_Status_t;

/**
 * PWM Output Level
 */
typedef enum
{
	ATMO_PWM_OutputLevel_Disabled    = 0x00u,
	ATMO_PWM_OutputLevel_ActiveLow   = 0x01u,
	ATMO_PWM_OutputLevel_ActiveHigh  = 0x02u,
} ATMO_PWM_OutputLevel_t;


/* Exported Structures -------------------------------------------------------*/

/**
 * PWM Peripheral Configuration
 */
typedef struct
{
	uint32_t pwmFreqHz;
	uint8_t dutyCycle; // Duty cycle between 0 and 100
} ATMO_PWM_Config_t;

typedef struct
{
	ATMO_PWM_Status_t ( *Init )( ATMO_DriverInstanceData_t *instanceData );
	ATMO_PWM_Status_t ( *DeInit )( ATMO_DriverInstanceData_t *instanceData );
	ATMO_PWM_Status_t ( *SetPinConfiguration )( ATMO_DriverInstanceData_t *instanceData, ATMO_GPIO_Device_Pin_t pin, ATMO_PWM_Config_t *config );
	ATMO_PWM_Status_t ( *GetPinConfiguration )( ATMO_DriverInstanceData_t *instanceData, ATMO_GPIO_Device_Pin_t pin, ATMO_PWM_Config_t *config );
	ATMO_PWM_Status_t ( *Enable )( ATMO_DriverInstanceData_t *instanceData, ATMO_GPIO_Device_Pin_t pin );
	ATMO_PWM_Status_t ( *Disable )( ATMO_DriverInstanceData_t *instanceData, ATMO_GPIO_Device_Pin_t pin );
} ATMO_PWM_DriverInstance_t;

/**
 * Register low level driver instance with main PWM driver
 *
 * @param driverInstance: Driver instance to register
 * @param instanceNumber: Returned instance number
 * @return status
 */
ATMO_Status_t ATMO_PWM_AddDriverInstance( const ATMO_PWM_DriverInstance_t *driverInstance, ATMO_DriverInstanceData_t *driverInstanceData, ATMO_DriverInstanceHandle_t *instanceNumber );

/**
 * This routine initializes data structures for the PWM Driver
 *
 * @param instance :The driver instance to be configured
 * @return status
 */
ATMO_PWM_Status_t ATMO_PWM_Init( ATMO_DriverInstanceHandle_t instance );

/**
 * This routine configures the specified PWM peripheral pin.
 *
 * @param instance :The driver instance to be configured.
 * @param pin :The pin to be configured
 * @param config :The pointer to a structure for storing the channel configuration.
 * @return status.
 */
ATMO_PWM_Status_t ATMO_PWM_SetPinConfiguration( ATMO_DriverInstanceHandle_t instance, ATMO_GPIO_Device_Pin_t pin, ATMO_PWM_Config_t *config );

/**
 * This routine retrieves the current configuration of the specified PWM peripheral pin.
 *
 * @param instance :The driver instance to be read.
 * @param pin :The pin to be read.
 * @param config :The pointer to a structure for storing the channel configuration.
 * @return status.
 */
ATMO_PWM_Status_t ATMO_PWM_GetPinConfiguration( ATMO_DriverInstanceHandle_t instance, ATMO_GPIO_Device_Pin_t pin, ATMO_PWM_Config_t *config );

/**
 * This routine de-initializes the specified PWM peripheral instance.
 *
 * @param instance :The driver instance to be de-initialized.
 * @return status.
 */
ATMO_PWM_Status_t ATMO_PWM_DeInit( ATMO_DriverInstanceHandle_t instance );

/**
 * This routine enables the specified PWM pin.
 *
 * @param instance :The driver instance to be enable.
 * @param pin : Pin to enable
 * @return status.
 */
ATMO_PWM_Status_t ATMO_PWM_Enable( ATMO_DriverInstanceHandle_t instance, ATMO_GPIO_Device_Pin_t pin );

/**
 * This routine disables the specified PWM pin.
 *
 * @param instance :The driver instance to be disabled.
 * @param pin :Pin to disable
 * @return status.
 */
ATMO_PWM_Status_t ATMO_PWM_Disable( ATMO_DriverInstanceHandle_t instance, ATMO_GPIO_Device_Pin_t pin );

#ifdef __cplusplus
}
#endif
#endif
/** @}*/