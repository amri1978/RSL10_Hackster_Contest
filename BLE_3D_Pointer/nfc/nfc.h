/**
 ******************************************************************************
 * @file    nfc.h
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - NFC header file
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
#ifndef __ATMO_NFC__H
#define __ATMO_NFC__H

/** \addtogroup NFC
 * The NFC driver is used for configuring NDEF records on an NFC tag.
 *  @{
 */

/* Includes ------------------------------------------------------------------*/
#include "../atmo/core.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported Constants --------------------------------------------------------*/

/* Exported Macros -----------------------------------------------------------*/
/// \cond DO_NOT_DOCUMENT
#define ATMO_NFC_FLAG_MSG_BEGIN_SHIFT (7)
#define ATMO_NFC_FLAG_MSG_BEGIN_MASK (0x1)

#define ATMO_NFC_FLAG_MSG_END_SHIFT (6)
#define ATMO_NFC_FLAG_MSG_END_MASK (0x1)

#define ATMO_NFC_FLAG_CHUNK_SHIFT (5)
#define ATMO_NFC_FLAG_CHUNK_MASK (0x1)

#define ATMO_NFC_FLAG_SHORT_RECORD_SHIFT (4)
#define ATMO_NFC_FLAG_SHORT_RECORD_MASK (0x1)

#define ATMO_NFC_FLAG_ID_SHIFT (3)
#define ATMO_NFC_FLAG_ID_MASK (0x1)

#define ATMO_NFC_FLAG_TNF_SHIFT (0)
#define ATMO_NFC_FLAG_TNF_MASK (0x7)

#define ATMO_NFC_MSG_BEGIN(BEGIN) ((uint8_t)(BEGIN & ATMO_NFC_FLAG_MSG_BEGIN_MASK) << ATMO_NFC_FLAG_MSG_BEGIN_SHIFT)
#define ATMO_NFC_MSG_END(END) ((uint8_t)(END & ATMO_NFC_FLAG_MSG_END_MASK) << ATMO_NFC_FLAG_MSG_END_SHIFT)
#define ATMO_NFC_MSG_CHUNK(CHUNK) ((uint8_t)(CHUNK & ATMO_NFC_FLAG_CHUNK_MASK) << ATMO_NFC_FLAG_CHUNK_SHIFT)
#define ATMO_NFC_MSG_SHORT_RECORD(SHORT) ((uint8_t)(SHORT & ATMO_NFC_FLAG_SHORT_RECORD_MASK) << ATMO_NFC_FLAG_SHORT_RECORD_SHIFT)
#define ATMO_NFC_FLAG_ID(ID) ((uint8_t)(ID & ATMO_NFC_FLAG_ID_MASK) << ATMO_NFC_FLAG_ID_SHIFT)
#define ATMO_NFC_FLAG_TNF(TNF) ((uint8_t)(TNF & ATMO_NFC_FLAG_TNF_MASK) << ATMO_NFC_FLAG_TNF_SHIFT)

#define ATMO_NFC_MSG_GET_BEGIN(BEGIN) ((uint8_t)(BEGIN >> ATMO_NFC_FLAG_MSG_BEGIN_SHIFT) & ATMO_NFC_FLAG_MSG_BEGIN_MASK)
#define ATMO_NFC_MSG_GET_END(END) ((uint8_t)(END >> ATMO_NFC_FLAG_MSG_END_SHIFT) & ATMO_NFC_FLAG_MSG_END_MASK)
#define ATMO_NFC_MSG_GET_CHUNK(CHUNK) ((uint8_t)(CHUNK >> ATMO_NFC_FLAG_CHUNK_SHIFT) & ATMO_NFC_FLAG_CHUNK_MASK)
#define ATMO_NFC_MSG_GET_SHORT_RECORD(SHORT) ((uint8_t)(SHORT >> ATMO_NFC_FLAG_SHORT_RECORD_SHIFT) & ATMO_NFC_FLAG_SHORT_RECORD_MASK)
#define ATMO_NFC_FLAG_GET_ID(ID) ((uint8_t)(ID >> ATMO_NFC_FLAG_ID_SHIFT) & ATMO_NFC_FLAG_ID_MASK)
#define ATMO_NFC_FLAG_GET_TNF(TNF) ((uint8_t)(TNF >> ATMO_NFC_FLAG_TNF_SHIFT) & ATMO_NFC_FLAG_TNF_MASK)

#define ATMO_NFC_MAX_PAYLOAD_TYPE_LEN (32)
#define ATMO_NFC_MAX_PAYLOAD_ID_LEN (32)
#define ATMO_NFC_MAX_PAYLOAD_LEN (256)
/// \endcond

/* Exported Types ------------------------------------------------------------*/

typedef enum
{
	ATMO_NFC_Status_Success       = 0x00u,  // Operation was successful
	ATMO_NFC_Status_Fail          = 0x01u,  // Operation failed
	ATMO_NFC_Status_Initialized   = 0x02u,  // Peripheral already initialized
	ATMO_NFC_Status_Invalid       = 0x03u,  // Invalid operation
	ATMO_NFC_Status_NotSupported  = 0x04u,  // Feature not supported by platform
	ATMO_NFC_Status_Unspecified   = 0x05u,  // Some other status not defined
} ATMO_NFC_Status_t;

/**
 * @brief NDEF Record header
 *
 */
typedef struct
{
	uint8_t flags;
	uint8_t typeLength;
	uint32_t payloadLength;
	uint8_t idLength;
	uint8_t payloadType[ATMO_NFC_MAX_PAYLOAD_TYPE_LEN];
	uint8_t payloadId[ATMO_NFC_MAX_PAYLOAD_ID_LEN];
} ATMO_NFC_Record_Header_t;

/**
 * @brief NDEF Record
 *
 */
typedef struct
{
	ATMO_NFC_Record_Header_t header;
	uint8_t payload[ATMO_NFC_MAX_PAYLOAD_LEN];
} ATMO_NFC_Record_t;

typedef enum
{
	ATMO_NFC_TNF_None = 0x0,
	ATMO_NFC_TNF_WellKnown,
	ATMO_NFC_TNF_MIME,
	ATMO_NFC_TNF_URI,
	ATMO_NFC_TNF_EXTERNAL,
} ATMO_NFC_TNF_Type_t;

typedef enum
{
	ATMO_NFC_TYPE_WELLKNOWN_TEXT,
	ATMO_NFC_TYPE_WELLKNOWN_URL
} ATMO_NFC_TYPE_WELLKNOWN_t;

typedef struct
{
	uint8_t unused;
} ATMO_NFC_Config_t;

// Some C gore so we can use the struct within itself
typedef struct ATMO_NFC_DriverInstance_t ATMO_NFC_DriverInstance_t;

struct ATMO_NFC_DriverInstance_t
{
	ATMO_NFC_Status_t ( *Init )( ATMO_DriverInstanceData_t *instanceData );
	ATMO_NFC_Status_t ( *DeInit )( ATMO_DriverInstanceData_t *instanceData );
	ATMO_NFC_Status_t ( *SetConfiguration )( ATMO_DriverInstanceData_t *instanceData, const ATMO_NFC_Config_t *config );
	ATMO_NFC_Status_t ( *SetMessage )( ATMO_DriverInstanceData_t *instanceData, unsigned int numRecords,
	                                   ATMO_NFC_Record_t *records );
	ATMO_NFC_Status_t ( *SetMessagePtr )( ATMO_DriverInstanceData_t *instanceData, unsigned int numRecords,
	                                      ATMO_NFC_Record_t **records );
	ATMO_NFC_Status_t ( *GetNumStoredRecords )( ATMO_DriverInstanceData_t *instanceData, unsigned int *numRecords );
	ATMO_NFC_Status_t ( *GetRecord )( ATMO_DriverInstanceData_t *instanceData, unsigned int index, ATMO_NFC_Record_t *record );
	ATMO_NFC_Status_t ( *SetMessageReceivedCallback )( ATMO_DriverInstanceData_t *instanceData, ATMO_Callback_t cb );
	ATMO_NFC_Status_t ( *SetMessageReceivedAbilityHandle )( ATMO_DriverInstanceData_t *instanceData, unsigned int abilityHandle );
};

/* Exported Function Prototypes -----------------------------------------------*/

/**
 * Add an instance of a driver for NFC into the NFC driver handler.
 *
 */
ATMO_Status_t ATMO_NFC_AddDriverInstance( const ATMO_NFC_DriverInstance_t *driverInstance,
        ATMO_DriverInstanceData_t *driverInstanceData, ATMO_DriverInstanceHandle_t *instanceNumber );

/**
 * Initialize the NFC driver.
 *
 * @param[in] instance
 *
 * @return status
 */
ATMO_NFC_Status_t ATMO_NFC_Init( ATMO_DriverInstanceHandle_t instance );

/**
 * De-initialize the NFC driver.
 *
 * @param[in] instance
 * @return status.
 */
ATMO_NFC_Status_t ATMO_NFC_DeInit( ATMO_DriverInstanceHandle_t instance );

/**
 * Set the NFC configuration.
 *
 * @param[in] instance
 * @param[in] config
 * @return status
 */
ATMO_NFC_Status_t ATMO_NFC_SetConfiguration( ATMO_DriverInstanceHandle_t instance, const ATMO_NFC_Config_t *config );

/**
 * Insert a list of records into the NFC tag.
 *
 * @param[in] numRecords
 * @param[in] records
 */
ATMO_NFC_Status_t ATMO_NFC_SetMessage( ATMO_DriverInstanceHandle_t instance, unsigned int numRecords,
                                       ATMO_NFC_Record_t *records );

/**
 * Insert a list of records into the NFC tag.
 *
 * @param[in] numRecords
 * @param[in] records - an array of pointers to records
 */
ATMO_NFC_Status_t ATMO_NFC_SetMessagePtr( ATMO_DriverInstanceHandle_t instance, unsigned int numRecords,
        ATMO_NFC_Record_t **records );
/**
 * Get the number of records currently stored in the NFC tag.
 *
 * @param[out] numRecords
 */
ATMO_NFC_Status_t ATMO_NFC_GetNumStoredRecords( ATMO_DriverInstanceHandle_t instance, unsigned int *numRecords );

/**
 * Get a single record from the NFC tag.
 *
 * @param[in] index
 * @param[out] record - Should be preallocated
 */
ATMO_NFC_Status_t ATMO_NFC_GetRecord( ATMO_DriverInstanceHandle_t instance, unsigned int index, ATMO_NFC_Record_t *record );

/**
 * Set the callback for an NFC transmission received.
 */
ATMO_NFC_Status_t ATMO_NFC_SetMessageReceivedCallback( ATMO_DriverInstanceHandle_t instance, ATMO_Callback_t cb );

/**
 * Set the ability handle for NFC transmission received.
 */
ATMO_NFC_Status_t ATMO_NFC_SetMessageReceivedAbilityHandle( ATMO_DriverInstanceHandle_t instance, unsigned int abilityHandle );

/**
 * Called by Atmo stack to write all stored Atmo records.
 */
ATMO_NFC_Status_t ATMO_NFC_SyncAtmoRecords( ATMO_DriverInstanceHandle_t instance );

ATMO_NFC_Status_t ATMO_NFC_RegisterStoredRecord( ATMO_DriverInstanceHandle_t instance, ATMO_NFC_Record_t *record, unsigned int *recordHandle );

/**
 * Make a WLT text record.
 *
 * @param[in] record
 * @param[in] text
 */
ATMO_NFC_Status_t ATMO_NFC_MakeRecordText( ATMO_NFC_Record_t *record, const char *text );

/**
 * Make a URL record.
 *
 * @param[in] url
 */
ATMO_NFC_Status_t ATMO_NFC_MakeRecordURL( ATMO_NFC_Record_t *record, const char *url );

/**
 * Make a MIME record.
 *
 * @param[in] mimeType
 * @param[in] data
 * @param[in] dataLen
 */
ATMO_NFC_Status_t ATMO_NFC_MakeRecordMime( ATMO_NFC_Record_t *record, const char *mimeType, const char *data,
        unsigned int dataLen );

/**
 * Make external type record.
 *
 * @param[in] type
 * @param[in] data
 * @param[in] dataLen
 */
ATMO_NFC_Status_t ATMO_NFC_MakeRecordExternal( ATMO_NFC_Record_t *record, const char *type, const char *data,
        unsigned int dataLen );

/**
 * Print the record to a console.
 */
ATMO_NFC_Status_t ATMO_NFC_PrintRecord( ATMO_NFC_Record_t *record );

#ifdef __cplusplus
}
#endif

#endif /* __ATMO_NFC__H */
/** @}*/