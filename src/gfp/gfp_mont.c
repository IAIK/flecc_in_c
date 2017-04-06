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

#include "flecc_in_c/gfp/gfp_mont.h"
#include "flecc_in_c/bi/bi.h"
#include "flecc_in_c/gfp/gfp_gen.h"

/**
 * Convert a normal number (mod p) into the montgomery domain.
 * @param res the resulting number
 * @param src the source number
 * @param prime_data the used prime data needed to do the conversion
 */
void gfp_normal_to_montgomery( gfp_t res, const gfp_t src, const gfp_prime_data_t *prime_data ) {
    gfp_mont_multiply( res, src, prime_data->r_squared, prime_data );
}

/**
 * Convert a number in Montgomery domain into a normal domain.
 * @param res the resulting number
 * @param src the source number
 * @param prime_data the used prime data needed to do the conversion
 */
void gfp_montgomery_to_normal( gfp_t res, const gfp_t src, const gfp_prime_data_t *prime_data ) {
    gfp_t temp;
    bigint_clear_var( temp, prime_data->words );
    temp[0] = 1;
    gfp_mont_multiply( res, src, temp, prime_data );
}

/**
 * Montgomery multiplication based on Separated Operand Scanning (SOS) method
 * Koc, ACar, Kaliski "Analyzing and Comparing Montgomery Multiplication
 * Algorithms"
 * @param res the result = a * b * R^-1 mod prime
 * @param a first operand
 * @param b second operand
 * @param prime_data the used prime data needed to do the multiplication
 */
void gfp_mont_multiply_sos( gfp_t res, const gfp_t a, const gfp_t b, const gfp_prime_data_t *prime_data ) {
    int i, j;
    ulong_t product;
    uint_t global_carry = 0;
    uint_t carry;
    uint_t temp;
    uint_t temp_buffer[2 * WORDS_PER_GFP];
    int length = prime_data->words;
    for( i = 0; i < length; i++ )
        temp_buffer[i] = 0;
    for( i = 0; i < length; i++ ) {
        carry = 0;
        temp = a[i];
        for( j = 0; j < length; j++ ) {
            product = temp_buffer[i + j];
            product += (ulong_t)temp * (ulong_t)b[j];
            product += carry;
            temp_buffer[i + j] = ( product & UINT_T_MAX );
            carry = product >> BITS_PER_WORD;
        }
        temp_buffer[i + length] = carry;
    }
    for( i = 0; i < length; i++ ) {
        carry = 0;
        temp = temp_buffer[i] * prime_data->n0;
        for( j = 0; j < length; j++ ) {
            product = temp_buffer[i + j];
            product += (ulong_t)temp * (ulong_t)prime_data->prime[j];
            product += carry;
            temp_buffer[i + j] = ( product & UINT_T_MAX );
            carry = product >> BITS_PER_WORD;
        }
        for( j = i + length; j < 2 * length; j++ ) {
            product = temp_buffer[j];
            product += carry;
            temp_buffer[j] = ( product & UINT_T_MAX );
            carry = product >> BITS_PER_WORD;
        }
        global_carry += carry;
    }
    for( i = 0; i < length; i++ ) {
        res[i] = temp_buffer[i + length];
    }
    if( global_carry || ( bigint_compare_var( res, prime_data->prime, length ) >= 0 ) ) {
        bigint_subtract_var( res, res, prime_data->prime, length );
    }
}

/**
 * Calculate the montgomery inverse for the given globally defined constants
 * based on Hankerson p. 42
 * @param res the inverse: (a * R)^1 * R^2 mod p
 * @param a the number to invert (within the montgomery domain)
 * @param prime_data the used prime data needed to do the multiplication
 */
void gfp_mont_inverse_binary( gfp_t res, const gfp_t a, const gfp_prime_data_t *prime_data ) {
    gfp_t u;
    gfp_t v;
    gfp_t x1;
    gfp_t x2;
    int k = 0, carry = 0, length = prime_data->words;
    bigint_copy_var( u, a, length );
    bigint_copy_var( v, prime_data->prime, length );
    bigint_clear_var( x1, length );
    x1[0] = 1;
    bigint_clear_var( x2, length );

    while( !bigint_is_zero_var( v, length ) ) {
        if( BIGINT_IS_EVEN( v ) ) {
            bigint_shift_right_one_var( v, v, length );
            carry = bigint_add_var( x1, x1, x1, length );
        } else if( BIGINT_IS_EVEN( u ) ) {
            bigint_shift_right_one_var( u, u, length );
            bigint_add_var( x2, x2, x2, length );
        } else if( bigint_compare_var( v, u, length ) >= 0 ) {
            bigint_subtract_var( v, v, u, length );
            bigint_shift_right_one_var( v, v, length );
            bigint_add_var( x2, x2, x1, length );
            carry = bigint_add_var( x1, x1, x1, length );
        } else {
            bigint_subtract_var( u, u, v, length );
            bigint_shift_right_one_var( u, u, length );
            bigint_add_var( x1, x1, x2, length );
            bigint_add_var( x2, x2, x2, length );
        }
        k++;
    }
    if( carry || ( bigint_compare_var( x1, prime_data->prime, length ) >= 0 ) ) {
        bigint_subtract_var( x1, x1, prime_data->prime, length );
    }

    /* at this point x1 = a^1 * 2^k mod prime */
    /* n <= k <= 2*n */

    if( k < ( BITS_PER_WORD * length ) ) {
        bigint_copy_var( x2, x1, length ); /* needed in case of future gfp_mont_multiply optimizations */
        gfp_mont_multiply( x1, x2, prime_data->r_squared, prime_data );
        k += ( BITS_PER_WORD * length );
    }
    /* now k >= Wt */
    gfp_mont_multiply( res, x1, prime_data->r_squared, prime_data );
    if( k > ( BITS_PER_WORD * length ) ) {
        k = ( 2 * BITS_PER_WORD * length ) - k;
        bigint_clear_var( x2, length );
        bigint_set_bit_var( x2, k, 1, length );
        bigint_copy_var( x1, res, length ); /* needed in case of future gfp_mont_multiply optimizations */
        gfp_mont_multiply( res, x1, x2, prime_data );
    }
}

/**
 * Invert a number by exponentiating it with (prime-2)
 * @param result the inverted number
 * @param to_invert the number to invert
 * @param prime_data the prime number data to reduce the result
 */
void gfp_mont_inverse_fermat( gfp_t result, const gfp_t to_invert, const gfp_prime_data_t *prime_data ) {
    gfp_t exponent;
    gfp_t two;

    bigint_copy_var( exponent, prime_data->prime, prime_data->words );
    bigint_clear_var( two, prime_data->words );
    two[0] = 2;
    bigint_subtract_var( exponent, exponent, two, prime_data->words );

    gfp_mont_exponent( result, to_invert, exponent, prime_data->words, prime_data );
}

/**
 * Perform an exponentiation with a custom modulus and custom length. Does support a=res.
 * @param res a^exponent mod modulus
 * @param a
 * @param exponent
 * @param exponent_length the number of words needed to represent the exponent
 * @param prime_data the used prime data needed to do the multiplication
 */
void gfp_mont_exponent(
    gfp_t res, const gfp_t a, const uint_t *exponent, const int exponent_length, const gfp_prime_data_t *prime_data ) {
    gfp_t temp;
    int bit;

    bigint_copy_var( temp, prime_data->gfp_one, prime_data->words );
    for( bit = bigint_get_msb_var( exponent, exponent_length ); bit >= 0; bit-- ) {
        gfp_mont_multiply( temp, temp, temp, prime_data );
        if( bigint_test_bit_var( exponent, bit, exponent_length ) == 1 ) {
            gfp_mont_multiply( temp, temp, a, prime_data );
        }
    }
    bigint_copy_var( res, temp, prime_data->words );
}

/**
 * Compute the constant R, needed for montgomery multiplications
 * @param res the param R mod prime
 * @param prime_data the prime number data to reduce the result
 */
void gfp_mont_compute_R( gfp_t res, gfp_prime_data_t *prime_data ) {
    uint32_t i;
    bigint_clear_var( res, prime_data->words );
    bigint_set_bit_var( res, prime_data->bits - 1, 1, prime_data->words );

    for( i = prime_data->bits - 1; i < prime_data->words * BITS_PER_WORD; i++ ) {
        gfp_gen_add( res, res, res, prime_data );
    }
}

/**
 * Compute the constant R^2, needed for montgomery multiplications
 * @param res the param R^2 mod prime
 * @param prime_data the prime number data to reduce the result
 */
void gfp_mont_compute_R_squared( gfp_t res, gfp_prime_data_t *prime_data ) {
    uint32_t i;
    bigint_clear_var( res, prime_data->words );
    bigint_set_bit_var( res, prime_data->bits - 1, 1, prime_data->words );

    for( i = prime_data->bits - 1; i < ( 2 * prime_data->words * BITS_PER_WORD ); i++ ) {
        gfp_gen_add( res, res, res, prime_data );
    }
}

/**
 * Computes the constant n0' required for montgomery multiplications.
 * @param prime_data the prime that is used for future montgomery multiplications
 * @return the constant n0'
 */
uint_t gfp_mont_compute_n0( gfp_prime_data_t *prime_data ) {
    int i;
    uint_t t = 1;

    for( i = 1; i < BITS_PER_WORD; i++ ) {
        t = t * t;
        t = t * prime_data->prime[0];
    }
    t = -t;
    return t;
}

/**
 * Use two montgomery multiplications to compute a * b mod prime
 *   T1 = a * b * R^-1
 *   res = T1 * R^2 * R^-1 = a * b
 * @param res the result = a * b  mod prime (not in Montgomery domain)
 * @param a first operand (not in Montgomery domain)
 * @param b second operand (not in Montgomery domain)
 * @param prime_data the used prime data needed to do the multiplication
 */
void gfp_mult_two_mont( gfp_t res, const gfp_t a, const gfp_t b, const gfp_prime_data_t *prime_data ) {
    gfp_mont_multiply( res, a, b, prime_data );
    gfp_mont_multiply( res, res, prime_data->r_squared, prime_data );
}
