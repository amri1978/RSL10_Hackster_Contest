/*
 * Base64 encoding/decoding (RFC1341)
 * Copyright (c) 2005, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef ATMO_BASE64_H
#define ATMO_BASE64_H

#include "../app_src/atmosphere_typedefs.h"

ATMO_BOOL_t ATMO_BASE64_Encode( const unsigned char *src, unsigned int len,
                                char *out, unsigned int out_len );

ATMO_BOOL_t ATMO_BASE64_Decode( const unsigned char *src, unsigned int len,
                                char *out, unsigned int out_len );

unsigned int ATMO_BASE64_GetEncodeOutputSize( const unsigned char *src, unsigned int len );

unsigned int ATMO_BASE64_GetDecodeOutputSize( const unsigned char *src, unsigned int len );

#endif /* BASE64_H */