/**
 ******************************************************************************
 * @file    adc.c
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - Core ADC Driver
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

#include "adc.h"

#ifdef ATMO_SLIM_STACK
	#ifndef NUMBER_OF_ADC_DRIVER_INSTANCES
		#define NUMBER_OF_ADC_DRIVER_INSTANCES 1
	#endif
#endif

#ifndef NUMBER_OF_ADC_DRIVER_INSTANCES
#define NUMBER_OF_ADC_DRIVER_INSTANCES 1
#endif

ATMO_DriverInstanceHandle_t numberOfADCDriverInstance = 0;

const ATMO_ADC_DriverInstance_t *adcInstances[NUMBER_OF_ADC_DRIVER_INSTANCES];
ATMO_DriverInstanceData_t *adcInstancesData[NUMBER_OF_ADC_DRIVER_INSTANCES];

ATMO_Status_t ATMO_ADC_AddDriverInstance( const ATMO_ADC_DriverInstance_t *driverInstance, ATMO_DriverInstanceData_t *driverInstanceData, ATMO_DriverInstanceHandle_t *instanceNumber )
{
	if ( !( numberOfADCDriverInstance < NUMBER_OF_ADC_DRIVER_INSTANCES ) )
	{
		return ATMO_Status_OutOfMemory;
	}

	adcInstances[numberOfADCDriverInstance] = driverInstance;
	adcInstancesData[numberOfADCDriverInstance] = driverInstanceData;
	*instanceNumber = numberOfADCDriverInstance;

	numberOfADCDriverInstance++;

	return ATMO_Status_Success;
}

ATMO_ADC_Status_t ATMO_ADC_Init( ATMO_DriverInstanceHandle_t instance )
{
	if ( !( instance < numberOfADCDriverInstance ) )
	{
		return ATMO_ADC_Status_Invalid;
	}

	return adcInstances[instance]->Init( adcInstancesData[instance] );
}

ATMO_ADC_Status_t ATMO_ADC_DeInit( ATMO_DriverInstanceHandle_t instance )
{
	if ( !( instance < numberOfADCDriverInstance ) )
	{
		return ATMO_ADC_Status_Invalid;
	}

	return adcInstances[instance]->DeInit( adcInstancesData[instance] );
}

ATMO_ADC_Status_t ATMO_ADC_ReadRaw( ATMO_DriverInstanceHandle_t instance, ATMO_GPIO_Device_Pin_t pin, int32_t *value, uint32_t numSamplesToAverage )
{
	if ( !( instance < numberOfADCDriverInstance ) )
	{
		return ATMO_ADC_Status_Invalid;
	}

	return adcInstances[instance]->ReadRaw( adcInstancesData[instance], pin, value, numSamplesToAverage );
}

ATMO_ADC_Status_t ATMO_ADC_Read( ATMO_DriverInstanceHandle_t instance, ATMO_GPIO_Device_Pin_t pin, ATMO_ADC_VoltageUnits_t units, int32_t *voltage, uint32_t numSamplesToAverage )
{
	if ( !( instance < numberOfADCDriverInstance ) )
	{
		return ATMO_ADC_Status_Invalid;
	}

	return adcInstances[instance]->Read( adcInstancesData[instance], pin, units, voltage, numSamplesToAverage );
}

ATMO_ADC_Status_t ATMO_ADC_GetReferenceVoltageMv( ATMO_DriverInstanceHandle_t instance, uint32_t *refVoltage )
{
	if ( !( instance < numberOfADCDriverInstance ) )
	{
		return ATMO_ADC_Status_Invalid;
	}

	return adcInstances[instance]->GetReferenceVoltageMv( adcInstancesData[instance], refVoltage );
}

