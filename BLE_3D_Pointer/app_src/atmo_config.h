#ifndef _ATMO_CONFIG_H_
#define _ATMO_CONFIG_H_

/**
 *  Static core configuration
 *
 *  If this is defined, the Nimbus Core will not use malloc or free. Given the risks of dynamic allocation on embedded systems, this is generally a good idea.
 */
#define ATMO_STATIC_CORE
#define ATMO_STATIC_SIZE 32

#define ATMO_SLIM_STACK

/* Use custom fast sqrt implementation instead of built in sqrt */
/* this saves a little bit of flash space */
#define ATMO_FAST_SQRT

/* BLE stack handles this characteristic itself */
#define ATMO_BLE_NO_SERVICE_CHANGED

#define NUMBER_OF_CLOUD_DRIVER_INSTANCES 1

// #define ATMO_NO_SPRINTF_FLOAT_SUPPORT

// #define ATMO_NO_DTOSTRF_SUPPORT

#define ATMO_PLATFORM_NO_SQRT

#endif
