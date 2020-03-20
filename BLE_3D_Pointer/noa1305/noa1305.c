#include "noa1305.h"
#include "noa1305_onsemi.h"

static ATMO_DriverInstanceHandle_t _NOA1305_I2CInstance = 0;
static ATMO_BOOL_t _NOA1305_Connected = false;
static struct noa1305_t _NOA1305_Dev;

#define NOA1305_DEVICE_ID		0x0519
#define NOA1305_REG_DEVICE_ID_LSB	0x08
#define NOA1305_REG_DEVICE_ID_MSB	0x09

#define NOA1305_INTEG_TIME_200MS 2

#define NOA1305_I2C_ADDR 0x39

static int32_t _NOA1305_BusRead(uint8_t dev_id, uint8_t addr, uint8_t *value)
{
    return ATMO_I2C_MasterRead(_NOA1305_I2CInstance, dev_id, &addr, 1, value, 1, 0);
}

static int32_t _NOA1305_BusWrite(uint8_t dev_id, uint8_t addr, uint8_t value)
{
    uint8_t data[2] = { addr, value };

    return ATMO_I2C_MasterWrite(_NOA1305_I2CInstance, dev_id, NULL, 0, data, 2, 0);
}

ATMO_BOOL_t NOA1305_Init( ATMO_DriverInstanceHandle_t i2cInstance )
{
	_NOA1305_Dev.id = NOA1305_I2C_ADDR;
	_NOA1305_Dev.integration_constatnt = 7700;

	_NOA1305_Dev.read_func = &_NOA1305_BusRead;
	_NOA1305_Dev.write_func = &_NOA1305_BusWrite;
	_NOA1305_Dev.delay_func = &ATMO_PLATFORM_DelayMilliseconds;

    // Initialize and verify device ID
	int retval = noa1305_init( &_NOA1305_Dev );

	if ( retval == NOA1305_OK )
	{
		retval = noa1305_set_int_select( NOA1305_INT_INACTIVE, &_NOA1305_Dev );

		if ( retval == NOA1305_OK )
		{
			retval = noa1305_set_integration_time( NOA1305_INTEG_TIME_200MS, &_NOA1305_Dev );

            if(retval == NOA1305_OK)
            {
                // Set to continuous mode for now
                retval = noa1305_set_power_mode(NOA1305_POWER_ON, &_NOA1305_Dev);
            }
		}
	}

    return (retval == NOA1305_OK);
}

ATMO_BOOL_t NOA1305_GetAmbientLight( uint32_t *lightLux )
{
    if( noa1305_convert_als_data_lux(lightLux, &_NOA1305_Dev) != NOA1305_OK )
    {
        return false;
    }

    return true;
}
