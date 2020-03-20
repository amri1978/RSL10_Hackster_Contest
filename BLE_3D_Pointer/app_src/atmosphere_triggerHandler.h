
#ifndef ATMO_TRIGGER_HANDLER_H
#define ATMO_TRIGGER_HANDLER_H

#include "atmosphere_platform.h"
#include "../atmo/core.h"

#ifdef __cplusplus
	extern "C"{
#endif

#define ATMO_TRIGGER(ELEMENT, NAME) ATMO_ ## ELEMENT ## _TRIGGER_  ## NAME

void ATMO_TriggerHandler(unsigned int triggerHandleId, ATMO_Value_t *value);

#define ATMO_EmbeddedBHI160_TRIGGER_triggered 0x1
#define ATMO_EmbeddedBHI160_TRIGGER_xAccelerationRead 0x2
#define ATMO_EmbeddedBHI160_TRIGGER_yAccelerationRead 0x3
#define ATMO_EmbeddedBHI160_TRIGGER_zAccelerationRead 0x4
#define ATMO_EmbeddedBHI160_TRIGGER_accelerationRead 0x5
#define ATMO_EmbeddedBHI160_TRIGGER_angularRateRead 0x6
#define ATMO_EmbeddedBHI160_TRIGGER_xAngularRateRead 0x7
#define ATMO_EmbeddedBHI160_TRIGGER_yAngularRateRead 0x8
#define ATMO_EmbeddedBHI160_TRIGGER_zAngularRateRead 0x9
#define ATMO_EmbeddedBHI160_TRIGGER_orientationRead 0xa
#define ATMO_EmbeddedBHI160_TRIGGER_xOrientationRead 0xb
#define ATMO_EmbeddedBHI160_TRIGGER_yOrientationRead 0xc
#define ATMO_EmbeddedBHI160_TRIGGER_zOrientationRead 0xd
#define ATMO_Interval_TRIGGER_triggered 0xe
#define ATMO_Interval_TRIGGER_interval 0xf
#define ATMO_OrientationChar_TRIGGER_triggered 0x10
#define ATMO_OrientationChar_TRIGGER_written 0x11
#define ATMO_OrientationChar_TRIGGER_subscibed 0x12
#define ATMO_OrientationChar_TRIGGER_unsubscribed 0x13
#define ATMO_OrientationPrint_TRIGGER_triggered 0x14
#define ATMO_OrientationPrint_TRIGGER_printed 0x15
#ifdef __cplusplus
}
#endif
#endif
