/**
 ******************************************************************************
 * @file    tcpserver.h
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - TCP Server header file
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

/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef __ATMO_TCP_SERVER__H
#define __ATMO_TCP_SERVER__H


/* Includes ------------------------------------------------------------------*/
#include "../atmo/core.h"

/** @defgroup TCPServer TCP Server
 *  @{
 */

/** \addtogroup TCPServer
 * The TCP server driver is used for allowing a project to serve as a TCP server and receive and send data packets.
 *  @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/* Exported Constants --------------------------------------------------------*/

/* Exported Macros -----------------------------------------------------------*/

/* Exported Types ------------------------------------------------------------*/

typedef enum
{
	ATMO_TCP_SERVER_Status_Success       = 0x00u,  // Operation was successful
	ATMO_TCP_SERVER_Status_Fail          = 0x01u,  // Operation failed
	ATMO_TCP_SERVER_Status_Initialized   = 0x02u,  // Peripheral already initialized
	ATMO_TCP_SERVER_Status_Invalid       = 0x03u,  // Invalid operation
	ATMO_TCP_SERVER_Status_NotSupported  = 0x04u,  // Feature not supported by platform
	ATMO_TCP_SERVER_Status_Unspecified   = 0x05u,  // Some other status not defined
} ATMO_TCP_SERVER_Status_t;

/* Exported Structures -------------------------------------------------------*/

typedef struct
{
	uint16_t port;
} ATMO_TCP_SERVER_Config_t;

typedef enum
{
	ATMO_TCP_SERVER_ClientConnected = 0,
	ATMO_TCP_SERVER_ClientDisconnected,
	ATMO_TCP_SERVER_DataAvailable,
	ATMO_TCP_SERVER_DataSent,
	ATMO_TCP_SERVER_RuntimeError,
	ATMO_TCP_SERVER_NumEvents
} ATMO_TCP_SERVER_Event_t;

// Some C gore so we can use the struct within itself
typedef struct ATMO_TCP_SERVER_DriverInstance_t ATMO_TCP_SERVER_DriverInstance_t;

struct ATMO_TCP_SERVER_DriverInstance_t
{
	ATMO_TCP_SERVER_Status_t ( *Init )( ATMO_DriverInstanceData_t *instanceData );
	ATMO_TCP_SERVER_Status_t ( *DeInit )( ATMO_DriverInstanceData_t *instanceData );
	ATMO_TCP_SERVER_Status_t ( *SetConfiguration )( ATMO_DriverInstanceData_t *instanceData, const ATMO_TCP_SERVER_Config_t *config );
	ATMO_TCP_SERVER_Status_t ( *Start )( ATMO_DriverInstanceData_t *instanceData );
	ATMO_TCP_SERVER_Status_t ( *Stop )( ATMO_DriverInstanceData_t *instanceData );
	ATMO_TCP_SERVER_Status_t ( *RegisterEventCallback )( ATMO_DriverInstanceData_t *instanceData, ATMO_TCP_SERVER_Event_t event, ATMO_Callback_t cb );
	ATMO_TCP_SERVER_Status_t ( *GetNumAvailableBytes )( ATMO_DriverInstanceData_t *instanceData, uint32_t connectionId, uint32_t *availableBytes );
	ATMO_TCP_SERVER_Status_t ( *ReadBytes )( ATMO_DriverInstanceData_t *instanceData, uint32_t connectionId, uint8_t *buffer, uint32_t numBytes );
	ATMO_TCP_SERVER_Status_t ( *WriteBytes )( ATMO_DriverInstanceData_t *instanceData, uint32_t connectionId, uint8_t *buffer, uint32_t numBytes );
	ATMO_TCP_SERVER_Status_t ( *CloseConnection )( ATMO_DriverInstanceData_t *instanceData, uint32_t connectionId );
};

/* Exported Function Prototypes -----------------------------------------------*/

/**
 * Add an instance of a driver for TCP_SERVER into the TCP_SERVER driver handler.
 *
 */
ATMO_Status_t ATMO_TCP_SERVER_AddDriverInstance( const ATMO_TCP_SERVER_DriverInstance_t *driverInstance, ATMO_DriverInstanceData_t *driverInstanceData, ATMO_DriverInstanceHandle_t *instanceNumber );

/**
 * Initialize the TCP_SERVER driver.
 *
 * @param[in] instance
 *
 * @return status
 */
ATMO_TCP_SERVER_Status_t ATMO_TCP_SERVER_Init( ATMO_DriverInstanceHandle_t instance );

/**
 * De-initialize the TCP_SERVER driver.
 *
 * @param[in] instance
 * @return status.
 */
ATMO_TCP_SERVER_Status_t ATMO_TCP_SERVER_DeInit( ATMO_DriverInstanceHandle_t instance );

/**
 * Set the TCP server configuration.
 *
 * @param instance
 * @param config
 * @return ATMO_TCP_SERVER_Status_t
 */
ATMO_TCP_SERVER_Status_t ATMO_TCP_SERVER_SetConfiguration( ATMO_DriverInstanceHandle_t instance, const ATMO_TCP_SERVER_Config_t *config );

/**
 * Start the TCP server. The underlying implementation may work differently depending on the platform. Some may run on threads, some may be polled in the main loop.
 *
 * @param instance
 * @return ATMO_TCP_SERVER_Status_t
 */
ATMO_TCP_SERVER_Status_t ATMO_TCP_SERVER_Start( ATMO_DriverInstanceHandle_t instance );

/**
 * Stop the TCP server. This is intended to be called when registering a device is complete.
 *
 * @param instance
 * @return ATMO_TCP_SERVER_Status_t
 */
ATMO_TCP_SERVER_Status_t ATMO_TCP_SERVER_Stop( ATMO_DriverInstanceHandle_t instance );

/**
 * Register to receive an event callback.
 *
 * @param instance
 * @param event
 * @param cb
 * @return ATMO_TCP_SERVER_Status_t
 */
ATMO_TCP_SERVER_Status_t ATMO_TCP_SERVER_RegisterEventCallback( ATMO_DriverInstanceHandle_t instance, ATMO_TCP_SERVER_Event_t event, ATMO_Callback_t cb );

/**
 * How many bytes are available waiting to be read.
 *
 * @param instance
 * @return ATMO_TCP_SERVER_Status_t
 */
ATMO_TCP_SERVER_Status_t ATMO_TCP_SERVER_GetNumAvailableBytes( ATMO_DriverInstanceHandle_t instance, uint32_t connectionId, uint32_t *availableBytes );

/**
 * Read bytes from the server's internal buffer.
 *
 * @param instance
 * @param buffer
 * @param numBytes
 * @return ATMO_TCP_SERVER_Status_t
 */
ATMO_TCP_SERVER_Status_t ATMO_TCP_SERVER_ReadBytes( ATMO_DriverInstanceHandle_t instance, uint32_t connectionId, uint8_t *buffer, uint32_t numBytes );

/**
 * Write bytes and send to the client.
 *
 * @param instance
 * @param buffer
 * @param numBytes
 * @return ATMO_TCP_SERVER_Status_t
 */
ATMO_TCP_SERVER_Status_t ATMO_TCP_SERVER_WriteBytes( ATMO_DriverInstanceHandle_t instance, uint32_t connectionId, uint8_t *buffer, uint32_t numBytes );

/**
 * Close a TCP client connection.
 *
 * @param instance
 * @param connectionId
 * @return ATMO_TCP_SERVER_Status_t
 */
ATMO_TCP_SERVER_Status_t ATMO_TCP_SERVER_CloseConnection( ATMO_DriverInstanceHandle_t instance, uint32_t connectionId );

#ifdef __cplusplus
}
#endif

#endif /* __ATMO_TCP_SERVER__H */
/** @}*/
/** @}*/