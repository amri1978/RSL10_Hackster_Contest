/**
 ******************************************************************************
 * @file    http.h
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - HTTP header file
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

/** \addtogroup HTTP
 * The HTTP driver is used to provide HTTP GET/POST functionality on capable platforms.
 *  @{
 */

/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef __ATMO_HTTP__H
#define __ATMO_HTTP__H


/* Includes ------------------------------------------------------------------*/
#include "../atmo/core.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported Constants --------------------------------------------------------*/
#define ATMO_HTTP_DEFAULT_PACKETBUF_SIZE 256


/* Exported Macros -----------------------------------------------------------*/

/* Exported Types ------------------------------------------------------------*/

typedef enum
{
	ATMO_HTTP_Status_Success       = 0x00u,  // Operation was successful
	ATMO_HTTP_Status_Fail          = 0x01u,  // Operation failed
	ATMO_HTTP_Status_Initialized   = 0x02u,  // Peripheral already initialized
	ATMO_HTTP_Status_Invalid       = 0x03u,  // Invalid operation
	ATMO_HTTP_Status_NotSupported  = 0x04u,  // Feature not supported by platform
	ATMO_HTTP_Status_Unspecified   = 0x05u,  // Some other status not defined
} ATMO_HTTP_Status_t;

typedef enum
{
	ATMO_HTTP_GET,
	ATMO_HTTP_POST,
} ATMO_HTTP_Method_t;

/* Exported Structures -------------------------------------------------------*/

/**
 * Single HTTP Header
 */
typedef struct
{
	const char *headerKey;
	const char *headerValue;
} ATMO_HTTP_Header_t;

/**
 * Generic HTTP transaction structure.
 */
typedef struct
{
	const char *url; /**< URL */
	ATMO_HTTP_Method_t method; /**< Get/Post */
	const char *contentType; /**< Content type for POST. Can be NULL, will default to application/json */
	const char *data; /**< Data for POST. Can be NULL. */
	unsigned int dataLen; /**< Length of POST data. */
	ATMO_HTTP_Header_t *headerOverlay; /**< Array of headers to overlay. */
	uint8_t headerOverlayLen; /**< Length of array. */
} ATMO_HTTP_Transaction_t;


typedef struct
{
	uint8_t unused; // Currently unused
} ATMO_HTTP_Config_t;


/* Exported Function Prototypes -----------------------------------------------*/
/**
 * Initialize the HTTP driver.
 *
 * @param[in] instance
 *
 * @return status
 */
ATMO_HTTP_Status_t ATMO_HTTP_Init( ATMO_DriverInstanceHandle_t tcpClientDriverInstance, uint8_t *packetBuf, uint32_t packetBufSize, ATMO_DriverInstanceHandle_t *httpInstance );

/**
 * De-initialize the HTTP driver.
 *
 * @param[in] instance
 * @return status.
 */
ATMO_HTTP_Status_t ATMO_HTTP_DeInit( ATMO_DriverInstanceHandle_t instance );

/**
 * Set the HTTP configuration.
 *
 * @param[in] config
 * @return status
 */
ATMO_HTTP_Status_t ATMO_HTTP_SetConfiguration( ATMO_DriverInstanceHandle_t instance, const ATMO_HTTP_Config_t *config );

/**
 * Perform a HTTP Get/Post transaction.
 *
 * This will technically *block*, but it will call ATMO_Tick once in awhile to keep things moving
 *
 * @param[in] transaction
 * @param[out] respCode -returned response code
 * @param[out] respDataLen - length of returned response data in bytes
 * @param[in] timeoutMs - timeout in milliseconds of transaction
 * @return status
 */
ATMO_HTTP_Status_t ATMO_HTTP_Perform( ATMO_DriverInstanceHandle_t instance, ATMO_HTTP_Transaction_t *transaction, unsigned int *respCode, unsigned int *respDataLen, unsigned int timeoutMs );

/**
 * Get the HTTP response data.
 *
 * @param buf
 * @param bufLen
 * @return ATMO_HTTP_Status_t
 */
ATMO_HTTP_Status_t ATMO_HTTP_GetRespData( ATMO_DriverInstanceHandle_t instance, uint8_t *buf, unsigned int bufLen );

#ifdef __cplusplus
}
#endif

#endif /* __ATMO_HTTP__H */
/** @}*/