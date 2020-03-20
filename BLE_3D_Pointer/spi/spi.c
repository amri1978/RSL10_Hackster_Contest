/**
 ******************************************************************************
 * @file    spi.c
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - Core SPI Driver
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

#include "spi.h"

#ifdef ATMO_SLIM_STACK
	#ifndef NUMBER_OF_SPI_DRIVER_INSTANCES
		#define NUMBER_OF_SPI_DRIVER_INSTANCES 1
	#endif
#endif

#ifndef NUMBER_OF_SPI_DRIVER_INSTANCES
#define NUMBER_OF_SPI_DRIVER_INSTANCES 4
#endif

ATMO_DriverInstanceHandle_t numberOfSPIDriverInstance = 0;

static const ATMO_SPI_DriverInstance_t *spiInstances[NUMBER_OF_SPI_DRIVER_INSTANCES];
static ATMO_DriverInstanceData_t *spiInstancesData[NUMBER_OF_SPI_DRIVER_INSTANCES];

ATMO_Status_t ATMO_SPI_AddDriverInstance( const ATMO_SPI_DriverInstance_t *driverInstance, ATMO_DriverInstanceData_t *driverInstanceData, ATMO_DriverInstanceHandle_t *instanceNumber )
{
	if ( !( numberOfSPIDriverInstance < NUMBER_OF_SPI_DRIVER_INSTANCES ) )
	{
		return ATMO_Status_OutOfMemory;
	}

	spiInstances[numberOfSPIDriverInstance] = driverInstance;
	spiInstancesData[numberOfSPIDriverInstance] = driverInstanceData;
	*instanceNumber = numberOfSPIDriverInstance;

	numberOfSPIDriverInstance++;

	return ATMO_Status_Success;
}

ATMO_SPI_Status_t ATMO_SPI_Init( ATMO_DriverInstanceHandle_t instance )
{
	if ( !( instance < numberOfSPIDriverInstance ) )
	{
		return ATMO_SPI_Status_Invalid;
	}

	return spiInstances[instance]->Init( spiInstancesData[instance] );
}

ATMO_SPI_Status_t ATMO_SPI_DeInit( ATMO_DriverInstanceHandle_t instance )
{
	if ( !( instance < numberOfSPIDriverInstance ) )
	{
		return ATMO_SPI_Status_Invalid;
	}

	return spiInstances[instance]->DeInit( spiInstancesData[instance] );
}

ATMO_SPI_Status_t ATMO_SPI_SetConfiguration( ATMO_DriverInstanceHandle_t instance, const ATMO_SPI_Peripheral_t *config )
{
	if ( !( instance < numberOfSPIDriverInstance ) )
	{
		return ATMO_SPI_Status_Invalid;
	}

	return spiInstances[instance]->SetConfiguration( spiInstancesData[instance], config );
}

ATMO_SPI_Status_t ATMO_SPI_GetConfiguration( ATMO_DriverInstanceHandle_t instance, ATMO_SPI_Peripheral_t *config )
{
	if ( !( instance < numberOfSPIDriverInstance ) )
	{
		return ATMO_SPI_Status_Invalid;
	}

	return spiInstances[instance]->GetConfiguration( spiInstancesData[instance], config );
}

ATMO_SPI_Status_t ATMO_SPI_MasterSetDeviceConfiguration( ATMO_DriverInstanceHandle_t instance, ATMO_SPI_CS_t csPin,
        const ATMO_SPI_Device_t *config )
{
	if ( !( instance < numberOfSPIDriverInstance ) )
	{
		return ATMO_SPI_Status_Invalid;
	}

	return spiInstances[instance]->MasterSetDeviceConfiguration( spiInstancesData[instance], csPin, config );
}

ATMO_SPI_Status_t ATMO_SPI_MasterGetDeviceConfiguration( ATMO_DriverInstanceHandle_t instance, ATMO_SPI_CS_t csPin,
        ATMO_SPI_Device_t *config )
{
	if ( !( instance < numberOfSPIDriverInstance ) )
	{
		return ATMO_SPI_Status_Invalid;
	}

	return spiInstances[instance]->MasterGetDeviceConfiguration( spiInstancesData[instance], csPin, config );
}

ATMO_SPI_Status_t ATMO_SPI_MasterAssertSlaveSelect( ATMO_DriverInstanceHandle_t instance, ATMO_SPI_CS_t csPin, ATMO_BOOL_t ssActiveLow )
{
	if ( !( instance < numberOfSPIDriverInstance ) )
	{
		return ATMO_SPI_Status_Invalid;
	}

	return spiInstances[instance]->MasterAssertSlaveSelect( spiInstancesData[instance], csPin, ssActiveLow );
}

ATMO_SPI_Status_t ATMO_SPI_MasterDeAssertSlaveSelect( ATMO_DriverInstanceHandle_t instance, ATMO_SPI_CS_t csPin, ATMO_BOOL_t ssActiveLow )
{
	if ( !( instance < numberOfSPIDriverInstance ) )
	{
		return ATMO_SPI_Status_Invalid;
	}

	return spiInstances[instance]->MasterDeAssertSlaveSelect( spiInstancesData[instance], csPin, ssActiveLow );
}

ATMO_SPI_Status_t ATMO_SPI_MasterWrite( ATMO_DriverInstanceHandle_t instance, ATMO_SPI_CS_t csPin, const uint8_t *cmdBytes,
                                        uint16_t numCmdBytes, const uint8_t *writeBytes, uint16_t numWriteBytes, uint16_t timeout_ms )
{
	if ( !( instance < numberOfSPIDriverInstance ) )
	{
		return ATMO_SPI_Status_Invalid;
	}

	return spiInstances[instance]->MasterWrite( spiInstancesData[instance], csPin, cmdBytes, numCmdBytes, writeBytes, numWriteBytes,
	        timeout_ms );
}

ATMO_SPI_Status_t ATMO_SPI_MasterRead( ATMO_DriverInstanceHandle_t instance, ATMO_SPI_CS_t csPin, const uint8_t *cmdBytes,
                                       uint16_t numCmdBytes, uint8_t *readBytes, uint16_t numReadBytes, uint16_t timeout_ms )
{
	if ( !( instance < numberOfSPIDriverInstance ) )
	{
		return ATMO_SPI_Status_Invalid;
	}

	return spiInstances[instance]->MasterRead( spiInstancesData[instance], csPin, cmdBytes, numCmdBytes, readBytes, numReadBytes,
	        timeout_ms );
}
