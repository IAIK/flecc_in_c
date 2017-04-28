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

#ifndef BI_GEN_H_
#define BI_GEN_H_

#include "flecc_in_c/types.h"

/** test if a big integer is even */
#define BIGINT_IS_EVEN( a ) ( ( a[0] & 1 ) == 0 )
/** test if a big integer is odd */
#define BIGINT_IS_ODD( a ) ( ( a[0] & 1 ) == 1 )

/**
 * Adds two big integers with configurable length
 *
 * Semantic: (res_carry, res) = a + b
 *
 * @param res the resulting big integer
 * @param a the first parameter to add
 * @param b the second parameter to add
 * @param length the number of uint_t elements in the big integers (>= 1)
 * @return the output carry flag
 */
int bigint_add_var( uint_t *res, const uint_t *a, const uint_t *b, const int length );

/**
 * Adds two big integers with configurable length honoring an input carry flag
 *
 * Semantic: (res_carry, res) = a + b + carry
 *
 * @param res the resulting big integer
 * @param a the first parameter to add
 * @param b the second parameter to add
 * @param length the number of uint_t elements in the big integers (>= 1)
 * @param carry input carry flag
 * @return the output carry flag
 */
int bigint_add_carry_var( uint_t *res, const uint_t *a, const uint_t *b, const int length, const int carry );

/**
 * Subtracts two big integers with configurable length
 *
 * Semantic: (res_carry, res) = a - b
 *
 * @param res the resulting difference
 * @param a the minuend
 * @param b the subtrahend
 * @param length the number of uint_t elements in the big integers (>= 1)
 * @return the output carry flag (0 or -1)
 */
int bigint_subtract_var( uint_t *res, const uint_t *a, const uint_t *b, const int length );

/**
 * Subtracts two big integers with configurable length honoring an input carry flag
 *
 * Semantic: (res_carry, res) = a - b + carry
 *
 * @param res the resulting difference
 * @param a the minuend
 * @param b the subtrahend
 * @param length the number of uint_t elements in the big integers (>= 1)
 * @param carry input carry flag
 * @return the output carry flag
 */
int bigint_subtract_carry_var( uint_t *res, const uint_t *a, const uint_t *b, const int length, const int carry );

/**
 * Clears a big integer
 *
 * Semantic: a = 0
 *
 * @param a the big integer to clear
 * @param length the number of uint_t elements in the big integers (>= 1)
 */
void bigint_clear_var( uint_t *a, const int length );

/**
 * Xors two big integers with configurable length
 *
 * Semantic: res = a ^ b
 *
 * @param res the resulting big integer
 * @param a the first operand
 * @param b the second operand
 * @param length the number of uint_t elements in the big integers (>= 1)
 */
void bigint_xor_var( uint_t *res, const uint_t *a, const uint_t *b, const int length );

/**
 * Shifts a big integer to the left with configurable length and displacement
 *
 * Semantic: res = a << left
 *
 * The bits that are shifted out of the big integer get dropped and zeros get
 * shifted into the big integer.
 *
 * @param res destination big integer
 * @param a the big integer to shift
 * @param left the number of bits to shift (0 <= left < BITS_PER_WORD * length)
 * @param length the number of uint_t elements in the big integers (>= 1)
 */
void bigint_shift_left_var( uint_t *res, const uint_t *a, const int left, const int length );

/**
 * Shifts a big integer to the right with configurable length and displacement
 *
 * Semantic: res = a >> right
 *
 * The bits that are shifted out of the big integer get dropped and zeros get
 * shifted into the big integer.
 *
 * @param res destination big integer
 * @param a the big integer to shift
 * @param right the number of bits to shift (0 <= right < BITS_PER_WORD * length)
 * @param length the number of uint_t elements in the big integers (>= 1)
 */
void bigint_shift_right_var( uint_t *res, const uint_t *a, const int right, const int length );

/**
 * Shifts a big integer with configurable length to the right by one bit
 *
 * Semantic: res = a >> 1
 *
 * The bit that is shifted out of the big integer is dropped and a zero gets
 * shifted into the big integer.
 *
 * @param res destination big integer
 * @param a the big integer to shift
 * @param length the number of uint_t elements in the big integers (>= 1)
 */
void bigint_shift_right_one_var( uint_t *res, const uint_t *a, const int length );

/**
 * Compares two big integers with configurable length.
 *
 * @param a the first operand for the comparison
 * @param b the second operand for the comparison
 * @param length the number of uint_t elements in the big integers (>= 1)
 * @return -1, 0 or 1 as *a* is numerically less than, equal to, or greater than *b*.
 */
int bigint_compare_var( const uint_t *a, const uint_t *b, const int length );

/**
 * Compares two big integers for equality with configurable length.
 *
 * @param a the first operand for the comparison
 * @param b the second operand for the comparison
 * @param length the number of uint_t elements in the big integers (>= 1)
 * @return 1 if equal; 0 if not equal
 */
int bigint_is_equal_var( const uint_t *a, const uint_t *b, const int length );

/**
 * Checks if a big integer is zero.
 *
 * @param a the big integer for the check
 * @param length the number of uint_t elements in the big integer (>= 1)
 * @return 1 if zero; otherwise 0.
 */
int bigint_is_zero_var( const uint_t *a, const int length );

/**
 * Checks if a big integer is one.
 *
 * @param a the big integer for the check
 * @param length the number of uint_t elements in the big integer (>= 1)
 * @return 1 if one; otherwise 0.
 */
int bigint_is_one_var( const uint_t *a, const int length );

/**
 * Multiply two big integers with individually configurable lengths
 *
 * Semantic: res = a * b
 *
 * @param res destination big integer (has to have length = lengthA + lengthB)
 * @param a first multiplicant
 * @param b second multiplicant
 * @param lengthA the number of uint_t elements in the big integers a
 * @param lengthB the number of uint_t elements in the big integers b
 */
void bigint_multiply_var( uint_t *res, const uint_t *a, const uint_t *b, const int lengthA, const int lengthB );

/**
 * Copy a big integer with configurable length.
 *
 * Semantic: res = a
 *
 * @param res destination big integer
 * @param a source big integer
 * @param length the number of uint_t elements in the big integers (>= 1)
 */
void bigint_copy_var( uint_t *res, const uint_t *a, const int length );

/**
 * Prints a big integer with configurable length in hex to a char buffer
 *
 * The individual words in the big integer are seperated by spaces and the final
 * string is zero terminated. The buffer therefore has to be at least
 * `length * sizeof(uint_t) * 2 + length` characters long.
 *
 * @param buffer the destination buffer (must be sufficiently large)
 * @param toPrint the big integer to print
 * @param length the number of uint_t elements in the big integer (>= 1)
 */
void bigint_print_var( char *buffer, const uint_t *toPrint, const int length );

/**
 * Sets a single bit to a specified value in a big integer with configurable length
 *
 * @param a big integer to modify
 * @param bit index of the bit to set (0 <= bit < BITS_PER_WORD * length)
 * @param value the bit value (0 || 1)
 * @param length the number of uint_t elements in the big integer (>= 1)
 */
void bigint_set_bit_var( uint_t *a, const int bit, const int value, const int length );

/**
 * Returns the state of a single bit in a big integer with configurable length
 *
 * @param a big integer to test
 * @param bit index of the bit to test (0 <= bit < BITS_PER_WORD * length)
 * @param length the number of uint_t elements in the big integer (>= 1)
 * @return 1 if bit is 1, 0 if bit is 0
 */
int bigint_test_bit_var( const uint_t *a, const int bit, const int length );

/**
 * Get the bit index of the most significant bit in a big integer with configurable length
 *
 * @param a the big integer to test
 * @param length the number of uint_t elements in the big integer (>= 1)
 * @return -1 if no bit is set, else index of the first set bit (0 <= index < BITS_PER_WORD * length)
 */
int bigint_get_msb_var( const uint_t *a, const int length );

/**
 * Access a single byte of the big integer with configurable length
 *
 * @param a the big integer to investigate
 * @param length the number of uint_t elements in the big integer (>= 1)
 * @param index the byte index to access (0 <= index < BYTES_PER_WORD * length)
 * @return the requested byte
 */
uint8_t bigint_get_byte_var( const uint_t *a, const int length, const int index );

/**
 * Set a single byte of a big integer with configurable length
 *
 * @param a the big integer to modify
 * @param length the number of uint_t elements in the big integer (>= 1)
 * @param index the byte index to access (0 <= index < BYTES_PER_WORD * length)
 * @param value the byte to write to the specified index
 */
void bigint_set_byte_var( uint_t *a, const int length, const int index, const uint8_t value );

/**
 * Parses a hex string into a big integer with configurable length
 *
 * All non hex characters (i.e., everything besides [0-9a-fA-F]) are simply
 * ignored. Note that for the uint_t elements outside of the parsed range no
 * initialization is performed.
 *
 * @param res the resulting big integer
 * @param length the number of uint_t elements in the big integer (>= 1)
 * @param array the data to parse
 * @param array_length the number of chars to parse (>= 1)
 * @return the number of uint_t elements in res used after parsing finished
 */
int bigint_parse_hex_var( uint_t *res, const int length, const char *string, const int string_length );

/**
 * Returns the number of set bits in a big integer with configurable length
 *
 * @param var the big integer to test
 * @param length the number of uint_t elements in the big integer (>= 1)
 * @return the number of set bits a big integer
 */
int bigint_hamming_weight_var( const uint_t *var, const int length );

/**
 * Simple bit-wise long division algorithm for big integers with configurable length
 *
 * Semantic: Q = N / D;  R = N % D
 *
 * @param Q Quotient
 * @param R Remainder
 * @param N Dividend
 * @param D Divisor (D != 0)
 * @param length the number of uint_t elements in the big integers (>= 1)
 */
void bigint_divide_simple_var( uint_t *Q, uint_t *R, const uint_t *N, const uint_t *D, const int length );

#endif /* BI_GEN_H_ */
