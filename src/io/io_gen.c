/****************************************************************************
**
** Copyright (C) 2014 Stiftung Secure Information and
**                    Communication Technologies SIC and
**                    Graz University of Technology
** Contact: http://opensource.iaik.tugraz.at
**
** This file is part of <product_name>.
**
** $BEGIN_LICENSE:DEFAULT$
** Commercial License Usage
** Licensees holding valid commercial licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and SIC. For further information
** contact us at http://opensource.iaik.tugraz.at.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** This software is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this software. If not, see http://www.gnu.org/licenses/.
**
** $END_LICENSE:DEFAULT$
**
****************************************************************************/

#include "flecc_in_c/bi/bi.h"
#include "flecc_in_c/io/io.h"
#include "flecc_in_c/types.h"
#include <stdio.h>

/** Hex lookup for converting hexadecimal nibbles
 *  to string representation. */
const char hex_lookup[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

/**
 * Prints an unsigned integer to a string.
 * @param buffer the buffer that is written to
 * @param u	the unsigned integer
 * @return a pointer to the end of the buffer
 */
char *print_uint_to_buffer( char *buffer, const uint_t u ) {

#if( BYTES_PER_WORD > 2 )
    *buffer++ = hex_lookup[( u >> 28 ) & 0xF];
    *buffer++ = hex_lookup[( u >> 24 ) & 0xF];
    *buffer++ = hex_lookup[( u >> 20 ) & 0xF];
    *buffer++ = hex_lookup[( u >> 16 ) & 0xF];
#endif

#if( BYTES_PER_WORD > 1 )
    *buffer++ = hex_lookup[( u >> 12 ) & 0xF];
    *buffer++ = hex_lookup[( u >> 8 ) & 0xF];
#endif

    *buffer++ = hex_lookup[( u >> 4 ) & 0xF];
    *buffer++ = hex_lookup[( u & 0xF )];

    return buffer;
}

/**
 * Writes a character string of specified length to the default output.
 * @param buffer the character string to be printed
 * @param length the length of the character string
 */
void io_gen_write( const char *buffer, const int length ) {
    int i;
    for( i = 0; i < length; i++ ) {
        putchar( buffer[i] );
    }
}

/**
 * Reads a character string with a maximum size of
 * length from the default input until the line ends.
 * @param buffer the buffer to be filled with the character string being read
 * @param length the size of the buffer
 * @return the number of characters actually read
 */
int io_gen_readline( char *buffer, const int length ) {
    int i = 0;
    for( ; i < length; ++i ) {
        uint8_t byte = getchar();
        if( byte == '\n' )
            break;
        buffer[i] = byte;
    }

    return i;
}

/**
 * Prints an array of bytes of arbitrary length to
 * a string buffer.
 * @param buffer the string buffer that is written to
 * @param value the value to be printed
 * @param length the length of the array
 */
void io_sprint_bytes_var( char *buffer, const uint8_t *value, const int length ) {
    int i;
    for( i = length - 1; i >= 0; i-- ) {
        *buffer++ = hex_lookup[( value[i] >> 4 ) & 0xF];
        *buffer++ = hex_lookup[( value[i] & 0xF )];
    }
    *buffer = '\0';
}

/**
 * Prints a big integer to the default output and appends a newline.
 * @param value the big integer to be printed
 * @param length the length of the big integer to be printed
 */
void io_print_bigint_var( const uint_t *value, const int length ) {
    char buffer[BYTES_PER_GFP * 4 + WORDS_PER_GFP * 2 + 10];
    bigint_print_var( buffer, value, length );
    puts( buffer );
}

/**
 * Prints a byte array to the default output and appends a newline.
 * @param value the byte array to be printed
 * @param length the length of the byte array to be printed
 */
void io_print_bytes_var( const uint8_t *value, const int length ) {
    char buffer[BYTES_PER_GFP * 2 + WORDS_PER_GFP + 1]; // TODO: correct size
    io_sprint_bytes_var( buffer, value, length );
    puts( buffer );
}

/**
 * Prints an integer to the default output and appends a newline.
 * @param value the integer to be printed
 */
void io_print_integer( const uint_t value ) {
    char buffer[BYTES_PER_WORD * 2 + 1];
    print_uint_to_buffer( buffer, value );
    buffer[2 * BYTES_PER_WORD] = 0;
    puts( buffer );
}

/**
 * Prints a point to the default output and appends a newline.
 * @param point the point to be printed
 * @param param elliptic curve parameters
 */
void io_print_affine_point( const eccp_point_affine_t *point, const eccp_parameters_t *param ) {
    io_print_bigint_var( point->x, param->prime_data.words );
    io_print_bigint_var( point->y, param->prime_data.words );
    io_print_integer( point->identity );
}
