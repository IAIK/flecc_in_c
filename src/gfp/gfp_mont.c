/*
 * gfp_mont.c
 *
 *  Created on: 18.03.2014
 *      Author: Erich Wenger
 */

/*
 * Copyright (C) 2014 Stiftung Secure Information and
 *                    Communication Technologies SIC
 * http://www.sic.st
 *
 * All rights reserved.
 *
 * This source is provided for inspection purposes and recompilation only,
 * unless specified differently in a contract with IAIK. This source has to
 * be kept in strict confidence and must not be disclosed to any third party
 * under any circumstances. Redistribution in source and binary forms, with
 * or without modification, are <not> permitted in any case!
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */ 
 
#include "gfp_mont.h"
#include "gfp_gen.h"
#include "../bi/bi.h"

/**
 * Convert a normal number (mod p) into the montgomery domain.
 * @param res the resulting number
 * @param src the source number
 * @param prime_data the used prime data needed to do the conversion
 */
void gfp_normal_to_montgomery(gfp_t res, const gfp_t src, const gfp_prime_data_t *prime_data) {
	gfp_mont_multiply(res, src, prime_data->r_squared, prime_data);
}

/**
 * Convert a number in Montgomery domain into a normal domain.
 * @param res the resulting number
 * @param src the source number
 * @param prime_data the used prime data needed to do the conversion
 */
void gfp_montgomery_to_normal(gfp_t res, const gfp_t src, const gfp_prime_data_t *prime_data) {
	gfp_t temp;
	bigint_clear_var(temp, prime_data->words);
	temp[0] = 1;
	gfp_mont_multiply(res, src, temp, prime_data);
}


/**
 * Montgomery multiplication based on Separated Operand Scanning (SOS) method
 * Koc, ACar, Kaliski "Analyzing and Comparing Montgomery Multiplication Algorithms"
 * @param res the result = a * b * R^-1 mod prime
 * @param a first operand
 * @param b second operand
 * @param prime_data the used prime data needed to do the multiplication
 */
void gfp_mont_multiply_sos(gfp_t res, const gfp_t a, const gfp_t b, const gfp_prime_data_t *prime_data) {
    int i,j;
    ulong_t product;
    uint_t global_carry = 0;
    uint_t carry;
    uint_t temp;
    uint_t temp_buffer[2*WORDS_PER_GFP];
    int length = prime_data->words;
    for (i=0; i < length; i++)
    	temp_buffer[i] = 0;
    for (i=0; i < length; i++) {
        carry = 0;
        temp = a[i];
        for (j=0; j < length; j++) {
            product = temp_buffer[i+j];
            product += (ulong_t)temp * (ulong_t)b[j];
            product += carry;
            temp_buffer[i+j] = (product & UINT_T_MAX);
            carry = product >> BITS_PER_WORD;
        }
        temp_buffer[i+length] = carry;
    }
    for (i=0; i < length; i++) {
        carry = 0;
        temp = temp_buffer[i] * prime_data->n0;
        for (j=0; j < length; j++) {
            product = temp_buffer[i+j];
            product += (ulong_t)temp * (ulong_t)prime_data->prime[j];
            product += carry;
            temp_buffer[i+j] = (product & UINT_T_MAX);
            carry = product >> BITS_PER_WORD;
        }
        for (j = i+length; j < 2*length; j++) {
            product = temp_buffer[j];
            product += carry;
            temp_buffer[j] = (product & UINT_T_MAX);
            carry = product >> BITS_PER_WORD;
        }
        global_carry += carry;
    }
    for (i=0; i < length; i++) {
        res[i] = temp_buffer[i + length];
    }
    if(global_carry || (bigint_compare_var(res, prime_data->prime, length) >= 0)) {
        bigint_subtract_var(res, res, prime_data->prime, length);
    }
}

/**
 * Calculate the montgomery inverse for the given globally defined constants
 * based on Hankerson p. 42
 * @param res the inverse: (a * R)^1 * R^2 mod p
 * @param a the number to invert (within the montgomery domain)
 * @param prime_data the used prime data needed to do the multiplication
 */
void gfp_mont_inverse(gfp_t res, const gfp_t a, const gfp_prime_data_t *prime_data) {
	gfp_t u;
	gfp_t v;
	gfp_t x1;
	gfp_t x2;
    int k = 0, carry = 0, length = prime_data->words;
    bigint_copy_var(u, a, length);
    bigint_copy_var(v, prime_data->prime, length);
    bigint_clear_var(x1, length); x1[0] = 1;
    bigint_clear_var(x2, length);

    while(!bigint_is_zero_var(v, length)) {
        if(BIGINT_IS_EVEN(v)) {
            bigint_shift_right_one_var(v, v, length);
            carry = bigint_add_var(x1, x1, x1, length);
        } else if(BIGINT_IS_EVEN(u)) {
            bigint_shift_right_one_var(u, u, length);
            bigint_add_var(x2, x2, x2, length);
        } else if(bigint_compare_var(v, u, length) >= 0) {
            bigint_subtract_var(v, v, u, length);
            bigint_shift_right_one_var(v, v, length);
            bigint_add_var(x2, x2, x1, length);
            carry = bigint_add_var(x1, x1, x1, length);
        } else {
            bigint_subtract_var(u, u, v, length);
            bigint_shift_right_one_var(u, u, length);
            bigint_add_var(x1, x1, x2, length);
            bigint_add_var(x2, x2, x2, length);
        }
        k++;
    }
    if(carry || (bigint_compare_var(x1, prime_data->prime, length) >= 0)) {
        bigint_subtract_var(x1, x1, prime_data->prime, length);
    }

    /* at this point x1 = a^1 * 2^k mod prime */
    /* n <= k <= 2*n */

    if(k < (BITS_PER_WORD * length)) {
        bigint_copy_var(x2, x1, length); /* needed in case of future gfp_mont_multiply optimizations */
        gfp_mont_multiply(x1, x2, prime_data->r_squared, prime_data);
        k += (BITS_PER_WORD * length);
    }
    /* now k >= Wt */
    gfp_mont_multiply(res, x1, prime_data->r_squared, prime_data);
    if(k > (BITS_PER_WORD * length)) {
        k = (2 * BITS_PER_WORD * length) - k;
        bigint_clear_var(x2, length);
        bigint_set_bit_var(x2, k, 1, length);
        bigint_copy_var(x1, res, length);  /* needed in case of future gfp_mont_multiply optimizations */
        gfp_mont_multiply(res, x1, x2, prime_data);
    }
}


/**
 * Perform an exponentiation with a custom modulus and custom length.
 * Does not support a==res.
 * @param res a^exponent mod modulus
 * @param a
 * @param exponent
 * @param exponent_length the number of words needed to represent the exponent
 * @param prime_data the used prime data needed to do the multiplication
 */
void gfp_mont_exponent(gfp_t res, const gfp_t a, const uint_t* exponent, const int exponent_length, const gfp_prime_data_t *prime_data) {
    int bit;

    bigint_copy_var(res, prime_data->gfp_one, prime_data->words);
    for (bit = bigint_get_msb_var(exponent, exponent_length); bit >= 0; bit --) {
    	gfp_mont_multiply(res, res, res, prime_data);
        if (bigint_test_bit_var(exponent, bit, exponent_length) == 1) {
        	gfp_mont_multiply(res, res, a, prime_data);
        }
    }
}

/**
 * Compute the constant R, needed for montgomery multiplications
 * @param res the param R mod prime
 * @param prime_data the prime number data to reduce the result
 */
void gfp_mont_compute_R(gfp_t res, gfp_prime_data_t *prime_data) {
	int i;
	bigint_clear_var(res, prime_data->words);
	bigint_set_bit_var(res,prime_data->bits-1,1,prime_data->words);

	for (i = prime_data->bits-1; i < prime_data->words*BITS_PER_WORD; i++) {
		gfp_gen_add(res, res, res, prime_data);
	}
}

/**
 * Compute the constant R^2, needed for montgomery multiplications
 * @param res the param R^2 mod prime
 * @param prime_data the prime number data to reduce the result
 */
void gfp_mont_compute_R_squared(gfp_t res, gfp_prime_data_t *prime_data) {
	int i;
	bigint_clear_var(res, prime_data->words);
	bigint_set_bit_var(res,prime_data->bits-1,1,prime_data->words);

	for (i = prime_data->bits-1; i < 2*prime_data->words*BITS_PER_WORD; i++) {
		gfp_gen_add(res, res, res, prime_data);
	}
}

/**
 * Computes the constant n0' required for montgomery multiplications.
 * @param prime_data the prime that is used for future montgomery multiplications
 * @return the constant n0'
 */
uint_t gfp_mont_compute_n0(gfp_prime_data_t *prime_data) {
	int i;
	uint_t t = 1;

	for (i = 1; i < BITS_PER_WORD; i++) {
		t = t*t;
		t = t*prime_data->prime[0];
	}
	t = -t;
	return t;
}

/**
 * Use two montgomery multiplications to compute a * b mod prime
 * @param res the result = a * b  mod prime
 * @param a first operand
 * @param b second operand
 * @param prime_data the used prime data needed to do the multiplication
 */
void gfp_mult_two_mont(gfp_t res, const gfp_t a, const gfp_t b, const gfp_prime_data_t *prime_data) {
	gfp_mont_multiply(res, a, b, prime_data);
	gfp_mont_multiply(res, res, prime_data->r_squared, prime_data);
}


