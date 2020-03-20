
#ifndef ATMO_INTERRUPTS_HANDLER_H
#define ATMO_INTERRUPTS_HANDLER_H

#ifdef __cplusplus
	extern "C"{
#endif

#include "../atmo/core.h"
#define ATMO_INTERRUPT(ELEMENT, NAME) ATMO_ ## ELEMENT ## _INTERRUPT_  ## NAME

void ATMO_EmbeddedBHI160_INTERRUPT_trigger(void *data);
void ATMO_EmbeddedBHI160_INTERRUPT_setup(void *data);
void ATMO_EmbeddedBHI160_INTERRUPT_xAcceleration(void *data);
void ATMO_EmbeddedBHI160_INTERRUPT_yAcceleration(void *data);
void ATMO_EmbeddedBHI160_INTERRUPT_zAcceleration(void *data);
void ATMO_EmbeddedBHI160_INTERRUPT_acceleration(void *data);
void ATMO_EmbeddedBHI160_INTERRUPT_angularRate(void *data);
void ATMO_EmbeddedBHI160_INTERRUPT_xAngularRate(void *data);
void ATMO_EmbeddedBHI160_INTERRUPT_yAngularRate(void *data);
void ATMO_EmbeddedBHI160_INTERRUPT_zAngularRate(void *data);
void ATMO_EmbeddedBHI160_INTERRUPT_orientation(void *data);
void ATMO_EmbeddedBHI160_INTERRUPT_xOrientation(void *data);
void ATMO_EmbeddedBHI160_INTERRUPT_yOrientation(void *data);
void ATMO_EmbeddedBHI160_INTERRUPT_zOrientation(void *data);

void ATMO_Interval_INTERRUPT_trigger(void *data);
void ATMO_Interval_INTERRUPT_setup(void *data);
void ATMO_Interval_INTERRUPT_interval(void *data);

void ATMO_OrientationChar_INTERRUPT_trigger(void *data);
void ATMO_OrientationChar_INTERRUPT_setup(void *data);
void ATMO_OrientationChar_INTERRUPT_setValue(void *data);
void ATMO_OrientationChar_INTERRUPT_written(void *data);
void ATMO_OrientationChar_INTERRUPT_subscibed(void *data);
void ATMO_OrientationChar_INTERRUPT_unsubscribed(void *data);

void ATMO_OrientationPrint_INTERRUPT_trigger(void *data);
void ATMO_OrientationPrint_INTERRUPT_setup(void *data);
void ATMO_OrientationPrint_INTERRUPT_print(void *data);


#ifdef __cplusplus
}
#endif

#endif
