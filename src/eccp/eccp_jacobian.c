/*
 * eccp_jacobian.c
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

#include "eccp_jacobian.h"
#include "../gfp/gfp.h"
#include "../bi/bi.h"
#include "../utils/rand.h"

/**
 * Tests if the given affine point fulfills the elliptic curve equation.
 * (Does not perform a cofactor multiplication to check the order of the given
 * point.)
 * @param P the point to test
 * @param param the used elliptic curve parameters
 * @return 1 if the point is valid
 */
int eccp_jacobian_point_is_valid( const eccp_point_projective_t *a, const eccp_parameters_t *param ) {
    gfp_t left;
    gfp_t right;
    gfp_t temp;

    if( a->identity == 1 )
        return 1;
    if( bigint_compare_var( a->x, param->prime_data.prime, param->prime_data.words ) >= 0 )
        return 0;
    if( bigint_compare_var( a->y, param->prime_data.prime, param->prime_data.words ) >= 0 )
        return 0;
    if( bigint_compare_var( a->z, param->prime_data.prime, param->prime_data.words ) >= 0 )
        return 0;

    /* calculate the right side */
    /* use left as additional temp */
    gfp_square( left, a->x );
    gfp_multiply( right, a->x, left );          /* X^3                         */
    gfp_square( left, a->z );                   /* Z^2                         */
    gfp_square( temp, left );                   /* Z^4                         */
    gfp_multiply( left, left, temp );           /* Z^6                         */
    gfp_multiply( left, left, param->param_b ); /* Z^6 * b                     */
    gfp_add( right, left, right );              /* X^3 + Z^6 * b               */
    gfp_multiply( temp, a->x, temp );           /* X * Z^4                     */
    gfp_multiply( temp, temp, param->param_a ); /* a * X * Z^4                 */
    gfp_add( right, temp, right );              /* X^3 + a * X * Z^4 + Z^6 * b */

    /* calculate the left side */
    gfp_square( left, a->y );

    /* check if Y^2 == X^3 + a*X*Z^4 + b*Z^6 */
    if( gfp_compare( left, right ) == 0 )
        return 1;
    else
        return 0;
}

/**
 * Returns 1 if both points (x,y)=(x/Z^2,y/Z^3) are equal
 * @param A first point to test
 * @param B second point to test
 * @param param elliptic curve parameters
 * @return 1 if equal, otherwise 0
 */
int eccp_jacobian_point_equals( const eccp_point_projective_t *a,
                                const eccp_point_projective_t *b,
                                const eccp_parameters_t *param ) {
    gfp_t temp_a_z2, temp_b_z2;
    gfp_t temp_a, temp_b;

    if( a->identity == 1 ) {
        if( b->identity == 1 )
            return 1;
        else
            return 0;
    }
    if( b->identity == 1 )
        return 0;

    gfp_square( temp_a_z2, a->z );
    gfp_square( temp_b_z2, b->z );
    gfp_multiply( temp_a, temp_a_z2, b->x );
    gfp_multiply( temp_b, temp_b_z2, a->x );

    if( gfp_compare( temp_a, temp_b ) != 0 )
        return 0;

    gfp_multiply( temp_a, temp_a_z2, a->z );
    gfp_multiply( temp_b, temp_b_z2, b->z );
    gfp_multiply( temp_a_z2, temp_a, b->y );
    gfp_multiply( temp_b_z2, temp_b, a->y );

    if( gfp_compare( temp_a_z2, temp_b_z2 ) != 0 )
        return 0;
    else
        return 1;
}

/**
 *  Copies an jacobian elliptic curve point.
 *  @param res the destination memory
 *  @param src the source memory
 *  @param param elliptic curve parameters
 */
void eccp_jacobian_point_copy( eccp_point_projective_t *dest,
                               const eccp_point_projective_t *src,
                               const eccp_parameters_t *param ) {
    dest->identity = src->identity;
    gfp_copy( dest->x, src->x );
    gfp_copy( dest->y, src->y );
    gfp_copy( dest->z, src->z );
}

/**
 * Transforms a Jacobian projective point to an affine point
 * @param res the resulting affine point
 * @param a   the Jacobian projective point
 * @param param elliptic curve parameters
 */
void eccp_jacobian_to_affine( eccp_point_affine_t *res, const eccp_point_projective_t *a, const eccp_parameters_t *param ) {
    gfp_t T1, T2;

    res->identity = a->identity;
    if( res->identity == 1 ) {
        return;
    }
    // TODO: exception handling
    if( gfp_is_zero( a->z ) == 1 ) {
        res->identity = 1;
        return;
    }

    gfp_inverse( T1, a->z );
    gfp_square( T2, T1 );
    gfp_multiply( res->x, a->x, T2 );
    gfp_multiply( res->y, a->y, T2 );
    gfp_multiply( res->y, res->y, T1 );
}

/**
 * Converts an affine point to a Jacobian projective point.
 * @param res the resulting Jacobian projective point
 * @param A   the give affine point
 * @param param elliptic curve parameters
 */
void eccp_affine_to_jacobian( eccp_point_projective_t *res, const eccp_point_affine_t *a, const eccp_parameters_t *param ) {
#if 0
	gfp_copy(res->x, a->x);
    gfp_copy(res->y, a->y);
    gfp_copy(res->z, param->prime_data.gfp_one);
    res->identity = a->identity;
#else
    /* randomized projective coordinates (just for testing) */
    gfp_t lambda;
    gfp_rand( lambda, &param->prime_data );
    gfp_multiply( res->x, a->x, lambda );
    gfp_multiply( res->x, res->x, lambda );
    gfp_multiply( res->y, a->y, lambda );
    gfp_multiply( res->y, res->y, lambda );
    gfp_multiply( res->y, res->y, lambda );
    gfp_copy( res->z, lambda );
    res->identity = a->identity;
#endif
}

/**
 * Doubles the given point in Jacobian coordinates
 * @param res the doubled point
 * @param a the point to double
 * @param param elliptic curve parameters
 */
void
    eccp_jacobian_point_double( eccp_point_projective_t *res, const eccp_point_projective_t *a, const eccp_parameters_t *param ) {
    gfp_t T1;
    gfp_t T2;
    gfp_t T3;

    /* only double if point isn't infinite */
    if( a->identity == 1 ) {
        res->identity = 1;
        return;
    }

    gfp_negate( T1, a->y );
    if( gfp_compare( T1, a->y ) == 0 ) {
        // this handles the special case of doubling a point of order 2
        res->identity = 1;
        return;
    }

#ifdef ECC_PARAM_A_IS_MINUS_THREE
    gfp_square( T1, a->z );
    gfp_subtract( T2, a->x, T1 );
    gfp_add( T1, a->x, T1 );
    gfp_multiply( T3, T1, T2 );
    gfp_add( T2, T3, T3 );
    gfp_add( T2, T2, T3 );
#else
    gfp_square( T1, a->x );
    gfp_square( T2, a->z );
    gfp_square( T2, T2 );
    gfp_multiply( T2, T2, param->param_a );

    gfp_add( T2, T2, T1 );
    gfp_add( T2, T2, T1 );
    gfp_add( T2, T2, T1 );
#endif
    gfp_add( res->y, a->y, a->y );
    gfp_multiply( T1, res->y, a->z );
    gfp_copy( res->z, T1 );
    gfp_square( T1, res->y );
    gfp_multiply( T3, a->x, T1 );
    gfp_square( res->x, T1 );
    gfp_halving( res->y, res->x );
    gfp_square( res->x, T2 );
    gfp_add( T1, T3, T3 );
    gfp_subtract( res->x, res->x, T1 );
    gfp_subtract( T1, T3, res->x );
    gfp_multiply( T3, T1, T2 );
    gfp_subtract( res->y, T3, res->y );
    res->identity = 0;
}

/**
 * Add two points in projective jacobian form.
 * @param res
 * @param A
 * @param B
 * @param param elliptic curve parameters
    Cost: 11M + 5S + 9add + 4*2.
    Cost: 10M + 4S + 9add + 4*2 dependent upon the first point.
    Source: 2007 Bernstein-Lange; note that the improvement from 12M+4S to
 11M+5S was already mentioned in 2001 Bernstein
 http://cr.yp.to/talks.html#2001.10.29.
    Explicit formulas:

          Z1Z1 = Z1^2
          Z2Z2 = Z2^2
          U1 = X1*Z2Z2
          U2 = X2*Z1Z1
          S1 = Y1*Z2*Z2Z2
          S2 = Y2*Z1*Z1Z1
          H = U2-U1
          I = (2*H)^2
          J = H*I
          r = 2*(S2-S1)
          V = U1*I
          X3 = r^2-J-2*V
          Y3 = r*(V-X3)-2*S1*J
          Z3 = ((Z1+Z2)^2-Z1Z1-Z2Z2)*H
 */
void eccp_jacobian_point_add( eccp_point_projective_t *res,
                              const eccp_point_projective_t *a,
                              const eccp_point_projective_t *b,
                              const eccp_parameters_t *param ) {
    gfp_t T1;
    gfp_t T2;
    gfp_t T3;
    gfp_t T4;
    gfp_t T5;
    gfp_t T6;

    /* if A is infinite, B is returned */
    if( a->identity == 1 ) {
        eccp_jacobian_point_copy( res, b, param );
        return;
    }
    /* if B is infinite, A is returned */
    if( b->identity == 1 ) {
        eccp_jacobian_point_copy( res, a, param );
        return;
    }

    gfp_square( T1, a->z );       // Z1Z1
    gfp_square( T2, b->z );       // Z2Z2
    gfp_multiply( T3, a->x, T2 ); // U1 = X1*Z2Z2
    gfp_multiply( T4, b->x, T1 ); // U2 = X2*Z1Z1
    gfp_multiply( T5, T1, a->z );
    gfp_multiply( T6, T2, b->z );
    gfp_add( T1, T1, T2 );        // Z1Z1+Z2Z2
    gfp_multiply( T2, a->y, T6 ); // S1 = Y1*Z2*Z2Z2
    gfp_multiply( T6, b->y, T5 ); // S2 = Y2*Z1*Z1Z1

    if( gfp_compare( T3, T4 ) == 0 ) {
        if( gfp_compare( T2, T6 ) == 0 ) {
            // CASE: A is equal to B
            eccp_jacobian_point_double( res, a, param );
            return;
        } else {
            // CASE: -A is equal to B
            // NOTE: There are only two possible values for y per x coordinate.
            res->identity = 1;
            return;
        }
    }

    gfp_subtract( T4, T4, T3 ); // H = U2-U1
    gfp_subtract( T6, T6, T2 );
    gfp_add( T6, T6, T6 ); // r = 2*(S2-S1)
    gfp_add( res->z, a->z, b->z );
    gfp_square( T5, res->z );
    gfp_subtract( T5, T5, T1 );
    gfp_multiply( res->z, T5, T4 ); // Z3 = ((Z1+Z2)^2-Z1Z1-Z2Z2)*H
    // mem: T1, S1, U1, H, T5, r
    gfp_add( T5, T4, T4 );
    gfp_square( T1, T5 ); // I = (2*H)^2
    // mem: I, S1, U1, H, T5, r
    gfp_multiply( T5, T3, T1 ); // V = U1*I
    // mem: I, S1, T3, H, V, r
    gfp_multiply( T3, T4, T1 ); // J = H*I
    // mem: T1, S1, J, T4, V, r
    gfp_square( res->x, T6 );
    gfp_subtract( res->x, res->x, T3 );
    gfp_subtract( res->x, res->x, T5 );
    gfp_subtract( res->x, res->x, T5 ); // X3 = r^2-J-2*V
    gfp_subtract( T5, T5, res->x );
    gfp_multiply( res->y, T6, T5 );
    gfp_multiply( T1, T2, T3 );
    gfp_subtract( res->y, res->y, T1 );
    gfp_subtract( res->y, res->y, T1 ); // Y3 = r*(V-X3)-2*S1*J
    res->identity = 0;
}

/**
 * Add a Jacobian and an affine point
 * @param res the sum in Jacobian coordinates
 * @param a projective Jacobian point
 * @param b affine point
 * @param param elliptic curve parameters
 */
void eccp_jacobian_point_add_affine( eccp_point_projective_t *res,
                                     const eccp_point_projective_t *a,
                                     const eccp_point_affine_t *b,
                                     const eccp_parameters_t *param ) {
    gfp_t T1;
    gfp_t T2;
    gfp_t T3;
    gfp_t T4;

    /* if a is infinite, b is returned */
    if( a->identity == 1 ) {
        eccp_affine_to_jacobian( res, b, param );
        return;
    }
    if( b->identity == 1 ) {
        eccp_jacobian_point_copy( res, a, param );
        return;
    }

    gfp_square( T1, a->z );
    gfp_multiply( T2, a->z, T1 );
    gfp_multiply( T3, b->x, T1 );
    gfp_multiply( T1, b->y, T2 );
    gfp_subtract( T3, T3, a->x );
    gfp_subtract( T1, T1, a->y );

    /* Check for errors */
    if( gfp_is_zero( T3 ) == 1 ) {
        if( gfp_is_zero( T1 ) == 1 ) {
            // CASE: A is equal to B
            // TODO: optimized formulas using b
            eccp_jacobian_point_double( res, a, param );
            return;
        } else {
            // CASE: -A is equal to B
            // NOTE: There are only two possible values for y per x coordinate.
            res->identity = 1;
            return;
        }
    }

    gfp_multiply( T2, a->z, T3 );
    gfp_copy( res->z, T2 );
    gfp_square( T2, T3 );
    gfp_multiply( T4, T2, T3 );
    gfp_multiply( T3, T2, a->x );
    gfp_add( T2, T3, T3 );
    gfp_square( res->x, T1 );
    gfp_subtract( res->x, res->x, T2 );
    gfp_subtract( res->x, res->x, T4 );
    gfp_subtract( T3, T3, res->x );
    gfp_multiply( T2, T3, T1 );
    gfp_multiply( T1, T4, a->y );
    gfp_subtract( res->y, T2, T1 );
    res->identity = 0;
}

/**
 * Negate a given point in jacobian coordinates.
 * @param res the resulting point
 * @param P   the point to negate
 * @param param elliptic curve parameters
 */
void
    eccp_jacobian_point_negate( eccp_point_projective_t *res, const eccp_point_projective_t *P, const eccp_parameters_t *param ) {
    gfp_copy( res->x, P->x );
    gfp_negate( res->y, P->y );
    gfp_copy( res->z, P->z );
    res->identity = P->identity;
}

/**
 * Performs a point scalar multiplication
 * @param result the resulting point
 * @param P The base point to multiply
 * @param scalar the multiplicant
 * @param param elliptic curve parameters
 * Hankerson p97 Alg-> 3->27
 */
void eccp_jacobian_point_multiply_L2R_DA( eccp_point_affine_t *result,
                                          const eccp_point_affine_t *P,
                                          const gfp_t scalar,
                                          const eccp_parameters_t *param ) {
    eccp_point_projective_t result_projective;
    int bit;

    result_projective.identity = 1;
    bit = bigint_get_msb_var( scalar, param->order_n_data.words );
    while( bit >= 0 ) {
        eccp_jacobian_point_double( &result_projective, &result_projective, param );
        if( bigint_test_bit_var( scalar, bit, param->order_n_data.words ) == 1 ) {
            eccp_jacobian_point_add_affine( &result_projective, &result_projective, P, param );
        }
        bit--;
    }

    eccp_jacobian_to_affine( result, &result_projective, param );
}
