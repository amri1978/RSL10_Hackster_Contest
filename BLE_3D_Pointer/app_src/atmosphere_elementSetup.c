#include "atmosphere_elementSetup.h"

#ifdef __cplusplus
	extern "C"{
#endif

void ATMO_ElementSetup() {
	ATMO_Value_t inOutValue;
	ATMO_InitValue(&inOutValue);
	EmbeddedBHI160_setup(&inOutValue, &inOutValue);
	Interval_setup(&inOutValue, &inOutValue);
	OrientationChar_setup(&inOutValue, &inOutValue);
	OrientationPrint_setup(&inOutValue, &inOutValue);
}

#ifdef __cplusplus
}
#endif
