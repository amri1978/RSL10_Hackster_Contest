/*
 * Base64 encoding/decoding (RFC1341)
 * Copyright (c) 2005-2011, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "atmo_base64.h"
#include "../app_src/atmosphere_platform.h"

static const unsigned char base64_table[65] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

unsigned int ATMO_BASE64_GetEncodeOutputSize( const unsigned char *src, unsigned int len )
{
	unsigned int olen = len * 4 / 3 + 4; /* 3-byte blocks to 4-byte */
	olen += olen / 72; /* line feeds */
	olen++; /* nul termination */
	return olen;
}

unsigned int ATMO_BASE64_GetDecodeOutputSize( const unsigned char *src, unsigned int len )
{
	unsigned char dtable[256];
	int i;
	memset( dtable, 0x80, 256 );

	for ( i = 0; i < sizeof( base64_table ) - 1; i++ )
	{
		dtable[base64_table[i]] = ( unsigned char ) i;
	}

	dtable['='] = 0;
	int count = 0;

	for ( i = 0; i < len; i++ )
	{
		if ( dtable[src[i]] != 0x80 )
		{
			count++;
		}
	}

	return count / 4 * 3;;
}
/**
 * base64_encode - Base64 encode
 * @src: Data to be encoded
 * @len: Length of the data to be encoded
 * @out: output buffer
 * @out_len: size of output buffer
 * Returns: true if everything went well, false otherwise
 *
 * Caller is responsible for freeing the returned buffer. Returned buffer is
 * nul terminated to make it easier to use as a C string. The nul terminator is
 * not included in out_len.
 */
ATMO_BOOL_t ATMO_BASE64_Encode( const unsigned char *src, unsigned int len,
                                char *out, unsigned int out_len )
{
	unsigned char *pos;
	const unsigned char *end, *in;
	unsigned int olen;
	int line_len;

	olen = len * 4 / 3 + 4; /* 3-byte blocks to 4-byte */
	olen += olen / 72; /* line feeds */
	olen++; /* nul termination */

	if ( olen < len )
	{
		return false;    /* integer overflow */
	}

	if ( olen > out_len )
	{
		return false;
	}

	end = src + len;
	in = src;
	pos = ( unsigned char * )out;
	line_len = 0;

	while ( end - in >= 3 )
	{
		*pos++ = base64_table[in[0] >> 2];
		*pos++ = base64_table[( ( in[0] & 0x03 ) << 4 ) | ( in[1] >> 4 )];
		*pos++ = base64_table[( ( in[1] & 0x0f ) << 2 ) | ( in[2] >> 6 )];
		*pos++ = base64_table[in[2] & 0x3f];
		in += 3;
		line_len += 4;

		if ( line_len >= 72 )
		{
			*pos++ = '\n';
			line_len = 0;
		}
	}

	if ( end - in )
	{
		*pos++ = base64_table[in[0] >> 2];

		if ( end - in == 1 )
		{
			*pos++ = base64_table[( in[0] & 0x03 ) << 4];
			*pos++ = '=';
		}
		else
		{
			*pos++ = base64_table[( ( in[0] & 0x03 ) << 4 ) |
			                                         ( in[1] >> 4 )];
			*pos++ = base64_table[( in[1] & 0x0f ) << 2];
		}

		*pos++ = '=';
		line_len += 4;
	}

	if ( line_len )
	{
		*pos++ = '\n';
	}

	*pos = '\0';
	return true;
}


/**
 * base64_decode - Base64 decode
 * @src: Data to be decoded
 * @len: Length of the data to be decoded
 * @out: output buffer
 * @out_len: size of output buffer
 * Returns: true if all went well, false otherwise
 *
 * Caller is responsible for freeing the returned buffer.
 */
ATMO_BOOL_t ATMO_BASE64_Decode( const unsigned char *src, unsigned int len,
                                char *out, unsigned int out_len )
{
	unsigned char dtable[256], *pos, block[4], tmp;
	unsigned int i, count, olen;
	int pad = 0;

	memset( dtable, 0x80, 256 );

	for ( i = 0; i < sizeof( base64_table ) - 1; i++ )
	{
		dtable[base64_table[i]] = ( unsigned char ) i;
	}

	dtable['='] = 0;

	count = 0;

	for ( i = 0; i < len; i++ )
	{
		if ( dtable[src[i]] != 0x80 )
		{
			count++;
		}
	}

	if ( count == 0 || count % 4 )
	{
		return false;
	}

	olen = count / 4 * 3;

	if ( olen > out_len )
	{

		return false;
	}

	pos = ( unsigned char * )out;

	count = 0;

	for ( i = 0; i < len; i++ )
	{
		tmp = dtable[src[i]];

		if ( tmp == 0x80 )
		{
			continue;
		}

		if ( src[i] == '=' )
		{
			pad++;
		}

		block[count] = tmp;
		count++;

		if ( count == 4 )
		{
			*pos++ = ( block[0] << 2 ) | ( block[1] >> 4 );
			*pos++ = ( block[1] << 4 ) | ( block[2] >> 2 );
			*pos++ = ( block[2] << 6 ) | block[3];
			count = 0;

			if ( pad )
			{
				if ( pad == 1 )
				{
					pos--;
				}
				else if ( pad == 2 )
				{
					pos -= 2;
				}
				else
				{
					/* Invalid padding */
					return false;
				}

				break;
			}
		}
	}

	return true;
}