/**
 ******************************************************************************
 * @file    i2c.h
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - I2C header file
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

/** \addtogroup I2C
 * The I2C driver is used for allowing a project to have access to and control basic I2C master functionality of an end device.
 *  @{
 */

/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef __ATMO_I2C__H
#define __ATMO_I2C__H


/* Includes ------------------------------------------------------------------*/
#include "../atmo/core.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported Constants --------------------------------------------------------*/

/* Exported Macros -----------------------------------------------------------*/

/* Exported Types ------------------------------------------------------------*/

/**
 * I2C Operating Mode
 */
typedef enum
{
	ATMO_I2C_OperatingMode_Uninitialized  = 0x00u,
	ATMO_I2C_OperatingMode_Master         = 0x01u,
	ATMO_I2C_OperatingMode_Slave          = 0x02u,
	ATMO_I2C_OperatingMode_Invalid        = 0xFFu,
} ATMO_I2C_OperatingMode_t;

/**
 * I2C Baud Rate
 */
typedef enum
{
	ATMO_I2C_BaudRate_Uninitialized     = 0x00u,
	ATMO_I2C_BaudRate_Low_Speed_Mode    = 0x01u,
	ATMO_I2C_BaudRate_Standard_Mode     = 0x02u,
	ATMO_I2C_BaudRate_Fast_Mode         = 0x03u,
	ATMO_I2C_BaudRate_Fast_Mode_Plus    = 0x04u,
	ATMO_I2C_BaudRate_High_Speed_Mode   = 0x05u,
	ATMO_I2C_BaudRate_Ultra_Speed_Mode  = 0x06u,
	ATMO_I2C_BaudRate_10kHz             = 0x81u,
	ATMO_I2C_BaudRate_100kHz            = 0x82u,
	ATMO_I2C_BaudRate_400kHz            = 0x83u,
	ATMO_I2C_BaudRate_1000kHz           = 0x84u,
	ATMO_I2C_BaudRate_3400kHz           = 0x85u,
	ATMO_I2C_BaudRate_5000kHz           = 0x86u,
	ATMO_I2C_BaudRate_Invalid           = 0xFFu,
} ATMO_I2C_BaudRate_t;

/**
 * I2C Driver Return Enumerations
 */
typedef enum
{
	ATMO_I2C_Status_Success              = 0x00u,  /**< Common - Operation was successful */
	ATMO_I2C_Status_Fail                 = 0x01u,  /**<  Common - Operation failed */
	ATMO_I2C_Status_Initialized          = 0x02u,  /**<  Common - Peripheral already initialized */
	ATMO_I2C_Status_Invalid              = 0x03u,  /**<  Common - Invalid operation or result */
	ATMO_I2C_Status_NotSupported         = 0x04u,  /**<  Common - Feature not supported by platform */
	ATMO_I2C_Status_Unknown              = 0x05u,  /**<  Common - Some other status not defined */
	ATMO_I2C_Status_Busy                 = 0x06u,  /**<  Common - Transfer in progress */
	ATMO_I2C_Status_Timeout              = 0x20u,  /**<  Master - Transfer timed out */
	ATMO_I2C_Status_ReceivedNak          = 0x21u,  /**<  Master - Received NAK from slave device */
	ATMO_I2C_Status_NoReceiveInProgress  = 0x22u,  /**<  Master - No transfer in progress when attempting to abort receive */
	ATMO_I2C_Status_NoSendInProgress     = 0x23u,  /**<  Master - No transfer in progress when attempting to abort send */
	ATMO_I2C_Status_ArbitrationLost      = 0x24u,  /**<  Master - Lost arbitration in multi-master mode */
	ATMO_I2C_Status_TxUnderrun           = 0x40u,  /**<  Slave  - TX Underrun error */
	ATMO_I2C_Status_RxOverrun            = 0x41u,  /**<  Slave  - RX Overrun error */
	ATMO_I2C_Status_Idle                 = 0x42u,  /**<  Slave  - Bus is Idle */
} ATMO_I2C_Status_t;


/* Exported Structures -------------------------------------------------------*/

/**
 * I2C Peripheral Configuration
 */
typedef struct
{
	ATMO_I2C_OperatingMode_t operatingMode; /**< I2C Operating Mode */
	ATMO_I2C_BaudRate_t baudRate; /**< I2C Baud Rate */
	uint16_t localAddress; /**< Peripheral address for slave mode (Future) */
} ATMO_I2C_Peripheral_t;


typedef struct ATMO_I2C_DriverInstance_t ATMO_I2C_DriverInstance_t;

struct ATMO_I2C_DriverInstance_t
{
	ATMO_I2C_Status_t ( *Init )( ATMO_DriverInstanceData_t *instanceData );
	ATMO_I2C_Status_t ( *DeInit )( ATMO_DriverInstanceData_t *instanceData );
	ATMO_I2C_Status_t ( *SetConfiguration )( ATMO_DriverInstanceData_t *instanceData, const ATMO_I2C_Peripheral_t *config );
	ATMO_I2C_Status_t ( *MasterWrite )( ATMO_DriverInstanceData_t *instanceData, uint16_t slaveAddress, const uint8_t *cmdBytes, uint16_t numCmdBytes, const uint8_t *writeBytes, uint16_t numWriteBytes, unsigned int timeoutMs );
	ATMO_I2C_Status_t ( *MasterRead )( ATMO_DriverInstanceData_t *instanceData, uint16_t slaveAddress, const uint8_t *cmdBytes, uint16_t numCmdBytes, uint8_t *readBytes, uint16_t numReadBytes, unsigned int timeoutMs );
};

/* Exported Function Prototypes -----------------------------------------------*/

/**
 * This routine will add an instance of a driver for I2C into the I2C driver handler.
 *
 */
ATMO_Status_t ATMO_I2C_AddDriverInstance( const ATMO_I2C_DriverInstance_t *driverInstance, ATMO_DriverInstanceData_t *driverInstanceData, ATMO_DriverInstanceHandle_t *instanceNumber );

/**
 * This routine initializes the specified I2C peripheral instance. The specific configuration
 * settings are either hard-coded in the platform driver or are pulled from a configuration
 * header file.
 *
 * @param instance :The peripheral instance to be configured.
 * @return status.
 */
ATMO_I2C_Status_t ATMO_I2C_Init( ATMO_DriverInstanceHandle_t instance );

/**
 * This routine de-initializes the specified I2C peripheral instance.
 *
 * @param instance :The peripheral instance to be de-initialized.
 * @return status.
 */
ATMO_I2C_Status_t ATMO_I2C_DeInit( ATMO_DriverInstanceHandle_t instance );

/**
 * This routine configures the specified I2C peripheral instance. It is similar to the init
 * function except the peripheral configuration settings are passed in via the config structure.
 *
 * @param instance :The peripheral instance to be configured.
 * @param config :The pointer to the peripheral configuration structure.
 * @return status.
 */
ATMO_I2C_Status_t ATMO_I2C_SetConfiguration( ATMO_DriverInstanceHandle_t instance, const ATMO_I2C_Peripheral_t *config );


/**
 * This routine will send one or more bytes to a slave device via the specified I2C peripheral. This is a blocking
 * operation and the function will not return until the I2C transfer completes or the timeout expires.
 *
 * @param instance :The peripheral instance used to write to the specified slave device.
 * @param slaveAddress :The address of the slave device.
 * @param cmdBytes :The pointer to a buffer holding the command that will be sent by the I2C peripheral. May be NULL.
 * @param numCmdBytes :The number of bytes in the cmdBytes buffer that will be sent by the I2C peripheral. May be 0.
 * @param writeBytes :The pointer to a buffer holding the data bytes that will be sent by the I2C peripheral.
 * @param numBytes :The number of bytes in the writeBytes buffer that will be sent by the I2C peripheral.
 * @param timeout_ms :The number of milliseconds to wait before the I2C transfer is aborted.
 * @return status.
 */
ATMO_I2C_Status_t ATMO_I2C_MasterWrite( ATMO_DriverInstanceHandle_t instance, uint16_t slaveAddress, const uint8_t *cmdBytes, uint16_t numCmdBytes, const uint8_t *writeBytes, uint16_t numWriteBytes, unsigned int timeoutMs );

/**
 * This routine will read one or more bytes from a slave device via the specified I2C peripheral. If the command buffer has
 * a valid pointer (i.e. not NULL) and the number of command bytes is greater than zero then the values in the command buffer are
 * first sent to the device followed by a repeated START condition. Otherwise a repeated START condition will not be generated.
 * This is a blocking operation and the function will not return until the I2C transfer completes or the timeout expires.
 *
 * @param instance :The peripheral instance used to read from the specified slave device.
 * @param slaveAddress :The address of the slave device.
 * @param cmdBytes :The pointer to a buffer holding the command that will be sent by the I2C peripheral. May be NULL.
 * @param numCmdBytes :The number of bytes in the cmdBytes buffer that will be sent by the I2C peripheral. May be 0.
 * @param readBytes :The pointer to a buffer for holding the bytes that will be received by the I2C peripheral.
 * @param numReadBytes :The number of bytes that are to be read and placed into the readBytes buffer.
 * @return status.
 */
ATMO_I2C_Status_t ATMO_I2C_MasterRead( ATMO_DriverInstanceHandle_t instance, uint16_t slaveAddress, const uint8_t *cmdBytes, uint16_t numCmdBytes, uint8_t *readBytes, uint16_t numReadBytes, unsigned int timeoutMs );

/**
 * Convert baud rate enum to its value in hz
 *
 * @param[in] baudRate
 * @return baud rate in Hz
 */
uint32_t ATMO_I2C_BaudToHz( ATMO_I2C_BaudRate_t baudRate );


#ifdef __cplusplus
}
#endif

#endif /* __ATMO_I2C__H */
/** @}*/