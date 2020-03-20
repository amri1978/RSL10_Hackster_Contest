
#ifndef ATMO_CALLBACKS_H
#define ATMO_CALLBACKS_H

#include "atmosphere_platform.h"
#include "atmosphere_properties.h"
#include "atmosphere_variables.h"
#include "atmosphere_triggerHandler.h"
#include "atmosphere_interruptsHandler.h"
#include "atmosphere_embedded_libraries.h"
#include "atmosphere_abilityHandler.h"

#include "atmosphere_driverinstance.h"

#include "atmosphere_cloudcommands.h"

#include "atmosphere_elementnames.h"

#define ATMO_CALLBACK(ELEMENT, NAME) ELEMENT ## _ ## NAME

void ATMO_Setup();

ATMO_Status_t EmbeddedBHI160_trigger(ATMO_Value_t *in, ATMO_Value_t *out);

ATMO_Status_t EmbeddedBHI160_setup(ATMO_Value_t *in, ATMO_Value_t *out);

ATMO_Status_t EmbeddedBHI160_xAcceleration(ATMO_Value_t *in, ATMO_Value_t *out);

ATMO_Status_t EmbeddedBHI160_yAcceleration(ATMO_Value_t *in, ATMO_Value_t *out);

ATMO_Status_t EmbeddedBHI160_zAcceleration(ATMO_Value_t *in, ATMO_Value_t *out);

ATMO_Status_t EmbeddedBHI160_acceleration(ATMO_Value_t *in, ATMO_Value_t *out);

ATMO_Status_t EmbeddedBHI160_angularRate(ATMO_Value_t *in, ATMO_Value_t *out);

ATMO_Status_t EmbeddedBHI160_xAngularRate(ATMO_Value_t *in, ATMO_Value_t *out);

ATMO_Status_t EmbeddedBHI160_yAngularRate(ATMO_Value_t *in, ATMO_Value_t *out);

ATMO_Status_t EmbeddedBHI160_zAngularRate(ATMO_Value_t *in, ATMO_Value_t *out);

ATMO_Status_t EmbeddedBHI160_orientation(ATMO_Value_t *in, ATMO_Value_t *out);

ATMO_Status_t EmbeddedBHI160_xOrientation(ATMO_Value_t *in, ATMO_Value_t *out);

ATMO_Status_t EmbeddedBHI160_yOrientation(ATMO_Value_t *in, ATMO_Value_t *out);

ATMO_Status_t EmbeddedBHI160_zOrientation(ATMO_Value_t *in, ATMO_Value_t *out);

ATMO_Status_t Interval_trigger(ATMO_Value_t *in, ATMO_Value_t *out);

ATMO_Status_t Interval_setup(ATMO_Value_t *in, ATMO_Value_t *out);

ATMO_Status_t Interval_interval(ATMO_Value_t *in, ATMO_Value_t *out);

ATMO_Status_t OrientationChar_trigger(ATMO_Value_t *in, ATMO_Value_t *out);

ATMO_Status_t OrientationChar_setup(ATMO_Value_t *in, ATMO_Value_t *out);

ATMO_Status_t OrientationChar_setValue(ATMO_Value_t *in, ATMO_Value_t *out);

ATMO_Status_t OrientationChar_written(ATMO_Value_t *in, ATMO_Value_t *out);

ATMO_Status_t OrientationChar_subscibed(ATMO_Value_t *in, ATMO_Value_t *out);

ATMO_Status_t OrientationChar_unsubscribed(ATMO_Value_t *in, ATMO_Value_t *out);

ATMO_Status_t OrientationPrint_trigger(ATMO_Value_t *in, ATMO_Value_t *out);

ATMO_Status_t OrientationPrint_setup(ATMO_Value_t *in, ATMO_Value_t *out);

ATMO_Status_t OrientationPrint_print(ATMO_Value_t *in, ATMO_Value_t *out);
#endif
