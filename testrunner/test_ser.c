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

#include "test_ser.h"
#include "assert.h"

#include <flecc_in_c/bi/bi.h>
#include <flecc_in_c/eccp/eccp.h>
#include <flecc_in_c/gfp/gfp.h>
#include <flecc_in_c/hash/sha1.h>
#include <flecc_in_c/hash/sha2.h>
#include <flecc_in_c/io/io.h>
#include <flecc_in_c/protocols/protocols.h>
#include <flecc_in_c/utils/param.h>
#include <flecc_in_c/utils/parse.h>
#include <flecc_in_c/utils/performance.h>

#include <stdio.h>
#include <string.h>

//#define READ_BUFFER_SIZE 4*BYTES_PER_GFP+2*WORDS_PER_GFP+10
#define READ_BUFFER_SIZE 1000

/**
 * Reads a message in hexadecimal format from the default
 * input source, parses it, and stores it to the array
 * message.
 * @param buffer the buffer to be used
 * @param buf_length the length of the buffer being used
 * @param message the message being read and parsed
 * @param message_length the maximum length of the message
 * @return the message length
 */
int read_message( char *buffer, const int buf_length, uint8_t *message, const int message_length ) {
    int nibbles = io_gen_readline( buffer, buf_length );
    return parse_hex_message( (char *)message, message_length, buffer, nibbles );
}

/**
 * Reads and integer from the default input source.
 * @param buffer the buffer to be used
 * @param buf_length the length of the buffer being used
 * @return the integer that was read
 */
int read_integer( char *buffer, const int buf_length ) {
    int digits = io_gen_readline( buffer, buf_length - 1 );
    buffer[digits] = 0;
    return parse_integer( buffer );
}

/**
 * Tests if the line starts with the specified string.
 * @param to_compare the string to compare
 * @return 1 if the line starts with to_compare
 */
int line_starts_with( const char *line, const char *to_compare ) {
    const char *buffer = line;

    while( *to_compare != '\0' ) {
        if( *buffer != *to_compare ) {
            return 0;
        }
        buffer++;
        to_compare++;
    }

    return 1;
}

/**
 * Reads a curve type from the default input source.
 * @param buffer the buffer to be used
 * @param buf_length the length of the buffer being used
 * @return the type of curve
 */
curve_type_t read_curve_type( char *buffer, const int buf_length ) {
    int length = io_gen_readline( buffer, buf_length );
    return param_get_curve_type_from_name( buffer, length );
}

/**
 * Reads a bigint from the default input source.
 * @param buffer the buffer to be used
 * @param buf_length the length of the buffer to be used
 * @param big_int the big integer to be read
 * @param bi_length the length of the big integer to be read
 */
void read_bigint( char *buffer, const int buf_length, uint_t *big_int, const int bi_length ) {
    int len = io_gen_readline( buffer, buf_length );
    bigint_parse_hex_var( big_int, bi_length, buffer, len );
}

/**
 * Reads an element in Fp from the default input source.
 * @param buffer the buffer to be used
 * @param buf_length the length of the buffer to be used
 * @param big_int the fp variable to be read
 * @param mont_data the parameters required for operating in Montgomery domain
 * @param montgomery_domain specifies whether data is converted to Montgomery
 * domain or not
 */
void read_gfp(
    char *buffer, const int buf_length, uint_t *big_int, const gfp_prime_data_t *prime_data, uint8_t montgomery_domain ) {
    read_bigint( buffer, buf_length, big_int, prime_data->words );
    if( prime_data->montgomery_domain == 1 && montgomery_domain == 1 ) {
        gfp_normal_to_montgomery( big_int, big_int, prime_data );
    }
}

/**
 * Reads an elliptic curve point in affine coordinates from
 * the default input source.
 * @param buffer the buffer to be used
 * @param buf_length the length of the buffer to be used
 * @param ec_point the elliptic curve point to be filled with the data being
 * read
 * @param mont_data the parameters required for operating in Montgomery domain
 * @param montgomery_domain specifies whether data is converted to Montgomery
 * domain or not
 */
void read_eccp_affine_point( char *buffer,
                             const int buf_length,
                             eccp_point_affine_t *ec_point,
                             const gfp_prime_data_t *mont_data,
                             uint8_t montgomery_domain ) {
    read_gfp( buffer, buf_length, ec_point->x, mont_data, montgomery_domain );
    read_gfp( buffer, buf_length, ec_point->y, mont_data, montgomery_domain );
    ec_point->identity = read_integer( buffer, buf_length );
}

/**
 * Reads parameters from the default input source.
 * @param buffer the buffer to be used
 * @param buf_length the length of the buffer to be used
 * @param param the to-be-initialized elliptic curve parameters
 */
void read_elliptic_curve_parameters( char *buffer, const int buf_length, eccp_parameters_t *param ) {
    // 1. name of curve as string (discarded)
    io_gen_readline( buffer, buf_length );

    // 2. number of bits of prime field
    read_bigint( buffer, buf_length, &param->prime_data.bits, WORDS_PER_BITS( 32 ) );
    if( param->prime_data.bits > BITS_PER_GFP ) {
        return;
    }
    param->prime_data.words = WORDS_PER_BITS( param->prime_data.bits );
    param->prime_data.montgomery_domain = 1;

    // 3. init the prime field
    read_bigint( buffer, buf_length, param->prime_data.prime, param->prime_data.words );
    gfp_mont_compute_R( param->prime_data.gfp_one, &( param->prime_data ) );
    gfp_mont_compute_R_squared( param->prime_data.r_squared, &( param->prime_data ) );
    param->prime_data.n0 = gfp_mont_compute_n0( &( param->prime_data ) );

    // 4. number of bits of group order
    read_bigint( buffer, buf_length, &param->order_n_data.bits, WORDS_PER_BITS( 32 ) );
    if( param->order_n_data.bits > BITS_PER_GFP ) {
        return;
    }
    param->order_n_data.words = WORDS_PER_BITS( param->order_n_data.bits );
    param->order_n_data.montgomery_domain = 0;

    // 5. init the prime field for the group order
    read_bigint( buffer, buf_length, param->order_n_data.prime, param->order_n_data.words );
    gfp_mont_compute_R( param->order_n_data.gfp_one, &( param->order_n_data ) );
    gfp_mont_compute_R_squared( param->order_n_data.r_squared, &( param->order_n_data ) );
    param->order_n_data.n0 = gfp_mont_compute_n0( &( param->order_n_data ) );

    // 6. elliptic curve parameters
    read_gfp( buffer, buf_length, param->param_a, &param->prime_data, 1 );
    read_gfp( buffer, buf_length, param->param_b, &param->prime_data, 1 );

    // 7. the co-factor
    read_bigint( buffer, buf_length, &param->h, WORDS_PER_BITS( 32 ) );

    // 8. the base point
    read_gfp( buffer, buf_length, param->base_point.x, &param->prime_data, 1 );
    read_gfp( buffer, buf_length, param->base_point.y, &param->prime_data, 1 );
    param->base_point.identity = 0;

    // 9. finalize
    param->curve_type = CUSTOM;
    param->eccp_mul = &eccp_protected_point_multiply;
    param->eccp_mul_base_point = NULL;
    param->base_point_precomputed_table = NULL;
    param->base_point_precomputed_table_width = 0;
}

#define TBL_WIDTH 5

/**
 * Reads test cases from the default input stream and executes and verifies them
 * @return the number of failed tests
 */
unsigned test_ser() {
    char buffer[READ_BUFFER_SIZE];
    char test_id[50];
    unsigned errors = 0;

    uint_t bi_var_a[WORDS_PER_GFP];
    uint_t bi_var_b[WORDS_PER_GFP];
    uint_t bi_var_c[2 * WORDS_PER_GFP];
    uint_t bi_var_expected[2 * WORDS_PER_GFP];
    eccp_parameters_t curve_params;
    eccp_parameters_t *param = &curve_params;
    int length;

    eccp_point_affine_t ecaff_var_a;
    eccp_point_affine_t ecaff_var_b;
    eccp_point_affine_t ecaff_var_c;
    eccp_point_affine_t ecaff_var_expected;

    eccp_point_projective_t ecproj_var_a;
    eccp_point_projective_t ecproj_var_b;
    eccp_point_projective_t ecproj_var_c;

    curve_params.curve_type = read_curve_type( buffer, READ_BUFFER_SIZE );
    if( curve_params.curve_type == CUSTOM ) {
        read_elliptic_curve_parameters( buffer, READ_BUFFER_SIZE, param );
    } else {
        param_load( param, curve_params.curve_type );
        if( param->curve_type == UNKNOWN ) {
            puts( "Error! Curve parameters could not be loaded!" );
            return 1;
        }
    }
    length = curve_params.prime_data.words;

    param->base_point_precomputed_table_width = TBL_WIDTH;

    eccp_point_affine_t comb_table[JCB_COMB_WOZ_TBL_SIZE( TBL_WIDTH )];
    param->base_point_precomputed_table = comb_table;
    eccp_jacobian_point_multiply_COMB_WOZ_precompute( param );

    // eccp_point_affine_t comb_table[JCB_COMB_TBL_SIZE( TBL_WIDTH )];
    // param->base_point_precomputed_table = comb_table;
    // eccp_jacobian_point_multiply_COMB_precompute( param );

    while( 1 ) {
        int line_lenth = io_gen_readline( buffer, READ_BUFFER_SIZE - 1 );
        buffer[line_lenth] = 0;

        strncpy( test_id, buffer, sizeof( test_id ) );
        test_id[sizeof( test_id ) - 1] = 0;

        if( line_starts_with( buffer, "exit" ) ) {
            return errors;
        }

        if( line_starts_with( buffer, "bigint_add_carry" ) ) {

            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_a, length );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_b, length );
            int carry = read_integer( buffer, READ_BUFFER_SIZE );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_expected, length );
            int carry_expected = read_integer( buffer, READ_BUFFER_SIZE );
            int carry_result = bigint_add_carry_var( bi_var_c, bi_var_a, bi_var_b, length, carry );
            errors += assert_bigint( test_id, bi_var_expected, bi_var_c, length );
            errors += assert_integer( test_id, carry_expected, carry_result );
        } else if( line_starts_with( buffer, "bigint_add" ) ) {

            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_a, length );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_b, length );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_expected, length );
            int carry_expected = read_integer( buffer, READ_BUFFER_SIZE );
            int carry_result = bigint_add_var( bi_var_c, bi_var_a, bi_var_b, length );
            errors += assert_bigint( test_id, bi_var_expected, bi_var_c, length );
            errors += assert_integer( test_id, carry_expected, carry_result );
        } else if( line_starts_with( buffer, "bigint_subtract_carry" ) ) {

            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_a, length );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_b, length );
            int carry = read_integer( buffer, READ_BUFFER_SIZE );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_expected, length );
            int carry_expected = read_integer( buffer, READ_BUFFER_SIZE );
            int carry_result = -bigint_subtract_carry_var( bi_var_c, bi_var_a, bi_var_b, length, carry );
            errors += assert_bigint( test_id, bi_var_expected, bi_var_c, length );
            errors += assert_integer( test_id, carry_expected, carry_result );
        } else if( line_starts_with( buffer, "bigint_subtract" ) ) {

            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_a, length );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_b, length );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_expected, length );
            int carry_expected = read_integer( buffer, READ_BUFFER_SIZE );
            int carry_result = -bigint_subtract_var( bi_var_c, bi_var_a, bi_var_b, length );
            errors += assert_bigint( test_id, bi_var_expected, bi_var_c, length );
            errors += assert_integer( test_id, carry_expected, carry_result );
        } else if( line_starts_with( buffer, "bigint_xor" ) ) {

            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_a, length );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_b, length );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_expected, length );
            bigint_xor_var( bi_var_c, bi_var_a, bi_var_b, length );
            errors += assert_bigint( test_id, bi_var_expected, bi_var_c, length );
        } else if( line_starts_with( buffer, "bigint_multiply" ) ) {

            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_a, length );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_b, length );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_expected, 2 * length );
            bigint_multiply_var( bi_var_c, bi_var_a, bi_var_b, length, length );
            errors += assert_bigint( test_id, bi_var_expected, bi_var_c, 2 * length );
        } else if( line_starts_with( buffer, "bigint_get_bit" ) ) {

            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_a, length );
            int bit = read_integer( buffer, READ_BUFFER_SIZE );
            int expected = read_integer( buffer, READ_BUFFER_SIZE );
            int value = bigint_test_bit_var( bi_var_a, bit, length );
            errors += assert_integer( test_id, expected, value );
        } else if( line_starts_with( buffer, "bigint_set_bit" ) ) {

            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_a, length );
            int bit = read_integer( buffer, READ_BUFFER_SIZE );
            int value = read_integer( buffer, READ_BUFFER_SIZE );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_expected, length );
            bigint_set_bit_var( bi_var_a, bit, value, length );
            errors += assert_bigint( test_id, bi_var_expected, bi_var_a, length );
        } else if( line_starts_with( buffer, "bigint_get_byte" ) ) {

            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_a, length );
            int byte_num = read_integer( buffer, READ_BUFFER_SIZE );
            int expected = read_integer( buffer, READ_BUFFER_SIZE );
            uint8_t byte_value = bigint_get_byte_var( bi_var_a, length, byte_num );
            errors += assert_integer( test_id, expected, byte_value );
        } else if( line_starts_with( buffer, "bigint_set_byte" ) ) {

            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_a, length );
            int byte_num = read_integer( buffer, READ_BUFFER_SIZE );
            int value = read_integer( buffer, READ_BUFFER_SIZE );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_expected, length );
            bigint_set_byte_var( bi_var_a, length, byte_num, (uint8_t)value );
            errors += assert_bigint( test_id, bi_var_expected, bi_var_a, length );
        } else if( line_starts_with( buffer, "bigint_get_msb" ) ) {

            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_a, length );
            int expected = read_integer( buffer, READ_BUFFER_SIZE );
            int msb = bigint_get_msb_var( bi_var_a, length );
            errors += assert_integer( test_id, expected, msb );
        } else if( line_starts_with( buffer, "bigint_is_zero" ) ) {

            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_a, length );
            int expected = read_integer( buffer, READ_BUFFER_SIZE );
            int is_zero = bigint_is_zero_var( bi_var_a, length );
            errors += assert_integer( test_id, expected, is_zero );
        } else if( line_starts_with( buffer, "bigint_shift_left" ) ) {

            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_a, length );
            int bits = read_integer( buffer, READ_BUFFER_SIZE );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_expected, length );
            bigint_shift_left_var( bi_var_b, bi_var_a, bits, length );
            errors += assert_bigint( test_id, bi_var_expected, bi_var_b, length );
        } else if( line_starts_with( buffer, "bigint_shift_right_one" ) ) {

            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_a, length );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_expected, length );
            bigint_shift_right_one_var( bi_var_b, bi_var_a, length );
            errors += assert_bigint( test_id, bi_var_expected, bi_var_b, length );
        } else if( line_starts_with( buffer, "bigint_shift_right" ) ) {

            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_a, length );
            int bits = read_integer( buffer, READ_BUFFER_SIZE );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_expected, length );
            bigint_shift_right_var( bi_var_b, bi_var_a, bits, length );
            errors += assert_bigint( test_id, bi_var_expected, bi_var_b, length );
        } else if( line_starts_with( buffer, "bigint_divide_simple" ) ) {

            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_a, length );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_b, length );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_expected, length );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_expected + length, length );
            bigint_divide_simple_var( bi_var_c, bi_var_c + length, bi_var_a, bi_var_b, length );
            errors += assert_bigint( test_id, bi_var_expected, bi_var_c, length );
            errors += assert_bigint( test_id, bi_var_expected + length, bi_var_c + length, length );
        } else if( line_starts_with( buffer, "bigint_hamming_weight" ) ) {

            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_a, length );
            int expected = read_integer( buffer, READ_BUFFER_SIZE );
            int hamming_weight = bigint_hamming_weight_var( bi_var_a, length );
            errors += assert_integer( test_id, expected, hamming_weight );
        } else if( line_starts_with( buffer, "gfp_add" ) ) {

            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_a, length );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_b, length );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_expected, length );
            gfp_add( bi_var_c, bi_var_a, bi_var_b );
            errors += assert_bigint( test_id, bi_var_expected, bi_var_c, length );
        } else if( line_starts_with( buffer, "gfp_subtract" ) ) {

            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_a, length );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_b, length );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_expected, length );
            gfp_subtract( bi_var_c, bi_var_a, bi_var_b );
            errors += assert_bigint( test_id, bi_var_expected, bi_var_c, length );
        } else if( line_starts_with( buffer, "gfp_halve" ) ) {

            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_a, length );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_expected, length );
            gfp_halving( bi_var_c, bi_var_a );
            errors += assert_bigint( test_id, bi_var_expected, bi_var_c, length );
        } else if( line_starts_with( buffer, "gfp_negate" ) ) {

            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_a, length );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_expected, length );
            gfp_negate( bi_var_c, bi_var_a );
            errors += assert_bigint( test_id, bi_var_expected, bi_var_c, length );
        } else if( line_starts_with( buffer, "gfp_mont_multiply_order_n" ) ) {

            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_a, curve_params.order_n_data.words );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_b, curve_params.order_n_data.words );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_expected, curve_params.order_n_data.words );
            gfp_mont_multiply( bi_var_c, bi_var_a, bi_var_b, &( curve_params.order_n_data ) );
            errors += assert_bigint( test_id, bi_var_expected, bi_var_c, curve_params.order_n_data.words );
        } else if( line_starts_with( buffer, "gfp_mont_exponentiate_order_n" ) ) {

            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_a, curve_params.order_n_data.words );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_b, curve_params.order_n_data.words );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_expected, curve_params.order_n_data.words );
            gfp_mont_exponent( bi_var_c, bi_var_a, bi_var_b, length, &( curve_params.order_n_data ) );
            errors += assert_bigint( test_id, bi_var_expected, bi_var_c, curve_params.order_n_data.words );
        } else if( line_starts_with( buffer, "gfp_mont_inverse_order_n" ) ) {

            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_a, curve_params.order_n_data.words );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_expected, curve_params.order_n_data.words );
            gfp_mont_inverse( bi_var_c, bi_var_a, &( curve_params.order_n_data ) );
            errors += assert_bigint( test_id, bi_var_expected, bi_var_c, curve_params.order_n_data.words );
        } else if( line_starts_with( buffer, "gfp_mont_parameters_order_n" ) ) {

            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_expected, curve_params.order_n_data.words );
            errors
                += assert_bigint( test_id, bi_var_expected, curve_params.order_n_data.gfp_one, curve_params.order_n_data.words );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_expected, curve_params.order_n_data.words );
            errors += assert_bigint( test_id, bi_var_expected, curve_params.order_n_data.r_squared,
                                     curve_params.order_n_data.words );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_expected, curve_params.order_n_data.words );
            errors += assert_integer( test_id, bi_var_expected[0], curve_params.order_n_data.n0 );
        } else if( line_starts_with( buffer, "gfp_mont_multiply" ) ) {

            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_a, length );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_b, length );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_expected, length );
            gfp_mont_multiply( bi_var_c, bi_var_a, bi_var_b, &( curve_params.prime_data ) );
            errors += assert_bigint( test_id, bi_var_expected, bi_var_c, length );
        } else if( line_starts_with( buffer, "gfp_mont_exponentiate" ) ) {

            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_a, length );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_b, length );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_expected, length );
            gfp_mont_exponent( bi_var_c, bi_var_a, bi_var_b, length, &( curve_params.prime_data ) );
            errors += assert_bigint( test_id, bi_var_expected, bi_var_c, length );
        } else if( line_starts_with( buffer, "gfp_mont_inverse" ) ) {

            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_a, length );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_expected, length );
            gfp_mont_inverse( bi_var_c, bi_var_a, &( curve_params.prime_data ) );
            errors += assert_bigint( test_id, bi_var_expected, bi_var_c, length );
        } else if( line_starts_with( buffer, "gfp_mont_parameters" ) ) {

            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_expected, length );
            errors += assert_bigint( test_id, bi_var_expected, curve_params.prime_data.gfp_one, length );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_expected, length );
            errors += assert_bigint( test_id, bi_var_expected, curve_params.prime_data.r_squared, length );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_expected, length );
            errors += assert_integer( test_id, bi_var_expected[0], curve_params.prime_data.n0 );
        } else if( line_starts_with( buffer, "eccp_affine_point_add" ) ) {

            read_eccp_affine_point( buffer, READ_BUFFER_SIZE, &ecaff_var_a, &( curve_params.prime_data ), 1 );
            read_eccp_affine_point( buffer, READ_BUFFER_SIZE, &ecaff_var_b, &( curve_params.prime_data ), 1 );
            read_eccp_affine_point( buffer, READ_BUFFER_SIZE, &ecaff_var_expected, &( curve_params.prime_data ), 1 );
            eccp_affine_point_add( &ecaff_var_c, &ecaff_var_a, &ecaff_var_b, param );
            errors += assert_integer( test_id, ecaff_var_expected.identity, ecaff_var_c.identity );
            if( ecaff_var_expected.identity == 0 ) {
                errors += assert_bigint( test_id, ecaff_var_expected.x, ecaff_var_c.x, length );
                errors += assert_bigint( test_id, ecaff_var_expected.y, ecaff_var_c.y, length );
            }
        } else if( line_starts_with( buffer, "eccp_affine_point_double" ) ) {

            read_eccp_affine_point( buffer, READ_BUFFER_SIZE, &ecaff_var_a, &( curve_params.prime_data ), 1 );
            read_eccp_affine_point( buffer, READ_BUFFER_SIZE, &ecaff_var_expected, &( curve_params.prime_data ), 1 );
            eccp_affine_point_double( &ecaff_var_c, &ecaff_var_a, param );
            errors += assert_integer( test_id, ecaff_var_expected.identity, ecaff_var_c.identity );
            if( ecaff_var_expected.identity == 0 ) {
                errors += assert_bigint( test_id, ecaff_var_expected.x, ecaff_var_c.x, length );
                errors += assert_bigint( test_id, ecaff_var_expected.y, ecaff_var_c.y, length );
            }
        } else if( line_starts_with( buffer, "eccp_affine_point_negate" ) ) {

            read_eccp_affine_point( buffer, READ_BUFFER_SIZE, &ecaff_var_a, &( curve_params.prime_data ), 1 );
            read_eccp_affine_point( buffer, READ_BUFFER_SIZE, &ecaff_var_expected, &( curve_params.prime_data ), 1 );
            eccp_affine_point_negate( &ecaff_var_c, &ecaff_var_a, param );
            errors += assert_integer( test_id, ecaff_var_expected.identity, ecaff_var_c.identity );
            if( ecaff_var_expected.identity == 0 ) {
                errors += assert_bigint( test_id, ecaff_var_expected.x, ecaff_var_c.x, length );
                errors += assert_bigint( test_id, ecaff_var_expected.y, ecaff_var_c.y, length );
            }
        } else if( line_starts_with( buffer, "eccp_affine_point_is_valid" ) ) {

            read_eccp_affine_point( buffer, READ_BUFFER_SIZE, &ecaff_var_a, &( curve_params.prime_data ), 1 );
            int expected = read_integer( buffer, READ_BUFFER_SIZE );
            int is_valid = eccp_affine_point_is_valid( &ecaff_var_a, param );
            errors += assert_integer( test_id, expected, is_valid );
        } else if( line_starts_with( buffer, "eccp_affine_point_compare" ) ) {

            read_eccp_affine_point( buffer, READ_BUFFER_SIZE, &ecaff_var_a, &( curve_params.prime_data ), 0 );
            read_eccp_affine_point( buffer, READ_BUFFER_SIZE, &ecaff_var_b, &( curve_params.prime_data ), 0 );

            int expected = read_integer( buffer, READ_BUFFER_SIZE );
            int compare = eccp_affine_point_compare( &ecaff_var_a, &ecaff_var_b, param );

            errors += assert_integer( test_id, expected, compare );
        } else if( line_starts_with( buffer, "eccp_jacobian_point_add_affine" ) ) {

            read_eccp_affine_point( buffer, READ_BUFFER_SIZE, &ecaff_var_a, &( curve_params.prime_data ), 1 );
            read_eccp_affine_point( buffer, READ_BUFFER_SIZE, &ecaff_var_b, &( curve_params.prime_data ), 1 );
            read_eccp_affine_point( buffer, READ_BUFFER_SIZE, &ecaff_var_expected, &( curve_params.prime_data ), 1 );

            eccp_affine_to_jacobian( &ecproj_var_a, &ecaff_var_a, param );
            eccp_jacobian_point_add_affine( &ecproj_var_c, &ecproj_var_a, &ecaff_var_b, param );
            eccp_jacobian_to_affine( &ecaff_var_c, &ecproj_var_c, param );

            errors += assert_integer( test_id, ecaff_var_expected.identity, ecaff_var_c.identity );
            if( ecaff_var_expected.identity == 0 ) {
                errors += assert_bigint( test_id, ecaff_var_expected.x, ecaff_var_c.x, length );
                errors += assert_bigint( test_id, ecaff_var_expected.y, ecaff_var_c.y, length );
            }
        } else if( line_starts_with( buffer, "eccp_jacobian_point_add" ) ) {

            read_eccp_affine_point( buffer, READ_BUFFER_SIZE, &ecaff_var_a, &( curve_params.prime_data ), 1 );
            read_eccp_affine_point( buffer, READ_BUFFER_SIZE, &ecaff_var_b, &( curve_params.prime_data ), 1 );
            read_eccp_affine_point( buffer, READ_BUFFER_SIZE, &ecaff_var_expected, &( curve_params.prime_data ), 1 );

            eccp_affine_to_jacobian( &ecproj_var_a, &ecaff_var_a, param );
            eccp_affine_to_jacobian( &ecproj_var_b, &ecaff_var_b, param );
            eccp_jacobian_point_add( &ecproj_var_c, &ecproj_var_a, &ecproj_var_b, param );
            eccp_jacobian_to_affine( &ecaff_var_c, &ecproj_var_c, param );

            errors += assert_integer( test_id, ecaff_var_expected.identity, ecaff_var_c.identity );
            if( ecaff_var_expected.identity == 0 ) {
                errors += assert_bigint( test_id, ecaff_var_expected.x, ecaff_var_c.x, length );
                errors += assert_bigint( test_id, ecaff_var_expected.y, ecaff_var_c.y, length );
            }
        } else if( line_starts_with( buffer, "eccp_jacobian_point_double" ) ) {

            read_eccp_affine_point( buffer, READ_BUFFER_SIZE, &ecaff_var_a, &( curve_params.prime_data ), 1 );
            read_eccp_affine_point( buffer, READ_BUFFER_SIZE, &ecaff_var_expected, &( curve_params.prime_data ), 1 );

            eccp_affine_to_jacobian( &ecproj_var_a, &ecaff_var_a, param );
            eccp_jacobian_point_double( &ecproj_var_c, &ecproj_var_a, param );
            eccp_jacobian_to_affine( &ecaff_var_c, &ecproj_var_c, param );

            errors += assert_integer( test_id, ecaff_var_expected.identity, ecaff_var_c.identity );
            if( ecaff_var_expected.identity == 0 ) {
                errors += assert_bigint( test_id, ecaff_var_expected.x, ecaff_var_c.x, length );
                errors += assert_bigint( test_id, ecaff_var_expected.y, ecaff_var_c.y, length );
            }
        } else if( line_starts_with( buffer, "eccp_jacobian_point_negate" ) ) {

            read_eccp_affine_point( buffer, READ_BUFFER_SIZE, &ecaff_var_a, &( curve_params.prime_data ), 1 );
            read_eccp_affine_point( buffer, READ_BUFFER_SIZE, &ecaff_var_expected, &( curve_params.prime_data ), 1 );

            eccp_affine_to_jacobian( &ecproj_var_a, &ecaff_var_a, param );
            eccp_jacobian_point_negate( &ecproj_var_c, &ecproj_var_a, param );
            eccp_jacobian_to_affine( &ecaff_var_c, &ecproj_var_c, param );

            errors += assert_integer( test_id, ecaff_var_expected.identity, ecaff_var_c.identity );
            if( ecaff_var_expected.identity == 0 ) {
                errors += assert_bigint( test_id, ecaff_var_expected.x, ecaff_var_c.x, length );
                errors += assert_bigint( test_id, ecaff_var_expected.y, ecaff_var_c.y, length );
            }
        } else if( line_starts_with( buffer, "eccp_jacobian_point_is_valid" ) ) {

            read_eccp_affine_point( buffer, READ_BUFFER_SIZE, &ecaff_var_a, &( curve_params.prime_data ), 1 );
            int expected = read_integer( buffer, READ_BUFFER_SIZE );

            eccp_affine_to_jacobian( &ecproj_var_a, &ecaff_var_a, param );
            int is_valid = eccp_jacobian_point_is_valid( &ecproj_var_a, param );
            errors += assert_integer( test_id, expected, is_valid );
        } else if( line_starts_with( buffer, "eccp_point_multiply" ) ) {

            read_eccp_affine_point( buffer, READ_BUFFER_SIZE, &ecaff_var_a, &( curve_params.prime_data ), 1 );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_a, param->order_n_data.words );
            read_eccp_affine_point( buffer, READ_BUFFER_SIZE, &ecaff_var_expected, &( curve_params.prime_data ), 1 );

            param->eccp_mul( &ecaff_var_c, &ecaff_var_a, bi_var_a, param );

            errors += assert_integer( test_id, ecaff_var_expected.identity, ecaff_var_c.identity );
            if( ecaff_var_expected.identity == 0 ) {
                errors += assert_bigint( test_id, ecaff_var_expected.x, ecaff_var_c.x, length );
                errors += assert_bigint( test_id, ecaff_var_expected.y, ecaff_var_c.y, length );
            }
        } else if( line_starts_with( buffer, "eccp_comb_point_multiply" ) ) {

            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_a, param->order_n_data.words );
            read_eccp_affine_point( buffer, READ_BUFFER_SIZE, &ecaff_var_expected, &( curve_params.prime_data ), 1 );

            param->eccp_mul_base_point( &ecaff_var_c, bi_var_a, param );

            errors += assert_integer( test_id, ecaff_var_expected.identity, ecaff_var_c.identity );
            if( ecaff_var_expected.identity == 0 ) {
                errors += assert_bigint( test_id, ecaff_var_expected.x, ecaff_var_c.x, length );
                errors += assert_bigint( test_id, ecaff_var_expected.y, ecaff_var_c.y, length );
            }
        } else if( line_starts_with( buffer, "eccp_jacobian_point_equals" ) ) {

            read_eccp_affine_point( buffer, READ_BUFFER_SIZE, &ecaff_var_a, &( curve_params.prime_data ), 1 );
            read_eccp_affine_point( buffer, READ_BUFFER_SIZE, &ecaff_var_b, &( curve_params.prime_data ), 1 );
            int expected = read_integer( buffer, READ_BUFFER_SIZE );

            eccp_affine_to_jacobian( &ecproj_var_a, &ecaff_var_a, param );
            eccp_affine_to_jacobian( &ecproj_var_b, &ecaff_var_b, param );

            int compare = eccp_jacobian_point_equals( &ecproj_var_a, &ecproj_var_b, param );

            errors += assert_integer( test_id, expected, compare );
        } else if( line_starts_with( buffer, "ecdh_phase_one" ) ) {

            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_a, param->order_n_data.words );
            read_eccp_affine_point( buffer, READ_BUFFER_SIZE, &ecaff_var_expected, &( curve_params.prime_data ), 0 );

            ecdh_phase_one( &ecaff_var_c, bi_var_a, param );

            errors += assert_integer( test_id, ecaff_var_expected.identity, ecaff_var_c.identity );
            if( ecaff_var_expected.identity == 0 ) {
                errors += assert_bigint( test_id, ecaff_var_expected.x, ecaff_var_c.x, length );
                errors += assert_bigint( test_id, ecaff_var_expected.y, ecaff_var_c.y, length );
            }
        } else if( line_starts_with( buffer, "ecdh_phase_two" ) ) {

            read_eccp_affine_point( buffer, READ_BUFFER_SIZE, &ecaff_var_a, &( curve_params.prime_data ), 0 );
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_a, param->order_n_data.words );
            read_eccp_affine_point( buffer, READ_BUFFER_SIZE, &ecaff_var_expected, &( curve_params.prime_data ), 0 );

            ecdh_phase_two( &ecaff_var_c, bi_var_a, &ecaff_var_a, param );

            errors += assert_integer( test_id, ecaff_var_expected.identity, ecaff_var_c.identity );
            if( ecaff_var_expected.identity == 0 ) {
                errors += assert_bigint( test_id, ecaff_var_expected.x, ecaff_var_c.x, length );
                errors += assert_bigint( test_id, ecaff_var_expected.y, ecaff_var_c.y, length );
            }
        } else if( line_starts_with( buffer, "ecdsa_signverify_selftest" ) ) {

            ecdsa_signature_t signature;

            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_a, param->order_n_data.words );                      // hash of message
            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_b, param->order_n_data.words );                      // private key
            read_eccp_affine_point( buffer, READ_BUFFER_SIZE, &ecaff_var_a, &( curve_params.prime_data ), 0 ); // public key
            int expected = read_integer( buffer, READ_BUFFER_SIZE );

            ecdsa_sign( &signature, bi_var_a, bi_var_b, param );
            int is_valid = ecdsa_is_valid( &signature, bi_var_a, &ecaff_var_a, param );

            errors += assert_integer( test_id, expected, is_valid );
        } else if( line_starts_with( buffer, "ecdsa_is_valid_sha1" ) ) {

            hash_sha1_t sha1_state;
            ecdsa_signature_t signature;
            uint8_t message[400];
            uint8_t hash[20];

            read_bigint( buffer, READ_BUFFER_SIZE, signature.r, param->order_n_data.words ); // read r of signature
            read_bigint( buffer, READ_BUFFER_SIZE, signature.s, param->order_n_data.words ); // read s of signature

            int message_length = read_message( buffer, READ_BUFFER_SIZE, message, 400 );
            read_eccp_affine_point( buffer, READ_BUFFER_SIZE, &ecaff_var_a, &( curve_params.prime_data ), 0 ); // public key
            int expected = read_integer( buffer, READ_BUFFER_SIZE );

            hash_sha1_init( &sha1_state );
            hash_sha1_final( &sha1_state, message, message_length, message_length );
            hash_sha1_to_byte_array( hash, &sha1_state );
            ecdsa_hash_to_gfp( bi_var_a, hash, 160, &( param->order_n_data ) );

            int is_valid = ecdsa_is_valid( &signature, bi_var_a, &ecaff_var_a, param );
            errors += assert_integer( test_id, expected, is_valid );
        } else if( line_starts_with( buffer, "ecdsa_is_valid_sha224" ) ) {

            hash_sha224_256_t sha2_state;
            ecdsa_signature_t signature;
            uint8_t message[400];
            uint8_t hash[28];

            read_bigint( buffer, READ_BUFFER_SIZE, signature.r, param->order_n_data.words ); // read r of signature
            read_bigint( buffer, READ_BUFFER_SIZE, signature.s, param->order_n_data.words ); // read s of signature

            int message_length = read_message( buffer, READ_BUFFER_SIZE, message, 400 );
            read_eccp_affine_point( buffer, READ_BUFFER_SIZE, &ecaff_var_a, &( curve_params.prime_data ), 0 ); // public key
            int expected = read_integer( buffer, READ_BUFFER_SIZE );

            hash_sha224_init( &sha2_state );
            hash_sha2_final( &sha2_state, message, message_length, message_length );
            hash_sha224_to_byte_array( hash, &sha2_state );
            ecdsa_hash_to_gfp( bi_var_a, hash, 224, &( param->order_n_data ) );

            int is_valid = ecdsa_is_valid( &signature, bi_var_a, &ecaff_var_a, param );
            errors += assert_integer( test_id, expected, is_valid );
        } else if( line_starts_with( buffer, "ecdsa_is_valid_sha256" ) ) {

            hash_sha224_256_t sha2_state;
            ecdsa_signature_t signature;
            uint8_t message[400];
            uint8_t hash[32];

            read_bigint( buffer, READ_BUFFER_SIZE, signature.r, param->order_n_data.words ); // read r of signature
            read_bigint( buffer, READ_BUFFER_SIZE, signature.s, param->order_n_data.words ); // read s of signature

            int message_length = read_message( buffer, READ_BUFFER_SIZE, message, 400 );
            read_eccp_affine_point( buffer, READ_BUFFER_SIZE, &ecaff_var_a, &( curve_params.prime_data ), 0 ); // public key
            int expected = read_integer( buffer, READ_BUFFER_SIZE );

            hash_sha256_init( &sha2_state );
            hash_sha2_final( &sha2_state, message, message_length, message_length );
            hash_sha256_to_byte_array( hash, &sha2_state );
            ecdsa_hash_to_gfp( bi_var_a, hash, 256, &( param->order_n_data ) );

            int is_valid = ecdsa_is_valid( &signature, bi_var_a, &ecaff_var_a, param );
            errors += assert_integer( test_id, expected, is_valid );
        } else if( line_starts_with( buffer, "ecdsa_is_valid_sha384" ) ) {
        } else if( line_starts_with( buffer, "ecdsa_is_valid_sha512" ) ) {
        } else if( line_starts_with( buffer, "ecdsa_is_valid" ) ) {

            ecdsa_signature_t signature;

            read_bigint( buffer, READ_BUFFER_SIZE, signature.r, param->order_n_data.words ); // read r of signature
            read_bigint( buffer, READ_BUFFER_SIZE, signature.s, param->order_n_data.words ); // read s of signature

            read_bigint( buffer, READ_BUFFER_SIZE, bi_var_a, param->order_n_data.words );                      // hash of message
            read_eccp_affine_point( buffer, READ_BUFFER_SIZE, &ecaff_var_a, &( curve_params.prime_data ), 0 ); // public key
            int expected = read_integer( buffer, READ_BUFFER_SIZE );

            int is_valid = ecdsa_is_valid( &signature, bi_var_a, &ecaff_var_a, param );
            errors += assert_integer( test_id, expected, is_valid );
        } else if( line_starts_with( buffer, "sha1_final" ) ) {

            hash_sha1_t sha1_state;
            uint8_t expected_hash[20];
            uint8_t hash[20];
            uint8_t message[256];

            int message_length = read_message( buffer, READ_BUFFER_SIZE, message, 256 );
            read_message( buffer, READ_BUFFER_SIZE, expected_hash, 20 );

            hash_sha1_init( &sha1_state );
            hash_sha1_final( &sha1_state, message, message_length, message_length );

            hash_sha1_to_byte_array( hash, &sha1_state );

            errors += assert_byte_array( test_id, expected_hash, hash, 20 );
        } else if( line_starts_with( buffer, "sha1_update" ) ) {

            hash_sha1_t sha1_state;
            uint8_t expected_hash[20];
            uint8_t hash[20];
            uint8_t message[64];
            int i, block_count;

            hash_sha1_init( &sha1_state );
            block_count = read_integer( buffer, READ_BUFFER_SIZE );

            for( i = 0; i < block_count; i++ ) {
                read_message( buffer, READ_BUFFER_SIZE, message, 64 );
                hash_sha1_update( &sha1_state, message );
            }

            int message_length = read_message( buffer, READ_BUFFER_SIZE, message, 256 );
            read_message( buffer, READ_BUFFER_SIZE, expected_hash, 20 );
            hash_sha1_final( &sha1_state, message, message_length, message_length + block_count * 64 );

            hash_sha1_to_byte_array( hash, &sha1_state );

            errors += assert_byte_array( test_id, expected_hash, hash, 20 );
        } else if( line_starts_with( buffer, "sha224_final" ) ) {

            hash_sha224_256_t sha2_state;
            uint8_t expected_hash[28];
            uint8_t hash[28];
            uint8_t message[256];

            int message_length = read_message( buffer, READ_BUFFER_SIZE, message, 256 );
            read_message( buffer, READ_BUFFER_SIZE, expected_hash, 28 );

            hash_sha224_init( &sha2_state );
            hash_sha2_final( &sha2_state, message, message_length, message_length );

            hash_sha224_to_byte_array( hash, &sha2_state );

            errors += assert_byte_array( test_id, expected_hash, hash, 28 );
        } else if( line_starts_with( buffer, "sha224_update" ) ) {

            hash_sha224_256_t sha2_state;
            uint8_t expected_hash[28];
            uint8_t hash[28];
            uint8_t message[64];
            int i, block_count;

            hash_sha224_init( &sha2_state );
            block_count = read_integer( buffer, READ_BUFFER_SIZE );

            for( i = 0; i < block_count; i++ ) {
                read_message( buffer, READ_BUFFER_SIZE, message, 64 );
                hash_sha2_update( &sha2_state, message );
            }

            int message_length = read_message( buffer, READ_BUFFER_SIZE, message, 256 );
            read_message( buffer, READ_BUFFER_SIZE, expected_hash, 28 );
            hash_sha2_final( &sha2_state, message, message_length, message_length + block_count * 64 );

            hash_sha224_to_byte_array( hash, &sha2_state );

            errors += assert_byte_array( test_id, expected_hash, hash, 28 );
        } else if( line_starts_with( buffer, "sha256_final" ) ) {

            hash_sha224_256_t sha2_state;
            uint8_t expected_hash[32];
            uint8_t hash[32];
            uint8_t message[256];

            int message_length = read_message( buffer, READ_BUFFER_SIZE, message, 256 );
            read_message( buffer, READ_BUFFER_SIZE, expected_hash, 32 );

            hash_sha256_init( &sha2_state );
            hash_sha2_final( &sha2_state, message, message_length, message_length );

            hash_sha256_to_byte_array( hash, &sha2_state );

            errors += assert_byte_array( test_id, expected_hash, hash, 32 );
        } else if( line_starts_with( buffer, "sha256_update" ) ) {

            hash_sha224_256_t sha2_state;
            uint8_t expected_hash[32];
            uint8_t hash[32];
            uint8_t message[64];
            int i, block_count;

            hash_sha256_init( &sha2_state );
            block_count = read_integer( buffer, READ_BUFFER_SIZE );

            for( i = 0; i < block_count; i++ ) {
                read_message( buffer, READ_BUFFER_SIZE, message, 64 );
                hash_sha2_update( &sha2_state, message );
            }

            int message_length = read_message( buffer, READ_BUFFER_SIZE, message, 256 );
            read_message( buffer, READ_BUFFER_SIZE, expected_hash, 32 );
            hash_sha2_final( &sha2_state, message, message_length, message_length + block_count * 64 );

            hash_sha256_to_byte_array( hash, &sha2_state );

            errors += assert_byte_array( test_id, expected_hash, hash, 32 );
        } else if( line_starts_with( buffer, "performance_test_eccp_mul" ) ) {
            performance_test_eccp_mul( param );
        } else if( line_starts_with( buffer, "performance_test_gfp_mul" ) ) {
            performance_test_gfp_mul( param );
        }
    }
    return errors;
}
