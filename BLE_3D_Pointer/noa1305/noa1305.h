#ifndef _NOA1305_H_
#define _NOA1305_H_

#include "../app_src/atmosphere_platform.h"

ATMO_BOOL_t NOA1305_Init(ATMO_DriverInstanceHandle_t i2cInstance);

ATMO_BOOL_t NOA1305_GetAmbientLight(uint32_t *lightLux);

#endif