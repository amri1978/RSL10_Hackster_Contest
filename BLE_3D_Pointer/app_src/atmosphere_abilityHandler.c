#include "atmosphere_abilityHandler.h"
#include "atmosphere_triggerHandler.h"

#ifdef __cplusplus
	extern "C"{
#endif

void ATMO_AbilityHandler(unsigned int abilityHandleId, ATMO_Value_t *value) {
	switch(abilityHandleId) {
		case ATMO_ABILITY(EmbeddedBHI160, trigger):
		{
			ATMO_Value_t EmbeddedBHI160_Value;
			ATMO_InitValue(&EmbeddedBHI160_Value);
			EmbeddedBHI160_trigger(value, &EmbeddedBHI160_Value);
			ATMO_TriggerHandler(ATMO_TRIGGER(EmbeddedBHI160, triggered), &EmbeddedBHI160_Value);
			ATMO_FreeValue(&EmbeddedBHI160_Value);
			break;
		}
		case ATMO_ABILITY(EmbeddedBHI160, setup):
		{
			ATMO_Value_t EmbeddedBHI160_Value;
			ATMO_InitValue(&EmbeddedBHI160_Value);
			EmbeddedBHI160_setup(value, &EmbeddedBHI160_Value);
			ATMO_FreeValue(&EmbeddedBHI160_Value);
			break;
		}
		case ATMO_ABILITY(EmbeddedBHI160, xAcceleration):
		{
			ATMO_Value_t EmbeddedBHI160_Value;
			ATMO_InitValue(&EmbeddedBHI160_Value);
			EmbeddedBHI160_xAcceleration(value, &EmbeddedBHI160_Value);
			ATMO_TriggerHandler(ATMO_TRIGGER(EmbeddedBHI160, xAccelerationRead), &EmbeddedBHI160_Value);
			ATMO_FreeValue(&EmbeddedBHI160_Value);
			break;
		}
		case ATMO_ABILITY(EmbeddedBHI160, yAcceleration):
		{
			ATMO_Value_t EmbeddedBHI160_Value;
			ATMO_InitValue(&EmbeddedBHI160_Value);
			EmbeddedBHI160_yAcceleration(value, &EmbeddedBHI160_Value);
			ATMO_TriggerHandler(ATMO_TRIGGER(EmbeddedBHI160, yAccelerationRead), &EmbeddedBHI160_Value);
			ATMO_FreeValue(&EmbeddedBHI160_Value);
			break;
		}
		case ATMO_ABILITY(EmbeddedBHI160, zAcceleration):
		{
			ATMO_Value_t EmbeddedBHI160_Value;
			ATMO_InitValue(&EmbeddedBHI160_Value);
			EmbeddedBHI160_zAcceleration(value, &EmbeddedBHI160_Value);
			ATMO_TriggerHandler(ATMO_TRIGGER(EmbeddedBHI160, zAccelerationRead), &EmbeddedBHI160_Value);
			ATMO_FreeValue(&EmbeddedBHI160_Value);
			break;
		}
		case ATMO_ABILITY(EmbeddedBHI160, acceleration):
		{
			ATMO_Value_t EmbeddedBHI160_Value;
			ATMO_InitValue(&EmbeddedBHI160_Value);
			EmbeddedBHI160_acceleration(value, &EmbeddedBHI160_Value);
			ATMO_TriggerHandler(ATMO_TRIGGER(EmbeddedBHI160, accelerationRead), &EmbeddedBHI160_Value);
			ATMO_FreeValue(&EmbeddedBHI160_Value);
			break;
		}
		case ATMO_ABILITY(EmbeddedBHI160, angularRate):
		{
			ATMO_Value_t EmbeddedBHI160_Value;
			ATMO_InitValue(&EmbeddedBHI160_Value);
			EmbeddedBHI160_angularRate(value, &EmbeddedBHI160_Value);
			ATMO_TriggerHandler(ATMO_TRIGGER(EmbeddedBHI160, angularRateRead), &EmbeddedBHI160_Value);
			ATMO_FreeValue(&EmbeddedBHI160_Value);
			break;
		}
		case ATMO_ABILITY(EmbeddedBHI160, xAngularRate):
		{
			ATMO_Value_t EmbeddedBHI160_Value;
			ATMO_InitValue(&EmbeddedBHI160_Value);
			EmbeddedBHI160_xAngularRate(value, &EmbeddedBHI160_Value);
			ATMO_TriggerHandler(ATMO_TRIGGER(EmbeddedBHI160, xAngularRateRead), &EmbeddedBHI160_Value);
			ATMO_FreeValue(&EmbeddedBHI160_Value);
			break;
		}
		case ATMO_ABILITY(EmbeddedBHI160, yAngularRate):
		{
			ATMO_Value_t EmbeddedBHI160_Value;
			ATMO_InitValue(&EmbeddedBHI160_Value);
			EmbeddedBHI160_yAngularRate(value, &EmbeddedBHI160_Value);
			ATMO_TriggerHandler(ATMO_TRIGGER(EmbeddedBHI160, yAngularRateRead), &EmbeddedBHI160_Value);
			ATMO_FreeValue(&EmbeddedBHI160_Value);
			break;
		}
		case ATMO_ABILITY(EmbeddedBHI160, zAngularRate):
		{
			ATMO_Value_t EmbeddedBHI160_Value;
			ATMO_InitValue(&EmbeddedBHI160_Value);
			EmbeddedBHI160_zAngularRate(value, &EmbeddedBHI160_Value);
			ATMO_TriggerHandler(ATMO_TRIGGER(EmbeddedBHI160, zAngularRateRead), &EmbeddedBHI160_Value);
			ATMO_FreeValue(&EmbeddedBHI160_Value);
			break;
		}
		case ATMO_ABILITY(EmbeddedBHI160, orientation):
		{
			ATMO_Value_t EmbeddedBHI160_Value;
			ATMO_InitValue(&EmbeddedBHI160_Value);
			EmbeddedBHI160_orientation(value, &EmbeddedBHI160_Value);
			ATMO_TriggerHandler(ATMO_TRIGGER(EmbeddedBHI160, orientationRead), &EmbeddedBHI160_Value);
			ATMO_FreeValue(&EmbeddedBHI160_Value);
			break;
		}
		case ATMO_ABILITY(EmbeddedBHI160, xOrientation):
		{
			ATMO_Value_t EmbeddedBHI160_Value;
			ATMO_InitValue(&EmbeddedBHI160_Value);
			EmbeddedBHI160_xOrientation(value, &EmbeddedBHI160_Value);
			ATMO_TriggerHandler(ATMO_TRIGGER(EmbeddedBHI160, xOrientationRead), &EmbeddedBHI160_Value);
			ATMO_FreeValue(&EmbeddedBHI160_Value);
			break;
		}
		case ATMO_ABILITY(EmbeddedBHI160, yOrientation):
		{
			ATMO_Value_t EmbeddedBHI160_Value;
			ATMO_InitValue(&EmbeddedBHI160_Value);
			EmbeddedBHI160_yOrientation(value, &EmbeddedBHI160_Value);
			ATMO_TriggerHandler(ATMO_TRIGGER(EmbeddedBHI160, yOrientationRead), &EmbeddedBHI160_Value);
			ATMO_FreeValue(&EmbeddedBHI160_Value);
			break;
		}
		case ATMO_ABILITY(EmbeddedBHI160, zOrientation):
		{
			ATMO_Value_t EmbeddedBHI160_Value;
			ATMO_InitValue(&EmbeddedBHI160_Value);
			EmbeddedBHI160_zOrientation(value, &EmbeddedBHI160_Value);
			ATMO_TriggerHandler(ATMO_TRIGGER(EmbeddedBHI160, zOrientationRead), &EmbeddedBHI160_Value);
			ATMO_FreeValue(&EmbeddedBHI160_Value);
			break;
		}
		case ATMO_ABILITY(Interval, trigger):
		{
			ATMO_Value_t Interval_Value;
			ATMO_InitValue(&Interval_Value);
			Interval_trigger(value, &Interval_Value);
			ATMO_TriggerHandler(ATMO_TRIGGER(Interval, triggered), &Interval_Value);
			ATMO_FreeValue(&Interval_Value);
			break;
		}
		case ATMO_ABILITY(Interval, setup):
		{
			ATMO_Value_t Interval_Value;
			ATMO_InitValue(&Interval_Value);
			Interval_setup(value, &Interval_Value);
			ATMO_FreeValue(&Interval_Value);
			break;
		}
		case ATMO_ABILITY(Interval, interval):
		{
			ATMO_Value_t Interval_Value;
			ATMO_InitValue(&Interval_Value);
			Interval_interval(value, &Interval_Value);
			ATMO_TriggerHandler(ATMO_TRIGGER(Interval, interval), &Interval_Value);
			ATMO_FreeValue(&Interval_Value);
			break;
		}
		case ATMO_ABILITY(OrientationChar, trigger):
		{
			ATMO_Value_t OrientationChar_Value;
			ATMO_InitValue(&OrientationChar_Value);
			OrientationChar_trigger(value, &OrientationChar_Value);
			ATMO_TriggerHandler(ATMO_TRIGGER(OrientationChar, triggered), &OrientationChar_Value);
			ATMO_FreeValue(&OrientationChar_Value);
			break;
		}
		case ATMO_ABILITY(OrientationChar, setup):
		{
			ATMO_Value_t OrientationChar_Value;
			ATMO_InitValue(&OrientationChar_Value);
			OrientationChar_setup(value, &OrientationChar_Value);
			ATMO_FreeValue(&OrientationChar_Value);
			break;
		}
		case ATMO_ABILITY(OrientationChar, setValue):
		{
			ATMO_Value_t OrientationChar_Value;
			ATMO_InitValue(&OrientationChar_Value);
			OrientationChar_setValue(value, &OrientationChar_Value);
			ATMO_FreeValue(&OrientationChar_Value);
			break;
		}
		case ATMO_ABILITY(OrientationChar, written):
		{
			ATMO_Value_t OrientationChar_Value;
			ATMO_InitValue(&OrientationChar_Value);
			OrientationChar_written(value, &OrientationChar_Value);
			ATMO_TriggerHandler(ATMO_TRIGGER(OrientationChar, written), &OrientationChar_Value);
			ATMO_FreeValue(&OrientationChar_Value);
			break;
		}
		case ATMO_ABILITY(OrientationChar, subscibed):
		{
			ATMO_Value_t OrientationChar_Value;
			ATMO_InitValue(&OrientationChar_Value);
			OrientationChar_subscibed(value, &OrientationChar_Value);
			ATMO_TriggerHandler(ATMO_TRIGGER(OrientationChar, subscibed), &OrientationChar_Value);
			ATMO_FreeValue(&OrientationChar_Value);
			break;
		}
		case ATMO_ABILITY(OrientationChar, unsubscribed):
		{
			ATMO_Value_t OrientationChar_Value;
			ATMO_InitValue(&OrientationChar_Value);
			OrientationChar_unsubscribed(value, &OrientationChar_Value);
			ATMO_TriggerHandler(ATMO_TRIGGER(OrientationChar, unsubscribed), &OrientationChar_Value);
			ATMO_FreeValue(&OrientationChar_Value);
			break;
		}
		case ATMO_ABILITY(OrientationPrint, trigger):
		{
			ATMO_Value_t OrientationPrint_Value;
			ATMO_InitValue(&OrientationPrint_Value);
			OrientationPrint_trigger(value, &OrientationPrint_Value);
			ATMO_TriggerHandler(ATMO_TRIGGER(OrientationPrint, triggered), &OrientationPrint_Value);
			ATMO_FreeValue(&OrientationPrint_Value);
			break;
		}
		case ATMO_ABILITY(OrientationPrint, setup):
		{
			ATMO_Value_t OrientationPrint_Value;
			ATMO_InitValue(&OrientationPrint_Value);
			OrientationPrint_setup(value, &OrientationPrint_Value);
			ATMO_FreeValue(&OrientationPrint_Value);
			break;
		}
		case ATMO_ABILITY(OrientationPrint, print):
		{
			ATMO_Value_t OrientationPrint_Value;
			ATMO_InitValue(&OrientationPrint_Value);
			OrientationPrint_print(value, &OrientationPrint_Value);
			ATMO_TriggerHandler(ATMO_TRIGGER(OrientationPrint, printed), &OrientationPrint_Value);
			ATMO_FreeValue(&OrientationPrint_Value);
			break;
		}
	}

}

#ifdef __cplusplus
}
#endif
