
#ifndef ATMO_PROPERTIES_H
#define ATMO_PROPERTIES_H

#ifdef __cplusplus
	extern "C"{
#endif

#define ATMO_PROPERTY(ELEMENT, NAME) ATMO_ ## ELEMENT ## _PROPERTY_  ## NAME

#define ATMO_EmbeddedBHI160_PROPERTY_errorData NULL
#define ATMO_EmbeddedBHI160_PROPERTY_codeUserChanged NULL
#define ATMO_EmbeddedBHI160_PROPERTY_i2cInstance ATMO_DRIVERINSTANCE_I2C_I2C1
#define ATMO_EmbeddedBHI160_PROPERTY_gpioInstance ATMO_DRIVERINSTANCE_GPIO_GPIO1
#define ATMO_EmbeddedBHI160_PROPERTY_intPin 9

#define ATMO_Interval_PROPERTY_errorData NULL
#define ATMO_Interval_PROPERTY_codeUserChanged NULL
#define ATMO_Interval_PROPERTY_instance ATMO_DRIVERINSTANCE_INTERVAL_INTERVAL1
#define ATMO_Interval_PROPERTY_time 100

#define ATMO_OrientationChar_PROPERTY_errorData NULL
#define ATMO_OrientationChar_PROPERTY_codeUserChanged NULL
#define ATMO_OrientationChar_PROPERTY_instance ATMO_DRIVERINSTANCE_BLE_BLE1
#define ATMO_OrientationChar_PROPERTY_bleServiceUuid "69c482ca-c200-44fc-b048-6e0d0be1191c"
#define ATMO_OrientationChar_PROPERTY_bleCharacteristicUuid "69c482ca-c200-44fc-b048-6e0d0be1191d"
#define ATMO_OrientationChar_PROPERTY_read true
#define ATMO_OrientationChar_PROPERTY_write true
#define ATMO_OrientationChar_PROPERTY_notify true
#define ATMO_OrientationChar_PROPERTY_readDataType ATMO_DATATYPE_3D_VECTOR_FLOAT
#define ATMO_OrientationChar_PROPERTY_writeDataType ATMO_DATATYPE_3D_VECTOR_FLOAT
#define ATMO_OrientationChar_PROPERTY_notifyDataType ATMO_DATATYPE_3D_VECTOR_FLOAT

#define ATMO_OrientationPrint_PROPERTY_errorData NULL
#define ATMO_OrientationPrint_PROPERTY_codeUserChanged NULL
#define ATMO_OrientationPrint_PROPERTY_prepend "Orientation"


#ifdef __cplusplus
}
#endif

#endif
