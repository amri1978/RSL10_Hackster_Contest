#ifndef ATMO_TYPEDEFS_H
#define ATMO_TYPEDEFS_H
#include "atmo_config.h"

#define ATMO_PLATFORM_NO_SSCANF_SUPPORT

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#ifdef ATMO_PLATFORM_CUSTOM_TYPE_INCLUDE
#include "atmosphere_typedefs_custom.h"
#endif

#ifdef ATMO_PLATFORM_NO_BOOL
#define ATMO_BOOL_t uint8_t
#define true 1
#define false 0
#else
#include <stdbool.h>
#define ATMO_BOOL_t bool
#endif
#include <stddef.h>

#ifndef ATMO_PLATFORM_NO_STDINT
#include <stdint.h>
#endif

#ifdef ATMO_PLATFORM_NO_STRCPY
#define strcpy ATMO_StrCpy
#endif

#ifdef ATMO_PLATFORM_NO_STRNCMP
#define strncmp ATMO_StrnCmp
#endif

#endif
