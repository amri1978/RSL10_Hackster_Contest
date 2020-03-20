/**
 ******************************************************************************
 * @file    nfc.c
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - Core NFC Driver
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

#include "nfc.h"
#include "../app_src/atmosphere_platform.h"
#include "../app_src/atmosphere_globals.h"
#include "../cloud/cloud.h"

#ifdef ATMO_SLIM_STACK
	#ifndef NUMBER_OF_NFC_DRIVER_INSTANCES
		#define NUMBER_OF_NFC_DRIVER_INSTANCES 1
	#endif
#endif

#ifndef NUMBER_OF_NFC_DRIVER_INSTANCES
#define NUMBER_OF_NFC_DRIVER_INSTANCES 1
#endif

#define NUMBER_STORED_RECORDS_PER_INSTANCE (24)

static ATMO_NFC_Record_t *storedRecords[NUMBER_OF_NFC_DRIVER_INSTANCES][NUMBER_STORED_RECORDS_PER_INSTANCE] = {0};
static uint8_t numStoredRecords[NUMBER_OF_NFC_DRIVER_INSTANCES] = {0};

ATMO_DriverInstanceHandle_t numberOfNFCDriverInstance = 0;

const ATMO_NFC_DriverInstance_t *nfcInstances[NUMBER_OF_NFC_DRIVER_INSTANCES];
ATMO_DriverInstanceData_t *nfcInstancesData[NUMBER_OF_NFC_DRIVER_INSTANCES];

static ATMO_NFC_Record_t atmoProjectRecord;
static unsigned int atmoProjectRecordHandle = 0;

ATMO_Status_t ATMO_NFC_AddDriverInstance( const ATMO_NFC_DriverInstance_t *driverInstance,
        ATMO_DriverInstanceData_t *driverInstanceData, ATMO_DriverInstanceHandle_t *instanceNumber )
{
	if ( !( numberOfNFCDriverInstance < NUMBER_OF_NFC_DRIVER_INSTANCES ) )
	{
		return ATMO_Status_OutOfMemory;
	}

	nfcInstances[numberOfNFCDriverInstance] = driverInstance;
	nfcInstancesData[numberOfNFCDriverInstance] = driverInstanceData;
	*instanceNumber = numberOfNFCDriverInstance;

	numberOfNFCDriverInstance++;

	return ATMO_Status_Success;
}

void __ATMO_NFC_RegistrationSetCb( void *data )
{
#ifndef ATMO_SLIM_STACK
	ATMO_PLATFORM_DebugPrint( "[NFC] Registration Set\r\n" );
#endif

	ATMO_CLOUD_RegistrationInfo_t regInfo;

	if ( ATMO_CLOUD_GetRegistration()->registered )
	{
		// Add default ATMO project record
		char recordData[128];
		char uuidStr[128];
		ATMO_CLOUD_GetRegistrationInfoUuid( ATMO_CLOUD_GetRegistration(), uuidStr, 128 );
		sprintf( recordData, "{\"uuid\": \"%s\"}", uuidStr );
		ATMO_NFC_MakeRecordMime( &atmoProjectRecord, "application/atmoiotid", recordData, strlen( recordData ) );

		unsigned int i;

		for ( i = 0; i < numberOfNFCDriverInstance; i++ )
		{
			ATMO_NFC_SyncAtmoRecords( i );
		}
	}
}

ATMO_NFC_Status_t ATMO_NFC_Init( ATMO_DriverInstanceHandle_t instance )
{
	if ( !( instance < numberOfNFCDriverInstance ) )
	{
		return ATMO_NFC_Status_Invalid;
	}

	ATMO_NFC_Status_t status = nfcInstances[instance]->Init( nfcInstancesData[instance] );

	if ( status != ATMO_NFC_Status_Success )
	{
		return status;
	}

	ATMO_NFC_RegisterStoredRecord( instance, &atmoProjectRecord, &atmoProjectRecordHandle );

	ATMO_CLOUD_RegisterRegistrationSetCallback( __ATMO_NFC_RegistrationSetCb );

	ATMO_NFC_SyncAtmoRecords( instance );

	return ATMO_NFC_Status_Success;
}

ATMO_NFC_Status_t ATMO_NFC_DeInit( ATMO_DriverInstanceHandle_t instance )
{
	if ( !( instance < numberOfNFCDriverInstance ) )
	{
		return ATMO_NFC_Status_Invalid;
	}

	return nfcInstances[instance]->DeInit( nfcInstancesData[instance] );
}


ATMO_NFC_Status_t ATMO_NFC_SetConfiguration( ATMO_DriverInstanceHandle_t instance, const ATMO_NFC_Config_t *config )
{
	if ( !( instance < numberOfNFCDriverInstance ) )
	{
		return ATMO_NFC_Status_Invalid;
	}

	return nfcInstances[instance]->SetConfiguration( nfcInstancesData[instance], config );
}

ATMO_NFC_Status_t ATMO_NFC_SetMessage( ATMO_DriverInstanceHandle_t instance, unsigned int numRecords,
                                       ATMO_NFC_Record_t *records )
{
	if ( !( instance < numberOfNFCDriverInstance ) )
	{
		return ATMO_NFC_Status_Invalid;
	}

	return nfcInstances[instance]->SetMessage( nfcInstancesData[instance], numRecords, records );
}

ATMO_NFC_Status_t ATMO_NFC_SetMessagePtr( ATMO_DriverInstanceHandle_t instance, unsigned int numRecords,
        ATMO_NFC_Record_t **records )
{
	if ( !( instance < numberOfNFCDriverInstance ) )
	{
		return ATMO_NFC_Status_Invalid;
	}

	return nfcInstances[instance]->SetMessagePtr( nfcInstancesData[instance], numRecords, records );
}

ATMO_NFC_Status_t ATMO_NFC_GetNumStoredRecords( ATMO_DriverInstanceHandle_t instance, unsigned int *numRecords )
{
	if ( !( instance < numberOfNFCDriverInstance ) )
	{
		return ATMO_NFC_Status_Invalid;
	}

	return nfcInstances[instance]->GetNumStoredRecords( nfcInstancesData[instance], numRecords );
}

ATMO_NFC_Status_t ATMO_NFC_GetRecord( ATMO_DriverInstanceHandle_t instance, unsigned int index, ATMO_NFC_Record_t *record )
{
	if ( !( instance < numberOfNFCDriverInstance ) )
	{
		return ATMO_NFC_Status_Invalid;
	}

	return nfcInstances[instance]->GetRecord( nfcInstancesData[instance], index, record );
}

ATMO_NFC_Status_t ATMO_NFC_SetMessageReceivedCallback( ATMO_DriverInstanceHandle_t instance, ATMO_Callback_t cb )
{
	if ( !( instance < numberOfNFCDriverInstance ) )
	{
		return ATMO_NFC_Status_Invalid;
	}

	return nfcInstances[instance]->SetMessageReceivedCallback( nfcInstancesData[instance], cb );
}

ATMO_NFC_Status_t ATMO_NFC_SetMessageReceivedAbilityHandle( ATMO_DriverInstanceHandle_t instance, unsigned int abilityHandle )
{
	if ( !( instance < numberOfNFCDriverInstance ) )
	{
		return ATMO_NFC_Status_Invalid;
	}

	return nfcInstances[instance]->SetMessageReceivedAbilityHandle( nfcInstancesData[instance], abilityHandle );
}

ATMO_NFC_Status_t ATMO_NFC_RegisterStoredRecord( ATMO_DriverInstanceHandle_t instance, ATMO_NFC_Record_t *record, unsigned int *recordHandle )
{
	if ( !( instance < numberOfNFCDriverInstance ) )
	{
		return ATMO_NFC_Status_Invalid;
	}

	if ( numStoredRecords[instance] >= NUMBER_STORED_RECORDS_PER_INSTANCE )
	{
		return ATMO_NFC_Status_Invalid;
	}

	// Default record
	ATMO_NFC_MakeRecordText( record, "undefined" );

	*recordHandle = numStoredRecords[instance]++;
	storedRecords[instance][*recordHandle] = record;

	return ATMO_NFC_Status_Success;
}

ATMO_NFC_Status_t ATMO_NFC_SyncAtmoRecords( ATMO_DriverInstanceHandle_t instance )
{
	if ( !( instance < numberOfNFCDriverInstance ) )
	{
		return ATMO_NFC_Status_Invalid;
	}

	return ATMO_NFC_SetMessagePtr( instance, numStoredRecords[instance], storedRecords[instance] );
}

ATMO_NFC_Status_t ATMO_NFC_MakeRecordText( ATMO_NFC_Record_t *record, const char *text )
{
	record->header.flags = 0;
	record->header.flags |= ATMO_NFC_FLAG_TNF( ATMO_NFC_TNF_WellKnown );
	record->header.typeLength = 1;
	record->header.idLength = 0;
	record->header.payloadLength = strlen( text ) + 3;
	record->header.payloadType[0] = 0x54; // Text
	record->payload[0] = 0x2;
	record->payload[1] = 'e';
	record->payload[2] = 'n';
	strncpy( ( char * )&record->payload[3], text, ATMO_NFC_MAX_PAYLOAD_LEN - 3 );
	return ATMO_NFC_Status_Success;
}

/**
 * Check for common protocols on url to save bytes
 *
 * There is a list of ~35 protocols in the spec, but only care about the commonly used ones right now.
 */
void _ATMO_NFC_GetUrlProtocolByte( const char *url, uint8_t *protocolCode, unsigned int *startPos )
{
	if ( strstr( url, "http://www." ) == url )
	{
		*startPos = strlen( "http://www." );
		*protocolCode = 0x1;
	}
	else if ( strstr( url, "https:/www." ) == url )
	{
		*startPos = strlen( "https://www." );
		*protocolCode = 0x2;
	}
	else if ( strstr( url, "http://" ) == url )
	{
		*startPos = strlen( "http://" );
		*protocolCode = 0x3;
	}
	else if ( strstr( url, "https://" ) == url )
	{
		*startPos = strlen( "https://" );
		*protocolCode = 0x4;
	}
	else
	{
		*startPos = 0;
		*protocolCode = 0x0;
	}

	return;
}

ATMO_NFC_Status_t ATMO_NFC_MakeRecordURL( ATMO_NFC_Record_t *record, const char *url )
{
	record->header.flags = 0;
	record->header.flags |= ATMO_NFC_FLAG_TNF( ATMO_NFC_TNF_WellKnown );
	record->header.typeLength = 1;
	record->header.idLength = 0;
	record->header.payloadLength = strlen( url ) + 1;
	record->header.payloadType[0] = 'U'; // URL

	unsigned int urlDataStart = 0;
	_ATMO_NFC_GetUrlProtocolByte( url, &record->payload[0], &urlDataStart );
	strncpy( ( char * )&record->payload[1], ( char * )&url[urlDataStart], ATMO_NFC_MAX_PAYLOAD_LEN - 1 );

	return ATMO_NFC_Status_Success;
}

ATMO_NFC_Status_t ATMO_NFC_MakeRecordMime( ATMO_NFC_Record_t *record, const char *mimeType, const char *data,
        unsigned int dataLen )
{
	if ( dataLen > ATMO_NFC_MAX_PAYLOAD_LEN )
	{
		return ATMO_NFC_Status_Fail;
	}

	record->header.flags = 0;
	record->header.flags |= ATMO_NFC_FLAG_TNF( ATMO_NFC_TNF_MIME ); // MIME
	record->header.typeLength = strlen( mimeType );
	record->header.idLength = 0;
	record->header.payloadLength = dataLen;
	strncpy( ( char * )record->header.payloadType, mimeType, ATMO_NFC_MAX_PAYLOAD_TYPE_LEN );
	memcpy( &record->payload[0], data, dataLen );
	return ATMO_NFC_Status_Success;
}

ATMO_NFC_Status_t ATMO_NFC_MakeRecordExternal( ATMO_NFC_Record_t *record, const char *type, const char *data,
        unsigned int dataLen )
{
	if ( dataLen > ATMO_NFC_MAX_PAYLOAD_LEN )
	{
		return ATMO_NFC_Status_Fail;
	}

	record->header.flags = 0;
	record->header.flags |= ATMO_NFC_FLAG_TNF( ATMO_NFC_TNF_EXTERNAL );
	record->header.typeLength = strlen( type );
	record->header.idLength = 0;
	record->header.payloadLength = dataLen;
	strncpy( ( char * )record->header.payloadType, type, ATMO_NFC_MAX_PAYLOAD_TYPE_LEN );
	memcpy( &record->payload[0], data, dataLen );
	return ATMO_NFC_Status_Success;
}

ATMO_NFC_Status_t ATMO_NFC_PrintRecord( ATMO_NFC_Record_t *record )
{
#ifndef ATMO_SLIM_STACK
	ATMO_PLATFORM_DebugPrint( "=== NFC Record ===\r\n" );

	switch ( ATMO_NFC_FLAG_GET_TNF( record->header.flags ) )
	{
		case ATMO_NFC_TNF_WellKnown:
		{
			ATMO_PLATFORM_DebugPrint( "TNF: Well Known\r\n" );
			break;
		}

		case ATMO_NFC_TNF_MIME:
		{
			ATMO_PLATFORM_DebugPrint( "TNF: MIME\r\n" );
			break;
		}

		case ATMO_NFC_TNF_URI:
		{
			ATMO_PLATFORM_DebugPrint( "TNF: Absolute URI\r\n" );
			break;
		}

		case ATMO_NFC_TNF_EXTERNAL:
		{
			ATMO_PLATFORM_DebugPrint( "TNF: External\r\n" );
			break;
		}
	}

	ATMO_PLATFORM_DebugPrint( "Payload Length: %d\r\n", record->header.payloadLength );
	ATMO_PLATFORM_DebugPrint( "Payload Type: %s\r\n", record->header.payloadType );
	ATMO_PLATFORM_DebugPrint( "Payload: %s\r\n\r\n", record->payload );
#endif
	return ATMO_NFC_Status_Success;
}


