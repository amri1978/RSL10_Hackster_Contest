#include "bhi160.h"
#include "bhy_support.h"
#include "bhy.h"
#include "bhy_uc_driver.h"
#include "bhy1_fw.h"

enum BHI160_NDOF_Sensor
{
	BHI160_NDOF_S_ORIENTATION          = VS_TYPE_ORIENTATION,
	BHI160_NDOF_S_GRAVITY              = VS_TYPE_GRAVITY,
	BHI160_NDOF_S_LINEAR_ACCELERATION  = VS_TYPE_LINEAR_ACCELERATION,
	BHI160_NDOF_S_RATE_OF_ROTATION     = VS_TYPE_GYROSCOPE,
	BHI160_NDOF_S_MAGNETIC_FIELD       = VS_TYPE_GEOMAGNETIC_FIELD
};

#define FIFO_SIZE                      300
#define MAX_PACKET_LENGTH              18
#define OUT_BUFFER_SIZE                60

/** \brief Translation matrix used to rotate axes of accelerometer and
 * gyroscope within BHI160.
 *
 * Can contain only values of -1, 0 or 1.
 */
#define BHI160_NDOF_BHI160_AXIS_MAPPING \
        { 0, -1,  0, \
          1,  0,  0, \
          0,  0,  1 }

/** \brief Translation matrix used to rotate axes of BMM150 magnetometer
 * to adjust for different package orientation on the PCB.
 *
 * Can contain only values of -1, 0 or 1.
 */
#define BHI160_NDOF_BMM150_AXIS_MAPPING \
        { -1,  0,  0, \
           0,  1,  0, \
           0,  0, -1 }

static uint8_t bhi160_fifo[FIFO_SIZE];
static uint16_t bhi160_accel_dyn_range = 4;
static uint16_t bhi160_gyro_dyn_range = 2000;
static uint16_t bhi160_mag_dyn_range = 1000;
static uint8_t bhi160_bytes_left_in_fifo = 0;
static uint16_t bytes_remaining = 0;

typedef void ( *BHI160_NDOF_SensorCallback )( bhy_data_generic_t *data,
        bhy_virtual_sensor_t sensor );

void BMI160_MagDataCb( bhy_data_generic_t *data, bhy_virtual_sensor_t sensor );
void BMI160_AccelDataCb( bhy_data_generic_t *data, bhy_virtual_sensor_t sensor );
void BMI160_GyroDataCb( bhy_data_generic_t *data, bhy_virtual_sensor_t sensor );

static BHI160_Config_t _BHI160_Config;

static ATMO_3dFloatVector_t _BMI160_AccData, _BMI160_GyroData, _BMI160_MagData;

static int32_t _BHI160_EnableSensor( enum BHI160_NDOF_Sensor sensor, BHI160_NDOF_SensorCallback cb, uint16_t sample_rate )
{
	int32_t retval = bhy_install_sensor_callback( sensor, VS_WAKEUP, cb );

	if ( retval != BHY_SUCCESS )
	{
		return retval;
	}

	retval = bhy_enable_virtual_sensor( sensor, VS_WAKEUP,
	                                    sample_rate, 0, VS_FLUSH_NONE, 0, 0 );

	return retval;
}

static void _BHI160_DynamicRangeCallback( bhy_data_meta_event_t *data, bhy_meta_event_type_t event )
{
	if ( event == BHY_META_EVENT_TYPE_DYNAMIC_RANGE_CHANGED )
	{
		struct accel_physical_status_t accel;
		struct gyro_physical_status_t gyro;
		struct mag_physical_status_t mag;

		int32_t retval = bhy_get_physical_sensor_status( &accel, &gyro, &mag );
		ASSERT_DEBUG( retval == BHY_SUCCESS );

		if ( retval == BHY_SUCCESS )
		{
			bhi160_accel_dyn_range = accel.accel_dynamic_range;
			bhi160_gyro_dyn_range = gyro.gyro_dynamic_range;
			bhi160_mag_dyn_range = mag.mag_dynamic_range;
		}
	}
}

void BMI160_MagDataCb( bhy_data_generic_t *data, bhy_virtual_sensor_t sensor )
{
	_BMI160_MagData.x = data->data_vector.x / 32768.0f * 360.0f;
	_BMI160_MagData.y = data->data_vector.y / 32768.0f * 360.0f;
	_BMI160_MagData.z = data->data_vector.z / 32768.0f * 360.0f;
}

void BMI160_AccDataCb( bhy_data_generic_t *data, bhy_virtual_sensor_t sensor )
{
	_BMI160_AccData.x = data->data_vector.x / 32768.0f * bhi160_accel_dyn_range;
	_BMI160_AccData.y = data->data_vector.y / 32768.0f * bhi160_accel_dyn_range;
	_BMI160_AccData.z = data->data_vector.z / 32768.0f * bhi160_accel_dyn_range;
}

void BMI160_GyroDataCb( bhy_data_generic_t *data, bhy_virtual_sensor_t sensor )
{
	_BMI160_GyroData.x = data->data_vector.x / 32768.0f * bhi160_gyro_dyn_range;
	_BMI160_GyroData.y = data->data_vector.y / 32768.0f * bhi160_gyro_dyn_range;
	_BMI160_GyroData.z = data->data_vector.z / 32768.0f * bhi160_gyro_dyn_range;
}

static void _BHI160_FifoRoutine( void *arg )
{
	( void )arg;
	uint8_t *fifoptr = bhi160_fifo;
	bhy_data_type_t packet_type = BHY_DATA_TYPE_PADDING;
	bhy_data_generic_t fifo_packet;
	uint16_t bytes_read = 0;
	BHY_RETURN_FUNCTION_TYPE result;

	bhy_read_fifo( bhi160_fifo + bhi160_bytes_left_in_fifo,
	               FIFO_SIZE - bhi160_bytes_left_in_fifo, &bytes_read,
	               &bytes_remaining );
	bytes_read += bhi160_bytes_left_in_fifo;

	do
	{
		/* this function will call callbacks that are registered */
		result = bhy_parse_next_fifo_packet( &fifoptr, &bytes_read, &fifo_packet,
		                                     &packet_type );

		/* prints all the debug packets */
		if ( packet_type == BHY_DATA_TYPE_DEBUG )
		{
			bhy_print_debug_packet( &fifo_packet.data_debug, bhy_printf );
		}

		/* the logic here is that if doing a partial parsing of the fifo, then we should not parse  */
		/* the last 18 bytes (max length of a packet) so that we don't try to parse an incomplete   */
		/* packet */
	}
	while ( ( result == BHY_SUCCESS )
	        && ( bytes_read > ( bytes_remaining ? MAX_PACKET_LENGTH : 0 ) ) );

	bhi160_bytes_left_in_fifo = 0;

	if ( bytes_remaining )
	{
		/* shifts the remaining bytes to the beginning of the buffer */
		while ( bhi160_bytes_left_in_fifo < bytes_read )
		{
			bhi160_fifo[bhi160_bytes_left_in_fifo++] = *( fifoptr++ );
		}
	}
}



ATMO_BOOL_t BHI160_Init( BHI160_Config_t *config )
{
	memcpy( &_BHI160_Config, config, sizeof( _BHI160_Config ) );

	ATMO_GPIO_Config_t gpioConfig;
	gpioConfig.pinMode = ATMO_GPIO_PinMode_Input_HighImpedance;
	ATMO_GPIO_SetPinConfiguration( config->gpioInstance, config->intPin, &gpioConfig );

	bhy_set_driver_instance( config->i2cInstance );

	int32_t retval = bhy_driver_init( bhy1_fw );

	if ( retval != BHY_SUCCESS )
	{
		ATMO_PLATFORM_DebugPrint( "BHI160 low level driver init failed\r\n" );
		return false;
	}

	// Wait for reset
	while ( ATMO_GPIO_Read( config->gpioInstance, config->intPin ) == ATMO_GPIO_PinState_High )
	{

	}

	while ( ATMO_GPIO_Read( config->gpioInstance, config->intPin ) == ATMO_GPIO_PinState_Low )
	{

	}

	ATMO_PLATFORM_DebugPrint( "BHI160 Reset Complete\r\n" );

	// Register for interrupts
	//ATMO_GPIO_RegisterInterruptCallback(config->gpioInstance, config->intPin, ATMO_GPIO_InterruptTrigger_RisingEdge, _BHI160_FifoRoutine);

	ATMO_PLATFORM_DebugPrint( "Interrupts registered\r\n" );

	/* Remap sensor axes based on relative placement of BHI160 and BMM150 on the
	* board */
	int8_t bhi160_mapping[9] = BHI160_NDOF_BHI160_AXIS_MAPPING;
	int8_t bmm150_mapping[9] = BHI160_NDOF_BMM150_AXIS_MAPPING;

	retval = bhy_mapping_matrix_set( PHYSICAL_SENSOR_INDEX_ACC, bhi160_mapping );

	if ( retval != BHY_SUCCESS )
	{
		ATMO_PLATFORM_DebugPrint( "BHI160: Error mapping accelerometer\r\n" );
		return false;
	}

	retval = bhy_mapping_matrix_set( PHYSICAL_SENSOR_INDEX_GYRO, bhi160_mapping );

	if ( retval != BHY_SUCCESS )
	{
		ATMO_PLATFORM_DebugPrint( "BHI160: Error mapping gyroscope\r\n" );
		return false;
	}

	retval = bhy_mapping_matrix_set( PHYSICAL_SENSOR_INDEX_MAG, bmm150_mapping );

	if ( retval != BHY_SUCCESS )
	{
		ATMO_PLATFORM_DebugPrint( "BHI160: Error mapping magnetometer\r\n" );
		return false;
	}

	retval = _BHI160_EnableSensor( BHI160_NDOF_S_ORIENTATION, BMI160_MagDataCb, 5 );

	if ( retval != BHY_SUCCESS )
	{
		ATMO_PLATFORM_DebugPrint( "BHI160: Error setting magnetometer callback\r\n" );
		return false;
	}

	retval = _BHI160_EnableSensor( BHI160_NDOF_S_LINEAR_ACCELERATION, BMI160_AccDataCb, 5 );

	if ( retval != BHY_SUCCESS )
	{
		ATMO_PLATFORM_DebugPrint( "BHI160: Error setting accelerometer callback\r\n" );
		return false;
	}

	retval = _BHI160_EnableSensor( BHI160_NDOF_S_RATE_OF_ROTATION, BMI160_GyroDataCb, 5 );

	if ( retval != BHY_SUCCESS )
	{
		ATMO_PLATFORM_DebugPrint( "BHI160: Error setting gyro callback\r\n" );
		return false;
	}

	retval = bhy_install_meta_event_callback( BHY_META_EVENT_TYPE_DYNAMIC_RANGE_CHANGED, &_BHI160_DynamicRangeCallback );

	if ( retval != BHY_SUCCESS )
	{
		ATMO_PLATFORM_DebugPrint( "BHI160: Error setting dynamic range callback\r\n" );
		return false;
	}

	ATMO_AddTickCallback( _BHI160_FifoRoutine );

	ATMO_PLATFORM_DebugPrint( "BHI160 successfully initialized\r\n" );

	return true;
}

ATMO_BOOL_t BHI160_GetData( ATMO_3dFloatVector_t *acceleration, ATMO_3dFloatVector_t *gyro, ATMO_3dFloatVector_t *mag )
{
	if ( acceleration != NULL )
	{
		memcpy( acceleration, &_BMI160_AccData, sizeof( ATMO_3dFloatVector_t ) );
	}

	if ( gyro != NULL )
	{
		memcpy( gyro, &_BMI160_GyroData, sizeof( ATMO_3dFloatVector_t ) );
	}

	if ( mag != NULL )
	{
		memcpy( mag, &_BMI160_MagData, sizeof( ATMO_3dFloatVector_t ) );
	}

	return true;
}


