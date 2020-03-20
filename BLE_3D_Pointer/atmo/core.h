/**
 ******************************************************************************
 * @file    core.h
 * @author
 * @version
 * @date
 * @brief   Atmosphere API - Core header file
 ******************************************************************************
 * @attention
 *
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/** \addtogroup Core
		The core driver includes basic functions to perform within the platform.
 *  @{
 */


/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef __ATMO_CORE__H
#define __ATMO_CORE__H

#include "../app_src/atmosphere_typedefs.h"
#include "../app_src/atmo_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t ATMO_DriverInstanceHandle_t;

typedef struct
{
	float x;
	float y;
	float z;
} ATMO_3dFloatVector_t;

typedef struct
{
	double x;
	double y;
	double z;
} ATMO_3dDoubleVector_t;

/**
 * Generic Atmosphere callback function.
 * The argument is typically an ATMO_Value_t
 */
typedef void ( *ATMO_Callback_t )( void *value );

/**
 * Datatype stored in ATMO_Value_t type.
 */
typedef enum
{
	ATMO_DATATYPE_VOID				= 0x01u,
	ATMO_DATATYPE_CHAR				= 0x02u,
	ATMO_DATATYPE_BOOL				= 0x03u,
	ATMO_DATATYPE_INT				= 0x04u,
	ATMO_DATATYPE_UNSIGNED_INT		= 0x05u,
	ATMO_DATATYPE_FLOAT				= 0x06u,
	ATMO_DATATYPE_DOUBLE			= 0x07u,
	ATMO_DATATYPE_STRING			= 0x08u,
	ATMO_DATATYPE_BINARY			= 0x09u,
	ATMO_DATATYPE_3D_VECTOR_FLOAT	= 0x0Au,
	ATMO_DATATYPE_3D_VECTOR_DOUBLE	= 0x0Bu,
	ATMO_DATATYPE_LIST				= 0x0Cu,
	ATMO_DATATYPE_MAX
} ATMO_DATATYPE;

typedef enum
{
	ATMO_Status_Success              = 0x00u,  // Common - Operation was successful
	ATMO_Status_Fail                 = 0x01u,  // Common - Operation failed
	ATMO_Status_NoInput              = 0x02u,  // Common - No input object
	ATMO_Status_InvalidInput         = 0x03u,  // Common - No way to convert input data
	ATMO_Status_OutOfMemory          = 0x04u,
	ATMO_Status_MissingSupport       = 0x05u
} ATMO_Status_t;

typedef enum
{
	ATMO_LESS_THAN,
	ATMO_LESS_THAN_EQUAL,
	ATMO_GREATER_THAN,
	ATMO_GREATER_THAN_EQUAL,
	ATMO_EQUAL
} ATMO_CompareCondition_t;

typedef enum
{
	ATMO_OPERATOR_ADD,
	ATMO_OPERATOR_SUBTRACT,
	ATMO_OPERATOR_MULTIPLY,
	ATMO_OPERATOR_DIVIDE
} ATMO_Operator_t;

typedef enum
{
	ATMO_DRIVERTYPE_ADC = 0x0,
	ATMO_DRIVERTYPE_BLE = 0x1,
	ATMO_DRIVERTYPE_BLOCK = 0x2,
	ATMO_DRIVERTYPE_CLOUD = 0x3,
	ATMO_DRIVERTYPE_DATETIME = 0x4,
	ATMO_DRIVERTYPE_FILESYSTEM = 0x5,
	ATMO_DRIVERTYPE_GPIO = 0x6,
	ATMO_DRIVERTYPE_HTTP = 0x7,
	ATMO_DRIVERTYPE_I2C = 0x8,
	ATMO_DRIVERTYPE_INTERVAL = 0x9,
	ATMO_DRIVERTYPE_NFC = 0xA,
	ATMO_DRIVERTYPE_PWM = 0xB,
	ATMO_DRIVERTYPE_SPI = 0xC,
	ATMO_DRIVERTYPE_UART = 0xD,
	ATMO_DRIVERTYPE_WIFI = 0xE,
	ATMO_DRIVERTYPE_CELLULAR = 0xF,
	ATMO_DRIVERTYPE_ETHERNET = 0x10
} ATMO_DriverType_t;

typedef enum
{
	ATMO_ENDIAN_Type_Big,
	ATMO_ENDIAN_Type_Little
} ATMO_ENDIAN_Type_t;

typedef enum
{
	ATMO_UUID_Type_Invalid = 0,
	ATMO_UUID_Type_16_Bit = 2,
	ATMO_UUID_Type_32_Bit = 4,
	ATMO_UUID_Type_128_Bit = 16,
} ATMO_UUID_Type_t;

typedef struct
{
	uint8_t data[16];
	ATMO_UUID_Type_t type;
	ATMO_ENDIAN_Type_t endianness;
} ATMO_UUID_t;

typedef struct
{
	ATMO_DriverType_t type;
} ATMO_CORE_DeviceHandle_t;

#define ATMO_DRIVERINSTANCE_DEFAULT (0)

#define ATMO_UUID_ENDIAN_DEFAULT (ATMO_ENDIAN_Type_Big)
#define ATMO_UUID_STR_LEN (37)

#ifdef ATMO_STATIC_CORE
#ifndef ATMO_STATIC_SIZE
#define ATMO_STATIC_SIZE (64)
#endif
#endif

typedef struct
{
	ATMO_DATATYPE type;
#ifdef ATMO_STATIC_CORE
	uint8_t size;
	uint8_t data[ATMO_STATIC_SIZE];
#else
	unsigned int size;
	void *data;
#endif
} ATMO_Value_t;


#define ATMO_LORA_APP_KEY_LEN 32
#define ATMO_LORA_DEV_EUI_LEN 16

typedef struct ATMO_ListEntry_t ATMO_ListEntry_t;

struct ATMO_ListEntry_t
{
	ATMO_Value_t *data;
	ATMO_ListEntry_t *next;
};

typedef struct
{
	uint8_t size;
	ATMO_ListEntry_t *head;
} ATMO_List_t;

typedef uint16_t ATMO_AbilityHandle_t;

/**
 * Structure to be filled and pushed onto Ability Handler list.
 * Generally, every loop, the main thread will check this list,
 * empty it, and execute any abilities.
 */
typedef struct
{
	ATMO_AbilityHandle_t abilityHandle; /**< The integer handle of the ability. */
	ATMO_Value_t value; /**< Any value that is to be passed along to the ability. */

} ATMO_Ability_Execute_Entry_t;

/**
 * Structure to be filled and pushed onto Callback Handler list.
 * Generally, every loop, the main thread will check this list,
 * empty it, and execute any abilities.
 */
typedef struct
{
	ATMO_Callback_t callback; /**< The callback function to be executed */
	ATMO_Value_t value; /**< Any value that is to be passed along to the callback */

} ATMO_Callback_Execute_Entry_t;

typedef struct
{
	const char *name;
	ATMO_BOOL_t initialized;
	ATMO_DriverInstanceHandle_t instanceNumber;
	void *argument;
} ATMO_DriverInstanceData_t;

#define ATMO_MAX_NUMBER_OF_ABILITY_EXECUTIONS 10
#define ATMO_MAX_NUMBER_OF_CALLBACK_EXECUTIONS 10
#define ATMO_MAX_NUMBER_OF_TICK_CALLBACKS 8

/**
 * Initialize atmosphere core. Should not be called by users.
 */
ATMO_Status_t ATMO_Init();

/**
 * Process single tick in atmosphere core. Should not be called by users.
 */
ATMO_Status_t ATMO_Tick();

/**
 * Add a callback to the execution list
 *
 * @param[in] callback
 * @param[in] value - value to go with callback
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_AddCallbackExecute( ATMO_Callback_t callback, ATMO_Value_t *value );

/**
 * Add ability to the execution list
 *
 * @param[in] abilityHandle
 * @param[in] value - value to go with ability
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_AddAbilityExecute( ATMO_AbilityHandle_t abilityHandle, ATMO_Value_t *value );

/**
 * Add a callback to be executed every tick
 *
 * @param[in] callback
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_AddTickCallback( ATMO_Callback_t callback );

/**
 * This routine gets the max size in bytes of an ATMO_DATATYPE list
 *
 * @param[in] numberOfTypes - Number of elements in the list
 * @param[in] bufferSize - Max buffer size
 * @param[in] types - List of data types
 * @return maximum size of data in buffer
 */
size_t ATMO_GetMaxValueSize( uint8_t numberOfTypes, size_t bufferSize, ATMO_DATATYPE *types );

ATMO_Status_t ATMO_InitValue( ATMO_Value_t *value );

/**
 * Initialize an ATMO_Value_t to void
 * @param[in] value
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_CreateValueVoid( ATMO_Value_t *value );

/**
 * Initialize an ATMO_Value_t to Char
 * @param[in] value
 * @param[in] data
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_CreateValueChar( ATMO_Value_t *value, char data );

/**
 * Initialize an ATMO_Value_t to Boolean
 * @param[in] value
 * @param[in] data
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_CreateValueBool( ATMO_Value_t *value, ATMO_BOOL_t data );

/**
 * Initialize an ATMO_Value_t to Integer
 * @param[in] value
 * @param[in] data
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_CreateValueInt( ATMO_Value_t *value, int data );

/**
 * Initialize an ATMO_Value_t to Unsigned Integer
 * @param[in] value
 * @param[in] data
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_CreateValueUnsignedInt( ATMO_Value_t *value, unsigned int data );

/**
 * Initialize an ATMO_Value_t to Float
 * @param[in] value
 * @param[in] data
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_CreateValueFloat( ATMO_Value_t *value, float data );

/**
 * Initialize an ATMO_Value_t to Double
 * @param[in] value
 * @param[in] data
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_CreateValueDouble( ATMO_Value_t *value, double data );

/**
 * Initialize an ATMO_Value_t to String
 * @param[in] value
 * @param[in] str
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_CreateValueString( ATMO_Value_t *value, const char *str );

/**
 * Initialize an ATMO_Value_t to Binary Array
 * @param[in] value
 * @param[in] data
 * @param[in] size - in bytes
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_CreateValueBinary( ATMO_Value_t *value, const void *data, unsigned int size );

/**
 * @brief Initialize an ATMO_Value_t to 3D Float Vector
 *
 * @param value
 * @param vector
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_CreateValue3dVectorFloat( ATMO_Value_t *value, ATMO_3dFloatVector_t *vector );

/**
 * @brief Initialize an ATMO_Value_t to 3D Double Vector
 *
 * @param value
 * @param vector
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_CreateValue3dVectorDouble( ATMO_Value_t *value, ATMO_3dDoubleVector_t *vector );

/**
 * @brief Initialize an empty list
 *
 * @param value
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_CreateValueList( ATMO_Value_t *value );

/**
 * @brief Push a value onto the list
 *
 * @param list - Your list structure
 * @param value  - Value to push
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_ListPushBack( ATMO_Value_t *list, ATMO_Value_t *value );

/**
 * @brief Push a value onto the front of a list
 *
 * @param list - Your list structure
 * @param value  - Value to push
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_ListPushFront( ATMO_Value_t *list, ATMO_Value_t *value );


/**
 * @brief Pop a value from the back of the list
 *
 * You are responsible for freeing the value.
 *
 * @param list
 * @param value - Value to be filled with popped value
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_ListPopBack( ATMO_Value_t *list, ATMO_Value_t **value );

/**
 * @brief Pop a value from the front of the list
 *
 * You are responsible for freeing the value.
 *
 * @param list
 * @param value - Value to be filled with popped value
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_ListPopFront( ATMO_Value_t *list, ATMO_Value_t **value );

/**
 * @brief Get the size of a list
 *
 * @param list
 * @param size
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_ListGetSize( ATMO_Value_t *list, unsigned int *size );

/**
 * @brief Get a list value by index
 *
 * This does not pop the value from the list.
 *
 * @param list
 * @param index
 * @param value
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_ListGetIndex( ATMO_Value_t *list, unsigned int index, ATMO_Value_t **value );

/**
 * Convert ATMO_Value_t from one datatype to another
 * @param[in] newValue
 * @param[in] type - desired new type
 * @param[in] convertValue - old value to be converted
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_CreateValueConverted( ATMO_Value_t *newValue, ATMO_DATATYPE type, ATMO_Value_t *convertValue );

/**
 * Create a copy of ATMO_Value_t
 * @param[in] newValue
 * @param[in] oldValue
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_CreateValueCopy( ATMO_Value_t *newValue, ATMO_Value_t *oldValue );

/**
 * Perform a mathematical operation on a value
 *
 * Will return error if oldValue is not a numerical type.
 *
 * @param[out] newValue
 * @param[in] operation
 * @param[in] operand
 * @param[in] oldValue
 */
ATMO_Status_t ATMO_PerformOperation( ATMO_Value_t *newValue, ATMO_Operator_t operation, float operand, ATMO_Value_t *oldValue );

/**
 * Compare value A to value B
 *
 * Note: The two values must both be numerical identical types
 *
 * @param[in] valueA
 * @param[in] valueB
 * @param[in] condition
 * @param[out] result
 */
ATMO_Status_t ATMO_CompareValues( ATMO_Value_t *valueA, ATMO_Value_t *valueB, ATMO_CompareCondition_t condition, ATMO_BOOL_t *result );


ATMO_Status_t ATMO_FreeValue( ATMO_Value_t *value );

/**
 * This routine retrieves a raw Character from an ATMO_Value_t
 * It does any necessary data conversions
 *
 * @param[in] value
 * @param[out] output
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_GetChar( ATMO_Value_t *value, char *output );

/**
 * This routine retrieves a raw Bool from an ATMO_Value_t
 * It does any necessary data conversions
 *
 * @param[in] value
 * @param[out] output
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_GetBool( ATMO_Value_t *value, ATMO_BOOL_t *output );

/**
 * This routine retrieves a raw Integer from an ATMO_Value_t
 * It does any necessary data conversions
 *
 * @param[in] value
 * @param[out] output
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_GetInt( ATMO_Value_t *value, int *output );

/**
 * This routine retrieves a raw Unsigned Integer from an ATMO_Value_t
 * It does any necessary data conversions
 *
 * @param[in] value
 * @param[out] output
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_GetUnsignedInt( ATMO_Value_t *value, unsigned int *output );

/**
 * This routine retrieves a raw Float from an ATMO_Value_t
 * It does any necessary data conversions
 *
 * @param[in] value
 * @param[out] output
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_GetFloat( ATMO_Value_t *value, float *output );

/**
 * This routine retrieves a raw Double from an ATMO_Value_t
 * It does any necessary data conversions
 *
 * @param[in] value
 * @param[out] output
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_GetDouble( ATMO_Value_t *value, double *output );

/**
 * This routine retrieves a raw String from an ATMO_Value_t
 * It does any necessary data conversions
 *
 * @param[in] value
 * @param[out] buffer
 * @param[in] size - size of buffer in bytes
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_GetString( ATMO_Value_t *value, char *buffer, unsigned int size );

/**
 * This routine retrieves a raw Binary Array from an ATMO_Value_t
 * It does any necessary data conversions
 *
 * @param[in] value
 * @param[out] buffer
 * @param[in] size - size of buffer in bytes
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_GetBinary( ATMO_Value_t *value, void *buffer, unsigned int size );

/**
 * @brief Initialize an ATMO_Value_t to 3D Float Vector
 *
 * @param value
 * @param vector
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_Get3dVectorFloat( ATMO_Value_t *value, ATMO_3dFloatVector_t *vector );

/**
 * @brief Initialize an ATMO_Value_t to 3D Double Vector
 *
 * @param value
 * @param vector
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_Get3dVectorDouble( ATMO_Value_t *value, ATMO_3dDoubleVector_t *vector );


/**
 * Convert a string UUID (Eg. "ae23626b-f51b-4995-b735-90e4ee9ae1f1") to binary data
 *
 * @param[in] str
 * @param[out] uuid
 * @return status
 */
ATMO_Status_t ATMO_StringToUuid( const char *str, ATMO_UUID_t *uuid, ATMO_ENDIAN_Type_t endianness );

/**
 * Convert a UUID from binary data to a formatted string
 *
 * @param[in] uuid
 * @param[out] str
 * @param[in] strBufLen - Length of passed in string buffer
 * @return status
 */
ATMO_Status_t ATMO_UuidToString( const ATMO_UUID_t *uuid, char *str, unsigned int strBufLen );

/**
 * @brief Delay for N milliseconds, calling ATMO_Tick every once in awhile to simulate non-blocking behavior
 * Should not be used for any sort of exact timing
 *
 * @param milliseconds
 * @return ATMO_Status_t
 */
ATMO_Status_t ATMO_DelayMillisecondsNonBlock( unsigned int milliseconds );

/**
 * @brief Print byte array to debug output
 *
 * @param bytes
 * @param len
 */
void ATMO_PrintBytes( uint8_t *bytes, uint32_t len );

#ifdef ATMO_PLATFORM_NO_STRCPY
char *ATMO_StrCpy( char *dest, const char *src );
#endif

#ifdef ATMO_PLATFORM_NO_STRNCMP
int ATMO_StrnCmp( const char *str1, const char *str2, size_t size );
#endif

#ifndef ATMO_PLATFORM_NO_SQRT
#define ATMO_Sqrt sqrt
#else
/**
 * @brief A wrapper around sqrt or a custom implementation for platforms without it
 * 
 * @param num 
 * @return float 
 */
float ATMO_Sqrt(float num);
#endif

#ifndef ATMO_PLATFORM_NO_POW
#define ATMO_Pow pow
#else
/**
 * @brief A wrapper around pow or a custom implementation for platforms without it
 * 
 * @param num 
 * @param exp 
 * @return float 
 */
float ATMO_Pow(float num, uint32_t exp);
#endif


#ifdef __cplusplus
}
#endif

#endif /* __ATMO_CORE__H */
/** @}*/
