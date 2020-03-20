#include "counter_atmo.h"

#define ATMO_COUNTER_DIR_INCREMENT true
#define ATMO_COUNTER_DIR_DECREMENT false

void ATMO_CounterInit( ATMO_Value_t *counter, int initialValue )
{
	ATMO_InitValue( counter );
	ATMO_CreateValueInt( counter, initialValue );
}

static ATMO_BOOL_t ATMO_CounterModify( ATMO_Value_t *counter, int initialValue, int rolloverValue, ATMO_BOOL_t rollover, ATMO_BOOL_t resetToInitial, ATMO_BOOL_t direction )
{
	ATMO_BOOL_t resetOccured = false;
	int currentValue = ATMO_CounterGetValue( counter );

	currentValue = (direction) ? (currentValue + 1) : (currentValue - 1);

	// If rollover is enabled
	if ( rollover )
	{
		if ( currentValue == rolloverValue )
		{
			currentValue = resetToInitial ? initialValue : 0;
			resetOccured = true;
		}
	}

	ATMO_CounterSetValue( counter, currentValue );
	return resetOccured;
}

ATMO_BOOL_t ATMO_CounterIncrement( ATMO_Value_t *counter, int initialValue, int rolloverValue, ATMO_BOOL_t rollover, ATMO_BOOL_t resetToInitial )
{
	return ATMO_CounterModify(counter, initialValue, rolloverValue, rollover, resetToInitial, ATMO_COUNTER_DIR_INCREMENT);
}

ATMO_BOOL_t ATMO_CounterDecrement( ATMO_Value_t *counter, int initialValue, int rolloverValue, ATMO_BOOL_t rollover, ATMO_BOOL_t resetToInitial )
{
	return ATMO_CounterModify(counter, initialValue, rolloverValue, rollover, resetToInitial, ATMO_COUNTER_DIR_DECREMENT);
}

int ATMO_CounterGetValue( ATMO_Value_t *counter )
{
	int value = 0;
	ATMO_GetInt( counter, &value );
	return value;
}

void ATMO_CounterSetValue( ATMO_Value_t *counter, int value )
{
	ATMO_FreeValue( counter );
	ATMO_InitValue( counter );
	ATMO_CreateValueInt( counter, value );
}