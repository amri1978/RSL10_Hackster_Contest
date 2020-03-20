#include "atmosphere_platform.h"
#include "../interval/interval_onsemi.h"
#include "../ble/ble_onsemi.h"
#include "../gpio/gpio_onsemi.h"
#include "../i2c/i2c_onsemi.h"
#include "../block/block_onsemi.h"
#include "../filesystem/filesystem_crastfs.h"

void ATMO_BtnCb( void *data )
{
	ATMO_PLATFORM_DebugPrint( "BUTTON PRESSED\r\n" );
}

void ATMO_PLATFORM_Init()
{
	ATMO_DriverInstanceHandle_t handle;
	ATMO_ONSEMI_INTERVAL_AddDriverInstance( &handle );
	ATMO_INTERVAL_Init( handle );

	ATMO_ONSEMI_BLE_AddDriverInstance( &handle );
	ATMO_BLE_PeripheralInit( handle );

	ATMO_ONSEMI_I2C_AddDriverInstance( &handle );
	ATMO_I2C_Init( 0 );

	ATMO_ONSEMI_GPIO_AddDriverInstance( &handle );
	ATMO_GPIO_Init( 0 );

	ATMO_ONSEMI_BLOCK_AddDriverInstance( &handle );
	ATMO_BLOCK_Init( 0 );

	ATMO_CRASTFS_FILESYSTEM_AddDriverInstance( &handle );
	ATMO_FILESYSTEM_Init( 0, 0 );

	ATMO_PLATFORM_DebugPrint("Crast FS init complete\r\n");

	ATMO_CLOUD_InitFilesystemData( 0 );

	ATMO_CLOUD_BLE_AddDriverInstance( &handle, 0 );




	Sys_ADC_Set_Config( ADC_VBAT_DIV2_NORMAL | ADC_NORMAL | ADC_PRESCALE_6400 );
	Sys_ADC_InputSelectConfig( 0, ADC_NEG_INPUT_GND | ADC_POS_INPUT_VBAT_DIV2 );
}

void ATMO_PLATFORM_PostInit()
{
	ATMO_CLOUD_Init( 0, true );
}

void ATMO_PLATFORM_DelayMilliseconds( uint32_t milliseconds )
{
	HAL_Delay( milliseconds );
}

void *ATMO_Malloc( uint32_t numBytes )
{
	return NULL;
}

void *ATMO_Calloc( size_t num, size_t size )
{
	return NULL;
}

void ATMO_Free( void *data )
{
	return;
}

void ATMO_Lock()
{

}

void ATMO_Unlock()
{

}

uint64_t ATMO_PLATFORM_UptimeMs()
{
	return 0;
}

uint32_t ATMO_PLATFORM_GetBattLevel()
{
	static int uptime = 0;
	uint32_t voltage = ADC->DATA_TRIM_CH[0];
	ATMO_PLATFORM_DebugPrint( "Uptime: %d\r\n", uptime );
	uptime += 60;
	uint32_t level = ( ( voltage - VBAT_2p0V_MEASURED ) * 100
	                   / ( VBAT_3p0V_MEASURED - VBAT_2p0V_MEASURED ) );
	level = ( ( level > 100 ) ? 100 : level );
	return level;
}
