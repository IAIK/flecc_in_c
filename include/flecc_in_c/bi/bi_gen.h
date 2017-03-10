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

#include "../types.h"

/** test if a big integer is even */
#define BIGINT_IS_EVEN( a ) ( ( a[0] & 1 ) == 0 )
/** test if a big integer is odd */
#define BIGINT_IS_ODD( a ) ( ( a[0] & 1 ) == 1 )

int bigint_add_var( uint_t *res, const uint_t *a, const uint_t *b, const int length );
int bigint_add_carry_var( uint_t *res, const uint_t *a, const uint_t *b, const int length, const int carry );
int bigint_subtract_var( uint_t *res, const uint_t *a, const uint_t *b, const int length );
int bigint_subtract_carry_var( uint_t *res, const uint_t *a, const uint_t *b, const int length, const int carry );
void bigint_clear_var( uint_t *a, const int length );
void bigint_xor_var( uint_t *res, const uint_t *a, const uint_t *b, const int length );
void bigint_shift_left_var( uint_t *res, const uint_t *a, const int left, const int length );
void bigint_shift_right_var( uint_t *res, const uint_t *a, const int right, const int length );
void bigint_shift_right_one_var( uint_t *res, const uint_t *a, const int length );
int bigint_compare_var( const uint_t *a, const uint_t *b, const int length );
int bigint_is_equal_var( const uint_t *a, const uint_t *b, const int length );
int bigint_is_zero_var( const uint_t *a, const int length );
int bigint_is_one_var( const uint_t *a, const int length );
void bigint_multiply_var( uint_t *result, const uint_t *a, const uint_t *b, const int length_a, const int length_b );
void bigint_copy_var( uint_t *dest, const uint_t *source, const int length );
void bigint_print_var( char *buffer, const uint_t *to_print, const int length );
int bigint_parse_hex_var( uint_t *a, const int length_a, const char *array, const int array_length );
void bigint_set_bit_var( uint_t *a, const int bit, const int value, const int length );
int bigint_test_bit_var( const uint_t *a, const int bit, const int length );
int bigint_get_msb_var( const uint_t *a, const int length );
uint8_t bigint_get_byte_var( const uint_t *a, const int length, const int index );
void bigint_set_byte_var( uint_t *a, const int length, const int index, const uint8_t value );
int bigint_hamming_weight_var( const uint_t *var, const int length );
void bigint_divide_simple_var( uint_t *Q, uint_t *R, const uint_t *N, const uint_t *D, const int words );

#endif /* BI_GEN_H_ */
