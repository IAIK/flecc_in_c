/*
 * bi_gen.h
 *
 *  Created on: 14.03.2014
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
 
#ifndef BI_GEN_H_
#define BI_GEN_H_

#include "../types.h"

/** test if a big integer is even */
#define BIGINT_IS_EVEN(a) ((a[0] & 1) == 0)
/** test if a big integer is odd */
#define BIGINT_IS_ODD(a) ((a[0] & 1) == 1)

int bigint_add_var(uint_t *res, const uint_t *a, const uint_t *b, const int length);
int bigint_add_carry_var(uint_t *res, const uint_t *a, const uint_t *b, const int length, const int carry);
int bigint_subtract_var(uint_t *res, const uint_t *a, const uint_t *b, const int length);
int bigint_subtract_carry_var(uint_t *res, const uint_t *a, const uint_t *b, const int length, const int carry);
void bigint_clear_var(uint_t *a, const int length);
void bigint_xor_var(uint_t *res, const uint_t *a, const uint_t *b, const int length);
void bigint_shift_left_var(uint_t *res, const uint_t *a, const int left, const int length);
void bigint_shift_right_var(uint_t *res, const uint_t *a, const int right, const int length);
void bigint_shift_right_one_var(uint_t *res, const uint_t *a, const int length);
int bigint_compare_var(const uint_t *a, const uint_t *b, const int length);
int bigint_is_zero_var(const uint_t *a, const int length);
void bigint_multiply_var(uint_t *result, const uint_t *a, const uint_t *b, const int length_a, const int length_b);
void bigint_copy_var(uint_t *dest, const uint_t *source, const int length);
void bigint_print_var(char *buffer, const uint_t* to_print, const int length);
int bigint_parse_hex_var(uint_t *a, const int length_a, const char *array, const int array_length);
void bigint_set_bit_var(uint_t* a, const int bit, const int value, const int length);
int bigint_test_bit_var( const uint_t* a, const int bit, const int length);
int bigint_get_msb_var( const uint_t* a, const int length);
uint8_t bigint_get_byte_var(const uint_t* a, const int length, const int index);
void bigint_set_byte_var(uint_t* a, const int length, const int index, const uint8_t value);
int bigint_hamming_weight_var(const uint_t *var, const int length);
void bigint_divide_simple_var(uint_t *Q, uint_t *R, const uint_t *N, const uint_t *D, const int words);


#endif /* BI_GEN_H_ */
