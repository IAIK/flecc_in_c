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

#include "bi_gen.h"

/**
 * Adds two big integers with variable length
 * @param a the first parameter to add
 * @param b the second parameter to add
 * @param res the result
 * @param length the number of uint_t to add
 * @return the carry flag
 */
int bigint_add_var( uint_t *res, const uint_t *a, const uint_t *b, const int length ) {
    ulong_t temp = 0;
    int i = 0;
    for( ; i < length; i++ ) {
        temp += *a;
        temp += *b;
        *res = (uint_t)temp;
        temp >>= BITS_PER_WORD;
        a++;
        b++;
        res++;
    }
    return temp;
}

/**
 * Adds two big integers with variable length
 * @param a the first parameter to add
 * @param b the second parameter to add
 * @param res the result
 * @param length the number of uint_t to add
 * @param carry input carry flag
 * @return the carry flag
 */
int bigint_add_carry_var( uint_t *res, const uint_t *a, const uint_t *b, const int length, const int carry ) {
    slong_t temp = carry;
    int i = 0;
    for( ; i < length; i++ ) {
        temp += *a;
        temp += *b;
        *res = (uint_t)temp;
        temp >>= BITS_PER_WORD;
        a++;
        b++;
        res++;
    }
    return temp;
}

/**
 * Subtracts two big integers with variable length: res = a - b
 * @param a the minuend
 * @param b the subtrahend
 * @param res the difference
 * @param length the number of uint_t elements to subtract
 * @return the carry flag
 */
int bigint_subtract_var( uint_t *res, const uint_t *a, const uint_t *b, const int length ) {
    slong_t temp = 0;
    int i = 0;
    for( ; i < length; i++ ) {
        temp += *a;
        temp -= *b;
        *res = (uint_t)temp;
        temp >>= BITS_PER_WORD;
        a++;
        b++;
        res++;
    }
    return temp;
}

/**
 * Subtracts two big integers with variable length: res = a - b
 * @param a the minuend
 * @param b the subtrahend
 * @param res the difference
 * @param length the number of uint_t elements to subtract
 * @return the carry flag
 */
int bigint_subtract_carry_var( uint_t *res, const uint_t *a, const uint_t *b, const int length, const int carry ) {
    slong_t temp = carry;
    int i = 0;
    for( ; i < length; i++ ) {
        temp += *a;
        temp -= *b;
        *res = (uint_t)temp;
        temp >>= BITS_PER_WORD;
        a++;
        b++;
        res++;
    }
    return temp;
}

/**
 * Clears the biginteger
 * @param a the data to clear
 * @param length how many uint_t to clear
 */
void bigint_clear_var( uint_t *a, const int length ) {
    int i;
    for( i = 0; i < length; i++ ) {
        a[i] = 0;
    }
}

/**
 * Xors two big integers
 * @param a the first parameter to xor
 * @param b the second parameter to xor
 * @param res the result
 * @param length the number of words per uint_t*
 */
void bigint_xor_var( uint_t *res, const uint_t *a, const uint_t *b, const int length ) {
    uint_t temp;
    int i = 0;
    for( ; i < length; i++ ) {
        temp = *a ^ *b;
        *res = temp;
        a++;
        b++;
        res++;
    }
}

/**
 * Shifts a biginteger to the lift
 * @param a the data to shift
 * @param left the number of bits to shift
 * @param destination big integer
 * @param length number of words which should be shifted (size of a & res)
 */
void bigint_shift_left_var( uint_t *res, const uint_t *a, const int left, const int length ) {
    uint_t temp, temp2;
    int i, nWords, nBits;

    if( length < 0 )
        return;
    if( left < 0 )
        bigint_shift_right_var( res, a, -left, length );

    nWords = left >> LD_BITS_PER_WORD;
    nBits = left & ( BITS_PER_WORD - 1 );

    res += length - 1;
    a += length - 1 - nWords;
    if( nBits != 0 ) {
        temp = *a-- << nBits;
        for( i = length - 2 - nWords; i >= 0; i-- ) {
            temp2 = *a--;
            temp |= temp2 >> ( BITS_PER_WORD - nBits );
            *res-- = temp;
            temp = temp2 << nBits;
        }
        *res-- = temp;
    } else {
        for( i = length - 1; i >= nWords; i-- ) {
            *res-- = *a--;
        }
    }
    for( i = nWords - 1; i >= 0; --i )
        *res-- = 0;
}

/**
 * Shifts a biginteger to the right
 * @param a the data to shift
 * @param right the number of bits to shift
 * @param destination big integer
 * @param length number of words in a and res
 */
void bigint_shift_right_var( uint_t *res, const uint_t *a, const int right, const int length ) {
    uint_t temp, temp2;
    int i, nWords, nBits;

    if( length < 0 )
        return;
    if( right < 0 )
        bigint_shift_left_var( res, a, -right, length );

    nWords = right >> LD_BITS_PER_WORD;
    nBits = right & ( BITS_PER_WORD - 1 );

    if( nBits == 0 ) {
        for( i = 0; i < ( length - nWords ); i++ ) {
            res[i] = a[i + nWords];
        }
    } else {
        temp = a[nWords] >> nBits;
        for( i = 1; i < ( length - nWords ); i++ ) {
            temp2 = a[nWords + i];
            /* WARNING gcc has a problem with (uint_t) << BITS_PER_WORD !! */
            temp |= temp2 << ( BITS_PER_WORD - nBits );
            res[i - 1] = temp;
            temp = temp2 >> nBits;
        }
        res[length - nWords - 1] = temp;
    }

    for( i = length - nWords; i < length; i++ )
        res[i] = 0;
}

/**
 * Shifts a biginteger to the right by one bit
 * @param a the data to shift
 * @param res destination big integer
 * @param length word count which should be shifted
 */
void bigint_shift_right_one_var( uint_t *res, const uint_t *a, const int length ) {
    uint_t temp, temp2;
    int i;

    if( length < 0 )
        return;

    temp = a[0] >> 1;
    for( i = 1; i < length; i++ ) {
        temp2 = a[i];
        temp |= temp2 << ( BITS_PER_WORD - 1 );
        res[i - 1] = temp;
        temp = temp2 >> 1;
    }
    res[length - 1] = temp;
}

/**
 * Compares two big integers with variable length.
 * @param a
 * @param b
 * @param length the size of a and b in words
 * @return -1, 0 or 1 as *a* is numerically less than, equal to, or greater than *b*.
 */
int bigint_compare_var( const uint_t *a, const uint_t *b, const int length ) {
    slong_t temp;
    int i = length - 1;
    for( ; i >= 0; i-- ) {
        temp = a[i];
        temp -= b[i];
        if( temp != 0 )
            return ( temp > 0 ? 1 : -1 );
    }
    return 0;
}

/**
 * Checks if a number is zero.
 * @param a
 * @param length the size of a in words
 * @return 1 if zero otherwise 0.
 */
int bigint_is_zero_var( const uint_t *a, const int length ) {
    int i;
    for( i = 0; i < length; i++ ) {
        if( a[i] != 0 )
            return 0;
    }
    return 1;
}

/**
 * Multiply two big integers with variable length
 * @param a first multiplicant
 * @param b second multiplicant
 * @param result An buffer (array) with (length_a+length_b) entries is required.
 * @param length_a the size of array a
 * @param length_b the size of array b
 */
void bigint_multiply_var( uint_t *result, const uint_t *a, const uint_t *b, const int length_a, const int length_b ) {
    int i, j;
    ulong_t product;
    uint_t carry;

    if( length_a < 0 )
        return;
    if( length_b < 0 )
        return;

    bigint_clear_var( result, length_a + length_b );
    for( i = 0; i < length_a; i++ ) {
        carry = 0;
        for( j = 0; j < length_b; j++ ) {
            product = result[i + j];
            product += (ulong_t)a[i] * (ulong_t)b[j];
            product += carry;
            result[i + j] = ( product & UINT_T_MAX );
            carry = product >> ( 8 * sizeof( uint_t ) );
        }
        result[i + length_b] = carry;
    }
}

/**
 * Copy length elements from the source array to the destination array.
 * @param source
 * @param dest
 * @param length the number of elements to copy
 */
void bigint_copy_var( uint_t *dest, const uint_t *source, const int length ) {
    int i;
    for( i = 0; i < length; i++ ) {
        dest[i] = source[i];
    }
}

/**
 * Prints the big integer in hex format to the char buffer.
 * @param to_print the big integer to print
 * @param buffer the destination buffer (must be sufficiently large)
 * @param length the number of elements stored within to_print
 */
void bigint_print_var( char *buffer, const uint_t *to_print, const int length ) {
    static const char lookup[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    int index;
#if BYTES_PER_WORD == 1
    for( index = length - 1; index >= 0; index-- ) {
        *buffer++ = lookup[to_print[index] >> 4];
        *buffer++ = lookup[to_print[index] & 0xF];
        if( index % 4 == 0 ) {
            *buffer++ = ' ';
        }
    }
#endif
#if BYTES_PER_WORD == 2
    for( index = length - 1; index >= 0; index-- ) {
        *buffer++ = lookup[to_print[index] >> 12];
        *buffer++ = lookup[( to_print[index] >> 8 ) & 0xF];
        *buffer++ = lookup[( to_print[index] >> 4 ) & 0xF];
        *buffer++ = lookup[to_print[index] & 0xF];
        if( index % 2 == 0 ) {
            *buffer++ = ' ';
        }
    }
#endif
#if BYTES_PER_WORD == 4
    for( index = length - 1; index >= 0; index-- ) {
        *buffer++ = lookup[to_print[index] >> 28];
        *buffer++ = lookup[( to_print[index] >> 24 ) & 0xF];
        *buffer++ = lookup[( to_print[index] >> 20 ) & 0xF];
        *buffer++ = lookup[( to_print[index] >> 16 ) & 0xF];
        *buffer++ = lookup[( to_print[index] >> 12 ) & 0xF];
        *buffer++ = lookup[( to_print[index] >> 8 ) & 0xF];
        *buffer++ = lookup[( to_print[index] >> 4 ) & 0xF];
        *buffer++ = lookup[to_print[index] & 0xF];
        *buffer++ = ' ';
    }
#endif
#if BYTES_PER_WORD == 8
    for( index = length - 1; index >= 0; index-- ) {
        *buffer++ = lookup[to_print[index] >> 60];
        *buffer++ = lookup[( to_print[index] >> 56 ) & 0xF];
        *buffer++ = lookup[( to_print[index] >> 52 ) & 0xF];
        *buffer++ = lookup[( to_print[index] >> 48 ) & 0xF];
        *buffer++ = lookup[( to_print[index] >> 44 ) & 0xF];
        *buffer++ = lookup[( to_print[index] >> 40 ) & 0xF];
        *buffer++ = lookup[( to_print[index] >> 36 ) & 0xF];
        *buffer++ = lookup[( to_print[index] >> 32 ) & 0xF];
        *buffer++ = ' ';
        *buffer++ = lookup[( to_print[index] >> 28 ) & 0xF];
        *buffer++ = lookup[( to_print[index] >> 24 ) & 0xF];
        *buffer++ = lookup[( to_print[index] >> 20 ) & 0xF];
        *buffer++ = lookup[( to_print[index] >> 16 ) & 0xF];
        *buffer++ = lookup[( to_print[index] >> 12 ) & 0xF];
        *buffer++ = lookup[( to_print[index] >> 8 ) & 0xF];
        *buffer++ = lookup[( to_print[index] >> 4 ) & 0xF];
        *buffer++ = lookup[to_print[index] & 0xF];
        *buffer++ = ' ';
    }
#endif
    *buffer = 0;
}

/**
 * Sets a single specific big in a
 * @param a big integer to modify
 * @param bit bit to set
 * @param value the bit value
 * @param length the maximum size of a in words
 */
void bigint_set_bit_var( uint_t *a, const int bit, const int value, const int length ) {
    int iWord, iBit;
    uint_t word;

    if( bit < 0 )
        return;

    iWord = bit >> LD_BITS_PER_WORD;
    iBit = bit & ( BITS_PER_WORD - 1 );

    if( iWord >= length )
        return;

    word = a[iWord];
    word &= ~( 1 << iBit );
    word |= value << iBit;
    a[iWord] = word;
}

/**
 * Returns the state of a specific bit.
 * @param a big integer to test
 * @param bit bit to test
 * @param length the maximum size of a in words
 * @return 1 if bit is 1, 0 if bit is 0, and 0 if out of bounds.
 */
int bigint_test_bit_var( const uint_t *a, const int bit, const int length ) {
    int iWord, iBit;

    if( bit < 0 )
        return 0;

    iWord = bit >> LD_BITS_PER_WORD;
    iBit = bit & ( BITS_PER_WORD - 1 );

    if( iWord >= length )
        return 0;

    return ( a[iWord] >> iBit ) & 0x01;
}

/**
 * Get the bit index of the most significant bit
 * @param a the big integer to test
 * @param param the size of the uint_t-array in words
 * @return 0 <= index < ECC_NUM_BITS, -1 if no bit is set
 */
int bigint_get_msb_var( const uint_t *a, const int length ) {
    int word, bit;
    uint_t temp;
    if( length < 0 )
        return -1;
    for( word = length - 1; word > 0; word-- ) {
        temp = a[word];
        if( temp != 0 )
            break;
    }
    temp = a[word];
    for( bit = BITS_PER_WORD - 1; bit >= 0; bit-- ) {
        if( ( ( temp >> bit ) & 0x01 ) == 1 )
            break;
    }
    return word * BITS_PER_WORD + bit;
}

/**
 * Access a single byte of the big integer
 * @param a the big integer to investigate
 * @param length the length of the uint_t array
 * @param index the byte index to access
 * @return the byte to access (zero if index is out of bounds)
 */
uint8_t bigint_get_byte_var( const uint_t *a, const int length, const int index ) {
    int wordIndex = index >> LD_BYTES_PER_WORD;
    int byteIndex = index & ( BYTES_PER_WORD - 1 );
    uint_t word;

    /* handle out of bounds */
    if( ( wordIndex < 0 ) || ( wordIndex >= length ) )
        return 0;

    word = a[wordIndex];

#if( BYTES_PER_WORD == 1 )
    return word;
#else
    return ( word >> ( byteIndex << 3 ) ) & 0xFF;
#endif
}

/**
 * Set a single byte of a big integer
 * @param a the big integer to set
 * @param length the length of the uint_t array
 * @param index the byte index to access
 * @param value the byte to write to the specified index
 */
void bigint_set_byte_var( uint_t *a, const int length, const int index, const uint8_t value ) {
    int wordIndex = index >> LD_BYTES_PER_WORD;
    int byteIndex = index & ( BYTES_PER_WORD - 1 );
    uint_t word;

    /* handle out of bounds */
    if( ( wordIndex < 0 ) || ( wordIndex >= length ) )
        return;

#if( BYTES_PER_WORD == 1 )
    word = value;
#else
    word = a[wordIndex];
    byteIndex <<= 3;
    word &= ~( 0xFF << byteIndex );
    word |= value << byteIndex;
#endif
    a[wordIndex] = word;
}

/**
 * Parses a hex array and returns a uint_t array
 * @param a the resulting uint_t array
 * @param length_a the maximum length of a (in words)
 * @param array the data to parse
 * @param array_length the number of chars to parse
 * @return the number of elements in a used after parsing finished
 */
int bigint_parse_hex_var( uint_t *a, const int length_a, const char *array, const int array_length ) {
    int nibble_i = 0, a_index = 0;
    uint_t word = 0, char_converted;
    char character;
    char *array_pointer = (char *)array + array_length - 1;

    if( array_length < 0 )
        return 0;
    if( length_a < 0 )
        return 0;

    while( array_pointer != ( array - 1 ) ) {
        character = *array_pointer--;
        if( ( character >= '0' ) && ( character <= '9' ) ) {
            char_converted = character - '0';
        } else if( ( character >= 'a' ) && ( character <= 'f' ) ) {
            char_converted = character - 'a' + 10;
        } else if( ( character >= 'A' ) && ( character <= 'F' ) ) {
            char_converted = character - 'A' + 10;
        } else {
            continue;
        }

        word |= char_converted << nibble_i;
        nibble_i += 4; /* 4 bits are necessary to represent a nibble */

        if( nibble_i == BITS_PER_WORD ) {
            a[a_index++] = word;
            if( a_index == length_a )
                return a_index;
            nibble_i = 0;
            word = 0;
        }
    }
    if( nibble_i > 0 )
        a[a_index++] = word;
    return a_index;
}

/**
 * Returns the number of set bits in the given big integer.
 * @param var
 * @param length the number of elements in the uint_t array
 */
int bigint_hamming_weight_var( const uint_t *var, const int length ) {
    int word, bit, counter = 0;
    uint_t temp;
    for( word = 0; word < length; word++ ) {
        temp = var[word];
        for( bit = 0; bit < BITS_PER_WORD; bit++ ) {
            if( ( temp & ( 1 << bit ) ) > 0 )
                counter++;
        }
    }
    return counter;
}

/**
 * Simple bit-wise long division algorithm. Not fast nor optimal but simple.
 * @param N Dividend
 * @param D Divisor
 * @param Q Quotient
 * @param R Remainder
 * @param len
 */
void bigint_divide_simple_var( uint_t *Q, uint_t *R, const uint_t *N, const uint_t *D, const int words ) {
    int i;
    if( bigint_is_zero_var( D, words ) )
        return;

    bigint_clear_var( Q, words );
    bigint_clear_var( R, words );
    for( i = BITS_PER_WORD * words - 1; i >= 0; i-- ) {
        bigint_shift_left_var( R, R, 1, words );
        R[0] |= bigint_test_bit_var( N, i, words );
        if( bigint_compare_var( R, D, words ) >= 0 ) {
            bigint_subtract_var( R, R, D, words );
            bigint_set_bit_var( Q, i, 1, words );
        }
    }
}
