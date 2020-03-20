#include "bme680.h"
#include "bme680_reg.h"
#include "bme680_env.h"

static struct bme680_dev _BME680_DriverConfig = {0};
static BME680_Config_t _BME680_PrivConfig;
static uint16_t _BLE680_Meas_Delay_Ms = 0;


static int8_t BME680_I2C_Write( uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len )
{
	uint8_t status;

	uint8_t writeData[len + 1];

	writeData[0] = reg_addr;
	memcpy( &writeData[1], reg_data, len );

	return ATMO_I2C_MasterWrite( _BME680_PrivConfig.i2cInstance, dev_id, NULL, 0, writeData, len + 1, 0 ) == ATMO_I2C_Status_Success ? 0 : 1;
}

static int8_t BME680_I2C_Read( uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len )
{
	ATMO_I2C_Status_t status = ATMO_I2C_Status_Success;
	uint8_t currentReg = reg_addr;

	unsigned int i;

	status = ATMO_I2C_MasterRead( _BME680_PrivConfig.i2cInstance, dev_id,
	                              &reg_addr, 1, reg_data, len, 0 );

	return ( status == ATMO_I2C_Status_Success ) ? 0 : 1;
}

BME680_Status_t BME680_Init( BME680_Config_t *config )
{
	if ( config == NULL )
	{
		return BME680_Status_Fail;
	}

	memcpy( &_BME680_PrivConfig, config, sizeof( _BME680_PrivConfig ) );

	ATMO_I2C_Peripheral_t i2cConfig;
	i2cConfig.operatingMode = ATMO_I2C_OperatingMode_Master;
	i2cConfig.baudRate = ATMO_I2C_BaudRate_Standard_Mode;
	ATMO_I2C_SetConfiguration( config->i2cInstance, &i2cConfig );

	ATMO_PLATFORM_DelayMilliseconds( 1000 );

	int8_t status;
	uint8_t sel_setting;

	_BME680_DriverConfig.dev_id = BME680_I2C_ADDR_PRIMARY;
	_BME680_DriverConfig.intf = BME680_I2C_INTF;
	_BME680_DriverConfig.read = &BME680_I2C_Read;
	_BME680_DriverConfig.write = &BME680_I2C_Write;
	_BME680_DriverConfig.delay_ms = &ATMO_PLATFORM_DelayMilliseconds;
	_BME680_DriverConfig.amb_temp = 25;

	status = bme680_init( &_BME680_DriverConfig );

	if ( status != BME680_OK )
	{
		ATMO_PLATFORM_DelayMilliseconds( "Error initializing BME680" );
		return BME680_Status_Fail;
	}

	_BME680_DriverConfig.tph_sett.os_hum = RTE_BME680_ENV_OS_HUM;
	_BME680_DriverConfig.tph_sett.os_pres = RTE_BME680_ENV_OS_PRES;
	_BME680_DriverConfig.tph_sett.os_temp = RTE_BME680_ENV_OS_TEMP;
	_BME680_DriverConfig.tph_sett.filter = RTE_BME680_ENV_FILTER_SIZE;

	_BME680_DriverConfig.gas_sett.run_gas = BME680_DISABLE_GAS_MEAS;
	_BME680_DriverConfig.gas_sett.heatr_temp = 320;
	_BME680_DriverConfig.gas_sett.heatr_dur = 150;

	_BME680_DriverConfig.power_mode = BME680_SLEEP_MODE;

	sel_setting = BME680_OST_SEL | BME680_OSP_SEL | BME680_OSH_SEL
	              | BME680_FILTER_SEL | BME680_GAS_SENSOR_SEL;

	status = bme680_set_sensor_settings( sel_setting, &_BME680_DriverConfig );

	if ( status != BME680_OK )
	{
		ATMO_PLATFORM_DelayMilliseconds( "Error setting sensor settings" );
		return BME680_Status_Fail;
	}

	status = bme680_set_sensor_mode( &_BME680_DriverConfig );

	if ( status != BME680_OK )
	{
		ATMO_PLATFORM_DelayMilliseconds( "Error setting sensor mode" );
		return BME680_Status_Fail;
	}

	// How long it takes between turning sensor on and getting a reading
	bme680_get_profile_dur( &_BLE680_Meas_Delay_Ms, &_BME680_DriverConfig );

	ATMO_PLATFORM_DebugPrint( "Meas Delay: %d\r\n", _BLE680_Meas_Delay_Ms );

	return 0;
}

static void _BME680_StartMeasurement()
{
	// Turn sensor on to start measure
	_BME680_DriverConfig.power_mode = BME680_FORCED_MODE;
	bme680_set_sensor_mode( &_BME680_DriverConfig );

	// Wait for measurement to complete
	ATMO_PLATFORM_DelayMilliseconds( _BLE680_Meas_Delay_Ms );
}

static void _BME680_Sleep()
{
	// Go back to sleep
	_BME680_DriverConfig.power_mode = BME680_SLEEP_MODE;
	bme680_set_sensor_mode( &_BME680_DriverConfig );
}

BME680_Status_t BME680_GetTempData( float *tempC )
{
	_BME680_StartMeasurement();

	struct bme680_field_data data;
	int8_t retval = bme680_get_sensor_data( &data, &_BME680_DriverConfig );

	if ( retval != 0 )
	{
		ATMO_PLATFORM_DebugPrint( "Error getting sensor data! %d\r\n", retval );
		*tempC = 0;
	}
	else
	{
		*tempC = data.temperature / 100.0;
	}

	_BME680_Sleep();
	return BME680_Status_Success;
}

BME680_Status_t BME680_GetPressure( float *pressureHpa )
{
	_BME680_StartMeasurement();

	struct bme680_field_data data;
	int8_t retval = bme680_get_sensor_data( &data, &_BME680_DriverConfig );

	if ( retval != 0 )
	{
		ATMO_PLATFORM_DebugPrint( "Error getting sensor data! %d\r\n", retval );
		*pressureHpa = 0;
	}
	else
	{
		*pressureHpa = data.pressure;
	}

	_BME680_Sleep();
	return BME680_Status_Success;
}

BME680_Status_t BME680_GetHumidity( float *humidity )
{
	_BME680_StartMeasurement();

	struct bme680_field_data data;
	int8_t retval = bme680_get_sensor_data( &data, &_BME680_DriverConfig );

	if ( retval != 0 )
	{
		ATMO_PLATFORM_DebugPrint( "Error getting sensor data! %d\r\n", retval );
		*humidity = 0;
	}
	else
	{
		*humidity = data.humidity / 1000.0;
	}

	_BME680_Sleep();
	return BME680_Status_Success;
}
