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

#include "flecc_in_c/utils/parse.h"

/**
 * Parses the integer that is contained in buffer.
 * @param buffer the buffer containing an integer in string format
 * @return the integer
 */
int parse_integer( const char *buffer ) {
    int negative = 0;
    int number = 0;

    if( *buffer == '-' ) {
        negative = 1;
        buffer++;
    }

    while( ( *buffer >= '0' ) && ( *buffer <= '9' ) ) {
        number *= 10;
        number += *buffer - '0';
        buffer++;
    }

    if( negative ) {
        number *= -1;
    }

    return number;
}

/**
 * Gets a byte from hexadecimal string representation.
 * @param buffer buffer[0] is MSB, buffer[1] is LSB of the byte to be read
 * @return the byte
 */
uint8_t parse_hex_byte( const char *buffer ) {
    uint8_t byte = 0;

    if( ( buffer[0] >= '0' ) && ( buffer[0] <= '9' ) ) {
        byte |= ( buffer[0] - '0' ) << 4;
    } else if( ( buffer[0] >= 'A' ) && ( buffer[0] <= 'F' ) ) {
        byte |= ( buffer[0] - 'A' + 10 ) << 4;
    } else if( ( buffer[0] >= 'a' ) && ( buffer[0] <= 'f' ) ) {
        byte |= ( buffer[0] - 'a' + 10 ) << 4;
    }
    if( ( buffer[1] >= '0' ) && ( buffer[1] <= '9' ) ) {
        byte |= buffer[1] - '0';
    } else if( ( buffer[1] >= 'A' ) && ( buffer[1] <= 'F' ) ) {
        byte |= buffer[1] - 'A' + 10;
    } else if( ( buffer[1] >= 'a' ) && ( buffer[1] <= 'f' ) ) {
        byte |= buffer[1] - 'a' + 10;
    }

    return byte;
}

/**
 * Parses a hexadecimal message in the buffer and stores
 * it to the array message. Note that the array message
 * needs to be of appropriate size.
 * @param message the parsed message
 * @param max_message_length the size of the allocated message buffer
 * @param buffer the buffer containing the hexadecimal message
 * @param buffer_length the number of characters stored in buffer
 * @return the length of the message
 */
int parse_hex_message( char *message, const int max_message_length, const char *buffer, const int buffer_length ) {
    int num_words = 0;
    int _buffer_length_ = buffer_length;
    uint8_t byte;

    while( _buffer_length_ > 1 ) {
        byte = parse_hex_byte( buffer );
        *message = byte;
        message++;
        num_words++;
        buffer += 2;
        _buffer_length_ -= 2;
        if( num_words == max_message_length ) {
            break;
        }
    }

    return num_words;
}
