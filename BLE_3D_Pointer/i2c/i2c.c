/**
 ******************************************************************************
 * @file    i2c.c
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - Core I2C Driver
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

#include "i2c.h"

#ifdef ATMO_SLIM_STACK
	#ifndef NUMBER_OF_I2C_DRIVER_INSTANCES
		#define NUMBER_OF_I2C_DRIVER_INSTANCES 1
	#endif
#endif

#ifndef NUMBER_OF_I2C_DRIVER_INSTANCES
#define NUMBER_OF_I2C_DRIVER_INSTANCES 4
#endif

ATMO_DriverInstanceHandle_t numberOfI2CDriverInstance = 0;

const ATMO_I2C_DriverInstance_t *i2cInstances[NUMBER_OF_I2C_DRIVER_INSTANCES];
ATMO_DriverInstanceData_t *i2cInstancesData[NUMBER_OF_I2C_DRIVER_INSTANCES];

ATMO_Status_t ATMO_I2C_AddDriverInstance( const ATMO_I2C_DriverInstance_t *driverInstance, ATMO_DriverInstanceData_t *driverInstanceData, ATMO_DriverInstanceHandle_t *instanceNumber )
{
	if ( !( numberOfI2CDriverInstance < NUMBER_OF_I2C_DRIVER_INSTANCES ) )
	{
		return ATMO_Status_OutOfMemory;
	}

	i2cInstances[numberOfI2CDriverInstance] = driverInstance;
	i2cInstancesData[numberOfI2CDriverInstance] = driverInstanceData;
	*instanceNumber = numberOfI2CDriverInstance;

	numberOfI2CDriverInstance++;

	return ATMO_Status_Success;
}

ATMO_I2C_Status_t ATMO_I2C_Init( ATMO_DriverInstanceHandle_t instance )
{
	if ( !( instance < numberOfI2CDriverInstance ) )
	{
		return ATMO_I2C_Status_Invalid;
	}

	return i2cInstances[instance]->Init( i2cInstancesData[instance] );
}

ATMO_I2C_Status_t ATMO_I2C_DeInit( ATMO_DriverInstanceHandle_t instance )
{
	if ( !( instance < numberOfI2CDriverInstance ) )
	{
		return ATMO_I2C_Status_Invalid;
	}

	return i2cInstances[instance]->DeInit( i2cInstancesData[instance] );
}

ATMO_I2C_Status_t ATMO_I2C_SetConfiguration( ATMO_DriverInstanceHandle_t instance, const ATMO_I2C_Peripheral_t *config )
{
	if ( !( instance < numberOfI2CDriverInstance ) )
	{
		return ATMO_I2C_Status_Invalid;
	}

	return i2cInstances[instance]->SetConfiguration( i2cInstancesData[instance], config );
}



ATMO_I2C_Status_t ATMO_I2C_MasterWrite( ATMO_DriverInstanceHandle_t instance, uint16_t slaveAddress, const uint8_t *cmdBytes, uint16_t numCmdBytes, const uint8_t *writeBytes, uint16_t numWriteBytes, unsigned int timeoutMs )
{
	if ( !( instance < numberOfI2CDriverInstance ) )
	{
		return ATMO_I2C_Status_Invalid;
	}

	return i2cInstances[instance]->MasterWrite( i2cInstancesData[instance], slaveAddress, cmdBytes, numCmdBytes, writeBytes, numWriteBytes, timeoutMs );
}

ATMO_I2C_Status_t ATMO_I2C_MasterRead( ATMO_DriverInstanceHandle_t instance, uint16_t slaveAddress, const uint8_t *cmdBytes, uint16_t numCmdBytes, uint8_t *readBytes, uint16_t numReadBytes, unsigned int timeoutMs )
{
	if ( !( instance < numberOfI2CDriverInstance ) )
	{
		return ATMO_I2C_Status_Invalid;
	}

	return i2cInstances[instance]->MasterRead( i2cInstancesData[instance], slaveAddress, cmdBytes, numCmdBytes, readBytes, numReadBytes, timeoutMs );
}

uint32_t ATMO_I2C_BaudToHz( ATMO_I2C_BaudRate_t baudRate )
{
	switch ( baudRate )
	{
		case ATMO_I2C_BaudRate_Low_Speed_Mode:
		case ATMO_I2C_BaudRate_10kHz:
		{
			return 10000;
		}

		case ATMO_I2C_BaudRate_Standard_Mode:
		case ATMO_I2C_BaudRate_100kHz:
		{
			return 100000;
		}

		case ATMO_I2C_BaudRate_Fast_Mode:
		case ATMO_I2C_BaudRate_400kHz:
		{
			return 400000;
		}

		case ATMO_I2C_BaudRate_Fast_Mode_Plus:
		case ATMO_I2C_BaudRate_1000kHz:
		{
			return 1000000;
		}

		case ATMO_I2C_BaudRate_High_Speed_Mode:
		case ATMO_I2C_BaudRate_3400kHz:
		{
			return 3400000;
		}

		case ATMO_I2C_BaudRate_Ultra_Speed_Mode:
		case ATMO_I2C_BaudRate_5000kHz:
		{
			return 5000000;
		}

		default:
		{
			return 100000;
		}
	}
}
