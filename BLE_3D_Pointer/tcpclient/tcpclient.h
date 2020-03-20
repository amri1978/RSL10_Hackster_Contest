/**
 ******************************************************************************
 * @file    tcpclient.h
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - TCP Client header file
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
#ifndef __ATMO_TCP_CLIENT__H
#define __ATMO_TCP_CLIENT__H

/** @defgroup TCPClient TCP Client
 *  @{
 */


/** \addtogroup TCPClient
 * The TCP Client driver is used for allowing a project to connect to a TCP server and send and receive data packets.
 *  @{
 */

/* Includes ------------------------------------------------------------------*/
#include "../atmo/core.h"

#ifdef __cplusplus
extern "C" {
#endif


/* Exported Constants --------------------------------------------------------*/

/* Exported Macros -----------------------------------------------------------*/

/* Exported Types ------------------------------------------------------------*/

typedef enum
{
	ATMO_TCP_CLIENT_Status_Success       = 0x00u,  // Operation was successful
	ATMO_TCP_CLIENT_Status_Fail          = 0x01u,  // Operation failed
	ATMO_TCP_CLIENT_Status_Initialized   = 0x02u,  // Peripheral already initialized
	ATMO_TCP_CLIENT_Status_Invalid       = 0x03u,  // Invalid operation
	ATMO_TCP_CLIENT_Status_NotSupported  = 0x04u,  // Feature not supported by platform
	ATMO_TCP_CLIENT_Status_Unspecified   = 0x05u,  // Some other status not defined
} ATMO_TCP_CLIENT_Status_t;

/* Exported Structures -------------------------------------------------------*/

typedef struct
{
	uint8_t unused;
} ATMO_TCP_CLIENT_Config_t;

/**
 * @brief The current connection status of the TCP client.
 *
 */
typedef enum
{
	ATMO_TCP_CLIENT_Connected, /**< Connected to server*/
	ATMO_TCP_CLIENT_Disconnected, /**< Disconnected from server */
	ATMO_TCP_CLIENT_Error /**< Internal error. */
} ATMO_TCP_CLIENT_ConnectionStatus_t;

// Some C gore so we can use the struct within itself
typedef struct ATMO_TCP_CLIENT_DriverInstance_t ATMO_TCP_CLIENT_DriverInstance_t;

struct ATMO_TCP_CLIENT_DriverInstance_t
{
	ATMO_TCP_CLIENT_Status_t ( *Init )( ATMO_DriverInstanceData_t *instanceData );
	ATMO_TCP_CLIENT_Status_t ( *DeInit )( ATMO_DriverInstanceData_t *instanceData );
	ATMO_TCP_CLIENT_Status_t ( *SetConfiguration )( ATMO_DriverInstanceData_t *instanceData, const ATMO_TCP_CLIENT_Config_t *config );
	ATMO_TCP_CLIENT_Status_t ( *Connect )( ATMO_DriverInstanceData_t *instanceData, const char *host, unsigned int port, ATMO_BOOL_t useSSL );
	ATMO_TCP_CLIENT_Status_t ( *Disconnect )( ATMO_DriverInstanceData_t *instanceData );
	ATMO_TCP_CLIENT_Status_t ( *GetConnectionStatus )( ATMO_DriverInstanceData_t *instanceData, ATMO_TCP_CLIENT_ConnectionStatus_t *status );
	ATMO_TCP_CLIENT_Status_t ( *WriteBytes )( ATMO_DriverInstanceData_t *instanceData, uint8_t *data, unsigned int dataLen );
	ATMO_TCP_CLIENT_Status_t ( *SetReceiveCallback )( ATMO_DriverInstanceData_t *instanceData, ATMO_Callback_t cb );
	ATMO_TCP_CLIENT_Status_t ( *GetNumAvailableBytes )( ATMO_DriverInstanceData_t *instanceData, uint32_t *numBytes, uint8_t **bytePtr );
};

/* Exported Function Prototypes -----------------------------------------------*/

/**
 * Add an instance of a driver for TCP_CLIENT into the TCP_CLIENT driver handler.
 *
 */
ATMO_Status_t ATMO_TCP_CLIENT_AddDriverInstance( const ATMO_TCP_CLIENT_DriverInstance_t *driverInstance, ATMO_DriverInstanceData_t *driverInstanceData, ATMO_DriverInstanceHandle_t *instanceNumber );

/**
 * Initialize the TCP_CLIENT driver.
 *
 * @param[in] instance
 *
 * @return status
 */
ATMO_TCP_CLIENT_Status_t ATMO_TCP_CLIENT_Init( ATMO_DriverInstanceHandle_t instance );

/**
 * De-initialize the TCP_CLIENT driver.
 *
 * @param[in] instance
 * @return status.
 */
ATMO_TCP_CLIENT_Status_t ATMO_TCP_CLIENT_DeInit( ATMO_DriverInstanceHandle_t instance );

/**
 * Set the TCP_CLIENT configuration.
 *
 * @param[in] instance
 * @param[in] config
 * @return status
 */
ATMO_TCP_CLIENT_Status_t ATMO_TCP_CLIENT_SetConfiguration( ATMO_DriverInstanceHandle_t instance, const ATMO_TCP_CLIENT_Config_t *config );

/**
 * Connect to a TCP endpoint.
 *
 * @param[in] instance
 * @param[in] host - Must be raw host without port or path
 * @param[in] port
 * @param[in] useSSL
 */
ATMO_TCP_CLIENT_Status_t ATMO_TCP_CLIENT_Connect( ATMO_DriverInstanceHandle_t instance, const char *host, unsigned int port, ATMO_BOOL_t useSSL );

/**
 * Disconnect from the TCP endpoint.
 *
 * @param instance
 * @return ATMO_TCP_CLIENT_Status_t
 */
ATMO_TCP_CLIENT_Status_t ATMO_TCP_CLIENT_Disconnect( ATMO_DriverInstanceHandle_t instance );

/**
 * Get the current connection status.
 *
 * @param instance
 * @param status
 * @return ATMO_TCP_CLIENT_Status_t
 */
ATMO_TCP_CLIENT_Status_t ATMO_TCP_CLIENT_GetConnectionStatus( ATMO_DriverInstanceHandle_t instance, ATMO_TCP_CLIENT_ConnectionStatus_t *status );

/**
* Send data to a TCP endpoint.
*
* @param[in] instance
* @param[in] data
* @param[in] dataLen
*/
ATMO_TCP_CLIENT_Status_t ATMO_TCP_CLIENT_WriteBytes( ATMO_DriverInstanceHandle_t instance, uint8_t *data, unsigned int dataLen );

/**
* Set the receive callback function. There will be no data transferred. Use ATMO_TCP_CLIENT_ReadBytes to read the data.
*
* @param[in] instance
* @param[in] cb
*/
ATMO_TCP_CLIENT_Status_t ATMO_TCP_CLIENT_SetReceiveCallback( ATMO_DriverInstanceHandle_t instance, ATMO_Callback_t cb );

/**
* Get the number of available bytes waiting to be processed. This is intended to be called from the callback set in ATMO_TCP_CLIENT_SetReceiveCallback.
* @param[in] instance
* @param[out] numBytes
* @param[out] bytePtr - A pointer to the data buffer. Make sure this data is processed before another TCP transaction is started.
*/
ATMO_TCP_CLIENT_Status_t ATMO_TCP_CLIENT_GetNumAvailableBytes( ATMO_DriverInstanceHandle_t instance, uint32_t *numBytes, uint8_t **bytePtr );

#ifdef __cplusplus
}
#endif

#endif /* __ATMO_TCP_CLIENT__H */
/** @}*/
/** @}*/