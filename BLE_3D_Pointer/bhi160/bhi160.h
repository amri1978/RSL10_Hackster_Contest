#ifndef _BHI160_H_
#define _BHI160_H_

#include "../app_src/atmosphere_platform.h"

typedef struct
{
	ATMO_DriverInstanceHandle_t i2cInstance;
	ATMO_DriverInstanceHandle_t gpioInstance;
	ATMO_GPIO_Device_Pin_t intPin;
} BHI160_Config_t;

ATMO_BOOL_t BHI160_Init( BHI160_Config_t *config );
ATMO_BOOL_t BHI160_GetData( ATMO_3dFloatVector_t *acceleration, ATMO_3dFloatVector_t *gyro, ATMO_3dFloatVector_t *mag );

#endif
