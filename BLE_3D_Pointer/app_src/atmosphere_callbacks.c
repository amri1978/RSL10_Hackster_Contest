#include "atmosphere_callbacks.h"

//HEADER START

//HEADER END

void ATMO_Setup() {

}


ATMO_Status_t EmbeddedBHI160_trigger(ATMO_Value_t *in, ATMO_Value_t *out) {
	return ATMO_Status_Success;
}


ATMO_Status_t EmbeddedBHI160_setup(ATMO_Value_t *in, ATMO_Value_t *out) {
    BHI160_Config_t config;
    config.i2cInstance = ATMO_PROPERTY(EmbeddedBHI160, i2cInstance);
    config.gpioInstance = ATMO_PROPERTY(EmbeddedBHI160, gpioInstance);
    config.intPin = ATMO_PROPERTY(EmbeddedBHI160, intPin);

    if(!BHI160_Init(&config))
    {
        return ATMO_Status_Fail;
    }
    return ATMO_Status_Success;
}


ATMO_Status_t EmbeddedBHI160_xAcceleration(ATMO_Value_t *in, ATMO_Value_t *out) {
    ATMO_3dFloatVector_t data;
    BHI160_GetData(&data, NULL, NULL);
    ATMO_CreateValueFloat(out, data.x);
    return ATMO_Status_Success;
}


ATMO_Status_t EmbeddedBHI160_yAcceleration(ATMO_Value_t *in, ATMO_Value_t *out) {
    ATMO_3dFloatVector_t data;
    BHI160_GetData(&data, NULL, NULL);
    ATMO_CreateValueFloat(out, data.y);
    return ATMO_Status_Success;
}


ATMO_Status_t EmbeddedBHI160_zAcceleration(ATMO_Value_t *in, ATMO_Value_t *out) {
    ATMO_3dFloatVector_t data;
    BHI160_GetData(&data, NULL, NULL);
    ATMO_CreateValueFloat(out, data.z);
    return ATMO_Status_Success;
}


ATMO_Status_t EmbeddedBHI160_acceleration(ATMO_Value_t *in, ATMO_Value_t *out) {
    ATMO_3dFloatVector_t data;
    BHI160_GetData(&data, NULL, NULL);
    ATMO_CreateValue3dVectorFloat(out, &data);
    return ATMO_Status_Success;
}


ATMO_Status_t EmbeddedBHI160_angularRate(ATMO_Value_t *in, ATMO_Value_t *out) {
    ATMO_3dFloatVector_t data;
    BHI160_GetData(NULL, &data, NULL);
    ATMO_CreateValue3dVectorFloat(out, &data);
    return ATMO_Status_Success;
}


ATMO_Status_t EmbeddedBHI160_xAngularRate(ATMO_Value_t *in, ATMO_Value_t *out) {
    ATMO_3dFloatVector_t data;
    BHI160_GetData(NULL, &data, NULL);
    ATMO_CreateValueFloat(out, data.x);
    return ATMO_Status_Success;
}


ATMO_Status_t EmbeddedBHI160_yAngularRate(ATMO_Value_t *in, ATMO_Value_t *out) {
    ATMO_3dFloatVector_t data;
    BHI160_GetData(NULL, &data, NULL);
    ATMO_CreateValueFloat(out, data.y);
    return ATMO_Status_Success;
}


ATMO_Status_t EmbeddedBHI160_zAngularRate(ATMO_Value_t *in, ATMO_Value_t *out) {
    ATMO_3dFloatVector_t data;
    BHI160_GetData(NULL, &data, NULL);
    ATMO_CreateValueFloat(out, data.z);
    return ATMO_Status_Success;
}


ATMO_Status_t EmbeddedBHI160_orientation(ATMO_Value_t *in, ATMO_Value_t *out) {
    ATMO_3dFloatVector_t data;
    BHI160_GetData(NULL, NULL, &data);
    ATMO_CreateValue3dVectorFloat(out, &data);
    return ATMO_Status_Success;
}


ATMO_Status_t EmbeddedBHI160_xOrientation(ATMO_Value_t *in, ATMO_Value_t *out) {
    ATMO_3dFloatVector_t data;
    BHI160_GetData(NULL, NULL, &data);
    ATMO_CreateValueFloat(out, data.x);
    return ATMO_Status_Success;
}


ATMO_Status_t EmbeddedBHI160_yOrientation(ATMO_Value_t *in, ATMO_Value_t *out) {
    ATMO_3dFloatVector_t data;
    BHI160_GetData(NULL, NULL, &data);
    ATMO_CreateValueFloat(out, data.y);
    return ATMO_Status_Success;
}


ATMO_Status_t EmbeddedBHI160_zOrientation(ATMO_Value_t *in, ATMO_Value_t *out) {
    ATMO_3dFloatVector_t data;
    BHI160_GetData(NULL, NULL, &data);
    ATMO_CreateValueFloat(out, data.z);
    return ATMO_Status_Success;
}


ATMO_Status_t Interval_trigger(ATMO_Value_t *in, ATMO_Value_t *out) {
	return ATMO_Status_Success;
}


ATMO_Status_t Interval_setup(ATMO_Value_t *in, ATMO_Value_t *out) {

	ATMO_INTERVAL_Handle_t intervalHandle;
    ATMO_INTERVAL_AddAbilityInterval(
		ATMO_PROPERTY(Interval, instance), 
		ATMO_ABILITY(Interval, interval), 
		ATMO_PROPERTY(Interval, time), 
		&intervalHandle
	);
	
	return ATMO_Status_Success;
	
}


ATMO_Status_t Interval_interval(ATMO_Value_t *in, ATMO_Value_t *out) {
	return ATMO_Status_Success;
}


ATMO_Status_t OrientationChar_trigger(ATMO_Value_t *in, ATMO_Value_t *out) {
	return ATMO_Status_Success;
}


ATMO_Status_t OrientationChar_setup(ATMO_Value_t *in, ATMO_Value_t *out) {

	ATMO_BLE_GATTSAddService(
		ATMO_PROPERTY(OrientationChar, instance),
		&ATMO_VARIABLE(OrientationChar, bleServiceHandle), 
		ATMO_PROPERTY(OrientationChar, bleServiceUuid));
	
	uint8_t property = 0;
	uint8_t permission = 0;
	
	property |= ATMO_PROPERTY(OrientationChar, read) ? ATMO_BLE_Property_Read : 0;
	property |= ATMO_PROPERTY(OrientationChar, write) ? ATMO_BLE_Property_Write : 0;
	property |= ATMO_PROPERTY(OrientationChar, notify) ? ATMO_BLE_Property_Notify : 0;

	permission |= ATMO_PROPERTY(OrientationChar, read) ? ATMO_BLE_Permission_Read : 0;
	permission |= ATMO_PROPERTY(OrientationChar, write) ? ATMO_BLE_Permission_Write : 0;

	ATMO_DATATYPE types[3] = {ATMO_PROPERTY(OrientationChar, writeDataType), ATMO_PROPERTY(OrientationChar, readDataType), ATMO_PROPERTY(OrientationChar, notifyDataType)};
	
	ATMO_BLE_GATTSAddCharacteristic(
		ATMO_PROPERTY(OrientationChar, instance),
		&ATMO_VARIABLE(OrientationChar, bleCharacteristicHandle), 
		ATMO_VARIABLE(OrientationChar, bleServiceHandle), 
		ATMO_PROPERTY(OrientationChar, bleCharacteristicUuid), 
		property, permission, ATMO_GetMaxValueSize(3, 64, types));
	
	ATMO_BLE_GATTSRegisterCharacteristicAbilityHandle(
		ATMO_PROPERTY(OrientationChar, instance),
		ATMO_VARIABLE(OrientationChar, bleCharacteristicHandle), 
		ATMO_BLE_Characteristic_Written, 
		ATMO_ABILITY(OrientationChar, written));
	
	return ATMO_Status_Success;
	
}


ATMO_Status_t OrientationChar_setValue(ATMO_Value_t *in, ATMO_Value_t *out) {

	
	// Convert to the desired write data type
	ATMO_Value_t convertedValue;
	ATMO_InitValue(&convertedValue);
	ATMO_CreateValueConverted(&convertedValue, ATMO_PROPERTY(OrientationChar, readDataType), in);

	ATMO_BLE_GATTSSetCharacteristic(
		ATMO_PROPERTY(OrientationChar, instance),
		ATMO_VARIABLE(OrientationChar, bleCharacteristicHandle),
		convertedValue.size, 
		(uint8_t *)convertedValue.data,
		NULL);
	
	ATMO_FreeValue(&convertedValue);
		
	return ATMO_Status_Success;
	
}


ATMO_Status_t OrientationChar_written(ATMO_Value_t *in, ATMO_Value_t *out) {

	ATMO_CreateValueConverted(out, ATMO_PROPERTY(OrientationChar, writeDataType), in);
	return ATMO_Status_Success;
	
}


ATMO_Status_t OrientationChar_subscibed(ATMO_Value_t *in, ATMO_Value_t *out) {
	return ATMO_Status_Success;
}


ATMO_Status_t OrientationChar_unsubscribed(ATMO_Value_t *in, ATMO_Value_t *out) {
	return ATMO_Status_Success;
}


ATMO_Status_t OrientationPrint_trigger(ATMO_Value_t *in, ATMO_Value_t *out) {
	return ATMO_Status_Success;
}


ATMO_Status_t OrientationPrint_setup(ATMO_Value_t *in, ATMO_Value_t *out) {

	return ATMO_Status_Success;
	
}


ATMO_Status_t OrientationPrint_print(ATMO_Value_t *in, ATMO_Value_t *out) {

    if((in != NULL) && (in->type != ATMO_DATATYPE_VOID))
    {
        char str[256];
        unsigned int prependLen = strlen(ATMO_PROPERTY(OrientationPrint, prepend));
        // If there's text to prepend, do that first
        if(prependLen > 0)
        {
            sprintf(str, "%s: ", ATMO_PROPERTY(OrientationPrint, prepend));
            prependLen += 2; // Adding 2 chars for ": "
        }
        
        // Grab the string version of the input data, place it after the prepend text
        if(ATMO_GetString(in, str + prependLen, 256 - prependLen) == ATMO_Status_Success)
        {
            ATMO_PLATFORM_DebugPrint("%s\r\n", str);
        }
    }
    else
    {
        ATMO_PLATFORM_DebugPrint("%s\r\n", ATMO_PROPERTY(OrientationPrint, prepend));
    }
    
    return ATMO_Status_Success;
    
}

//FOOTER START

//FOOTER END

