
#ifndef __ATMO_DRIVERINSTANCE__H
#include "../atmo/core.h"

#define ATMO_INSTANCE(NAME, DRIVER) ATMO_DRIVERINSTANCE_ ## DRIVER ## _ ## NAME

static ATMO_DriverInstanceHandle_t ATMO_DRIVERINSTANCE_BLE_BLE1 = 0;
static ATMO_DriverInstanceHandle_t ATMO_DRIVERINSTANCE_BLOCK_BLOCK1 = 0;
static ATMO_DriverInstanceHandle_t ATMO_DRIVERINSTANCE_FILESYSTEM_FILESYSTEM1 = 0;
static ATMO_DriverInstanceHandle_t ATMO_DRIVERINSTANCE_GPIO_GPIO1 = 0;
static ATMO_DriverInstanceHandle_t ATMO_DRIVERINSTANCE_I2C_I2C1 = 0;
static ATMO_DriverInstanceHandle_t ATMO_DRIVERINSTANCE_INTERVAL_INTERVAL1 = 0;

#endif
