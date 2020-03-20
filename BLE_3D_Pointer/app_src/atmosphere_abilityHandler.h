
#ifndef ATMO_ABILITY_HANDLER_H
#define ATMO_ABILITY_HANDLER_H

#include "atmosphere_platform.h"
#include "atmosphere_callbacks.h"

#include "../atmo/core.h"

#ifdef __cplusplus
	extern "C"{
#endif

#define ATMO_ABILITY(ELEMENT, NAME) ATMO_ ## ELEMENT ## _ABILITY_  ## NAME

void ATMO_AbilityHandler(unsigned int abilityHandleId, ATMO_Value_t *value);

#define ATMO_EmbeddedBHI160_ABILITY_trigger 0x1
#define ATMO_EmbeddedBHI160_ABILITY_setup 0x2
#define ATMO_EmbeddedBHI160_ABILITY_xAcceleration 0x3
#define ATMO_EmbeddedBHI160_ABILITY_yAcceleration 0x4
#define ATMO_EmbeddedBHI160_ABILITY_zAcceleration 0x5
#define ATMO_EmbeddedBHI160_ABILITY_acceleration 0x6
#define ATMO_EmbeddedBHI160_ABILITY_angularRate 0x7
#define ATMO_EmbeddedBHI160_ABILITY_xAngularRate 0x8
#define ATMO_EmbeddedBHI160_ABILITY_yAngularRate 0x9
#define ATMO_EmbeddedBHI160_ABILITY_zAngularRate 0xa
#define ATMO_EmbeddedBHI160_ABILITY_orientation 0xb
#define ATMO_EmbeddedBHI160_ABILITY_xOrientation 0xc
#define ATMO_EmbeddedBHI160_ABILITY_yOrientation 0xd
#define ATMO_EmbeddedBHI160_ABILITY_zOrientation 0xe
#define ATMO_Interval_ABILITY_trigger 0xf
#define ATMO_Interval_ABILITY_setup 0x10
#define ATMO_Interval_ABILITY_interval 0x11
#define ATMO_OrientationChar_ABILITY_trigger 0x12
#define ATMO_OrientationChar_ABILITY_setup 0x13
#define ATMO_OrientationChar_ABILITY_setValue 0x14
#define ATMO_OrientationChar_ABILITY_written 0x15
#define ATMO_OrientationChar_ABILITY_subscibed 0x16
#define ATMO_OrientationChar_ABILITY_unsubscribed 0x17
#define ATMO_OrientationPrint_ABILITY_trigger 0x18
#define ATMO_OrientationPrint_ABILITY_setup 0x19
#define ATMO_OrientationPrint_ABILITY_print 0x1a
#ifdef __cplusplus
}
#endif
#endif
