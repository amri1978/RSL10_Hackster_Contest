#ifndef _COUNTER_NIMBUS_H_
#define _COUNTER_NIMBUS_H_

#include "../app_src/atmosphere_platform.h"

/**
 * @brief Initialize counter
 * 
 * @param counter - Counter value
 * @param initialValue - Initial value integer
 */
void ATMO_CounterInit(ATMO_Value_t *counter, int initialValue);

/**
 * @brief Increment counter by 1
 * 
 * @param counter 
 * @param initialValue 
 * @param rolloverValue 
 * @param rollover 
 * @param resetToInitial 
 * @return ATMO_BOOL_t true if reset occured
 */
ATMO_BOOL_t ATMO_CounterIncrement(ATMO_Value_t *counter, int initialValue, int rolloverValue, ATMO_BOOL_t rollover, ATMO_BOOL_t resetToInitial);

/**
 * @brief Decrement counter by 1
 * 
 * @param counter 
 * @param initialValue 
 * @param rolloverValue 
 * @param rollover 
 * @param resetToInitial 
 * @return ATMO_BOOL_t true if reset occured
 */
ATMO_BOOL_t ATMO_CounterDecrement(ATMO_Value_t *counter, int initialValue, int rolloverValue, ATMO_BOOL_t rollover, ATMO_BOOL_t resetToInitial);

/**
 * @brief Get current value of counter
 * 
 * @param counter 
 * @return int - current value of counter
 */
int ATMO_CounterGetValue(ATMO_Value_t *counter);

/**
 * @brief Set the value of the counter
 * 
 * @param counter 
 * @param value 
 * @return void 
 */
void ATMO_CounterSetValue(ATMO_Value_t *counter, int value);

#endif