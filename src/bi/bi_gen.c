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

#include "flecc_in_c/bi/bi_gen.h"

#include <assert.h>

int bigint_add_var( uint_t *res, const uint_t *a, const uint_t *b, const int length ) {
    assert( length > 0 );

    ulong_t temp = 0;
    for( int i = 0; i < length; ++i, ++a, ++b, ++res ) {
        temp += *a;
        temp += *b;
        *res = (uint_t)temp;
        temp >>= BITS_PER_WORD;
    }
    return temp;
}

int bigint_add_carry_var( uint_t *res, const uint_t *a, const uint_t *b, const int length, const int carry ) {
    assert( length > 0 );

    slong_t temp = carry;
    for( int i = 0; i < length; ++i, ++a, ++b, ++res ) {
        temp += *a;
        temp += *b;
        *res = (uint_t)temp;
        temp >>= BITS_PER_WORD;
    }
    return temp;
}

int bigint_subtract_var( uint_t *res, const uint_t *a, const uint_t *b, const int length ) {
    assert( length > 0 );

    slong_t temp = 0;
    for( int i = 0; i < length; ++i, ++a, ++b, ++res ) {
        temp += *a;
        temp -= *b;
        *res = (uint_t)temp;
        temp >>= BITS_PER_WORD;
    }
    return temp;
}

int bigint_subtract_carry_var( uint_t *res, const uint_t *a, const uint_t *b, const int length, const int carry ) {
    assert( length > 0 );

    slong_t temp = carry;
    for( int i = 0; i < length; ++i, ++a, ++b, ++res ) {
        temp += *a;
        temp -= *b;
        *res = (uint_t)temp;
        temp >>= BITS_PER_WORD;
    }
    return temp;
}

void bigint_clear_var( uint_t *a, const int length ) {
    assert( length > 0 );

    for( int i = 0; i < length; i++ ) {
        a[i] = 0;
    }
}

void bigint_xor_var( uint_t *res, const uint_t *a, const uint_t *b, const int length ) {
    assert( length > 0 );

    for( int i = 0; i < length; ++i, ++a, ++b, ++res ) {
        *res = *a ^ *b;
    }
}

void bigint_shift_left_var( uint_t *res, const uint_t *a, const int left, const int length ) {
    assert( length > 0 );
    assert( left >= 0 && left < BITS_PER_WORD * length );

    int nWords = left >> LD_BITS_PER_WORD;
    int nBits = left & ( BITS_PER_WORD - 1 );

    // shift is performed from the most significant word downwards
    res += length - 1;
    a += length - 1 - nWords;
    if( nBits != 0 ) {
        uint_t temp = *a-- << nBits;
        for( int i = length - 2 - nWords; i >= 0; i-- ) {
            uint_t temp2 = *a--;
            temp |= temp2 >> ( BITS_PER_WORD - nBits );
            *res-- = temp;
            temp = temp2 << nBits;
        }
        *res-- = temp;
    } else {
        for( int i = length - 1; i >= nWords; i-- ) {
            *res-- = *a--;
        }
    }

    // fill the remaining words with zeros
    for( int i = nWords - 1; i >= 0; --i ) {
        *res-- = 0;
    }
}

void bigint_shift_right_var( uint_t *res, const uint_t *a, const int right, const int length ) {
    assert( length > 0 );
    assert( right >= 0 && right < BITS_PER_WORD * length );

    int nWords = right >> LD_BITS_PER_WORD;
    int nBits = right & ( BITS_PER_WORD - 1 );

    // shift is performed from the least significant word upwards
    if( nBits == 0 ) {
        for( int i = 0; i < ( length - nWords ); i++ ) {
            res[i] = a[i + nWords];
        }
    } else {
        uint_t temp = a[nWords] >> nBits;
        for( int i = 1; i < ( length - nWords ); i++ ) {
            uint_t temp2 = a[nWords + i];
            temp |= temp2 << ( BITS_PER_WORD - nBits );
            res[i - 1] = temp;
            temp = temp2 >> nBits;
        }
        res[length - nWords - 1] = temp;
    }

    // fill the remaining words with zeros
    for( int i = length - nWords; i < length; i++ ) {
        res[i] = 0;
    }
}

void bigint_shift_right_one_var( uint_t *res, const uint_t *a, const int length ) {
    assert( length > 0 );

    uint_t temp = a[0] >> 1;
    for( int i = 1; i < length; i++ ) {
        uint_t temp2 = a[i];
        temp |= temp2 << ( BITS_PER_WORD - 1 );
        res[i - 1] = temp;
        temp = temp2 >> 1;
    }
    res[length - 1] = temp;
}

int bigint_compare_var( const uint_t *a, const uint_t *b, const int length ) {
    assert( length > 0 );

    for( int i = length - 1; i >= 0; i-- ) {
        slong_t temp = a[i];
        temp -= b[i];
        if( temp != 0 ) {
            return ( temp > 0 ? 1 : -1 );
        }
    }
    return 0;
}

int bigint_is_equal_var( const uint_t *a, const uint_t *b, const int length ) {
    assert( length > 0 );

    for( int i = length - 1; i >= 0; i-- ) {
        if( a[i] != b[i] ) {
            return 0;
        }
    }
    return 1;
}

int bigint_is_zero_var( const uint_t *a, const int length ) {
    assert( length > 0 );

    for( int i = 0; i < length; i++ ) {
        if( a[i] != 0 ) {
            return 0;
        }
    }
    return 1;
}

int bigint_is_one_var( const uint_t *a, const int length ) {
    assert( length > 0 );

    if( a[0] != 1 ) {
        return 0;
    }

    for( int i = 1; i < length; i++ ) {
        if( a[i] != 0 ) {
            return 0;
        }
    }
    return 1;
}

void bigint_multiply_var( uint_t *res, const uint_t *a, const uint_t *b, const int lengthA, const int lengthB ) {
    assert( lengthA > 0 );
    assert( lengthB > 0 );

    // all results with index larger than lengthB are written before read
    bigint_clear_var( res, lengthB );
    for( int i = 0; i < lengthA; i++ ) {
        uint_t carry = 0;
        for( int j = 0; j < lengthB; j++ ) {
            ulong_t product = res[i + j];
            product += (ulong_t)a[i] * (ulong_t)b[j];
            product += carry;
            res[i + j] = ( product & UINT_T_MAX );
            carry = product >> BITS_PER_WORD;
        }
        res[i + lengthB] = carry;
    }
}

void bigint_copy_var( uint_t *res, const uint_t *a, const int length ) {
    assert( length > 0 );

    for( int i = 0; i < length; i++ ) {
        res[i] = a[i];
    }
}

void bigint_print_var( char *buffer, const uint_t *toPrint, const int length ) {
    assert( length > 0 );

    const char lookup[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

#if BYTES_PER_WORD == 1
    for( int index = length - 1; index >= 0; index-- ) {
        *buffer++ = lookup[toPrint[index] >> 4];
        *buffer++ = lookup[toPrint[index] & 0xF];
        if( index % 4 == 0 ) {
            *buffer++ = ' ';
        }
    }
#endif
#if BYTES_PER_WORD == 2
    for( int index = length - 1; index >= 0; index-- ) {
        *buffer++ = lookup[toPrint[index] >> 12];
        *buffer++ = lookup[( toPrint[index] >> 8 ) & 0xF];
        *buffer++ = lookup[( toPrint[index] >> 4 ) & 0xF];
        *buffer++ = lookup[toPrint[index] & 0xF];
        if( index % 2 == 0 ) {
            *buffer++ = ' ';
        }
    }
#endif
#if BYTES_PER_WORD == 4
    for( int index = length - 1; index >= 0; index-- ) {
        *buffer++ = lookup[toPrint[index] >> 28];
        *buffer++ = lookup[( toPrint[index] >> 24 ) & 0xF];
        *buffer++ = lookup[( toPrint[index] >> 20 ) & 0xF];
        *buffer++ = lookup[( toPrint[index] >> 16 ) & 0xF];
        *buffer++ = lookup[( toPrint[index] >> 12 ) & 0xF];
        *buffer++ = lookup[( toPrint[index] >> 8 ) & 0xF];
        *buffer++ = lookup[( toPrint[index] >> 4 ) & 0xF];
        *buffer++ = lookup[toPrint[index] & 0xF];
        *buffer++ = ' ';
    }
#endif
#if BYTES_PER_WORD == 8
    for( int index = length - 1; index >= 0; index-- ) {
        *buffer++ = lookup[toPrint[index] >> 60];
        *buffer++ = lookup[( toPrint[index] >> 56 ) & 0xF];
        *buffer++ = lookup[( toPrint[index] >> 52 ) & 0xF];
        *buffer++ = lookup[( toPrint[index] >> 48 ) & 0xF];
        *buffer++ = lookup[( toPrint[index] >> 44 ) & 0xF];
        *buffer++ = lookup[( toPrint[index] >> 40 ) & 0xF];
        *buffer++ = lookup[( toPrint[index] >> 36 ) & 0xF];
        *buffer++ = lookup[( toPrint[index] >> 32 ) & 0xF];
        *buffer++ = ' ';
        *buffer++ = lookup[( toPrint[index] >> 28 ) & 0xF];
        *buffer++ = lookup[( toPrint[index] >> 24 ) & 0xF];
        *buffer++ = lookup[( toPrint[index] >> 20 ) & 0xF];
        *buffer++ = lookup[( toPrint[index] >> 16 ) & 0xF];
        *buffer++ = lookup[( toPrint[index] >> 12 ) & 0xF];
        *buffer++ = lookup[( toPrint[index] >> 8 ) & 0xF];
        *buffer++ = lookup[( toPrint[index] >> 4 ) & 0xF];
        *buffer++ = lookup[toPrint[index] & 0xF];
        *buffer++ = ' ';
    }
#endif
    *--buffer = 0;
}

void bigint_set_bit_var( uint_t *a, const int bit, const int value, const int length ) {
    assert( length > 0 );
    assert( bit >= 0 && bit < BITS_PER_WORD * length );
    assert( value == 0 || value == 1 );

    int iWord = bit >> LD_BITS_PER_WORD;
    int iBit = bit & ( BITS_PER_WORD - 1 );

    uint_t word = a[iWord];
    word &= ~( 1 << iBit );
    word |= value << iBit;
    a[iWord] = word;
}

int bigint_test_bit_var( const uint_t *a, const int bit, const int length ) {
    assert( length > 0 );
    assert( bit >= 0 && bit < BITS_PER_WORD * length );

    int iWord = bit >> LD_BITS_PER_WORD;
    int iBit = bit & ( BITS_PER_WORD - 1 );

    return ( a[iWord] >> iBit ) & 0x01;
}

int bigint_get_msb_var( const uint_t *a, const int length ) {
    assert( length > 0 );

    // iterate over the words to find the first one which is not zero
    for( int word = length - 1; word >= 0; --word ) {
        uint_t temp = a[word];
        if( temp == 0 ) {
            continue;
        }
        // found non zero word, iterate over the bits
        for( int bit = BITS_PER_WORD - 1; bit >= 0; --bit ) {
            if( ( ( temp >> bit ) & 0x01 ) == 1 ) {
                return word * BITS_PER_WORD + bit;
            }
        }
    }

    // no set bit found
    return -1;
}

uint8_t bigint_get_byte_var( const uint_t *a, const int length, const int index ) {
    assert( length > 0 );
    assert( index >= 0 && index < BYTES_PER_WORD * length );

    int wordIndex = index >> LD_BYTES_PER_WORD;
    int byteIndex = index & ( BYTES_PER_WORD - 1 );

    uint_t word = a[wordIndex];
    return ( word >> ( byteIndex << 3 ) ) & 0xFF;
}

void bigint_set_byte_var( uint_t *a, const int length, const int index, const uint8_t value ) {
    assert( length > 0 );
    assert( index >= 0 && index < BYTES_PER_WORD * length );

    int wordIndex = index >> LD_BYTES_PER_WORD;
    int byteIndex = index & ( BYTES_PER_WORD - 1 );

    uint_t word = a[wordIndex];
    word &= ~( 0xFF << ( byteIndex * 8 ) );
    word |= value << ( byteIndex * 8 );
    a[wordIndex] = word;
}

int bigint_parse_hex_var( uint_t *res, const int length, const char *string, const int string_length ) {
    assert( length > 0 );
    assert( string_length > 0 );

    int nibble_i = 0, index = 0;
    uint_t word = 0;

    // iterate over the characters of the string
    for( const char *ptr = string + string_length - 1; ptr >= string; --ptr ) {
        char character = *ptr;
        uint_t char_converted;
        if( ( character >= '0' ) && ( character <= '9' ) ) {
            char_converted = character - '0';
        } else if( ( character >= 'a' ) && ( character <= 'f' ) ) {
            char_converted = character - 'a' + 10;
        } else if( ( character >= 'A' ) && ( character <= 'F' ) ) {
            char_converted = character - 'A' + 10;
        } else {
            continue;
        }

        // found a valid character, add it to the current word
        word |= char_converted << nibble_i;
        nibble_i += 4;

        // check if the current word is finished and write it to the big integer
        // if it is the case
        if( nibble_i == BITS_PER_WORD ) {
            res[index++] = word;
            if( index == length ) {
                return index;
            }
            nibble_i = 0;
            word = 0;
        }
    }
    if( nibble_i > 0 ) {
        res[index++] = word;
    }
    return index;
}

int bigint_hamming_weight_var( const uint_t *var, const int length ) {
    assert( length > 0 );

    int counter = 0;
    for( int word = 0; word < length; word++ ) {
        uint_t temp = var[word];
        for( int bit = 0; bit < BITS_PER_WORD; bit++ ) {
            if( ( temp & ( 1 << bit ) ) > 0 ) {
                counter++;
            }
        }
    }
    return counter;
}

void bigint_divide_simple_var( uint_t *Q, uint_t *R, const uint_t *N, const uint_t *D, const int length ) {
    assert( length > 0 );
    assert( !bigint_is_zero_var( D, length ) );

    bigint_clear_var( Q, length );
    bigint_clear_var( R, length );
    for( int i = BITS_PER_WORD * length - 1; i >= 0; i-- ) {
        bigint_shift_left_var( R, R, 1, length );
        R[0] |= bigint_test_bit_var( N, i, length );
        if( bigint_compare_var( R, D, length ) >= 0 ) {
            bigint_subtract_var( R, R, D, length );
            bigint_set_bit_var( Q, i, 1, length );
        }
    }
}
