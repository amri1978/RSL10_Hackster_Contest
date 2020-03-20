#include "atmosphere_interruptsHandler.h"
#include "atmosphere_abilityHandler.h"


#ifdef __cplusplus
extern "C"{
#endif
void ATMO_EmbeddedBHI160_INTERRUPT_trigger(void *data) {
	ATMO_AddAbilityExecute(ATMO_ABILITY(EmbeddedBHI160, trigger), (ATMO_Value_t *)data);
}
void ATMO_EmbeddedBHI160_INTERRUPT_setup(void *data) {
	ATMO_AddAbilityExecute(ATMO_ABILITY(EmbeddedBHI160, setup), (ATMO_Value_t *)data);
}
void ATMO_EmbeddedBHI160_INTERRUPT_xAcceleration(void *data) {
	ATMO_AddAbilityExecute(ATMO_ABILITY(EmbeddedBHI160, xAcceleration), (ATMO_Value_t *)data);
}
void ATMO_EmbeddedBHI160_INTERRUPT_yAcceleration(void *data) {
	ATMO_AddAbilityExecute(ATMO_ABILITY(EmbeddedBHI160, yAcceleration), (ATMO_Value_t *)data);
}
void ATMO_EmbeddedBHI160_INTERRUPT_zAcceleration(void *data) {
	ATMO_AddAbilityExecute(ATMO_ABILITY(EmbeddedBHI160, zAcceleration), (ATMO_Value_t *)data);
}
void ATMO_EmbeddedBHI160_INTERRUPT_acceleration(void *data) {
	ATMO_AddAbilityExecute(ATMO_ABILITY(EmbeddedBHI160, acceleration), (ATMO_Value_t *)data);
}
void ATMO_EmbeddedBHI160_INTERRUPT_angularRate(void *data) {
	ATMO_AddAbilityExecute(ATMO_ABILITY(EmbeddedBHI160, angularRate), (ATMO_Value_t *)data);
}
void ATMO_EmbeddedBHI160_INTERRUPT_xAngularRate(void *data) {
	ATMO_AddAbilityExecute(ATMO_ABILITY(EmbeddedBHI160, xAngularRate), (ATMO_Value_t *)data);
}
void ATMO_EmbeddedBHI160_INTERRUPT_yAngularRate(void *data) {
	ATMO_AddAbilityExecute(ATMO_ABILITY(EmbeddedBHI160, yAngularRate), (ATMO_Value_t *)data);
}
void ATMO_EmbeddedBHI160_INTERRUPT_zAngularRate(void *data) {
	ATMO_AddAbilityExecute(ATMO_ABILITY(EmbeddedBHI160, zAngularRate), (ATMO_Value_t *)data);
}
void ATMO_EmbeddedBHI160_INTERRUPT_orientation(void *data) {
	ATMO_AddAbilityExecute(ATMO_ABILITY(EmbeddedBHI160, orientation), (ATMO_Value_t *)data);
}
void ATMO_EmbeddedBHI160_INTERRUPT_xOrientation(void *data) {
	ATMO_AddAbilityExecute(ATMO_ABILITY(EmbeddedBHI160, xOrientation), (ATMO_Value_t *)data);
}
void ATMO_EmbeddedBHI160_INTERRUPT_yOrientation(void *data) {
	ATMO_AddAbilityExecute(ATMO_ABILITY(EmbeddedBHI160, yOrientation), (ATMO_Value_t *)data);
}
void ATMO_EmbeddedBHI160_INTERRUPT_zOrientation(void *data) {
	ATMO_AddAbilityExecute(ATMO_ABILITY(EmbeddedBHI160, zOrientation), (ATMO_Value_t *)data);
}
void ATMO_Interval_INTERRUPT_trigger(void *data) {
	ATMO_AddAbilityExecute(ATMO_ABILITY(Interval, trigger), (ATMO_Value_t *)data);
}
void ATMO_Interval_INTERRUPT_setup(void *data) {
	ATMO_AddAbilityExecute(ATMO_ABILITY(Interval, setup), (ATMO_Value_t *)data);
}
void ATMO_Interval_INTERRUPT_interval(void *data) {
	ATMO_AddAbilityExecute(ATMO_ABILITY(Interval, interval), (ATMO_Value_t *)data);
}
void ATMO_OrientationChar_INTERRUPT_trigger(void *data) {
	ATMO_AddAbilityExecute(ATMO_ABILITY(OrientationChar, trigger), (ATMO_Value_t *)data);
}
void ATMO_OrientationChar_INTERRUPT_setup(void *data) {
	ATMO_AddAbilityExecute(ATMO_ABILITY(OrientationChar, setup), (ATMO_Value_t *)data);
}
void ATMO_OrientationChar_INTERRUPT_setValue(void *data) {
	ATMO_AddAbilityExecute(ATMO_ABILITY(OrientationChar, setValue), (ATMO_Value_t *)data);
}
void ATMO_OrientationChar_INTERRUPT_written(void *data) {
	ATMO_AddAbilityExecute(ATMO_ABILITY(OrientationChar, written), (ATMO_Value_t *)data);
}
void ATMO_OrientationChar_INTERRUPT_subscibed(void *data) {
	ATMO_AddAbilityExecute(ATMO_ABILITY(OrientationChar, subscibed), (ATMO_Value_t *)data);
}
void ATMO_OrientationChar_INTERRUPT_unsubscribed(void *data) {
	ATMO_AddAbilityExecute(ATMO_ABILITY(OrientationChar, unsubscribed), (ATMO_Value_t *)data);
}
void ATMO_OrientationPrint_INTERRUPT_trigger(void *data) {
	ATMO_AddAbilityExecute(ATMO_ABILITY(OrientationPrint, trigger), (ATMO_Value_t *)data);
}
void ATMO_OrientationPrint_INTERRUPT_setup(void *data) {
	ATMO_AddAbilityExecute(ATMO_ABILITY(OrientationPrint, setup), (ATMO_Value_t *)data);
}
void ATMO_OrientationPrint_INTERRUPT_print(void *data) {
	ATMO_AddAbilityExecute(ATMO_ABILITY(OrientationPrint, print), (ATMO_Value_t *)data);
}

#ifdef __cplusplus
}
#endif
	