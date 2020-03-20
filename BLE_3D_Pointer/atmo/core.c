
#include "core.h"
#include "../app_src/atmosphere_variantSetup.h"
#include "../app_src/atmosphere_elementSetup.h"
#include "../app_src/atmosphere_platform.h"
#include "../app_src/atmosphere_callbacks.h"
#include "../app_src/atmosphere_abilityHandler.h"
#include "../ringbuffer/atmosphere_ringbuffer.h"
#include "atmo_strtof.h"

ATMO_RingBuffer_t abilityExecuteList;
ATMO_RingBuffer_t callbackExecuteList;
ATMO_RingBuffer_t tickCallbacks;

#ifdef ATMO_STATIC_CORE
static uint8_t abilityListBuf[ATMO_MAX_NUMBER_OF_ABILITY_EXECUTIONS * sizeof( ATMO_Ability_Execute_Entry_t )];
static uint8_t callbackListBuf[ATMO_MAX_NUMBER_OF_CALLBACK_EXECUTIONS * sizeof( ATMO_Callback_Execute_Entry_t )];
static uint8_t tickListBuf[ATMO_MAX_NUMBER_OF_TICK_CALLBACKS * sizeof( ATMO_Callback_t )];
#endif

/**
 * @brief Utility function for absolute value
 *
 * @param value
 * @return unsigned _ATMO_Abs
 */
static unsigned int _ATMO_Abs( int value )
{
	return ( value < 0 ) ? ( value * -1 ) : value;
}

#ifdef ATMO_PLATFORM_NO_STRCPY
char *ATMO_StrCpy( char *dest, const char *src )
{
	memcpy( dest, src, strlen( src ) + 1 );
	return dest;
}
#endif

#ifdef ATMO_PLATFORM_NO_STRNCMP

int ATMO_StrnCmp( const char *str1, const char *str2, size_t size )
{
	return memcmp( str1, str2, size );
}

#endif

#ifdef ATMO_NO_STRTOL_SUPPORT
#ifndef ULONG_MAX
#define	ULONG_MAX	((unsigned long)(~0L))		/* 0xFFFFFFFF */
#endif

#ifndef LONG_MAX
#define	LONG_MAX	((long)(ULONG_MAX >> 1))	/* 0x7FFFFFFF */
#endif

#ifndef LONG_MIN
#define	LONG_MIN	((long)(~LONG_MAX))		/* 0x80000000 */
#endif

#define IS_ALPHA(x) (((x) >= 'a' && (x) <= 'z') || ((x) >= 'A' && (x) <= 'Z'))
#define IS_SPACE(x) ((x) == ' ' || (x) == '\t')
#define IS_DIGIT(x) (((x) >= '0') && ((x) <= '9') )
#define IS_UPPER(x) (((x) >= 'A') && ((x) <= 'Z'))

/*
 * Convert a string to a long integer.
 *
 * Ignores `locale' stuff.  Assumes that the upper and lower case
 * alphabets and digits are each contiguous.
 */
long
strtol( const char *nptr, char **endptr, int base )
{
	const char *s = nptr;
	unsigned long acc;
	int c;
	unsigned long cutoff;
	int neg = 0, any, cutlim;

	/*
	 * Skip white space and pick up leading +/- sign if any.
	 * If base is 0, allow 0x for hex and 0 for octal, else
	 * assume decimal; if base is already 16, allow 0x.
	 */
	do
	{
		c = *s++;
	}
	while ( IS_SPACE( c ) );

	if ( c == '-' )
	{
		neg = 1;
		c = *s++;
	}
	else if ( c == '+' )
	{
		c = *s++;
	}

	if ( ( base == 0 || base == 16 ) &&
	        c == '0' && ( *s == 'x' || *s == 'X' ) )
	{
		c = s[1];
		s += 2;
		base = 16;
	}

	if ( base == 0 )
	{
		base = ( c == '0' ) ? 8 : 10;
	}

	/*
	 * Compute the cutoff value between legal numbers and illegal
	 * numbers.  That is the largest legal value, divided by the
	 * base.  An input number that is greater than this value, if
	 * followed by a legal input character, is too big.  One that
	 * is equal to this value may be valid or not; the limit
	 * between valid and invalid numbers is then based on the last
	 * digit.  For instance, if the range for longs is
	 * [-2147483648..2147483647] and the input base is 10,
	 * cutoff will be set to 214748364 and cutlim to either
	 * 7 (neg==0) or 8 (neg==1), meaning that if we have accumulated
	 * a value > 214748364, or equal but the next digit is > 7 (or 8),
	 * the number is too big, and we will return a range error.
	 *
	 * Set any if any `digits' consumed; make it negative to indicate
	 * overflow.
	 */
	cutoff = neg ? -( unsigned long )LONG_MIN : LONG_MAX;
	cutlim = cutoff % ( unsigned long )base;
	cutoff /= ( unsigned long )base;

	for ( acc = 0, any = 0;; c = *s++ )
	{
		if ( IS_DIGIT( c ) )
		{
			c -= '0';
		}
		else if ( IS_ALPHA( c ) )
		{
			c -= IS_UPPER( c ) ? 'A' - 10 : 'a' - 10;
		}
		else
		{
			break;
		}

		if ( c >= base )
		{
			break;
		}

		if ( any < 0 || acc > cutoff || ( acc == cutoff && c > cutlim ) )
		{
			any = -1;
		}
		else
		{
			any = 1;
			acc *= base;
			acc += c;
		}
	}

	if ( any < 0 )
	{
		acc = neg ? LONG_MIN : LONG_MAX;
	}
	else if ( neg )
	{
		acc = -acc;
	}

	if ( endptr != 0 )
	{
		*endptr = ( char * ) ( any ? s - 1 : nptr );
	}

	return ( acc );
}
#endif

static void _ATMO_RingBuffer_FreeAbilityEntry( void *entry )
{
	ATMO_Ability_Execute_Entry_t *abilityEntry = ( ATMO_Ability_Execute_Entry_t * )entry;
	ATMO_FreeValue( &abilityEntry->value );
}

static void _ATMO_RingBuffer_FreeCallbackEntry( void *entry )
{
	ATMO_Callback_Execute_Entry_t *cbEntry = ( ATMO_Callback_Execute_Entry_t * )entry;
	ATMO_FreeValue( &cbEntry->value );
}

static ATMO_BOOL_t __ATMO_IsNumericalType( ATMO_Value_t *value )
{
	switch ( value->type )
	{
		case ATMO_DATATYPE_CHAR:
		case ATMO_DATATYPE_INT:
		case ATMO_DATATYPE_UNSIGNED_INT:
		case ATMO_DATATYPE_FLOAT:
		case ATMO_DATATYPE_DOUBLE:
		{
			return true;
		}

		default:
		{
			return false;
		}
	}

	return false;
}

static ATMO_BOOL_t __ATMO_IsComparableType( ATMO_Value_t *value )
{
	return __ATMO_IsNumericalType( value ) || value->type == ATMO_DATATYPE_BOOL || value->type == ATMO_DATATYPE_STRING;
}

ATMO_Status_t ATMO_Init()
{
#ifdef ATMO_STATIC_CORE
	ATMO_RingBuffer_InitWithBuf( &abilityExecuteList, abilityListBuf, ATMO_MAX_NUMBER_OF_ABILITY_EXECUTIONS, sizeof( ATMO_Ability_Execute_Entry_t ), _ATMO_RingBuffer_FreeAbilityEntry );
	ATMO_RingBuffer_InitWithBuf( &callbackExecuteList, callbackListBuf, ATMO_MAX_NUMBER_OF_CALLBACK_EXECUTIONS, sizeof( ATMO_Callback_Execute_Entry_t ), _ATMO_RingBuffer_FreeCallbackEntry );
	ATMO_RingBuffer_InitWithBuf( &tickCallbacks, tickListBuf, ATMO_MAX_NUMBER_OF_TICK_CALLBACKS, sizeof( ATMO_Callback_t ), NULL );
#else
	ATMO_RingBuffer_Init( &abilityExecuteList, ATMO_MAX_NUMBER_OF_ABILITY_EXECUTIONS, sizeof( ATMO_Ability_Execute_Entry_t ), _ATMO_RingBuffer_FreeAbilityEntry );
	ATMO_RingBuffer_Init( &callbackExecuteList, ATMO_MAX_NUMBER_OF_CALLBACK_EXECUTIONS, sizeof( ATMO_Callback_Execute_Entry_t ), _ATMO_RingBuffer_FreeCallbackEntry );
	ATMO_RingBuffer_Init( &tickCallbacks, ATMO_MAX_NUMBER_OF_TICK_CALLBACKS, sizeof( ATMO_Callback_t ), NULL );
#endif

	ATMO_PLATFORM_Init();
	ATMO_PLATFORM_VariantSetup();
	ATMO_ElementSetup();
	ATMO_Setup();
	ATMO_PLATFORM_PostInit();

	return ATMO_Status_Success;
}

static void ATMO_EmptyCallbackList()
{
	while ( !ATMO_RingBuffer_Empty( &callbackExecuteList ) )
	{
		// Get the next callback
		ATMO_Lock();
		ATMO_Callback_Execute_Entry_t *pCallbackEntry = ( ATMO_Callback_Execute_Entry_t * )ATMO_RingBuffer_Pop( &callbackExecuteList );
		ATMO_Callback_Execute_Entry_t entry;
		memcpy(&entry, pCallbackEntry, sizeof(entry));
		ATMO_Unlock();
		entry.callback( &entry.value );
		ATMO_FreeValue( &entry.value );
	}
}

ATMO_Status_t ATMO_Tick()
{
	uint8_t i = 0;

	for ( i = 0; i < tickCallbacks.count; i++ )
	{
		ATMO_Callback_t *cb = ( ATMO_Callback_t * )ATMO_RingBuffer_Index( &tickCallbacks, i );
		( *cb )( NULL );
	}

	ATMO_EmptyCallbackList();

	while ( !ATMO_RingBuffer_Empty( &abilityExecuteList ) )
	{
		// Get the next callback
		ATMO_Lock();
		ATMO_Ability_Execute_Entry_t *pAbilityEntry = ( ATMO_Ability_Execute_Entry_t * )ATMO_RingBuffer_Pop( &abilityExecuteList );
		ATMO_Ability_Execute_Entry_t entry;
		memcpy(&entry, pAbilityEntry, sizeof(entry));
		ATMO_Unlock();
		ATMO_AbilityHandler( entry.abilityHandle, &entry.value );
		ATMO_FreeValue( &entry.value );

		// If any callbacks were added, do them now
		ATMO_EmptyCallbackList();
	}

	return ATMO_Status_Success;
}

ATMO_Status_t ATMO_AddTickCallback( ATMO_Callback_t callback )
{
	ATMO_Lock();

	if ( ATMO_RingBuffer_Full( &tickCallbacks ) )
	{
		ATMO_Unlock();
		return ATMO_Status_OutOfMemory;
	}

	ATMO_RingBuffer_Push( &tickCallbacks, &callback );
	ATMO_Unlock();
	return ATMO_Status_Success;
}

ATMO_Status_t ATMO_AddCallbackExecute( ATMO_Callback_t callback, ATMO_Value_t *value )
{
	ATMO_Lock();

	if ( ATMO_RingBuffer_Full( &callbackExecuteList ) )
	{
		ATMO_Unlock();
		return ATMO_Status_OutOfMemory;
	}

	ATMO_Callback_Execute_Entry_t entry;

	ATMO_InitValue( &entry.value );

	if ( value != NULL )
	{
		ATMO_CreateValueCopy( &entry.value, value );
	}

	entry.callback = callback;

	ATMO_RingBuffer_Push( &callbackExecuteList, &entry );

#ifdef ATMO_ASYNC_TICK
	ATMO_PLATFORM_SendTickEvent();
#endif
	ATMO_Unlock();
	return ATMO_Status_Success;
}

ATMO_Status_t ATMO_AddAbilityExecute( ATMO_AbilityHandle_t abilityHandle, ATMO_Value_t *value )
{
	ATMO_Lock();

	if ( ATMO_RingBuffer_Full( &abilityExecuteList ) )
	{
		ATMO_Unlock();
		return ATMO_Status_OutOfMemory;
	}

	ATMO_Ability_Execute_Entry_t entry;

	ATMO_InitValue( &entry.value );

	if ( value != NULL )
	{
		ATMO_CreateValueCopy( &entry.value, value );
	}

	entry.abilityHandle = abilityHandle;

	ATMO_RingBuffer_Push( &abilityExecuteList, &entry );

#ifdef ATMO_ASYNC_TICK
	ATMO_PLATFORM_SendTickEvent();
#endif
	ATMO_Unlock();
	return ATMO_Status_Success;
}

#ifdef ATMO_PLATFORM_NO_SQRT
float ATMO_Sqrt( float number )
{
	float guess, e, upperbound;
		guess = 1;
		e = 0.001;
		do 
		{
			upperbound = number / guess;
			guess = (upperbound + guess) / 2;
		} while (!(guess * guess >= number - e && 
				guess * guess <= number + e));
		return guess;
}
#endif

#ifdef ATMO_PLATFORM_NO_POW
float ATMO_Pow(float num, uint32_t exp)
{
    float temp;
    if (exp == 0)
    return 1;
    temp = ATMO_Pow (num, exp / 2);
    if ((exp % 2) == 0) {
        return temp * temp;
    } else {
        if (exp > 0)
            return num * temp * temp;
        else
            return (temp * temp) / num;
    }

	return 0;
}
#endif

static float _ATMO_Calc3dFloatMagnitude( ATMO_3dFloatVector_t vec )
{
	return ATMO_Sqrt( ( vec.x * vec.x ) + ( vec.y * vec.y ) + ( vec.z * vec.z ) );
}

static double _ATMO_Calc3dDoubleMagnitude( ATMO_3dDoubleVector_t vec )
{
	return ATMO_Sqrt( ( vec.x * vec.x ) + ( vec.y * vec.y ) + ( vec.z * vec.z ) );
}

/**
 * This routine initilizies an ATMO_Value_t structure. This must be called when
 * creating a new instance of ATMO_Value_t.
 *
 * @param value :
 * @param output :
 * @return status.
 */

size_t ATMO_GetMaxValueSize( uint8_t numberOfTypes, size_t bufferSize, ATMO_DATATYPE *types )
{
	uint8_t i;
	size_t maxSize = 0;

	for ( i = 0; i < numberOfTypes; i++ )
	{
		switch ( types[i] )

		{
			case ATMO_DATATYPE_VOID:
				break;

			case ATMO_DATATYPE_CHAR:
				if ( maxSize < sizeof( char ) )
				{
					maxSize = sizeof( char );
				}

				break;

			case ATMO_DATATYPE_BOOL:
				if ( maxSize < sizeof( ATMO_BOOL_t ) )
				{
					maxSize = sizeof( ATMO_BOOL_t );
				}

				break;

			case ATMO_DATATYPE_INT:
				if ( maxSize < sizeof( int ) )
				{
					maxSize = sizeof( int );
				}

				break;

			case ATMO_DATATYPE_UNSIGNED_INT:
				if ( maxSize < sizeof( unsigned int ) )
				{
					maxSize = sizeof( unsigned int );
				}

				break;

			case ATMO_DATATYPE_FLOAT:
				if ( maxSize < sizeof( float ) )
				{
					maxSize = sizeof( float );
				}

				break;

			case ATMO_DATATYPE_DOUBLE:
				if ( maxSize < sizeof( double ) )
				{
					maxSize = sizeof( double );
				}

				break;

			case ATMO_DATATYPE_STRING:
				if ( maxSize < bufferSize )
				{
					maxSize = bufferSize;
				}

				break;

			case ATMO_DATATYPE_BINARY:
				if ( maxSize < bufferSize )
				{
					maxSize = bufferSize;
				}

				break;

			case ATMO_DATATYPE_3D_VECTOR_FLOAT:
				if ( maxSize < sizeof( ATMO_3dFloatVector_t ) )
				{
					maxSize = sizeof( ATMO_3dFloatVector_t );
				}

				break;

			case ATMO_DATATYPE_3D_VECTOR_DOUBLE:
				if ( maxSize < sizeof( ATMO_3dDoubleVector_t ) )
				{
					maxSize = sizeof( ATMO_3dDoubleVector_t );
				}

			case ATMO_DATATYPE_MAX:
			default:
				break;
		}

	}

	return maxSize;
}

ATMO_Status_t ATMO_InitValue( ATMO_Value_t *value )
{
	value->type = ATMO_DATATYPE_VOID;
#ifdef ATMO_STATIC_CORE
	memset( value->data, 0, ATMO_STATIC_SIZE );
#else
	value->data = NULL;
#endif
	value->size = 0;
	return ATMO_Status_Success;
}

ATMO_Status_t ATMO_CreateValueVoid( ATMO_Value_t *value )
{

	ATMO_FreeValue( value );

	ATMO_InitValue( value );

	return ATMO_Status_Success;
}

ATMO_Status_t ATMO_CreateValueChar( ATMO_Value_t *value, char data )
{

	ATMO_FreeValue( value );

#ifndef ATMO_STATIC_CORE
	value->data = ATMO_Malloc( sizeof( char ) );
#endif

	if ( value->data == NULL )
	{
		ATMO_CreateValueVoid( value );
		return ATMO_Status_OutOfMemory;
	}

	*( char * )value->data = data;

	value->type = ATMO_DATATYPE_CHAR;
	value->size = sizeof( char );

	return ATMO_Status_Success;
}

ATMO_Status_t ATMO_CreateValueBool( ATMO_Value_t *value, ATMO_BOOL_t data )
{

	ATMO_FreeValue( value );

#ifndef ATMO_STATIC_CORE
	value->data = ATMO_Malloc( sizeof( ATMO_BOOL_t ) );
#endif

	if ( value->data == NULL )
	{
		ATMO_CreateValueVoid( value );
		return ATMO_Status_OutOfMemory;
	}

	memcpy( value->data, &data, sizeof( data ) );
	value->type = ATMO_DATATYPE_BOOL;
	value->size = sizeof( ATMO_BOOL_t );

	return ATMO_Status_Success;
}

ATMO_Status_t ATMO_CreateValueInt( ATMO_Value_t *value, int data )
{

	ATMO_FreeValue( value );

#ifndef ATMO_STATIC_CORE
	value->data = ATMO_Malloc( sizeof( int ) );
#endif

	if ( value->data == NULL )
	{
		ATMO_CreateValueVoid( value );
		return ATMO_Status_OutOfMemory;
	}

	memcpy( value->data, &data, sizeof( data ) );
	value->type = ATMO_DATATYPE_INT;
	value->size = sizeof( int );

	return ATMO_Status_Success;
}

ATMO_Status_t ATMO_CreateValueUnsignedInt( ATMO_Value_t *value, unsigned int data )
{

	ATMO_FreeValue( value );

#ifndef ATMO_STATIC_CORE
	value->data = ATMO_Malloc( sizeof( unsigned int ) );
#endif

	if ( value->data == NULL )
	{
		ATMO_CreateValueVoid( value );
		return ATMO_Status_OutOfMemory;
	}

	memcpy( value->data, &data, sizeof( data ) );
	value->type = ATMO_DATATYPE_UNSIGNED_INT;
	value->size = sizeof( unsigned int );

	return ATMO_Status_Success;
}

ATMO_Status_t ATMO_CreateValueFloat( ATMO_Value_t *value, float data )
{

	ATMO_FreeValue( value );

#ifndef ATMO_STATIC_CORE
	value->data = ATMO_Malloc( sizeof( float ) );
#endif

	if ( value->data == NULL )
	{
		ATMO_CreateValueVoid( value );
		return ATMO_Status_OutOfMemory;
	}

	memcpy( value->data, &data, sizeof( data ) );
	value->type = ATMO_DATATYPE_FLOAT;
	value->size = sizeof( float );

	return ATMO_Status_Success;
}

ATMO_Status_t ATMO_CreateValueDouble( ATMO_Value_t *value, double data )
{

	ATMO_FreeValue( value );

#ifndef ATMO_STATIC_CORE
	value->data = ATMO_Malloc( sizeof( double ) );
#endif

	if ( value->data == NULL )
	{
		ATMO_CreateValueVoid( value );
		return ATMO_Status_OutOfMemory;
	}

	memcpy( value->data, &data, sizeof( data ) );
	value->type = ATMO_DATATYPE_DOUBLE;
	value->size = sizeof( double );

	return ATMO_Status_Success;
}

ATMO_Status_t ATMO_CreateValueString( ATMO_Value_t *value, const char *str )
{

	ATMO_FreeValue( value );

	if ( str == NULL )
	{
		return ATMO_Status_Fail;
	}

	size_t length = strlen( str ) + sizeof( char );

#ifndef ATMO_STATIC_CORE
	value->data = ATMO_Malloc( length );
#else

	if ( length >= ATMO_STATIC_SIZE )
	{
		ATMO_CreateValueVoid( value );
		return ATMO_Status_OutOfMemory;
	}

#endif

	if ( value->data == NULL )
	{
		ATMO_CreateValueVoid( value );
		return ATMO_Status_OutOfMemory;
	}


	strncpy( ( char * )value->data, str, length );

	value->type = ATMO_DATATYPE_STRING;
	value->size = length;

	return ATMO_Status_Success;

}

ATMO_Status_t ATMO_CreateValueBinary( ATMO_Value_t *value, const void *data, unsigned int size )
{

	ATMO_FreeValue( value );

	if ( data == NULL )
	{
		return ATMO_Status_Fail;
	}

#ifndef ATMO_STATIC_CORE
	value->data = ATMO_Malloc( size );
#else

	if ( size > ATMO_STATIC_SIZE )
	{
		ATMO_CreateValueVoid( value );
		return ATMO_Status_OutOfMemory;
	}

#endif


	if ( value->data == NULL )
	{
		ATMO_CreateValueVoid( value );
		return ATMO_Status_OutOfMemory;
	}

	memcpy( value->data, data, size );

	value->type = ATMO_DATATYPE_BINARY;
	value->size = size;

	return ATMO_Status_Success;

}

ATMO_Status_t ATMO_CreateValue3dVectorFloat( ATMO_Value_t *value, ATMO_3dFloatVector_t *vector )
{
	ATMO_FreeValue( value );

#ifndef ATMO_STATIC_CORE
	value->data = ATMO_Malloc( sizeof( ATMO_3dFloatVector_t ) );
#endif

	if ( value->data == NULL )
	{
		ATMO_CreateValueVoid( value );
		return ATMO_Status_OutOfMemory;
	}

	memcpy( value->data, vector, sizeof( ATMO_3dFloatVector_t ) );
	value->type = ATMO_DATATYPE_3D_VECTOR_FLOAT;
	value->size = sizeof( ATMO_3dFloatVector_t );

	return ATMO_Status_Success;
}

ATMO_Status_t ATMO_CreateValue3dVectorDouble( ATMO_Value_t *value, ATMO_3dDoubleVector_t *vector )
{
	ATMO_FreeValue( value );

#ifndef ATMO_STATIC_CORE
	value->data = ATMO_Malloc( sizeof( ATMO_3dDoubleVector_t ) );
#endif

	if ( value->data == NULL )
	{
		ATMO_CreateValueVoid( value );
		return ATMO_Status_OutOfMemory;
	}

	memcpy( value->data, vector, sizeof( ATMO_3dDoubleVector_t ) );
	value->type = ATMO_DATATYPE_3D_VECTOR_DOUBLE;
	value->size = sizeof( ATMO_3dDoubleVector_t );

	return ATMO_Status_Success;
}

#ifndef ATMO_SLIM_STACK
ATMO_Status_t ATMO_CreateValueList( ATMO_Value_t *value )
{
	ATMO_FreeValue( value );

#ifndef ATMO_STATIC_CORE
	value->data = ATMO_Malloc( sizeof( ATMO_List_t ) );
#endif

	ATMO_List_t *list = ( ATMO_List_t * )value->data;
	value->type = ATMO_DATATYPE_LIST;
	list->size = 0;
	return ATMO_Status_Success;
}

ATMO_Status_t ATMO_ListPushBack( ATMO_Value_t *list, ATMO_Value_t *value )
{
	ATMO_List_t *meta = ( ATMO_List_t * )list->data;
	ATMO_ListEntry_t *head = meta->head;
	int i;

	if ( meta->size == 0 )
	{
		meta->head = ATMO_Malloc( sizeof( ATMO_ListEntry_t ) );
		meta->head->data = value;
		meta->head->next = NULL;
		meta->size = 1;
		return ATMO_Status_Success;
	}

	for ( i = 0; i < meta->size - 1; i++ )
	{
		head = head->next;
	}

	head->next = ATMO_Malloc( sizeof( ATMO_ListEntry_t ) );
	head->next->data = value;
	head->next->next = NULL;
	meta->size++;
	return ATMO_Status_Success;
}

ATMO_Status_t ATMO_ListPushFront( ATMO_Value_t *list, ATMO_Value_t *value )
{
	ATMO_List_t *meta = ( ATMO_List_t * )list->data;
	ATMO_ListEntry_t *head = meta->head;

	if ( meta->size == 0 )
	{
		return ATMO_ListPushBack( list, value );
	}

	ATMO_ListEntry_t *entry = ATMO_Malloc( sizeof( ATMO_ListEntry_t ) );
	entry->data = head->data;
	entry->next = head->next;

	meta->head->data = value;
	meta->head->next = entry;
	meta->size++;

	return ATMO_Status_Success;
}

ATMO_Status_t ATMO_ListPopBack( ATMO_Value_t *list, ATMO_Value_t **value )
{
	ATMO_List_t *meta = ( ATMO_List_t * )list->data;
	ATMO_ListEntry_t *head = meta->head;
	int i;

	if ( meta->size == 0 )
	{
		*value = NULL;
		return ATMO_Status_Fail;
	}

	if ( meta->size == 1 )
	{
		*value = meta->head->data;
		meta->size = 0;
		ATMO_Free( meta->head );
		return ATMO_Status_Success;
	}

	for ( i = 1; i < meta->size - 1; i++ )
	{
		head = head->next;
	}

	*value = head->next->data;
	ATMO_Free( head->next );
	head->next = NULL;
	meta->size--;

	return ATMO_Status_Success;
}

ATMO_Status_t ATMO_ListPopFront( ATMO_Value_t *list, ATMO_Value_t **value )
{
	ATMO_List_t *meta = ( ATMO_List_t * )list->data;

	if ( meta->size == 0 )
	{
		*value = NULL;
		return ATMO_Status_Fail;
	}


	*value = meta->head->data;

	if ( meta->size == 1 )
	{
		ATMO_Free( meta->head );
		meta->size--;
		return ATMO_Status_Success;
	}

	// If the size is 2 or more
	// Copy the second entry ptrs to head and free it
	ATMO_ListEntry_t *secondEntry = meta->head->next;
	meta->head->next = secondEntry->next;
	meta->head->data = secondEntry->data;
	ATMO_Free( secondEntry );
	meta->size--;

	return ATMO_Status_Success;
}

ATMO_Status_t ATMO_ListGetSize( ATMO_Value_t *list, unsigned int *size )
{
	if ( list->type != ATMO_DATATYPE_LIST )
	{
		return ATMO_Status_Fail;
	}

	ATMO_List_t *meta = ( ATMO_List_t * )list->data;

	*size = meta->size;
	return ATMO_Status_Success;
}

ATMO_Status_t ATMO_ListGetIndex( ATMO_Value_t *list, unsigned int index, ATMO_Value_t **value )
{
	ATMO_List_t *meta = ( ATMO_List_t * )list->data;
	ATMO_ListEntry_t *head = meta->head;
	int i;

	if ( index >= meta->size )
	{
		return ATMO_Status_Fail;
	}

	for ( i = 0; i < index; i++ )
	{
		head = head->next;
	}

	*value = head->data;

	return ATMO_Status_Success;
}

#endif

ATMO_Status_t ATMO_CreateValueCopy( ATMO_Value_t *newValue, ATMO_Value_t *oldValue )
{
	return ATMO_CreateValueConverted( newValue, oldValue->type, oldValue );
}

ATMO_Status_t ATMO_CreateValueConverted( ATMO_Value_t *newValue, ATMO_DATATYPE type, ATMO_Value_t *convertValue )
{

	switch ( type )
	{

		case ATMO_DATATYPE_VOID:
		{
			return ATMO_CreateValueVoid( newValue );

			break;
		}

		case ATMO_DATATYPE_BINARY:
		{
			return ATMO_CreateValueBinary( newValue, convertValue->data, convertValue->size );

			break;
		}

		case ATMO_DATATYPE_CHAR:
		{
			char newChar;
			ATMO_GetChar( convertValue, &newChar );
			return ATMO_CreateValueChar( newValue, newChar );

			break;
		}

		case ATMO_DATATYPE_BOOL:
		{
			ATMO_BOOL_t newBool;
			ATMO_GetBool( convertValue, &newBool );
			return ATMO_CreateValueBool( newValue, newBool );

			break;
		}

		case ATMO_DATATYPE_INT:
		{
			int newInt;
			ATMO_GetInt( convertValue, &newInt );
			return ATMO_CreateValueInt( newValue, newInt );

			break;
		}

		case ATMO_DATATYPE_UNSIGNED_INT:
		{
			unsigned int newUnsignedInt;
			ATMO_GetUnsignedInt( convertValue, &newUnsignedInt );
			return ATMO_CreateValueUnsignedInt( newValue, newUnsignedInt );

			break;
		}

#ifndef ATMO_PLATFORM_NO_FLOAT_SUPPORT

		case ATMO_DATATYPE_FLOAT:
		{
			float newFloat;
			ATMO_GetFloat( convertValue, &newFloat );
			return ATMO_CreateValueFloat( newValue, newFloat );

			break;
		}

#endif

#ifndef ATMO_PLATFORM_NO_DOUBLE_SUPPORT

		case ATMO_DATATYPE_DOUBLE:
		{
			double newDouble;
			ATMO_GetDouble( convertValue, &newDouble );
			return ATMO_CreateValueDouble( newValue, newDouble );

			break;
		}

#endif

		case ATMO_DATATYPE_STRING:
		{
			//TODO: Use dynamic size detection for the new string.
			char newString[128];
			ATMO_GetString( convertValue, newString, 128 );
			return ATMO_CreateValueString( newValue, newString );

			break;
		}

		case ATMO_DATATYPE_3D_VECTOR_FLOAT:
		{
			ATMO_3dFloatVector_t vec;
			ATMO_Get3dVectorFloat( convertValue, &vec );
			return ATMO_CreateValue3dVectorFloat( newValue, &vec );
			break;
		}

		case ATMO_DATATYPE_3D_VECTOR_DOUBLE:
		{
			ATMO_3dDoubleVector_t vec;
			ATMO_Get3dVectorDouble( convertValue, &vec );
			return ATMO_CreateValue3dVectorDouble( newValue, &vec );
			break;
		}

		default:
		{
			return ATMO_CreateValueVoid( newValue );

			break;
		}
	}
}

/**
 * This routine free's the data object from a value and then set's it to a ATMO_VOID object
 *
 * @param value :
 * @param output :
 * @return status.
 */
ATMO_Status_t ATMO_FreeValue( ATMO_Value_t *value )
{

#ifndef ATMO_STATIC_CORE

	if ( value->data != NULL )
	{
		ATMO_Free( value->data );
	}

#endif

#ifndef ATMO_SLIM_STACK

	if ( value->type == ATMO_DATATYPE_LIST )
	{
		ATMO_List_t *meta = ( ATMO_List_t * )value->data;
		ATMO_Value_t *tmpVal;
		unsigned int i;

		for ( i = 0; i < meta->size; i++ )
		{
			ATMO_ListPopFront( value, &tmpVal );
			ATMO_FreeValue( tmpVal );
		}
	}

#endif

	ATMO_InitValue( value );
	return ATMO_Status_Success;
}

/**
 * This routine
 *
 * @param value :
 * @param output :
 * @return status.
 */
ATMO_Status_t ATMO_GetChar( ATMO_Value_t *value, char *output )
{
	if ( output == NULL )
	{
		return ATMO_Status_Fail;
	}

	if ( value == NULL )
	{
		*output = '\x00';
		return ATMO_Status_NoInput;
	}

	if ( value->data == NULL )
	{
		*output = '\x00';
		return ATMO_Status_InvalidInput;
	}

	switch ( value->type )
	{

		case ATMO_DATATYPE_VOID:
		{
			*output = '\x00';
			break;
		}

		case ATMO_DATATYPE_BINARY:
		{
			if ( value->size < sizeof( char ) )
			{
				*output = '\x00';
			}

			else
			{
				memcpy( output, value->data, sizeof( char ) );
			}

			break;
		}

		case ATMO_DATATYPE_CHAR:
		{
			memcpy( output, value->data, sizeof( char ) );
			break;
		}

		case ATMO_DATATYPE_BOOL:
		{
			ATMO_BOOL_t bData;
			ATMO_GetBool( value, &bData );

			if ( bData )
			{
				*output = '\x01';
			}

			else
			{
				*output = '\x00';
			}

			break;
		}

		case ATMO_DATATYPE_INT:
		{
			int iData = 0;
			ATMO_GetInt( value, &iData );
			*output = ( char )( iData % 255 );
			break;
		}

		case ATMO_DATATYPE_UNSIGNED_INT:
		{
			unsigned int iData = 0;
			ATMO_GetUnsignedInt( value, &iData );
			*output = ( char )( iData % 255 );
			break;
		}

#ifndef ATMO_PLATFORM_NO_FLOAT_SUPPORT

		case ATMO_DATATYPE_FLOAT:
		{
			float fData = 0;
			ATMO_GetFloat( value, &fData );
			int intData = ( int )( fData );
			*output = ( char )( intData % 255 );
			break;
		}

#endif

#ifndef ATMO_PLATFORM_NO_DOUBLE_SUPPORT

		case ATMO_DATATYPE_DOUBLE:
		{
			double dData = 0;
			ATMO_GetDouble( value, &dData );
			int intData = ( int )( dData );
			*output = ( char )( intData % 255 );
			break;
		}

#endif

		case ATMO_DATATYPE_STRING:
		{
			memcpy( output, value->data, sizeof( char ) );
			break;
		}

		default:
		{
			*output = '\x00';
			return ATMO_Status_MissingSupport;
		}
	}

	return ATMO_Status_Success;
}

/**
 * This routine
 *
 * @param value :
 * @param output :
 * @return status.
 */
ATMO_Status_t ATMO_GetBool( ATMO_Value_t *value, ATMO_BOOL_t *output )
{
	if ( output == NULL )
	{
		return ATMO_Status_Fail;
	}

	if ( value == NULL )
	{
		*output = false;
		return ATMO_Status_NoInput;
	}

	if ( value->data == NULL )
	{
		*output = false;
		return ATMO_Status_InvalidInput;
	}

	switch ( value->type )
	{

		case ATMO_DATATYPE_VOID:
			*output = false;
			break;

		case ATMO_DATATYPE_BINARY:
			if ( value->size < sizeof( ATMO_BOOL_t ) )
			{
				*output = false;
			}

			else
			{
				memcpy( output, value->data, sizeof( ATMO_BOOL_t ) );
			}

			break;

		case ATMO_DATATYPE_CHAR:
		{
			char cData = 0;
			ATMO_GetChar( value, &cData );
			*output = ( cData ) ? true : false;
			break;
		}

		case ATMO_DATATYPE_BOOL:
		{
			memcpy( output, value->data, sizeof( ATMO_BOOL_t ) );
			break;
		}

		case ATMO_DATATYPE_INT:
		{
			int iData = 0;
			ATMO_GetInt( value, &iData );
			*output = ( iData ) ? true : false;
			break;
		}

		case ATMO_DATATYPE_UNSIGNED_INT:
		{
			unsigned int iData = 0;
			ATMO_GetUnsignedInt( value, &iData );
			*output = ( iData ) ? true : false;
			break;
		}

#ifndef ATMO_PLATFORM_NO_FLOAT_SUPPORT

		case ATMO_DATATYPE_FLOAT:
		{
			float fData = 0;
			ATMO_GetFloat( value, &fData );
			int intData = ( int )( fData );
			*output = ( intData ) ? true : false;
			break;
		}

#endif

#ifndef ATMO_PLATFORM_NO_DOUBLE_SUPPORT

		case ATMO_DATATYPE_DOUBLE:
		{
			double dData = 0;
			ATMO_GetDouble( value, &dData );
			int intData = ( int )( dData );
			*output = ( intData ) ? true : false;
			break;
		}

#endif

		case ATMO_DATATYPE_STRING:
		{
			char *pData = ( char * )value->data;
			unsigned int dataLen = strlen( pData );

			if ( dataLen >= 4 && strstr( pData, "true" ) == pData )
			{
				*output = true;
				break;
			}

			if ( dataLen >= 5 && strstr( pData, "false" ) == pData )
			{
				*output = false;
				break;
			}

			if ( pData[0] == 0 )
			{
				*output = false;
			}
			else
			{
				*output = true;
			}

			break;
		}

		default:
			*output = false;
			return ATMO_Status_MissingSupport;
			break;
	}

	return ATMO_Status_Success;
}

/**
 * This routine
 *
 * @param value :
 * @param output :
 * @return status.
 */
ATMO_Status_t ATMO_GetInt( ATMO_Value_t *value, int *output )
{
	if ( output == NULL )
	{
		return ATMO_Status_Fail;
	}

	if ( value == NULL )
	{
		*output = 0;
		return ATMO_Status_NoInput;
	}

	if ( value->data == NULL )
	{
		*output = 0;
		return ATMO_Status_InvalidInput;
	}

	switch ( value->type )
	{

		case ATMO_DATATYPE_VOID:
			*output = 0;

			break;

		case ATMO_DATATYPE_BINARY:
		{
			if ( value->size < sizeof( int ) )
			{
				*output = 0;
				return ATMO_Status_InvalidInput;
			}

			memcpy( output, value->data, sizeof( int ) );
			break;
		}

		case ATMO_DATATYPE_CHAR:
		{
			char cData = 0;
			ATMO_GetChar( value, &cData );
			*output = ( int )cData;
			break;
		}

		case ATMO_DATATYPE_BOOL:
		{
			ATMO_BOOL_t boolData = 0;
			ATMO_GetBool( value, &boolData );
			*output = ( int )boolData;
			break;
		}

		case ATMO_DATATYPE_INT:
		{
			memcpy( output, value->data, sizeof( int ) );
			break;
		}

		case ATMO_DATATYPE_UNSIGNED_INT:
		{
			unsigned int iData = 0;
			ATMO_GetUnsignedInt( value, &iData );
			*output = ( int )iData;
			break;
		}

#ifndef ATMO_PLATFORM_NO_FLOAT_SUPPORT

		case ATMO_DATATYPE_FLOAT:
		{
			float fData = 0;
			ATMO_GetFloat( value, &fData );
			*output = ( int )fData;
			break;
		}

		case ATMO_DATATYPE_3D_VECTOR_FLOAT:
		{
			ATMO_3dFloatVector_t vec;
			ATMO_Get3dVectorFloat( value, &vec );
			float mag = _ATMO_Calc3dFloatMagnitude( vec );
			*output = ( int )mag;
			break;
		}

#endif

#ifndef ATMO_PLATFORM_NO_DOUBLE_SUPPORT

		case ATMO_DATATYPE_DOUBLE:
		{
			double dblData = 0;
			ATMO_GetDouble( value, &dblData );
			*output = ( int )dblData;
			break;
		}

		case ATMO_DATATYPE_3D_VECTOR_DOUBLE:
		{
			ATMO_3dDoubleVector_t vec;
			ATMO_Get3dVectorDouble( value, &vec );
			double mag = _ATMO_Calc3dDoubleMagnitude( vec );
			*output = ( int )mag;
			break;
		}

#endif

#ifndef ATMO_PLATFORM_NO_SSCANF_SUPPORT

		case ATMO_DATATYPE_STRING:
		{
			int retValue = 0;
			sscanf( value->data, "%d", &retValue );
			*output = retValue;

			break;
		}

#else

		case ATMO_DATATYPE_STRING:
		{
			char *cData = ( char * )value->data;
			int retValue = ( int )strtol( cData, NULL, 0 );
			*output = retValue;
			break;
		}

#endif

		default:
			*output = 0;

			break;
	}

	return ATMO_Status_Success;
}

/**
 * This routine
 *
 * @param value :
 * @param output :
 * @return status.
 */
ATMO_Status_t ATMO_GetUnsignedInt( ATMO_Value_t *value, unsigned int *output )
{
	if ( output == NULL )
	{
		return ATMO_Status_Fail;
	}

	if ( value == NULL )
	{
		*output = 0;
		return ATMO_Status_NoInput;
	}

	if ( value->data == NULL )
	{
		*output = 0;
		return ATMO_Status_InvalidInput;
	}

	switch ( value->type )
	{

		case ATMO_DATATYPE_VOID:
			*output = 0;

			break;

		case ATMO_DATATYPE_BINARY:
		{
			if ( value->size < sizeof( int ) )
			{
				*output = 0;
				return ATMO_Status_InvalidInput;
			}

			memcpy( output, value->data, sizeof( unsigned int ) );
			break;
		}

		case ATMO_DATATYPE_CHAR:
		{

			char cData = 0;
			ATMO_GetChar( value, &cData );
			*output = ( unsigned int )cData;
			break;
		}

		case ATMO_DATATYPE_BOOL:
		{
			ATMO_BOOL_t boolData;
			ATMO_GetBool( value, &boolData );
			*output = ( unsigned int )boolData;
			break;
		}

		case ATMO_DATATYPE_INT:
		{
			int iData = 0;
			ATMO_GetInt( value, &iData );
			*output = ( unsigned int )iData;
			break;
		}

		case ATMO_DATATYPE_UNSIGNED_INT:
		{
			memcpy( output, value->data, sizeof( unsigned int ) );
			break;
		}

#ifndef ATMO_PLATFORM_NO_FLOAT_SUPPORT

		case ATMO_DATATYPE_FLOAT:
		{
			float floatData = 0;
			ATMO_GetFloat( value, &floatData );
			*output = ( unsigned int )floatData;
			break;
		}

		case ATMO_DATATYPE_3D_VECTOR_FLOAT:
		{
			ATMO_3dFloatVector_t vec;
			ATMO_Get3dVectorFloat( value, &vec );
			float mag = _ATMO_Calc3dFloatMagnitude( vec );
			*output = ( unsigned int )mag;
			break;
		}

#endif

#ifndef ATMO_PLATFORM_NO_DOUBLE_SUPPORT

		case ATMO_DATATYPE_DOUBLE:
		{
			double dblData = 0;
			ATMO_GetDouble( value, &dblData );
			*output = ( unsigned int )dblData;
			break;
		}

		case ATMO_DATATYPE_3D_VECTOR_DOUBLE:
		{
			ATMO_3dDoubleVector_t vec;
			ATMO_Get3dVectorDouble( value, &vec );
			double mag = _ATMO_Calc3dDoubleMagnitude( vec );
			*output = ( double )mag;
			break;
		}

#endif

#ifndef ATMO_PLATFORM_NO_SSCANF_SUPPORT

		case ATMO_DATATYPE_STRING:
		{
			unsigned int retValue = 0;
			sscanf( value->data, "%u", &retValue );
			*output = retValue;

			break;
		}

#else

		case ATMO_DATATYPE_STRING:
		{
			char *cData = ( char * )value->data;
#ifndef ATMO_NO_STRTOUL_SUPPORT
			unsigned int retValue = ( unsigned int )strtoul( cData, NULL, 0 );
#else
			unsigned int retValue = ( unsigned int )strtol( cData, NULL, 0 );
#endif
			*output = retValue;
			break;
		}

#endif

		default:
			*output = 0;

			break;
	}

	return ATMO_Status_Success;
}

/**
 * This routine
 *
 * @param value :
 * @param output :
 * @return status.
 */
ATMO_Status_t ATMO_GetFloat( ATMO_Value_t *value, float *output )
{
	if ( output == NULL )
	{
		return ATMO_Status_Fail;
	}

	if ( value == NULL )
	{
		*output = 0.0f;
		return ATMO_Status_NoInput;
	}

#ifndef ATMO_PLATFORM_NO_FLOAT_SUPPORT

	if ( value->data == NULL )
	{
		*output = 0.0f;
		return ATMO_Status_InvalidInput;
	}

	switch ( value->type )
	{

		case ATMO_DATATYPE_VOID:
			*output = 0.0f;

			break;

		case ATMO_DATATYPE_BINARY:
		{
			if ( value->size < sizeof( float ) )
			{
				*output = 0;
				return ATMO_Status_InvalidInput;
			}

			memcpy( output, value->data, sizeof( float ) );

			break;
		}

		case ATMO_DATATYPE_CHAR:
		{
			char cData = 0;
			ATMO_GetChar( value, &cData );
			int iData = ( int )cData;
			*output = ( float )iData;
			break;
		}

		case ATMO_DATATYPE_BOOL:
		{
			ATMO_BOOL_t bData = 0;
			ATMO_GetBool( value, &bData );
			int iData = ( int )bData;
			*output = ( float )iData;

			break;
		}

		case ATMO_DATATYPE_INT:
		{
			int iData = 0;
			ATMO_GetInt( value, &iData );
			*output = ( float )iData;

			break;
		}

		case ATMO_DATATYPE_UNSIGNED_INT:
		{
			unsigned int iData = 0;
			ATMO_GetUnsignedInt( value, &iData );
			*output = ( float )iData;

			break;
		}

#ifndef ATMO_PLATFORM_NO_FLOAT_SUPPORT

		case ATMO_DATATYPE_FLOAT:
		{
			memcpy( output, value->data, sizeof( float ) );
			break;
		}

		case ATMO_DATATYPE_3D_VECTOR_FLOAT:
		{
			ATMO_3dFloatVector_t vec;
			ATMO_Get3dVectorFloat( value, &vec );
			float mag = _ATMO_Calc3dFloatMagnitude( vec );
			*output = ( float )mag;
			break;
		}

#endif

#ifndef ATMO_PLATFORM_NO_DOUBLE_SUPPORT

		case ATMO_DATATYPE_DOUBLE:
		{
			double dData = 0;
			ATMO_GetDouble( value, &dData );
			*output = ( float )dData;

			break;
		}

		case ATMO_DATATYPE_3D_VECTOR_DOUBLE:
		{
			ATMO_3dDoubleVector_t vec;
			ATMO_Get3dVectorDouble( value, &vec );
			double mag = _ATMO_Calc3dDoubleMagnitude( vec );
			*output = ( float )mag;
			break;
		}

#endif

#ifndef ATMO_PLATFORM_NO_SSCANF_SUPPORT

		case ATMO_DATATYPE_STRING:
		{
			sscanf( value->data, "%f", output );

			break;
		}

#else

		case ATMO_DATATYPE_STRING:
		{
			char *cData = ( char * )value->data;
#ifndef ATMO_PLATFORM_NO_STRTOF_SUPPORT
			float retValue = ( float )strtof( cData, NULL );
#else
#ifndef ATMO_PLATFORM_NO_STRTOD_SUPPORT
			float retValue = (float)strtod(cData, NULL);
#else
			float retValue = ATMO_Strtof((const char *)cData);
#endif
#endif
			*output = retValue;
			break;
		}

#endif

		default:
			*output = 0.0f;

			break;
	}

#else
	*output = 0.0f;
	return ATMO_Status_MissingSupport;
#endif

	return ATMO_Status_Success;
}

/**
 * This routine
 *
 * @param value :
 * @param output :
 * @return status.
 */
ATMO_Status_t ATMO_GetDouble( ATMO_Value_t *value, double *output )
{
	if ( output == NULL )
	{
		return ATMO_Status_Fail;
	}

	if ( value == NULL )
	{
		*output = 0.0;
		return ATMO_Status_NoInput;
	}

#ifndef ATMO_PLATFORM_NO_DOUBLE_SUPPORT

	if ( value->data == NULL )
	{
		*output = 0.0;
	}

	switch ( value->type )
	{

		case ATMO_DATATYPE_VOID:
			*output = 0.0;
			break;

		case ATMO_DATATYPE_BINARY:
		{
			if ( value->size < sizeof( double ) )
			{
				*output = 0;
				return ATMO_Status_InvalidInput;
			}

			memcpy( output, value->data, sizeof( double ) );
			break;
		}

		case ATMO_DATATYPE_CHAR:
		{
			char cData = 0;
			ATMO_GetChar( value, &cData );
			int iData = ( int )cData;
			*output = ( double )iData;
			break;
		}

		case ATMO_DATATYPE_BOOL:
		{
			ATMO_BOOL_t bData = false;
			ATMO_GetBool( value, &bData );
			int iData = ( int )bData;
			*output = ( double )iData;
			break;
		}

		case ATMO_DATATYPE_INT:
		{
			int iData = 0;
			ATMO_GetInt( value, &iData );
			*output = ( double )( iData );
			break;
		}

		case ATMO_DATATYPE_UNSIGNED_INT:
		{
			unsigned int iData = 0;
			ATMO_GetUnsignedInt( value, &iData );
			*output = ( double )( iData );
			break;
		}

#ifndef ATMO_PLATFORM_NO_FLOAT_SUPPORT

		case ATMO_DATATYPE_FLOAT:
		{
			float fData = 0;
			ATMO_GetFloat( value, &fData );
			*output = ( double )( fData );
			break;
		}

		case ATMO_DATATYPE_3D_VECTOR_FLOAT:
		{
			ATMO_3dFloatVector_t vec;
			ATMO_Get3dVectorFloat( value, &vec );
			float mag = _ATMO_Calc3dFloatMagnitude( vec );
			*output = ( double )mag;
			break;
		}

#endif

#ifndef ATMO_PLATFORM_NO_DOUBLE_SUPPORT

		case ATMO_DATATYPE_DOUBLE:
		{
			memcpy( output, value->data, sizeof( double ) );
			break;
		}

		case ATMO_DATATYPE_3D_VECTOR_DOUBLE:
		{
			ATMO_3dDoubleVector_t vec;
			ATMO_Get3dVectorDouble( value, &vec );
			double mag = _ATMO_Calc3dDoubleMagnitude( vec );
			*output = ( double )mag;
			break;
		}

#endif

#ifndef ATMO_PLATFORM_NO_SSCANF_SUPPORT

		case ATMO_DATATYPE_STRING:
		{
			sscanf( value->data, "%lf", output );
			break;
		}

#else

		case ATMO_DATATYPE_STRING:
		{
			double retValue = /*(double)strtod(value->data, NULL);*/ 0;
			*output = retValue;
			break;
		}

#endif

		default:
			*output = 0.0;
			break;
	}

#else
	*output = 0.0;

#endif

	return ATMO_Status_Success;
}

/**
 * This routine
 *
 * @param value :
 * @param buffer :
 * @param size :
 * @return status.
 */
ATMO_Status_t ATMO_GetString( ATMO_Value_t *value, char *buffer, unsigned int size )
{
	if ( buffer == NULL || size < 1 )
	{
		return ATMO_Status_Fail;
	}

	if ( value == NULL )
	{
		buffer[0] = '\x00';
		return ATMO_Status_NoInput;
	}

#ifndef ATMO_PLATFORM_NO_SPRINTF_SUPPORT

	int cx;

	switch ( value->type )
	{

		case ATMO_DATATYPE_VOID:
		{
			buffer[0] = '\x00';
			return ATMO_Status_Success;
		}

		case ATMO_DATATYPE_BINARY:
		{
			char *cData = ( char * )value->data;
			strncpy( buffer, cData, size );

			break;
		}

		case ATMO_DATATYPE_CHAR:
		{
			if ( !( size > 1 ) )
			{
				buffer[0] = '\x00';
				return ATMO_Status_OutOfMemory;
			}

			char cData;
			ATMO_GetChar( value, &cData );

			buffer[0] = cData;
			buffer[1] = '\x00';
			return ATMO_Status_Success;
		}

		case ATMO_DATATYPE_BOOL:
		{
			ATMO_BOOL_t bData = false;
			ATMO_GetBool( value, &bData );

			if ( bData )
			{
				cx = snprintf( buffer, size, "true" );
			}

			else
			{
				cx = snprintf( buffer, size, "false" );
			}

			if ( cx < 0 || cx >= size )
			{
				return ATMO_Status_OutOfMemory;
			}

			return ATMO_Status_Success;
		}

		case ATMO_DATATYPE_INT:
		{
			int iData = 0;
			ATMO_GetInt( value, &iData );
			cx = snprintf( buffer, size, "%d", iData );

			if ( cx < 0 || cx >= size )
			{
				return ATMO_Status_OutOfMemory;
			}

			return ATMO_Status_Success;
		}

		case ATMO_DATATYPE_UNSIGNED_INT:
		{
			unsigned int iData = 0;
			ATMO_GetUnsignedInt( value, &iData );
			cx = snprintf( buffer, size, "%u", iData );

			if ( cx < 0 || cx >= size )
			{
				return ATMO_Status_OutOfMemory;
			}

			return ATMO_Status_Success;
		}

		case ATMO_DATATYPE_FLOAT:
		{
			float fData = 0;
			ATMO_GetFloat( value, &fData );

#ifndef ATMO_NO_SPRINTF_FLOAT_SUPPORT
			cx = snprintf( buffer, size, "%.2f", fData );
#elif defined(ATMO_NO_DTOSTRF_SUPPORT)
			long fixedPointNum = ( long )( fData * 100 ); // 2 decimals
			cx = sprintf( buffer, "%ld.%lu", ( long )fixedPointNum / 100, ( unsigned long )_ATMO_Abs( fixedPointNum ) % 100 );
#else
			dtostrf( fData, 1, 2, buffer );
#endif

			if ( cx < 0 || cx >= size )
			{
				return ATMO_Status_OutOfMemory;
			}

			return ATMO_Status_Success;
		}

		case ATMO_DATATYPE_DOUBLE:
		{
			double dData = 0;
			ATMO_GetDouble( value, &dData );
#ifndef ATMO_NO_SPRINTF_FLOAT_SUPPORT
			cx = snprintf( buffer, size, "%.2lf", dData );
#elif defined(ATMO_NO_DTOSTRF_SUPPORT)
			long fixedPointNum = ( long )( dData * 100 ); // 2 decimals
			cx = sprintf( buffer, "%ld.%lu", ( long )fixedPointNum / 100, ( unsigned long )_ATMO_Abs( fixedPointNum ) % 100 );
#else
			dtostrf( dData, 1, 2, buffer );
#endif

			if ( cx < 0 || cx >= size )
			{
				return ATMO_Status_OutOfMemory;
			}

			return ATMO_Status_Success;
		}

		case ATMO_DATATYPE_STRING:
		{
			char *cData = ( char * )value->data;
			strncpy( buffer, cData, size );
			return ATMO_Status_Success;
		}

		case ATMO_DATATYPE_3D_VECTOR_FLOAT:
		{
			ATMO_3dFloatVector_t vec;
			ATMO_Get3dVectorFloat( value, &vec );
#ifndef ATMO_NO_SPRINTF_FLOAT_SUPPORT
			snprintf( buffer, size, "{\"x\": %.2f, \"y\": %.2f, \"z\": %.2f}", vec.x, vec.y, vec.z );
#else
			char xBuf[32], yBuf[32], zBuf[32];
			int fixedPointNum = ( int )( vec.x * 100 ); // 2 decimals
			sprintf( xBuf, "%d.%u", fixedPointNum / 100, ( unsigned int )( _ATMO_Abs( fixedPointNum ) % 100 ) );
			fixedPointNum = ( int )( vec.y * 100 ); // 2 decimals
			sprintf( yBuf, "%d.%u", fixedPointNum / 100, ( unsigned int )( _ATMO_Abs( fixedPointNum ) % 100 ) );
			fixedPointNum = ( int )( vec.z * 100 ); // 2 decimals
			sprintf( zBuf, "%d.%u", fixedPointNum / 100, ( unsigned int )( _ATMO_Abs( fixedPointNum ) % 100 ) );
			snprintf( buffer, size, "{\"x\": %s, \"y\": %s, \"z\": %s}", xBuf, yBuf, zBuf );
#endif
			return ATMO_Status_Success;
		}

		case ATMO_DATATYPE_3D_VECTOR_DOUBLE:
		{
			ATMO_3dDoubleVector_t vec;
			ATMO_Get3dVectorDouble( value, &vec );
#ifndef ATMO_NO_SPRINTF_FLOAT_SUPPORT
			snprintf( buffer, size, "{\"x\": %.2f, \"y\": %.2f, \"z\": %.2f}", vec.x, vec.y, vec.z );
#else
			char xBuf[32], yBuf[32], zBuf[32];
			int fixedPointNum = ( int )( vec.x * 100 ); // 2 decimals
			sprintf( xBuf, "%d.%u", fixedPointNum / 100, ( unsigned int )( _ATMO_Abs( fixedPointNum ) % 100 ) );
			fixedPointNum = ( int )( vec.y * 100 ); // 2 decimals
			sprintf( yBuf, "%d.%u", fixedPointNum / 100, ( unsigned int )( _ATMO_Abs( fixedPointNum ) % 100 ) );
			fixedPointNum = ( int )( vec.z * 100 ); // 2 decimals
			sprintf( zBuf, "%d.%u", fixedPointNum / 100, ( unsigned int )( _ATMO_Abs( fixedPointNum ) % 100 ) );
			snprintf( buffer, size, "{\"x\": %s, \"y\": %s, \"z\": %s}", xBuf, yBuf, zBuf );
#endif
			return ATMO_Status_Success;
		}

		default:
			buffer[0] = '\x00';
			return ATMO_Status_Success;
	}

#else
	buffer[0] = '\x00';
	return ATMO_Status_MissingSupport;
#endif

	return ATMO_Status_MissingSupport;
}

ATMO_Status_t ATMO_GetBinary( ATMO_Value_t *value, void *buffer, unsigned int size )
{
	// Just a straight up memcpy
	if ( size < value->size )
	{
		return ATMO_Status_Fail;
	}

	memcpy( buffer, value->data, size );
	return ATMO_Status_Success;
}

ATMO_Status_t ATMO_Get3dVectorFloat( ATMO_Value_t *value, ATMO_3dFloatVector_t *vector )
{
	memset( vector, 0, sizeof( ATMO_3dFloatVector_t ) );

	switch ( value->type )
	{
		case ATMO_DATATYPE_3D_VECTOR_FLOAT:
		{
			memcpy( vector, value->data, sizeof( ATMO_3dFloatVector_t ) );
			break;
		}

		case ATMO_DATATYPE_3D_VECTOR_DOUBLE:
		{
			ATMO_3dDoubleVector_t dbVector;
			ATMO_Get3dVectorDouble( value, &dbVector );
			vector->x = dbVector.x;
			vector->y = dbVector.y;
			vector->z = dbVector.z;
			break;
		}

		case ATMO_DATATYPE_BINARY:
		{
			if ( value->size != 12 )
			{
				return ATMO_Status_Fail;
			}

			memcpy( vector, value->data, 12 );
			break;
		}

		case ATMO_DATATYPE_FLOAT:
		{
			float val;
			ATMO_GetFloat( value, &val );
			vector->x = val;
			break;
		}

		case ATMO_DATATYPE_DOUBLE:
		{
			double val;
			ATMO_GetDouble( value, &val );
			vector->x = ( float )val;
			break;
		}

		case ATMO_DATATYPE_INT:
		{
			int val;
			ATMO_GetInt( value, &val );
			vector->x = ( float )val;
			break;
		}

		case ATMO_DATATYPE_UNSIGNED_INT:
		{
			unsigned int val;
			ATMO_GetUnsignedInt( value, &val );
			vector->x = ( float )val;
			break;
		}

		case ATMO_DATATYPE_CHAR:
		{
			char val;
			ATMO_GetChar( value, &val );
			vector->x = ( float )val;
			break;
		}

		default:
		{
			return ATMO_Status_Fail;
		}
	}

	return ATMO_Status_Success;
}

ATMO_Status_t ATMO_Get3dVectorDouble( ATMO_Value_t *value, ATMO_3dDoubleVector_t *vector )
{
	memset( vector, 0, sizeof( ATMO_3dDoubleVector_t ) );

	switch ( value->type )
	{
		case ATMO_DATATYPE_3D_VECTOR_DOUBLE:
		{
			memcpy( vector, value->data, sizeof( ATMO_3dDoubleVector_t ) );
			break;
		}

		case ATMO_DATATYPE_3D_VECTOR_FLOAT:
		{
			ATMO_3dFloatVector_t flVector;
			ATMO_Get3dVectorFloat( value, &flVector );
			vector->x = flVector.x;
			vector->y = flVector.y;
			vector->z = flVector.z;
			break;
		}

		case ATMO_DATATYPE_BINARY:
		{
			if ( value->size != 24 )
			{
				return ATMO_Status_Fail;
			}

			memcpy( vector, value->data, 24 );
			break;
		}

		case ATMO_DATATYPE_FLOAT:
		{
			float val;
			ATMO_GetFloat( value, &val );
			vector->x = ( double )val;
			break;
		}

		case ATMO_DATATYPE_DOUBLE:
		{
			double val;
			ATMO_GetDouble( value, &val );
			vector->x = ( double )val;
			break;
		}

		case ATMO_DATATYPE_INT:
		{
			int val;
			ATMO_GetInt( value, &val );
			vector->x = ( double )val;
			break;
		}

		case ATMO_DATATYPE_UNSIGNED_INT:
		{
			unsigned int val;
			ATMO_GetUnsignedInt( value, &val );
			vector->x = ( double )val;
			break;
		}

		case ATMO_DATATYPE_CHAR:
		{
			char val;
			ATMO_GetChar( value, &val );
			vector->x = ( double )val;
			break;
		}

		default:
		{
			return ATMO_Status_Fail;
		}
	}

	return ATMO_Status_Success;
}

ATMO_Status_t ATMO_CompareValuesLessThan( ATMO_Value_t *valueA, ATMO_Value_t *valueB, ATMO_BOOL_t *result )
{
	if ( ( valueA->type != valueB->type ) || !__ATMO_IsComparableType( valueA ) || !__ATMO_IsComparableType( valueB ) )
	{
		return ATMO_Status_Fail;
	}

	switch ( valueA->type )
	{
		case ATMO_DATATYPE_CHAR:
		case ATMO_DATATYPE_UNSIGNED_INT:
		case ATMO_DATATYPE_BOOL:
		{
			unsigned int a, b;
			ATMO_GetUnsignedInt( valueA, &a );
			ATMO_GetUnsignedInt( valueB, &b );
			*result = a < b;
			return ATMO_Status_Success;
		}

		case ATMO_DATATYPE_INT:
		{
			int a, b;
			ATMO_GetInt( valueA, &a );
			ATMO_GetInt( valueB, &b );
			*result = a < b;
			return ATMO_Status_Success;
		}

		case ATMO_DATATYPE_FLOAT:
		case ATMO_DATATYPE_DOUBLE:
		{
			double a, b;
			ATMO_GetDouble( valueA, &a );
			ATMO_GetDouble( valueB, &b );
			*result = a < b;
			return ATMO_Status_Success;
		}

		case ATMO_DATATYPE_STRING:
		{
			*result = strcmp( ( char * )valueA->data, ( char * )valueB->data ) < 0;
			break;
		}

		default:
		{
			return ATMO_Status_Fail;
		}
	}

	return ATMO_Status_Success;
}

ATMO_Status_t ATMO_CompareValuesEqual( ATMO_Value_t *valueA, ATMO_Value_t *valueB, ATMO_BOOL_t *result )
{
	if ( ( valueA->type != valueB->type ) || !__ATMO_IsComparableType( valueA ) || !__ATMO_IsComparableType( valueB ) )
	{
		return ATMO_Status_Fail;
	}

	// Do a straight memory comparison
	*result = memcmp( valueA->data, valueB->data, valueA->size ) == 0;

	return ATMO_Status_Success;
}

ATMO_Status_t ATMO_CompareValuesLessThanEqual( ATMO_Value_t *valueA, ATMO_Value_t *valueB, ATMO_BOOL_t *result )
{
	if ( ( valueA->type != valueB->type ) || !__ATMO_IsComparableType( valueA ) || !__ATMO_IsComparableType( valueB ) )
	{
		return ATMO_Status_Fail;
	}

	ATMO_BOOL_t isLessThan = false;
	ATMO_BOOL_t isEqual = false;

	ATMO_CompareValuesLessThan( valueA, valueB, &isLessThan );

	if ( !isLessThan )
	{
		ATMO_CompareValuesEqual( valueA, valueB, &isEqual );
	}

	*result = isLessThan || isEqual;

	return ATMO_Status_Success;

}

ATMO_Status_t ATMO_CompareValuesGreaterThan( ATMO_Value_t *valueA, ATMO_Value_t *valueB, ATMO_BOOL_t *result )
{
	if ( ( valueA->type != valueB->type ) || !__ATMO_IsComparableType( valueA ) || !__ATMO_IsComparableType( valueB ) )
	{
		return ATMO_Status_Fail;
	}

	ATMO_BOOL_t isLessThan = false;
	ATMO_BOOL_t isEqual = false;

	ATMO_CompareValuesLessThan( valueA, valueB, &isLessThan );
	ATMO_CompareValuesEqual( valueA, valueB, &isEqual );

	*result = ( !isLessThan ) && ( !isEqual );

	return ATMO_Status_Success;
}

ATMO_Status_t ATMO_CompareValuesGreaterThanEqual( ATMO_Value_t *valueA, ATMO_Value_t *valueB, ATMO_BOOL_t *result )
{
	if ( ( valueA->type != valueB->type ) || !__ATMO_IsComparableType( valueA ) || !__ATMO_IsComparableType( valueB ) )
	{
		return ATMO_Status_Fail;
	}

	ATMO_BOOL_t isLessThan = false;

	ATMO_CompareValuesLessThan( valueA, valueB, &isLessThan );

	*result = ( !isLessThan );

	return ATMO_Status_Success;
}

ATMO_Status_t ATMO_CompareValues( ATMO_Value_t *valueA, ATMO_Value_t *valueB, ATMO_CompareCondition_t condition, ATMO_BOOL_t *result )
{
	switch ( condition )
	{
		case ATMO_LESS_THAN:
		{
			return ATMO_CompareValuesLessThan( valueA, valueB, result );
		}

		case ATMO_LESS_THAN_EQUAL:
		{
			return ATMO_CompareValuesLessThanEqual( valueA, valueB, result );
		}

		case ATMO_GREATER_THAN:
		{
			return ATMO_CompareValuesGreaterThan( valueA, valueB, result );
		}

		case ATMO_GREATER_THAN_EQUAL:
		{
			return ATMO_CompareValuesGreaterThanEqual( valueA, valueB, result );
		}

		case ATMO_EQUAL:
		{
			return ATMO_CompareValuesEqual( valueA, valueB, result );
		}

		default:
		{
			return ATMO_Status_Fail;
		}
	}

	return ATMO_Status_Fail;
}

/**
 * Probably could use some C Macro gore here, but it's not worth it.
 *
 * This is easy to read, understand, and debug.
 */
char __ATMO_PerformOperationChar( char c, ATMO_Operator_t operation, float operand )
{
	switch ( operation )
	{
		case ATMO_OPERATOR_ADD:
		{
			return c + ( char )operand;
		}

		case ATMO_OPERATOR_SUBTRACT:
		{
			return c - ( char )operand;
		}

		case ATMO_OPERATOR_MULTIPLY:
		{
			return c * ( char )operand;
		}

		case ATMO_OPERATOR_DIVIDE:
		{
			return c / ( char )operand;
		}

		default:
		{
			return c;
		}
	}
}

unsigned int __ATMO_PerformOperationUnsignedInt( unsigned int c, ATMO_Operator_t operation, float operand )
{
	switch ( operation )
	{
		case ATMO_OPERATOR_ADD:
		{
			return c + ( unsigned int )operand;
		}

		case ATMO_OPERATOR_SUBTRACT:
		{
			return c - ( unsigned int )operand;
		}

		case ATMO_OPERATOR_MULTIPLY:
		{
			return c * ( unsigned int )operand;
		}

		case ATMO_OPERATOR_DIVIDE:
		{
			return c / ( unsigned int )operand;
		}

		default:
		{
			return c;
		}
	}
}

float __ATMO_PerformOperationFloat( float c, ATMO_Operator_t operation, float operand )
{
	switch ( operation )
	{
		case ATMO_OPERATOR_ADD:
		{
			return c + operand;
		}

		case ATMO_OPERATOR_SUBTRACT:
		{
			return c - operand;
		}

		case ATMO_OPERATOR_MULTIPLY:
		{
			return c * operand;
		}

		case ATMO_OPERATOR_DIVIDE:
		{
			return c / operand;
		}

		default:
		{
			return c;
		}
	}
}

double __ATMO_PerformOperationDouble( double c, ATMO_Operator_t operation, float operand )
{
	switch ( operation )
	{
		case ATMO_OPERATOR_ADD:
		{
			return c + operand;
		}

		case ATMO_OPERATOR_SUBTRACT:
		{
			return c - operand;
		}

		case ATMO_OPERATOR_MULTIPLY:
		{
			return c * operand;
		}

		case ATMO_OPERATOR_DIVIDE:
		{
			return c / operand;
		}

		default:
		{
			return c;
		}
	}
}

int __ATMO_PerformOperationInt( int c, ATMO_Operator_t operation, float operand )
{
	switch ( operation )
	{
		case ATMO_OPERATOR_ADD:
		{
			return c + ( int )operand;
		}

		case ATMO_OPERATOR_SUBTRACT:
		{
			return c - ( int )operand;
		}

		case ATMO_OPERATOR_MULTIPLY:
		{
			return c * ( int )operand;
		}

		case ATMO_OPERATOR_DIVIDE:
		{
			return c / ( int )operand;
		}

		default:
		{
			return c;
		}
	}
}

ATMO_Status_t ATMO_PerformOperation( ATMO_Value_t *newValue, ATMO_Operator_t operation, float operand, ATMO_Value_t *oldValue )
{
	if ( !__ATMO_IsNumericalType( oldValue ) )
	{
		return ATMO_Status_Fail;
	}

	ATMO_InitValue( newValue );

	switch ( oldValue->type )
	{
		case ATMO_DATATYPE_CHAR:
		{
			char a;
			ATMO_GetChar( oldValue, &a );
			ATMO_CreateValueChar( newValue, __ATMO_PerformOperationChar( a, operation, operand ) );
			break;
		}

		case ATMO_DATATYPE_UNSIGNED_INT:
		{
			unsigned int a;
			ATMO_GetUnsignedInt( oldValue, &a );
			ATMO_CreateValueUnsignedInt( newValue, __ATMO_PerformOperationUnsignedInt( a, operation, operand ) );
			break;
		}

		case ATMO_DATATYPE_INT:
		{
			int a;
			ATMO_GetInt( oldValue, &a );
			ATMO_CreateValueInt( newValue, __ATMO_PerformOperationInt( a, operation, operand ) );
			break;
		}

		case ATMO_DATATYPE_FLOAT:
		{
			float a;
			ATMO_GetFloat( oldValue, &a );
			ATMO_CreateValueFloat( newValue, __ATMO_PerformOperationFloat( a, operation, operand ) );
			break;
		}

		case ATMO_DATATYPE_DOUBLE:
		{
			double a;
			ATMO_GetDouble( oldValue, &a );
			ATMO_CreateValueDouble( newValue, __ATMO_PerformOperationDouble( a, operation, operand ) );
			break;
		}

		default:
		{
			return ATMO_Status_Fail;
		}
	}

	return ATMO_Status_Success;
}


ATMO_Status_t ATMO_StringToUuid( const char *str, ATMO_UUID_t *uuid, ATMO_ENDIAN_Type_t endianness )
{
	uint16_t lenBytes = 0;
	uint8_t uuidTmp[16] = {0};
	char byteStr[3] = {0};
	unsigned int strLen = strlen( str );

	unsigned int i = 0;

	while ( i < strLen )
	{
		// Skip hyphens
		if ( str[i] == '-' )
		{
			i++;
		}
		else
		{
			// Convert byte to string, then to an integer
			byteStr[0] = str[i];
			byteStr[1] = str[i + 1];
			byteStr[2] = 0;
			uint8_t c = strtol( byteStr, NULL, 16 );
			uuidTmp[lenBytes++] = c;
			i += 2;
		}
	}

	if ( endianness == ATMO_ENDIAN_Type_Little )
	{
		// Copy reversed array
		unsigned int j;

		for ( j = 0; j < lenBytes; j++ )
		{
			uuid->data[j] = uuidTmp[lenBytes - 1 - j];
		}
	}
	else
	{
		memcpy( uuid->data, uuidTmp, lenBytes );
	}


	switch ( lenBytes )
	{
		case 2:
		{
			uuid->type = ATMO_UUID_Type_16_Bit;
			break;
		}

		case 4:
		{
			uuid->type = ATMO_UUID_Type_32_Bit;
			break;
		}

		case 16:
		{
			uuid->type = ATMO_UUID_Type_128_Bit;
			break;
		}

		default:
		{
			uuid->type = ATMO_UUID_Type_Invalid;
			break;
		}
	}

	uuid->endianness = endianness;

	return ATMO_Status_Success;
}

/**
 * Convert a UUID from binary data to a formatted string
 *
 * @param[in] uuid
 * @param[out] str
 * @param[in] strBufLen - Length of passed in string buffer
 * @return status
 */
ATMO_Status_t ATMO_UuidToString( const ATMO_UUID_t *uuid, char *str, unsigned int strBufLen )
{
	if ( uuid->type == ATMO_UUID_Type_16_Bit )
	{
		if ( strBufLen < 5 )
		{
			return ATMO_Status_Fail;
		}

		if ( uuid->endianness == ATMO_ENDIAN_Type_Little )
		{
			sprintf( str, "%02x%02x", uuid->data[1], uuid->data[0] );
		}
		else
		{
			sprintf( str, "%02x%02x", uuid->data[0], uuid->data[1] );
		}
	}
	else if ( uuid->type == ATMO_UUID_Type_32_Bit )
	{
		if ( strBufLen < 9 )
		{
			return ATMO_Status_Fail;
		}

		if ( uuid->endianness == ATMO_ENDIAN_Type_Little )
		{
			sprintf( str, "%02x%02x%02x%02x", uuid->data[3], uuid->data[2], uuid->data[1], uuid->data[0] );
		}
		else
		{
			sprintf( str, "%02x%02x%02x%02x", uuid->data[0], uuid->data[1], uuid->data[2], uuid->data[3] );
		}
	}
	else if ( uuid->type == ATMO_UUID_Type_128_Bit )
	{
		if ( strBufLen < 37 )
		{
			return ATMO_Status_Fail;
		}

		if ( uuid->endianness == ATMO_ENDIAN_Type_Little )
		{
			sprintf( str,
			         "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
			         uuid->data[15], uuid->data[14], uuid->data[13], uuid->data[12], uuid->data[11], uuid->data[10], uuid->data[9], uuid->data[8],
			         uuid->data[7], uuid->data[6], uuid->data[5], uuid->data[4], uuid->data[3], uuid->data[2], uuid->data[1], uuid->data[0]
			       );
		}
		else
		{
			sprintf( str,
			         "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
			         uuid->data[0], uuid->data[1], uuid->data[2], uuid->data[3], uuid->data[4], uuid->data[5], uuid->data[6], uuid->data[7],
			         uuid->data[8], uuid->data[9], uuid->data[10], uuid->data[11], uuid->data[12], uuid->data[13], uuid->data[14], uuid->data[15]
			       );
		}
	}
	else
	{
		return ATMO_Status_Fail;
	}

	return ATMO_Status_Success;
}

ATMO_Status_t ATMO_DelayMillisecondsNonBlock( unsigned int milliseconds )
{
	unsigned int leftOver = milliseconds % 200;
	unsigned int numLoops = milliseconds / 200;
	unsigned int i;

	for ( i = 0; i < numLoops; i++ )
	{
		ATMO_PLATFORM_DelayMilliseconds( 200 );
		ATMO_Tick();
	}

	ATMO_PLATFORM_DelayMilliseconds( leftOver );

	if ( numLoops == 0 )
	{
		ATMO_Tick();
	}

	return ATMO_Status_Success;
}

static ATMO_BOOL_t ATMO_IsPrintable(uint8_t c)
{
	ATMO_BOOL_t isLowerAlpha = (c >= 'a' && c <= 'z');
	ATMO_BOOL_t isUpperAlpha = (c >= 'A' && c <= 'Z');
	ATMO_BOOL_t isNum = (c >= '0' && c <= '9');
	return isLowerAlpha || isUpperAlpha || isNum;
}

void ATMO_PrintBytes( uint8_t *bytes, uint32_t len )
{
	unsigned int i;

	for ( i = 0; i < len; i++ )
	{
		if(ATMO_IsPrintable(bytes[i]))
		{
			ATMO_PLATFORM_DebugPrint( "Data[%04d]: %02X (%c)\r\n", i, bytes[i], (char)bytes[i] );
		}
		else
		{
			ATMO_PLATFORM_DebugPrint( "Data[%04d]: %02X\r\n", i, bytes[i] );
		}
		
		
	}
}
