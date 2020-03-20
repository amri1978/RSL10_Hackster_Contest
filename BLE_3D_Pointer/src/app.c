//-----------------------------------------------------------------------------
// Copyright (c) 2018 Semiconductor Components Industries LLC
// (d/b/a "ON Semiconductor").  All rights reserved.
// This software and/or documentation is licensed by ON Semiconductor under
// limited terms and conditions.  The terms and conditions pertaining to the
// software and/or documentation are available at
// http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf ("ON Semiconductor Standard
// Terms and Conditions of Sale, Section 8 Software") and if applicable the
// software license agreement.  Do not use this software and/or documentation
// unless you have carefully read and you agree to the limited terms and
// conditions.  By using this software and/or documentation, you agree to the
// limited terms and conditions.
//-----------------------------------------------------------------------------
#include <stdio.h>

#include "app.h"
#include "../app_src/atmosphere_platform.h"

enum App_StateStruct app_state = APP_STATE_INIT;

int main(void)
{
	SEGGER_RTT_Init();
	TRACE_PRINTF("Hello world\r\n");
    Device_Initialize();

    /* Indication - Initialization complete. */
    LED_On(LED_GREEN);
    HAL_Delay(250);
    LED_Off(LED_GREEN);

	ATMO_Init();

    Main_Loop();
}

static void App_StateMachine(void)
{
	static enum App_StateStruct last_app_state = APP_STATE_INIT;
	static bool initialized = false;

#ifndef SLEEP_ENABLED
	if(!initialized || last_app_state != app_state)
	{
		initialized = true;
		last_app_state = app_state;
#endif
		switch (app_state)
		{
		case APP_STATE_INIT:
			TRACE_PRINTF("State: Init\r\n");
			/* no break */
		case APP_STATE_START_ADVERTISING:
			TRACE_PRINTF("State: Advertising start\r\n");
			_ATMO_ONSEMI_BLE_SetInitComplete();
			app_state = APP_STATE_ADVERTISING;
			break;

		case APP_STATE_ADVERTISING:
			TRACE_PRINTF("State: Advertising\r\n");
			break;

		case APP_STATE_SLEEP:
			TRACE_PRINTF("State: Sleep\r\n");
			break;

		case APP_STATE_START_CONNECTION:
			TRACE_PRINTF("State: Start connection\r\n");


			// Switch to connected state.
			app_state = APP_STATE_CONNECTED;
			break;

		case APP_STATE_CONNECTED:
			TRACE_PRINTF("State: Connected\r\n");
			// Nothing to do, wait for peer device to disconnect.
			// Next state change will be from peer device disconnect hook.
			break;
		}
#ifndef SLEEP_ENABLED
	}
#endif
}

void Main_Loop(void)
{
    bool sleep_allowed = false;

    TRACE_PRINTF("Main_Loop: Enter\r\n");

    while (1)
    {
        /* Refresh watchdog timer. */
        Sys_Watchdog_Refresh();

        /* Execute any events that have occurred. */
        Kernel_Schedule();

        /* Application stuff follows here. */
        App_StateMachine();

		ATMO_Tick();

        // if(stimer_is_expired(&counter_timer) == true)
        // {
        // 	TRACE_PRINTF("Updating time\r\n");
        // 	BLE_CRAST_UpdateCounter(true);
        // 	stimer_expire_from_now_s(&counter_timer, 1);
        // }

#ifdef SLEEP_ENABLED
        /* Set RTC wake up event to nearest timer. */
        if ( Timer_SetWakeupAtNextEvent() != APP_TIMER_ALARM_NOW)
        {
            /* Prepare device for entering deep sleep mode. */
            trace_deinit();
            HAL_I2C_DeInit();

            /* Attempt to enter deep sleep mode.
             * BLE_Power_Mode_Enter will not return when deep sleep is
             * entered. */
            __disable_irq();
            sleep_allowed = BLE_Power_Mode_Enter(&sleep_mode_env,
                    POWER_MODE_SLEEP);
            __enable_irq();

            /* Re-enable basic functionality needed by main loop until device is
             * ready to enter deep sleep. */
            trace_init();
            if (sleep_allowed == false)
            {
                TRACE_PRINTF("Main_Loop: sleep not allowed\r\n");
            }
        }
#else
        HAL_Delay(100);
#endif

        /* Enter sleep mode until an interrupt occurs. */
        SYS_WAIT_FOR_INTERRUPT;
    }
}
