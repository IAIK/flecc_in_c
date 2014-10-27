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

#include "gfp_gen.h"
#include "../bi/bi.h"

/**
 * Adds two numbers a,b and stores the result in res. A finite field reduction
 * using modulo is automatically performed.
 * @param res the result
 * @param a the first parameter to add
 * @param b the second parameter to add
 * @param prime_data the prime number data to reduce the result
 */
void gfp_gen_add( gfp_t res, const gfp_t a, const gfp_t b, const gfp_prime_data_t *prime_data ) {
    int carry = bigint_add_var( res, a, b, prime_data->words );
    if( carry ) {
        bigint_subtract_var( res, res, prime_data->prime, prime_data->words );
    } else {
        if( bigint_compare_var( res, prime_data->prime, prime_data->words ) >= 0 ) {
            bigint_subtract_var( res, res, prime_data->prime, prime_data->words );
        }
    }
}

/**
 * Subtract b from a, store it in res, use the modulo. res = (a - b) mod modulo.
 * @param a the minuend
 * @param b the subtrahend
 * @param res the difference
 * @param prime_data the prime number data to reduce the result
 */
void gfp_gen_subtract( gfp_t res, const gfp_t a, const gfp_t b, const gfp_prime_data_t *prime_data ) {
    int carry = bigint_subtract_var( res, a, b, prime_data->words );
    if( carry ) {
        bigint_add_var( res, res, prime_data->prime, prime_data->words );
    }
}

/**
 * divides a by two and stores the result in res
 * if number is even, divide by 2
 * if number is odd, add prime and divide by 2
 * @param res the result
 * @param a the operand to divide
 * @param prime_data the prime number data to reduce the result
 */
void gfp_gen_halving( gfp_t res, const gfp_t a, const gfp_prime_data_t *prime_data ) {
    uint_t carry;
    if( BIGINT_IS_ODD( a ) ) {
        carry = bigint_add_var( res, a, prime_data->prime, prime_data->words );
        bigint_shift_right_one_var( res, res, prime_data->words );
        res[prime_data->words - 1] |= carry << ( BITS_PER_WORD - 1 );
    } else {
        bigint_shift_right_one_var( res, a, prime_data->words );
    }
}

/**
 * Negate a element modulo p
 * @param a the gfp element to negate
 * @param res the negated element
 * @param prime_data the prime number data to reduce the result
 */
void gfp_gen_negate( gfp_t res, const gfp_t a, const gfp_prime_data_t *prime_data ) {
    if( bigint_is_zero_var( a, prime_data->words ) ) {
        bigint_copy_var( res, a, prime_data->words );
    } else {
        bigint_subtract_var( res, prime_data->prime, a, prime_data->words );
    }
}

/**
 * (Slow) Division based GF(p) multiplication.
 * @param res the product
 * @param a first operand
 * @param b second operand
 * @param prime_data the prime number data to reduce the result
 */
void gfp_gen_multiply_div( gfp_t res, const gfp_t a, const gfp_t b, const gfp_prime_data_t *prime_data ) {
    uint_t product[2 * WORDS_PER_GFP];
    uint_t extended_prime[2 * WORDS_PER_GFP];
    uint_t quotient[2 * WORDS_PER_GFP];
    uint_t remainder[2 * WORDS_PER_GFP];

    bigint_multiply_var( product, a, b, prime_data->words, prime_data->words );
    bigint_clear_var( extended_prime + prime_data->words, prime_data->words );
    bigint_copy_var( extended_prime, prime_data->prime, prime_data->words );
    bigint_divide_simple_var( quotient, remainder, product, extended_prime, prime_data->words );
    bigint_copy_var( res, remainder, prime_data->words );
}

/**
 * Stupidly calculate a-=prime until a < prime
 * @param a the parameter to reduce
 * @param prime_data the prime number data to reduce the result
 */
void gfp_reduce( gfp_t a, const gfp_prime_data_t *prime_data ) {
    while( bigint_compare_var( a, prime_data->prime, prime_data->words ) >= 0 ) {
        bigint_subtract_var( a, a, prime_data->prime, prime_data->words );
    }
}
