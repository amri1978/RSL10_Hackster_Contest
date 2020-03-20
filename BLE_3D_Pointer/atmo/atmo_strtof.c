/*
 * System Interface Library for games
 * Copyright (c) 2007-2019 Andrew Church <achurch@achurch.org>
 * Released under the GNU GPL version 3 or later; NO WARRANTY is provided.
 * See the file COPYING.txt for details.
 *
 * src/utility/strtof.c: Custom implementation of the standard strtof()
 * function which uses only single-precision arithmetic.  This
 * implementation does not accept hexadecimal numbers or NaNs, and may
 * introduce a slight amount of error (on the order of 1 part in 10^7) in
 * the least-significant digits of the mantissa.
 */

#include "atmo_strtof.h"
#include "core.h"

#ifdef ATMO_PLATFORM_NO_STRTOF_SUPPORT
/*************************************************************************/
/*************************************************************************/

static uint32_t ATMO_Pow10(uint32_t nPow)
{
    uint32_t result = 1;
    
    while(nPow != 0)
    {
        result*=10;
        nPow--;
    }
    
    return result;
}

/**
 * @brief This is a dumb last resort version of strtof. Error checking is probably not bulletproof.
 * 
 * @param s 
 * @param endptr 
 * @return float 
 */
float ATMO_Strtof(const char *s)
{
	float wholeNum = 0;
	uint32_t currentDecimal = 1;
	const char *c = s;
	ATMO_BOOL_t isWhole = true;

	while(*c)
	{
		// If after decimal, start dividing
		if((*c) == '.')
		{
			if(isWhole == false)
			{
				// There are two decimals in the string???
				return 0;
			}

			isWhole = false;
		}
		else
		{
			uint8_t digitVal = *c - '0';
			if(digitVal > 9)
			{
				// Error
				return 0;
			}

			if(isWhole)
			{
				wholeNum = (wholeNum * 10) + digitVal;
			}
			else
			{
			    float pow10 = (float)ATMO_Pow10(currentDecimal);
				wholeNum += (digitVal/pow10);
				currentDecimal++;
			}

			// Overflow!
			if(currentDecimal == 0)
			{
				return 0;
			}
			
		}
		c++;
	}

	return wholeNum;
}

#endif