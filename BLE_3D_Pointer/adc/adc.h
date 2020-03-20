/**
 ******************************************************************************
 * @file    atmo_adc.h
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - ADC header file
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

/** \addtogroup ADC
 * 	The ADC driver is used for reading analog voltage and interacting with an end device’s ADC functionality.
 *  @{
 */

/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef ATMO_ADC_H_
#define ATMO_ADC_H_

#include "../atmo/core.h"
#include "../gpio/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported Constants --------------------------------------------------------*/

/* Exported Macros -----------------------------------------------------------*/

/* Exported Types ------------------------------------------------------------*/

/**
 * ADC Driver Return Enumerations
 */
typedef enum
{
	ATMO_ADC_Status_Success       = 0x00u,  /**< Operation was successful */
	ATMO_ADC_Status_Fail          = 0x01u,  /**< Operation failed */
	ATMO_ADC_Status_Initialized   = 0x02u,  /**< Peripheral already initialized */
	ATMO_ADC_Status_Invalid       = 0x03u,  /**< Invalid operation */
	ATMO_ADC_Status_NotSupported  = 0x04u,  /**< Feature not supported by platform */
	ATMO_ADC_Status_Unknown       = 0x05u,  /**< Some other status not defined */
	ATMO_ADC_Status_Busy          = 0x20u,  /**< Conversion in progress */
	ATMO_ADC_Status_Timeout       = 0x21u,  /**< Conversion timed out */
} ATMO_ADC_Status_t;

typedef enum
{
	ATMO_ADC_VoltageUnits_Volts       = 0x0001u,
	ATMO_ADC_VoltageUnits_MilliVolts  = 0x0002u,
	ATMO_ADC_VoltageUnits_MicroVolts  = 0x0003u,
	ATMO_ADC_VoltageUnits_NanoVolts   = 0x0004u,
} ATMO_ADC_VoltageUnits_t;

typedef struct ATMO_ADC_DriverInstance_t ATMO_ADC_DriverInstance_t;

struct ATMO_ADC_DriverInstance_t
{
	ATMO_ADC_Status_t ( *Init )( ATMO_DriverInstanceData_t *instanceData );
	ATMO_ADC_Status_t ( *DeInit )( ATMO_DriverInstanceData_t *instanceData );
	ATMO_ADC_Status_t ( *ReadRaw )( ATMO_DriverInstanceData_t *instanceData, ATMO_GPIO_Device_Pin_t pin, int32_t *value, uint32_t numSamplesToAverage );
	ATMO_ADC_Status_t ( *Read )( ATMO_DriverInstanceData_t *instanceData, ATMO_GPIO_Device_Pin_t pin, ATMO_ADC_VoltageUnits_t units, int32_t *voltage, uint32_t numSamplesToAverage );
	ATMO_ADC_Status_t ( *GetReferenceVoltageMv )( ATMO_DriverInstanceData_t *instanceData, uint32_t *refVoltage );
};

/**
 * Register low level driver instance with main ADC driver
 *
 * @param driverInstance: Driver instance to register
 * @param instanceNumber: Returned instance number
 * @return status
 */
ATMO_Status_t ATMO_ADC_AddDriverInstance( const ATMO_ADC_DriverInstance_t *driverInstance, ATMO_DriverInstanceData_t *driverInstanceData, ATMO_DriverInstanceHandle_t *instanceNumber );

/**
 * This routine initializes data structures needed for the ADC driver
 *
 * @param instance :The peripheral instance to be configured
 * @return status
 */
ATMO_ADC_Status_t ATMO_ADC_Init( ATMO_DriverInstanceHandle_t instance );

/**
 * This routine de-initializes the specified ADC peripheral instance.
 *
 * @param instance :The peripheral instance to be de-initialized.
 * @return status.
 */
ATMO_ADC_Status_t ATMO_ADC_DeInit( ATMO_DriverInstanceHandle_t instance );

/**
 * This routine initiates an ADC conversion and reads the raw data. The value is formated as
 * signed 2s complement with 'precision' data bits plus sign bit (sign extended). The precision
 * is set during initialization. This is a blocking function and will not return until all
 * samples have been taken or a timeout occurs.
 *
 * @param instance :The peripheral instance from which to read conversion data.
 * @param pin :The ADC pin for which to do a conversion.
 * @param value :The pointer to a variable for storing the value read from the ADC peripheral.
 * @param numSamplesToAverage :The number of data samples to read from the ADC peripheral and calculate an average.
 * @return status.
 */
ATMO_ADC_Status_t ATMO_ADC_ReadRaw( ATMO_DriverInstanceHandle_t instance, ATMO_GPIO_Device_Pin_t pin, int32_t *value, uint32_t numSamplesToAverage );

/**
 * This routine initiates an ADC conversion and converts the raw data to voltage. The calculated
 * voltage is returned as an integer where 1 lsb = the specified unit(e.g. volts, millivolts,
 * microvolts). This is a blocking function and will not return until all samples have been taken
 * or a timeout occurs.
 *
 * @param instance :The peripheral instance from which to read conversion data.
 * @param pin :The ADC pin for which to do a conversion.
 * @param units :A scaling factor for formating the measured value.
 * @param voltage :The pointer to a variable for storing the value read from the ADC peripheral.
 * @param numSamplesToAverage :The number of data samples to read from the ADC peripheral and calculate an average.
 * @return status.
 */
ATMO_ADC_Status_t ATMO_ADC_Read( ATMO_DriverInstanceHandle_t instance, ATMO_GPIO_Device_Pin_t pin, ATMO_ADC_VoltageUnits_t units, int32_t *voltage, uint32_t numSamplesToAverage );

/**
 * This routine gets the ADC reference voltage.
 *
 * @param instance :The peripheral instance to be de-initialized.
 * @param refVoltage :The pointer to a variable for storing the value of ADC reference voltage.
 * @return status.
 */
ATMO_ADC_Status_t ATMO_ADC_GetReferenceVoltageMv( ATMO_DriverInstanceHandle_t instance, uint32_t *refVoltage );

#ifdef __cplusplus
}
#endif

#endif

/** @}*/

