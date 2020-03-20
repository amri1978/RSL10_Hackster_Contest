#ifndef ATMO_PLATFORM_H
#define ATMO_PLATFORM_H

#include "../atmo/atmo.h"
#include <rsl10.h>

#include <HAL.h>
#include <HAL_RTC.h>
#include <BDK_Task.h>
#include <BSP_Components.h>
#include <BLE_Components.h>

#include "app_trace.h"
#include "app_timer.h"
#include "app_ble_hooks.h"
#include "app_sleep.h"

void ATMO_PLATFORM_Init();

void ATMO_PLATFORM_PostInit();

#define ATMO_PLATFORM_DebugPrint TRACE_PRINTF

void ATMO_PLATFORM_DelayMilliseconds( uint32_t milliseconds );

void *ATMO_Malloc( uint32_t numBytes );

void *ATMO_Calloc( size_t num, size_t size );

void ATMO_Free( void *data );

void ATMO_Lock();

void ATMO_Unlock();

uint64_t ATMO_PLATFORM_UptimeMs();

uint32_t ATMO_PLATFORM_GetBattLevel();

#endif
