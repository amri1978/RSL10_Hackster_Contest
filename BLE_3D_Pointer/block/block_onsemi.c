#include "block_onsemi.h"
#include "../filesystem/filesystem_crastfs.h"
#include <rsl10_flash_rom.h>
#include <rsl10_flash.h>

ATMO_BLOCK_DriverInstance_t ONSEMIBlockDriverInstance =
{
	ATMO_ONSEMI_BLOCK_Init,
	ATMO_ONSEMI_BLOCK_Read,
	ATMO_ONSEMI_BLOCK_Program,
	ATMO_ONSEMI_BLOCK_Erase,
	ATMO_ONSEMI_BLOCK_Sync,
	ATMO_ONSEMI_BLOCK_GetDeviceInfo
};

#define ATMO_ONSEMI_NVRAM_SIZE 256

// #define ATMO_ONSEMI_BLOCK_DBG

typedef struct
{
	unsigned int 	sysInfo[4]; // Built in sysinfo that needs to remain
	uint8_t atmoData[ATMO_ONSEMI_NVRAM_SIZE];
} _ATMO_ONSEMI_FlashData_t;

#define ATMO_ONSEMI_DATA_BASE 0x00080200

static _ATMO_ONSEMI_FlashData_t _ATMO_ONSEMI_FlashData = {0};

ATMO_Status_t ATMO_ONSEMI_BLOCK_AddDriverInstance( ATMO_DriverInstanceHandle_t *instanceNumber )
{
	static ATMO_DriverInstanceData_t driver;
	driver.name = "";
	driver.initialized = false;
	driver.instanceNumber = *instanceNumber;
	driver.argument = NULL;

	return ATMO_BLOCK_AddDriverInstance( &ONSEMIBlockDriverInstance, &driver, instanceNumber );
}

static void _ATMO_ONSEMI_BLOCK_MemDump()
{
	ATMO_PLATFORM_DebugPrint( "FLASH DATA\r\n" );

	for ( unsigned int i = 0; i < ATMO_ONSEMI_NVRAM_SIZE / 16; i++ )
	{

		ATMO_PLATFORM_DebugPrint( "%03d: ", i * 16 );

		for ( unsigned int j = 0; j < 16; j++ )
		{
			ATMO_PLATFORM_DebugPrint( "%02X ", _ATMO_ONSEMI_FlashData.atmoData[( 16 * i ) + j] );
		}

		ATMO_PLATFORM_DebugPrint( "\r\n" );
	}
}

static void _ATMO_ONSEMI_BLOCK_RegCb( void *data )
{
#ifdef ATMO_ONSEMI_BLOCK_DBG
	ATMO_PLATFORM_DebugPrint( "SYNCING FLASH\r\n" );
	_ATMO_ONSEMI_BLOCK_MemDump();
#endif
	ATMO_ONSEMI_BLOCK_Sync( NULL );
}

ATMO_BLOCK_Status_t ATMO_ONSEMI_BLOCK_Init( ATMO_DriverInstanceData_t *instance )
{
	// Sync with nvram
	memcpy( ( void * )_ATMO_ONSEMI_FlashData.atmoData, ( void * ) ATMO_ONSEMI_DATA_BASE, ATMO_ONSEMI_NVRAM_SIZE );

#ifdef ATMO_ONSEMI_BLOCK_DBG
	_ATMO_ONSEMI_BLOCK_MemDump();

	ATMO_PLATFORM_DebugPrint( "Block init complete\r\n" );
#endif

	ATMO_CLOUD_RegisterRegistrationSetCallback( _ATMO_ONSEMI_BLOCK_RegCb );

	return ATMO_BLOCK_Status_Success;
}

ATMO_BLOCK_Status_t ATMO_ONSEMI_BLOCK_Read( ATMO_DriverInstanceData_t *instance, uint32_t block, uint32_t offset, void *buffer, uint32_t size )
{
	if ( block != 0 || ( offset + size ) >= ATMO_ONSEMI_NVRAM_SIZE )
	{
		return ATMO_BLOCK_Status_Fail;
	}

#ifdef ATMO_ONSEMI_BLOCK_DBG
	ATMO_PLATFORM_DebugPrint("Reading at offset %d\r\n", offset);

	_ATMO_ONSEMI_BLOCK_MemDump();
#endif

	memcpy( buffer, &_ATMO_ONSEMI_FlashData.atmoData[offset], size );

#ifdef ATMO_ONSEMI_BLOCK_DBG
	for(unsigned int i = 0; i < size; i++)
	{
		ATMO_PLATFORM_DebugPrint("Buffer[%03d]: %02X\r\n", i, ((uint8_t *)buffer)[i]);
	}
#endif
	return ATMO_BLOCK_Status_Success;
}

static FlashStatus _ATMO_ONSEMI_BLOCK_ResetFlash()
{
	FlashStatus flash_result;
	/// Store NVR1 data
	memcpy( ( void * )_ATMO_ONSEMI_FlashData.sysInfo, ( void * ) SYS_INFO_START_ADDR, 4 );

	/// Erase NVR1
	// Write enable NVR1
	FLASH->NVR_CTRL = ( NVR1_WRITE_ENABLE | NVR2_WRITE_DISABLE |
	                    NVR3_WRITE_DISABLE );
	FLASH->NVR_WRITE_UNLOCK = FLASH_NVR_KEY;
	// Erase NVR1
	flash_result = Flash_EraseSector( SYS_INFO_START_ADDR );

	if ( flash_result != FLASH_ERR_NONE )
	{
		ATMO_PLATFORM_DebugPrint( "Error erasing sector\r\n" );
		return flash_result;
	}

	/// Restore NVR1 data and add new address
	// Restore SYS_INFO_START_ADDR
	flash_result = Flash_WriteBuffer( SYS_INFO_START_ADDR, 4, _ATMO_ONSEMI_FlashData.sysInfo );

	if ( flash_result != FLASH_ERR_NONE )
	{
		ATMO_PLATFORM_DebugPrint( "Error writing sys info to sector\r\n" );
		return flash_result;
	}

	// TODO: lock writing again?

	/// Return success
#ifdef ATMO_ONSEMI_BLOCK_DBG
	ATMO_PLATFORM_DebugPrint( "Sector erased\r\n" );
#endif
	return ( FLASH_ERR_NONE );
}

static ATMO_BLOCK_Status_t _ATMO_ONSEMI_BLOCK_SyncFlash()
{
#ifdef ATMO_ONSEMI_BLOCK_DBG
	ATMO_PLATFORM_DebugPrint( "Sync flash\r\n" );
#endif

	if ( _ATMO_ONSEMI_BLOCK_ResetFlash() != FLASH_ERR_NONE )
	{
		return ATMO_BLOCK_Status_Fail;
	}

#ifdef ATMO_ONSEMI_BLOCK_DBG
	ATMO_PLATFORM_DebugPrint( "Flash erased\r\n" );
#endif

// Write to flash
	// Copied from flashwrite.h in bdk lighting example
	/// Erase NVR1
	// Write enable NVR1
	FLASH->NVR_CTRL = ( NVR1_WRITE_ENABLE | NVR2_WRITE_DISABLE |
	                    NVR3_WRITE_DISABLE );
	FLASH->NVR_WRITE_UNLOCK = FLASH_NVR_KEY;

	// I think we can only write in certain sized chunks
	for ( unsigned int i = 0; i < ATMO_ONSEMI_NVRAM_SIZE; i += 8 )
	{
#ifdef ATMO_ONSEMI_BLOCK_DBG
		ATMO_PLATFORM_DebugPrint( "Syncing %d percent\r\n", ( i * 100 ) / ATMO_ONSEMI_NVRAM_SIZE );
#endif
		// Write word pair
		uint32_t w1, w2;
		uint8_t *pData = &_ATMO_ONSEMI_FlashData.atmoData[i];
		w1 = pData[3] << 24 | pData[2] << 16 | pData[1] << 8 | pData[0];
		w2 = pData[7] << 24 | pData[6] << 16 | pData[5] << 8 | pData[4];
		FlashStatus flash_result = Flash_WriteWordPair( ATMO_ONSEMI_DATA_BASE + i, w1, w2 );

		if ( flash_result != FLASH_ERR_NONE )
		{
			ATMO_PLATFORM_DebugPrint( "Error writing sector %d offset %d\r\n", flash_result, i );
			return ATMO_BLOCK_Status_Fail;
		}
	}

	// lock writing to flash again
	FLASH->NVR_CTRL = ( NVR1_WRITE_DISABLE |
	                    NVR2_WRITE_DISABLE |
	                    NVR3_WRITE_DISABLE );

	ATMO_PLATFORM_DebugPrint( "Write success\r\n" );

	return ATMO_BLOCK_Status_Success;
}

ATMO_BLOCK_Status_t ATMO_ONSEMI_BLOCK_Erase( ATMO_DriverInstanceData_t *instance, uint32_t block )
{
	if ( block != 0 )
	{
		return ATMO_BLOCK_Status_Fail;
	}

	ATMO_PLATFORM_DebugPrint("BLOCK ERASE\r\n");

	memset( _ATMO_ONSEMI_FlashData.atmoData, 0x0, ATMO_ONSEMI_NVRAM_SIZE );

	return ATMO_BLOCK_Status_Success;
}

ATMO_BLOCK_Status_t ATMO_ONSEMI_BLOCK_Program( ATMO_DriverInstanceData_t *instance, uint32_t block, uint32_t offset, void *buffer, uint32_t size )
{
	if ( block != 0 || ( offset + size ) >= ATMO_ONSEMI_NVRAM_SIZE )
	{
		return ATMO_BLOCK_Status_Fail;
	}

	memcpy( &_ATMO_ONSEMI_FlashData.atmoData[offset], buffer, size );

	return ATMO_BLOCK_Status_Success;
}

ATMO_BLOCK_Status_t ATMO_ONSEMI_BLOCK_Sync( ATMO_DriverInstanceData_t *instance )
{
	_ATMO_ONSEMI_BLOCK_SyncFlash();
	return ATMO_BLOCK_Status_Success;
}

ATMO_BLOCK_Status_t ATMO_ONSEMI_BLOCK_GetDeviceInfo( ATMO_DriverInstanceData_t *instance, ATMO_BLOCK_DeviceInfo_t *info )
{
	info->blockCount = 1; //Only one block
	info->blockSize = ATMO_ONSEMI_NVRAM_SIZE;
	info->progSize = 1;
	info->readSize = 1;
	return ATMO_BLOCK_Status_Success;
}
