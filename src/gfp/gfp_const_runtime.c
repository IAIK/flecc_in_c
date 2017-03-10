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
#include "flecc_in_c/gfp/gfp_gen.h"

/**
 * Adds two numbers a,b and stores the result in res IN CONSTANT TIME.
 * A finite field reduction is automatically performed.
 * @param res the result
 * @param a the first parameter to add
 * @param b the second parameter to add
 * @param prime_data the prime number data to reduce the result
 */
void gfp_cr_add( gfp_t res, const gfp_t a, const gfp_t b, const gfp_prime_data_t *prime_data ) {
    gfp_t temp;
    int carry = bigint_add_var( res, a, b, prime_data->words );
    int carry2 = 1 + bigint_subtract_var( temp, res, prime_data->prime, prime_data->words );

    bigint_cr_select_2( res, res, temp, carry | carry2, prime_data->words );
}

/**
 * Subtract b from a, store it in res, use the modulo IN CONSTANT TIME.
 * res = (a - b) mod modulo.
 * @param a the minuend
 * @param b the subtrahend
 * @param res the difference
 * @param prime_data the prime number data to reduce the result
 */
void gfp_cr_subtract( gfp_t res, const gfp_t a, const gfp_t b, const gfp_prime_data_t *prime_data ) {
    gfp_t temp;
    int carry = -bigint_subtract_var( res, a, b, prime_data->words );
    bigint_add_var( temp, res, prime_data->prime, prime_data->words );

    bigint_cr_select_2( res, res, temp, carry, prime_data->words );
}

/**
 * Montgomery multiplication based on Separated Operand Scanning (SOS) method
 * Koc, ACar, Kaliski "Analyzing and Comparing Montgomery Multiplication
 * Algorithms" IN CONSTANT TIME
 * @param res the result = a * b * R^-1 mod prime
 * @param a first operand
 * @param b second operand
 * @param prime_data the used prime data needed to do the multiplication
 */
void gfp_cr_mont_multiply_sos( gfp_t res, const gfp_t a, const gfp_t b, const gfp_prime_data_t *prime_data ) {
    int i, j;
    ulong_t product;
    uint_t global_carry = 0;
    uint_t carry;
    uint_t temp;
    uint_t temp_buffer[2 * WORDS_PER_GFP];
    int length = prime_data->words;
    bigint_clear_var( temp_buffer, length );
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
        // TODO: carry propagation can be optimized
        for( j = i + length; j < 2 * length; j++ ) {
            product = temp_buffer[j];
            product += carry;
            temp_buffer[j] = ( product & UINT_T_MAX );
            carry = product >> BITS_PER_WORD;
        }
        global_carry += carry;
    }
    carry = 1 + bigint_subtract_var( temp_buffer, temp_buffer + length, prime_data->prime, length );
    bigint_cr_select_2( res, temp_buffer + length, temp_buffer, global_carry | carry, prime_data->words );
}

/**
 * divides a number by two and stores the result IN CONSTANT TIME
 * if number is even, divide by 2
 * if number is odd, add prime and divide by 2
 * @param res the result
 * @param a the operand to divide
 * @param prime_data the prime number data to reduce the result
 */
void gfp_cr_halving( gfp_t res, const gfp_t a, const gfp_prime_data_t *prime_data ) {
    uint_t do_addition = a[0] & 1;
    gfp_t temp;
    uint_t carry = bigint_add_var( temp, a, prime_data->prime, prime_data->words );
    bigint_cr_select_2( res, a, temp, do_addition, prime_data->words );

    bigint_shift_right_one_var( res, res, prime_data->words );
    carry &= do_addition;
    res[prime_data->words - 1] |= carry << ( BITS_PER_WORD - 1 );
}

/**
 * Negate a element modulo p IN CONSTANT TIME
 * @param a the gfp element to negate
 * @param res the negated element
 * @param prime_data the prime number data to reduce the result
 */
void gfp_cr_negate( gfp_t res, const gfp_t a, const gfp_prime_data_t *prime_data ) {
    gfp_t temp;
    bigint_subtract_var( temp, prime_data->prime, a, prime_data->words );
    int is_zero = bigint_cr_is_zero_var( a, prime_data->words );
    bigint_cr_select_2( res, temp, a, is_zero, prime_data->words );
}
