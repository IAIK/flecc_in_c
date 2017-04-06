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

#include "flecc_in_c/eccp/eccp_std_projective.h"
#include "flecc_in_c/bi/bi.h"
#include "flecc_in_c/eccp/eccp_affine.h"
#include "flecc_in_c/gfp/gfp.h"
#include "flecc_in_c/utils/rand.h"

/**
 * Tests if the given affine point fulfills the elliptic curve equation.
 * (Does not perform a cofactor multiplication to check the order of the given
 * point.)
 * @param P the point to test
 * @param param the used elliptic curve parameters
 * @return 1 if the point is valid
 *
 * Y^2*Z = X^3 + aXZ^2 + bZ^3
 */
int eccp_std_projective_point_is_valid( const eccp_point_projective_t *a, const eccp_parameters_t *param ) {
    gfp_t left;
    gfp_t right;
    gfp_t temp;

    if( a->identity == 1 ) {
        return 1;
    }

    if( bigint_compare_var( a->x, param->prime_data.prime, param->prime_data.words ) >= 0
        || bigint_compare_var( a->y, param->prime_data.prime, param->prime_data.words ) >= 0
        || bigint_compare_var( a->z, param->prime_data.prime, param->prime_data.words ) >= 0 ) {
        return 0;
    }

    /* calculate the right side */
    /* use left as additional temp */
    gfp_multiply( temp, param->param_a, a->x ); /* aX                         */
    gfp_multiply( left, param->param_b, a->z ); /* bZ                         */
    gfp_add( temp, temp, left );                /* aX + bZ                    */
    gfp_square( left, a->z );                   /* Z^2                        */
    gfp_multiply( right, temp, left );          /* aXZ^2 + bZ^3               */
    gfp_square( left, a->x );                   /* X^2                        */
    gfp_multiply( temp, left, a->x );           /* X^3                        */
    gfp_add( right, right, temp );              /* X^3 + aXZ^2 + bZ^3         */

    /* calculate the left side */
    gfp_square( temp, a->y );         /* Y^2                        */
    gfp_multiply( left, temp, a->z ); /* ZY^2                       */

    /* check if Y^2*Z == X^3 + a*X*Z^2 + b*Z^3 */
    return gfp_is_equal( left, right );
}

/**
 * Returns 1 if both points (x,y)=(x/Z,y/Z) are equal
 * @param A first point to test
 * @param B second point to test
 * @param param elliptic curve parameters
 * @return 1 if equal, otherwise 0
 */
int eccp_std_projective_point_equals( const eccp_point_projective_t *a,
                                      const eccp_point_projective_t *b,
                                      const eccp_parameters_t *param ) {
    gfp_t temp_a, temp_b;

    if( a->identity == 1 ) {
        if( b->identity == 1 ) {
            return 1;
        }
        return 0;
    }
    if( b->identity == 1 ) {
        return 0;
    }

    gfp_multiply( temp_a, a->z, b->x );
    gfp_multiply( temp_b, b->z, a->x );

    if( !gfp_is_equal( temp_a, temp_b ) ) {
        return 0;
    }

    gfp_multiply( temp_a, a->z, b->y );
    gfp_multiply( temp_b, b->z, a->y );

    return gfp_is_equal( temp_a, temp_b );
}

/**
 *  Copies a standard projective elliptic curve point.
 *  @param res the destination memory
 *  @param src the source memory
 *  @param param elliptic curve parameters
 */
void eccp_std_projective_point_copy( eccp_point_projective_t *dest,
                                     const eccp_point_projective_t *src,
                                     const eccp_parameters_t *param ) {
    dest->identity = src->identity;
    gfp_copy( dest->x, src->x );
    gfp_copy( dest->y, src->y );
    gfp_copy( dest->z, src->z );
}

/**
 * Transforms a standard projective point to an affine point
 * @param res the resulting affine point
 * @param a   the standard projective point
 * @param param elliptic curve parameters
 */
void eccp_std_projective_to_affine( eccp_point_affine_t *res, const eccp_point_projective_t *a, const eccp_parameters_t *param ) {
    gfp_t z_inverse;

    res->identity = a->identity;
    if( res->identity == 1 ) {
        return;
    }
    // TODO: exception handling
    if( gfp_is_zero( a->z ) == 1 ) {
        res->identity = 1;
        return;
    }

    gfp_inverse( z_inverse, a->z );
    gfp_multiply( res->x, a->x, z_inverse );
    gfp_multiply( res->y, a->y, z_inverse );
}

/**
 * Converts an affine point to a standard projective point.
 * @param res the resulting standard projective point
 * @param A   the give affine point
 * @param param elliptic curve parameters
 */
void eccp_affine_to_std_projective( eccp_point_projective_t *res, const eccp_point_affine_t *a, const eccp_parameters_t *param ) {
#if 1
    gfp_copy( res->x, a->x );
    gfp_copy( res->y, a->y );
    gfp_copy( res->z, param->prime_data.gfp_one );
    res->identity = a->identity;
#else
    /* randomized projective coordinates (just for testing) */
    gfp_rand( res->z, &param->prime_data );
    gfp_multiply( res->x, a->x, res->z );
    gfp_multiply( res->y, a->y, res->z );
    res->identity = a->identity;
#endif
}

/**
 * Negate a given point in std_projective coordinates.
 * @param res the resulting point
 * @param P   the point to negate
 * @param param elliptic curve parameters
 */
void eccp_std_projective_point_negate( eccp_point_projective_t *res,
                                       const eccp_point_projective_t *P,
                                       const eccp_parameters_t *param ) {
    gfp_copy( res->x, P->x );
    gfp_negate( res->y, P->y );
    gfp_copy( res->z, P->z );
    res->identity = P->identity;
}
