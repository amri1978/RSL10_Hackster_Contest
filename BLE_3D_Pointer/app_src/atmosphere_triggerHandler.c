#include "atmosphere_triggerHandler.h"
#include "atmosphere_abilityHandler.h"

#ifdef __cplusplus
	extern "C"{
#endif

void ATMO_TriggerHandler(unsigned int triggerHandleId, ATMO_Value_t *value) {
	switch(triggerHandleId) {
		case ATMO_TRIGGER(EmbeddedBHI160, triggered):
		{
			break;
		}

		case ATMO_TRIGGER(EmbeddedBHI160, xAccelerationRead):
		{
			break;
		}

		case ATMO_TRIGGER(EmbeddedBHI160, yAccelerationRead):
		{
			break;
		}

		case ATMO_TRIGGER(EmbeddedBHI160, zAccelerationRead):
		{
			break;
		}

		case ATMO_TRIGGER(EmbeddedBHI160, accelerationRead):
		{
			break;
		}

		case ATMO_TRIGGER(EmbeddedBHI160, angularRateRead):
		{
			break;
		}

		case ATMO_TRIGGER(EmbeddedBHI160, xAngularRateRead):
		{
			break;
		}

		case ATMO_TRIGGER(EmbeddedBHI160, yAngularRateRead):
		{
			break;
		}

		case ATMO_TRIGGER(EmbeddedBHI160, zAngularRateRead):
		{
			break;
		}

		case ATMO_TRIGGER(EmbeddedBHI160, orientationRead):
		{
			ATMO_AbilityHandler(ATMO_ABILITY(OrientationChar, setValue), value);
			ATMO_AbilityHandler(ATMO_ABILITY(OrientationPrint, print), value);
			break;
		}

		case ATMO_TRIGGER(EmbeddedBHI160, xOrientationRead):
		{
			break;
		}

		case ATMO_TRIGGER(EmbeddedBHI160, yOrientationRead):
		{
			break;
		}

		case ATMO_TRIGGER(EmbeddedBHI160, zOrientationRead):
		{
			break;
		}

		case ATMO_TRIGGER(Interval, triggered):
		{
			break;
		}

		case ATMO_TRIGGER(Interval, interval):
		{
			ATMO_AbilityHandler(ATMO_ABILITY(EmbeddedBHI160, orientation), value);
			break;
		}

		case ATMO_TRIGGER(OrientationChar, triggered):
		{
			break;
		}

		case ATMO_TRIGGER(OrientationChar, written):
		{
			break;
		}

		case ATMO_TRIGGER(OrientationChar, subscibed):
		{
			break;
		}

		case ATMO_TRIGGER(OrientationChar, unsubscribed):
		{
			break;
		}

		case ATMO_TRIGGER(OrientationPrint, triggered):
		{
			break;
		}

		case ATMO_TRIGGER(OrientationPrint, printed):
		{
			break;
		}

	}

}

#ifdef __cplusplus
}
#endif
