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

#include "flecc_in_c/eccp/eccp_protected.h"
#include "flecc_in_c/bi/bi.h"
#include "flecc_in_c/eccp/eccp_affine.h"
#include "flecc_in_c/gfp/gfp.h"
#include "flecc_in_c/utils/rand.h"

/**
 * Checks the given x, y and z coordinate, if they are a valid combination.
 * @param px
 * @param py
 * @param pz
 * @param left  temporary
 * @param right temporary
 * @param tmep  temporary
 * @return 1 if valid, else 0
 */
int eccp_protected_std_projective_point_is_valid(
    const gfp_t px, const gfp_t py, const gfp_t pz, gfp_t left, gfp_t right, gfp_t temp, const eccp_parameters_t *param ) {

    /* calculate the right side */
    /* use left as additional temp */

    /* use left as additional temp */
    gfp_multiply( temp, param->param_a, px ); /* aX                         */
    gfp_multiply( left, param->param_b, pz ); /* bZ                         */
    gfp_add( temp, temp, left );              /* aX + bZ                    */
    gfp_square( left, pz );                   /* Z^2                        */
    gfp_multiply( right, temp, left );        /* aXZ^2 + bZ^3               */
    gfp_square( left, px );                   /* X^2                        */
    gfp_multiply( temp, left, px );           /* X^3                        */
    gfp_add( right, right, temp );            /* X^3 + aXZ^2 + bZ^3         */

    /* calculate the left side */
    gfp_square( temp, py );
    gfp_multiply( left, temp, pz );

    /* check if ZY^2 == X^3 + a*X*Z^2 + b*Z^3 */
    return gfp_is_equal( left, right );
}

/** can be set to zero on cache-less microprocessor architectures
 *  (faster on cache-less microprocessor architectures)
 */
#define AVOID_CACHE_TIMING_ATTACKS 1

/**
 * Performs a point scalar multiplication based on "8/16/32 shades of ECC on embedded microprocessors"
 * @param result the resulting point (set to identity when error happens)
 * @param P The base point to multiply
 * @param scalar the multiplicant
 * @param param elliptic curve parameters
 */
void eccp_protected_point_multiply( eccp_point_affine_t *result,
                                    const eccp_point_affine_t *P,
                                    const gfp_t scalar,
                                    const eccp_parameters_t *param ) {
    gfp_t Z_, R1, R2, R3, R4, ECC_curve_b_4;
    int bit, bit_is_set;
#if( AVOID_CACHE_TIMING_ATTACKS == 1 )
    gfp_t X1, X2;
    int last_bit = 1;
#else
    gfp_t X1_, X2_;
    uint_t *X1 = X1_, *X2 = X2_;
    uint_t *dest[2];
    dest[0] = X1;
    dest[1] = X2;
#endif

    if( P->identity == 1 ) {
        result->identity = 1;
        return;
    }

    if( !eccp_affine_point_is_valid( P, param ) ) {
        /* TODO: fancier error handling? */
        result->identity = 1;
        return;
    }

    /* deal with the case that the scalar is larger than the group order */
    if( bigint_compare_var( scalar, param->order_n_data.prime, param->order_n_data.words ) >= 0 ) {
        /* TODO: fancier error handling? */
        result->identity = 1;
        return;
    }

    /* deal with the case that the scalar is zero */
    if( bigint_is_zero_var( scalar, param->order_n_data.words ) ) {
        result->identity = 1;
        return;
    }

    bigint_copy_var( R3, param->order_n_data.prime, param->order_n_data.words );
    bigint_clear_var( R4, param->order_n_data.words );
    R4[0] = 1;
    bigint_subtract_var( R3, R3, R4, param->order_n_data.words );

    /* deal with the case that the scalar is (order-1) */
    if( bigint_compare_var( scalar, R3, param->order_n_data.words ) == 0 ) {
        eccp_affine_point_negate( result, P, param );
        return;
    }

    /* randomize projective coordinates */
    gfp_rand( R4, &param->prime_data );
    gfp_multiply( R1, P->x, R4 );
    gfp_multiply( R2, P->y, R4 );
    gfp_copy( R3, R4 );

    if( !eccp_protected_std_projective_point_is_valid( R1, R2, R3, R4, X1, X2, param ) ) {
        /* TODO: fancier error handling? */
        result->identity = 1;
        return;
    }

    gfp_add( ECC_curve_b_4, param->param_b, param->param_b );
    gfp_add( ECC_curve_b_4, ECC_curve_b_4, ECC_curve_b_4 );

    /* manually double the base point P */
    /* X1 = 4*X*Y^2*Z                   */
    /* X3 = (3*X^2+a*Z^2)^2 - 8*X*Y^2*Z */
    /* Z  = 4*Y^2*Z^2                   */
    /* base point is in R1, R2, R3      */
    gfp_square( Z_, R2 );       /* Y^2 */
    gfp_add( Z_, Z_, Z_ );      /* 2*Y^2 */
    gfp_add( R4, Z_, Z_ );      /* 4*Y^2 */
    gfp_multiply( Z_, R1, R4 ); /* 4*X*Y^2 */
    gfp_multiply( X1, Z_, R3 ); /* 4*X*Y^2*Z */
    gfp_square( X2, R1 );       /* X^2 */
    gfp_add( R2, X2, X2 );      /* 2*X^2 */
    gfp_add( R1, X2, R2 );      /* 3*X^2 */
    gfp_square( R2, R3 );       /* Z^2 */
    gfp_multiply( Z_, R4, R2 ); /* 4*Y^2*Z^2 */
#ifdef ECC_PARAM_A_IS_MINUS_THREE
    gfp_subtract( R1, R1, R2 );
    gfp_subtract( R1, R1, R2 );
    gfp_subtract( R1, R1, R2 );
#else
    gfp_multiply( R3, R2, param->param_a );
    gfp_add( R1, R1, R3 ); /* 3*X^2+a*Z^2 */
#endif
    gfp_square( X2, R1 );       /* (3*X^2+a*Z^2)^2 */
    gfp_subtract( X2, X2, X1 ); /* (3*X^2+a*Z^2)^2 - 4*X*Y^2 */
    gfp_subtract( X2, X2, X1 ); /* (3*X^2+a*Z^2)^2 - 8*X*Y^2 */

    /* the multiplication starts with the most significant bit*/
    bit = bigint_get_msb_var( scalar, param->order_n_data.words ) - 1;

    while( bit >= 0 ) {

        bit_is_set = bigint_test_bit_var( scalar, bit, param->order_n_data.words );
#if( AVOID_CACHE_TIMING_ATTACKS == 1 )
        bigint_cr_switch( X1, X2, bit_is_set ^ last_bit, param->prime_data.words );
        last_bit = bit_is_set;
#else
        X1 = dest[1 ^ bit_is_set];
        X2 = dest[bit_is_set];
#endif
        bit--;

        /*
         Point X1' = X1 + X2
         Point X2' =  2 * X2
         */

        gfp_multiply( R1, X1, X2 );
        gfp_square( R3, Z_ );
        gfp_multiply( R4, Z_, R3 );

#ifdef ECC_PARAM_A_IS_MINUS_THREE
        /*gfp_multiply(R2, ECC_curve_a, R3); optimized for a=-3*/
        gfp_subtract( R2, bigint_zero, R3 );
        gfp_subtract( R2, R2, R3 );
        gfp_subtract( R2, R2, R3 );
#else
        gfp_multiply( R2, param->param_a, R3 );
#endif

        gfp_add( R1, R1, R2 );
        gfp_add( X1, X1, X2 );
        gfp_multiply( R3, X1, R1 );
        gfp_subtract( X1, X1, X2 );
        gfp_subtract( X1, X1, X2 );
        gfp_multiply( R1, ECC_curve_b_4, R4 );
        gfp_square( R4, X1 );
        gfp_multiply( X1, R4, Z_ );
        gfp_add( R3, R3, R3 );
        gfp_add( R3, R3, R1 );
        gfp_multiply( Z_, X2, R4 );
        gfp_multiply( R4, R1, X2 );
        gfp_square( R1, X2 );
        gfp_add( R2, R1, R2 );
        gfp_add( R1, R1, R1 );
        gfp_multiply( X2, P->x, X1 );
        gfp_subtract( R3, R3, X2 );
        gfp_multiply( X2, R1, R2 );
        gfp_add( X2, X2, X2 );
        gfp_subtract( R2, R2, R1 );
        gfp_add( R1, R4, R4 );
        gfp_add( R4, X2, R4 );
        gfp_square( X2, R2 );
        gfp_subtract( R1, X2, R1 );
        gfp_multiply( X2, R1, Z_ );
        gfp_multiply( Z_, X1, R4 );
        gfp_multiply( X1, R3, R4 );
    }
#if( AVOID_CACHE_TIMING_ATTACKS == 1 )
    bigint_cr_switch( X1, X2, 1 ^ last_bit, param->prime_data.words );
#else
    X1 = dest[0];
    X2 = dest[1];
#endif

    /* y-recovery (taken from Hutter) */
    gfp_multiply( R1, P->x, Z_ );
    gfp_subtract( R2, X1, R1 );
    gfp_square( R3, R2 );
    gfp_multiply( R4, R3, X2 );
    gfp_multiply( R2, R1, X1 );
    gfp_add( R1, X1, R1 );
    gfp_square( X2, Z_ );
#ifdef ECC_PARAM_A_IS_MINUS_THREE
    gfp_add( R3, X2, X2 );
    gfp_add( R3, R3, R3 );
    gfp_subtract( R3, X2, R3 );
#else
    gfp_multiply( R3, param->param_a, X2 );
#endif
    gfp_add( R2, R2, R3 );
    gfp_multiply( R3, R2, R1 );
    gfp_subtract( R3, R3, R4 );
    gfp_add( R3, R3, R3 );
    gfp_add( R1, P->y, P->y );
    gfp_add( R1, R1, R1 );
    gfp_multiply( R2, R1, X1 );
    gfp_multiply( X1, R2, X2 );
    gfp_multiply( R2, X2, Z_ );
    gfp_multiply( Z_, R2, R1 );
    gfp_multiply( R4, ECC_curve_b_4, R2 );
    gfp_add( X2, R4, R3 );

    if( !eccp_protected_std_projective_point_is_valid( X1, X2, Z_, R1, R2, R3, param ) ) {
        /* TODO: fancier error handling? */
        result->identity = 1;
        return;
    }

    if( gfp_is_zero( Z_ ) ) {
        /* TODO: fancier error handling? */
        result->identity = 1;
        return;
    }

    /**
     * X' - R1
     * Y' - R7
     * Z' - R3
     */
    /* gfp_inverse_binary_mem_optimized(Z, ECC_prime, R1, R2, R3); */
    /* gfp_inverse_mem_optimized(R1, Z_, R2, R3); */
    gfp_inverse( R1, Z_ );
    gfp_multiply( result->x, X1, R1 );
    gfp_multiply( result->y, X2, R1 );
    result->identity = 0;

    if( !eccp_affine_point_is_valid( result, param ) ) {
        /* TODO: fancier error handling? */
        result->identity = 1;
        return;
    }
}
