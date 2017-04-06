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

#include "flecc_in_c/eccp/eccp_affine.h"
#include "flecc_in_c/gfp/gfp.h"

/**
 * Tests if the given affine point fulfills the elliptic curve equation.
 * (Does not perform a cofactor multiplication to check the order of the given
 * point.)
 * @param A point to test
 * @param param elliptic curve parameters
 * @return 1 if point is valid, otherwise 0
 */
int eccp_affine_point_is_valid( const eccp_point_affine_t *A, const eccp_parameters_t *param ) {
    gfp_t left, right;

    if( A->identity == 1 ) {
        return 1;
    }

    if( bigint_compare_var( A->x, param->prime_data.prime, param->prime_data.words ) >= 0 ||
        bigint_compare_var( A->y, param->prime_data.prime, param->prime_data.words ) >= 0 ) {
        return 0;
    }

    /* calculate the right side */
    /* use left as additional temp */
    gfp_square( left, A->x );
    gfp_multiply( right, A->x, left );          /* x^3 */
    gfp_multiply( left, A->x, param->param_a ); /* a*x */
    gfp_add( right, right, left );              /* x^3 + a*x */
    gfp_add( right, right, param->param_b );    /* x^3 + a*x + b */

    /* calculate the left side */
    gfp_square( left, A->y );

    /* check if y^2 == x^3 + a*x + b */
    return gfp_is_equal( left, right );
}

/**
 *  Compares the two given points for equality. (identity is smaller, then compare x and y coordinates)
 *  @param A
 *  @param B
 *  @param param elliptic curve parameters
 *  @return -1 if A is smaller, 0 if equal, 1 if A is larger.
 */
int eccp_affine_point_compare( const eccp_point_affine_t *A, const eccp_point_affine_t *B, const eccp_parameters_t *param ) {
    int compare;
    if( A->identity == 1 ) {
        if( B->identity == 1 ) {
            return 0;
        }
        return -1;
    }
    if( B->identity == 1 ) {
        return 1;
    }
    compare = bigint_compare_var( A->x, B->x, param->prime_data.words );
    if( compare != 0 ) {
        return compare;
    }
    return bigint_compare_var( A->y, B->y, param->prime_data.words );
}

/**
 *  Copies an affine elliptic curve point.
 *  @param res the destination memory
 *  @param src the source memory
 *  @param param elliptic curve parameters
 */
void eccp_affine_point_copy( eccp_point_affine_t *dest, const eccp_point_affine_t *src, const eccp_parameters_t *param ) {
    dest->identity = src->identity;
    gfp_copy( dest->x, src->x );
    gfp_copy( dest->y, src->y );
}

/**
 * Adds two affine points. Handles the case of R=A and R=B.
 * @param res
 * @param A
 * @param B
 * @param param elliptic curve parameters
 */
void eccp_affine_point_add( eccp_point_affine_t *res,
                            const eccp_point_affine_t *A,
                            const eccp_point_affine_t *B,
                            const eccp_parameters_t *param ) {
    gfp_t lambda, temp1, temp2;

    if( A->identity == 1 ) {
        eccp_affine_point_copy( res, B, param );
        return;
    }
    if( B->identity == 1 ) {
        eccp_affine_point_copy( res, A, param );
        return;
    }
    if( gfp_is_equal( A->x, B->x ) ) {
        if( gfp_is_equal( A->y, B->y ) ) {
            // CASE: A is equal to B
            eccp_affine_point_double( res, A, param );
            return;
        }
        // CASE: -A is equal to B
        // NOTE: There are only two possible values for y per x coordinate.
        res->identity = 1;
        return;
    }

    gfp_subtract( temp2, B->x, A->x );
    gfp_inverse( temp1, temp2 );
    gfp_subtract( temp2, B->y, A->y );
    gfp_multiply( lambda, temp1, temp2 ); // (y2-y1) / (x2-x1)
    gfp_square( temp1, lambda );
    gfp_subtract( temp1, temp1, A->x );
    gfp_subtract( temp1, temp1, B->x ); // L^2 - x1 - x2
    gfp_subtract( temp2, A->x, temp1 ); // (x1 - x3)
    gfp_copy( res->x, temp1 );
    gfp_multiply( temp1, temp2, lambda );
    gfp_subtract( res->y, temp1, A->y ); // L*(x1-x3)-y1
    res->identity = 0;
}

/**
 * Subtracts two affine points. Handles the case of R=A and R=B.
 * @param res res = minuend - subtrahend
 * @param minuend positive point
 * @param subtrahend point that is subtracted
 * @param param elliptic curve parameters
 */
void eccp_affine_point_subtract( eccp_point_affine_t *res,
                                 const eccp_point_affine_t *minuend,
                                 const eccp_point_affine_t *subtrahend,
                                 const eccp_parameters_t *param ) {
    eccp_point_affine_t temp;
    eccp_affine_point_negate( &temp, subtrahend, param );
    eccp_affine_point_add( res, minuend, &temp, param );
}

/**
 * Doubles an affine point. Handles the case of R=A.
 * @param res
 * @param A
 * @param param elliptic curve parameters
 */
void eccp_affine_point_double( eccp_point_affine_t *res, const eccp_point_affine_t *A, const eccp_parameters_t *param ) {
    gfp_t temp1, temp2, temp3;

    if( A->identity == 1 ) {
        res->identity = 1;
        return;
    }
    gfp_negate( temp1, A->y );
    if( gfp_is_equal( temp1, A->y ) ) {
        // this handles the special case of doubling a point of order 2
        res->identity = 1;
        return;
    }

    gfp_add( temp1, A->y, A->y );
    gfp_inverse( temp2, temp1 );
    gfp_square( temp1, A->x );
    gfp_add( temp3, temp1, temp1 );
    gfp_add( temp3, temp3, temp1 );          // 3*x1^2
    gfp_add( temp3, temp3, param->param_a ); // 3*x1^2 + a
    gfp_multiply( temp1, temp2, temp3 );     // lambda
    gfp_square( temp3, temp1 );
    gfp_subtract( temp2, temp3, A->x );
    gfp_subtract( temp2, temp2, A->x ); // x3 = L^2 - 2*x1
    gfp_subtract( temp3, A->x, temp2 );
    gfp_copy( res->x, temp2 );
    gfp_multiply( temp2, temp1, temp3 );
    gfp_subtract( res->y, temp2, A->y ); // y3 = L * (x1-x3) - y1
    res->identity = 0;
}

/**
 * Negates the given affine point.
 * @param res the resulting point (-P)
 * @param P the point to negate
 * @param param elliptic curve parameters
 */
void eccp_affine_point_negate( eccp_point_affine_t *res, const eccp_point_affine_t *P, const eccp_parameters_t *param ) {
    gfp_copy( res->x, P->x );
    gfp_negate( res->y, P->y );
    res->identity = P->identity;
}
