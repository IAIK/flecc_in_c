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

#include "param.h"
#include <string.h>
#include "../bi/bi.h"
#include "../gfp/gfp.h"
#include "../eccp/eccp.h"

extern const uint_t SECP192R1_ORDER_N[6];
extern const uint_t SECP192R1_PRIME[6];
extern const uint32_t SECP192R1_PRIME_BITS;
extern const uint32_t SECP192R1_ORDER_N_BITS;
extern const uint32_t SECP192R1_COFACTOR;
extern const uint_t SECP192R1_A[6];
extern const uint_t SECP192R1_B[6];
extern const uint_t SECP192R1_BASE_X[6];
extern const uint_t SECP192R1_BASE_Y[6];

extern const uint_t SECP224R1_ORDER_N[7];
extern const uint_t SECP224R1_PRIME[7];
extern const uint32_t SECP224R1_PRIME_BITS;
extern const uint32_t SECP224R1_ORDER_N_BITS;
extern const uint32_t SECP224R1_COFACTOR;
extern const uint_t SECP224R1_A[7];
extern const uint_t SECP224R1_B[7];
extern const uint_t SECP224R1_BASE_X[7];
extern const uint_t SECP224R1_BASE_Y[7];

extern const uint_t SECP256R1_ORDER_N[8];
extern const uint_t SECP256R1_PRIME[8];
extern const uint32_t SECP256R1_PRIME_BITS;
extern const uint32_t SECP256R1_ORDER_N_BITS;
extern const uint32_t SECP256R1_COFACTOR;
extern const uint_t SECP256R1_A[8];
extern const uint_t SECP256R1_B[8];
extern const uint_t SECP256R1_BASE_X[8];
extern const uint_t SECP256R1_BASE_Y[8];

extern const uint_t SECP384R1_ORDER_N[12];
extern const uint_t SECP384R1_PRIME[12];
extern const uint32_t SECP384R1_PRIME_BITS;
extern const uint32_t SECP384R1_ORDER_N_BITS;
extern const uint32_t SECP384R1_COFACTOR;
extern const uint_t SECP384R1_A[12];
extern const uint_t SECP384R1_B[12];
extern const uint_t SECP384R1_BASE_X[12];
extern const uint_t SECP384R1_BASE_Y[12];

extern const uint_t SECP521R1_ORDER_N[16];
extern const uint_t SECP521R1_PRIME[16];
extern const uint32_t SECP521R1_PRIME_BITS;
extern const uint32_t SECP521R1_ORDER_N_BITS;
extern const uint32_t SECP521R1_COFACTOR;
extern const uint_t SECP521R1_A[16];
extern const uint_t SECP521R1_B[16];
extern const uint_t SECP521R1_BASE_X[16];
extern const uint_t SECP521R1_BASE_Y[16];

/**
 * Determines the type of curve from the string in the buffer.
 * @param buffer the string defining the type of curve
 * @return type of curve
 */
curve_type_t param_get_curve_type_from_name( const char *buffer ) {
    curve_type_t curve = UNKNOWN;

    if( strncmp( buffer, "secp192r1", 9 ) == 0 ) {
        curve = SECP192R1;
    } else if( strncmp( buffer, "secp224r1", 9 ) == 0 ) {
        curve = SECP224R1;
    } else if( strncmp( buffer, "secp256r1", 9 ) == 0 ) {
        curve = SECP256R1;
    } else if( strncmp( buffer, "secp384r1", 9 ) == 0 ) {
        curve = SECP384R1;
    } else if( strncmp( buffer, "secp521r1", 9 ) == 0 ) {
        curve = SECP521R1;
    }

    return curve;
}

/**
 * Loads parameters for a given type of curve and stores them
 * into the structure referenced by param.
 * @param param the structure that is filled with elliptic curve parameters
 * @param type the type of curve to be used
 */
void param_load( eccp_parameters_t *param, const curve_type_t type ) {
    param->curve_type = type;
    param->eccp_mul = &eccp_jacobian_point_multiply_L2R_NAF;
    param->eccp_mul_base_point = NULL;
    param->base_point_precomputed_table = NULL;
    param->base_point_precomputed_table_width = 0;
    
    if( type == SECP192R1 ) {
        int bi_length = WORDS_PER_BITS( SECP192R1_PRIME_BITS );

        // set prime data
        param->prime_data.bits = SECP192R1_PRIME_BITS;
        param->prime_data.words = bi_length;
        param->prime_data.montgomery_domain = 1;
        bigint_copy_var( param->prime_data.prime, SECP192R1_PRIME, param->prime_data.words );

        // compute Montgomery constants
        gfp_mont_compute_R( param->prime_data.gfp_one, &( param->prime_data ) );
        gfp_mont_compute_R_squared( param->prime_data.r_squared, &( param->prime_data ) );
        param->prime_data.n0 = gfp_mont_compute_n0( &( param->prime_data ) );

        // set prime data (group order)
        param->order_n_data.bits = SECP192R1_ORDER_N_BITS;
        param->order_n_data.words = WORDS_PER_BITS( SECP192R1_ORDER_N_BITS );
        param->order_n_data.montgomery_domain = 1;
        bigint_copy_var( param->order_n_data.prime, SECP192R1_ORDER_N, param->order_n_data.words );

        // compute Montgomery constants (group order)
        gfp_mont_compute_R( param->order_n_data.gfp_one, &( param->order_n_data ) );
        gfp_mont_compute_R_squared( param->order_n_data.r_squared, &( param->order_n_data ) );
        param->order_n_data.n0 = gfp_mont_compute_n0( &( param->order_n_data ) );

        // copy ECC parameters a, b, and cofactor h
        bigint_copy_var( param->param_a, SECP192R1_A, bi_length );
        bigint_copy_var( param->param_b, SECP192R1_B, bi_length );
        param->h = SECP192R1_COFACTOR;

        // copy ECC base point
        bigint_copy_var( param->base_point.x, SECP192R1_BASE_X, bi_length );
        bigint_copy_var( param->base_point.y, SECP192R1_BASE_Y, bi_length );
        param->base_point.identity = 0;
    } else if( type == SECP224R1 ) {
        int bi_length = WORDS_PER_BITS( SECP224R1_PRIME_BITS );

        // set prime data
        param->prime_data.bits = SECP224R1_PRIME_BITS;
        param->prime_data.words = bi_length;
        param->prime_data.montgomery_domain = 1;
        bigint_copy_var( param->prime_data.prime, SECP224R1_PRIME, param->prime_data.words );

        // compute Montgomery constants
        gfp_mont_compute_R( param->prime_data.gfp_one, &( param->prime_data ) );
        gfp_mont_compute_R_squared( param->prime_data.r_squared, &( param->prime_data ) );
        param->prime_data.n0 = gfp_mont_compute_n0( &( param->prime_data ) );

        // set prime data (group order)
        param->order_n_data.bits = SECP224R1_ORDER_N_BITS;
        param->order_n_data.words = WORDS_PER_BITS( SECP224R1_ORDER_N_BITS );
        param->order_n_data.montgomery_domain = 0;
        bigint_copy_var( param->order_n_data.prime, SECP224R1_ORDER_N, param->order_n_data.words );

        // compute Montgomery constants (group order)
        gfp_mont_compute_R( param->order_n_data.gfp_one, &( param->order_n_data ) );
        gfp_mont_compute_R_squared( param->order_n_data.r_squared, &( param->order_n_data ) );
        param->order_n_data.n0 = gfp_mont_compute_n0( &( param->order_n_data ) );

        // copy ECC parameters a, b, and cofactor h
        bigint_copy_var( param->param_a, SECP224R1_A, bi_length );
        bigint_copy_var( param->param_b, SECP224R1_B, bi_length );
        param->h = SECP224R1_COFACTOR;

        // copy ECC base point
        bigint_copy_var( param->base_point.x, SECP224R1_BASE_X, bi_length );
        bigint_copy_var( param->base_point.y, SECP224R1_BASE_Y, bi_length );
        param->base_point.identity = 0;
    } else if( type == SECP256R1 ) {
        int bi_length = WORDS_PER_BITS( SECP256R1_PRIME_BITS );

        // set prime data
        param->prime_data.bits = SECP256R1_PRIME_BITS;
        param->prime_data.words = bi_length;
        param->prime_data.montgomery_domain = 1;
        bigint_copy_var( param->prime_data.prime, SECP256R1_PRIME, param->prime_data.words );

        // compute Montgomery constants
        gfp_mont_compute_R( param->prime_data.gfp_one, &( param->prime_data ) );
        gfp_mont_compute_R_squared( param->prime_data.r_squared, &( param->prime_data ) );
        param->prime_data.n0 = gfp_mont_compute_n0( &( param->prime_data ) );

        // set prime data (group order)
        param->order_n_data.bits = SECP256R1_ORDER_N_BITS;
        param->order_n_data.words = WORDS_PER_BITS( SECP256R1_ORDER_N_BITS );
        param->order_n_data.montgomery_domain = 0;
        bigint_copy_var( param->order_n_data.prime, SECP256R1_ORDER_N, param->order_n_data.words );

        // compute Montgomery constants (group order)
        gfp_mont_compute_R( param->order_n_data.gfp_one, &( param->order_n_data ) );
        gfp_mont_compute_R_squared( param->order_n_data.r_squared, &( param->order_n_data ) );
        param->order_n_data.n0 = gfp_mont_compute_n0( &( param->order_n_data ) );

        // copy ECC parameters a, b, and cofactor h
        bigint_copy_var( param->param_a, SECP256R1_A, bi_length );
        bigint_copy_var( param->param_b, SECP256R1_B, bi_length );
        param->h = SECP256R1_COFACTOR;

        // copy ECC base point
        bigint_copy_var( param->base_point.x, SECP256R1_BASE_X, bi_length );
        bigint_copy_var( param->base_point.y, SECP256R1_BASE_Y, bi_length );
        param->base_point.identity = 0;
    } else if( type == SECP384R1 ) {
        int bi_length = WORDS_PER_BITS( SECP384R1_PRIME_BITS );

        // set prime data
        param->prime_data.bits = SECP384R1_PRIME_BITS;
        param->prime_data.words = bi_length;
        param->prime_data.montgomery_domain = 1;
        bigint_copy_var( param->prime_data.prime, SECP384R1_PRIME, param->prime_data.words );

        // compute Montgomery constants
        gfp_mont_compute_R( param->prime_data.gfp_one, &( param->prime_data ) );
        gfp_mont_compute_R_squared( param->prime_data.r_squared, &( param->prime_data ) );
        param->prime_data.n0 = gfp_mont_compute_n0( &( param->prime_data ) );

        // set prime data (group order)
        param->order_n_data.bits = SECP384R1_ORDER_N_BITS;
        param->order_n_data.words = WORDS_PER_BITS( SECP384R1_ORDER_N_BITS );
        param->order_n_data.montgomery_domain = 0;
        bigint_copy_var( param->order_n_data.prime, SECP384R1_ORDER_N, param->order_n_data.words );

        // compute Montgomery constants (group order)
        gfp_mont_compute_R( param->order_n_data.gfp_one, &( param->order_n_data ) );
        gfp_mont_compute_R_squared( param->order_n_data.r_squared, &( param->order_n_data ) );
        param->order_n_data.n0 = gfp_mont_compute_n0( &( param->order_n_data ) );

        // copy ECC parameters a, b, and cofactor h
        bigint_copy_var( param->param_a, SECP384R1_A, bi_length );
        bigint_copy_var( param->param_b, SECP384R1_B, bi_length );
        param->h = SECP384R1_COFACTOR;

        // copy ECC base point
        bigint_copy_var( param->base_point.x, SECP384R1_BASE_X, bi_length );
        bigint_copy_var( param->base_point.y, SECP384R1_BASE_Y, bi_length );
        param->base_point.identity = 0;
    } else if( type == SECP521R1 ) {
        int bi_length = WORDS_PER_BITS( SECP521R1_PRIME_BITS );

        // set prime data
        param->prime_data.bits = SECP521R1_PRIME_BITS;
        param->prime_data.words = bi_length;
        param->prime_data.montgomery_domain = 1;
        bigint_copy_var( param->prime_data.prime, SECP521R1_PRIME, param->prime_data.words );

        // compute Montgomery constants
        gfp_mont_compute_R( param->prime_data.gfp_one, &( param->prime_data ) );
        gfp_mont_compute_R_squared( param->prime_data.r_squared, &( param->prime_data ) );
        param->prime_data.n0 = gfp_mont_compute_n0( &( param->prime_data ) );

        // set prime data (group order)
        param->order_n_data.bits = SECP521R1_ORDER_N_BITS;
        param->order_n_data.words = WORDS_PER_BITS( SECP521R1_ORDER_N_BITS );
        param->order_n_data.montgomery_domain = 0;
        bigint_copy_var( param->order_n_data.prime, SECP521R1_ORDER_N, param->order_n_data.words );

        // compute Montgomery constants (group order)
        gfp_mont_compute_R( param->order_n_data.gfp_one, &( param->order_n_data ) );
        gfp_mont_compute_R_squared( param->order_n_data.r_squared, &( param->order_n_data ) );
        param->order_n_data.n0 = gfp_mont_compute_n0( &( param->order_n_data ) );

        // copy ECC parameters a, b, and cofactor h
        bigint_copy_var( param->param_a, SECP521R1_A, bi_length );
        bigint_copy_var( param->param_b, SECP521R1_B, bi_length );
        param->h = SECP521R1_COFACTOR;

        // copy ECC base point
        bigint_copy_var( param->base_point.x, SECP521R1_BASE_X, bi_length );
        bigint_copy_var( param->base_point.y, SECP521R1_BASE_Y, bi_length );
        param->base_point.identity = 0;
    }
}
