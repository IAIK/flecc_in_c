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

#ifndef TYPES_H_
#define TYPES_H_

#include <stdint.h>

typedef uint32_t uint_t;
typedef uint64_t ulong_t;
typedef int64_t slong_t;
#define UINT_T_MAX 0xFFFFFFFF

/** The number of bytes represented by a word. */
#define BYTES_PER_WORD 4

/** The number of bits represented by a word. */
#define BITS_PER_WORD ( BYTES_PER_WORD << 3 )

/** binary logarithm of BITS_PER_WORD*/
#define LD_BITS_PER_WORD 5

/** binary logarithm of BYTES_PER_WORD*/
#define LD_BYTES_PER_WORD 2

/** returns the number of words needed to store the defined number of bits */
#define WORDS_PER_BITS( bits ) ( ( ( bits - 1 ) >> LD_BITS_PER_WORD ) + 1 )

/** returns the number of bytes needed to store the defined number of bits */
#define BYTES_PER_BITS( bits ) ( ( ( bits - 1 ) >> 3 ) + 1 )

/** the number of bits that has to fit within bigint_t */
#define MIN_BITS_PER_GFP 521

/** the number of words that fit within bigint_t */
#define WORDS_PER_GFP ( WORDS_PER_BITS( MIN_BITS_PER_GFP ) )

/** the number of bytes that fit within bigint_t */
#define BYTES_PER_GFP ( WORDS_PER_GFP * BYTES_PER_WORD )

/** the number of bits that fit within bigint_t */
#define BITS_PER_GFP ( WORDS_PER_GFP * BITS_PER_WORD )

/** Represent a number in GF(p) - same as bigint_t */
typedef uint_t gfp_t[WORDS_PER_GFP];

/** Set of parameters needed for general GF(p) operations.
 *  Includes a set of parameters needed for Montgomery GF(p) multiplications.
 *  R is assumed to be (1 << (words * BITS_PER_WORD)).
 */
typedef struct {
    /** the prime number used for reduction */
    gfp_t prime;
    /** the number of bits needed to represent the prime */
    uint32_t bits;
    /** the number of words needed to represent the prime */
    uint32_t words;
    /** specifies whether computations are performed in
     * Montgomery domain using Montgomery multiplication*/
    uint8_t montgomery_domain;
    /** constant needed for Montgomery multiplication */
    uint_t n0;
    /** R^2 to be used for Montgomery conversion */
    gfp_t r_squared;
    /** 1*R equals one */
    gfp_t gfp_one;
} gfp_prime_data_t;

/** Elliptic curve point in affine coordinates. */
typedef struct {
    gfp_t x;
    gfp_t y;
    uint8_t identity;
} eccp_point_affine_t;

/** Elliptic curve point using projective (x,y,z) coordinates. */
typedef struct {
    gfp_t x;
    gfp_t y;
    gfp_t z;
    uint8_t identity;
} eccp_point_projective_t;

/** specifies the used eccp_parameters_t */
typedef enum _curve_type_t { UNKNOWN, SECP192R1, SECP224R1, SECP256R1, SECP384R1, SECP521R1, CUSTOM } curve_type_t;

/** function point to a gfp operation (TODO: integrate into eccp_parameters_t) */
typedef void (*gfp_operation_t)(gfp_t, const gfp_t, const gfp_t, const gfp_prime_data_t*);

/** Parameters needed to do elliptic curve computations. */
struct _eccp_parameters_t_;

/** typedef of function pointer to an optimized eccp scalar multiplication (used in eccp_parameters_t) */
typedef void (*eccp_mul_t)(eccp_point_affine_t *,const eccp_point_affine_t*,const gfp_t,const struct _eccp_parameters_t_*);

/** typedef of function pointer to an optimized scalar multiplication with constant point (used in eccp_parameters_t). */
typedef void (*eccp_mul_const_t)(eccp_point_affine_t *,const gfp_t,const struct _eccp_parameters_t_*);

/** Parameters needed to do elliptic curve computations. */
typedef struct _eccp_parameters_t_ {
    /** data needed to do computations modulo the prime */
    gfp_prime_data_t prime_data;
    /** data needed to do computations modulo the group order n */
    gfp_prime_data_t order_n_data;
    /** (cofactor h)*(order n) is the total number of points representable with
     * the equation y^2=x^3+ax+b */
    uint_t h;
    /** parameter a of the used elliptic curve y^2=x^3+ax+b */
    gfp_t param_a;
    /** parameter b of the used elliptic curve y^2=x^3+ax+b */
    gfp_t param_b;
    /** the standardized base point */
    eccp_point_affine_t base_point;
    /** specifies the used elliptic curve */
    curve_type_t curve_type;
    /** generic scalar multiplication to be used for protocols */
    eccp_mul_t eccp_mul;
    /** pointer to a table with precomputed multiples of the base_point to be used by eccp_mul_base_point */
    eccp_point_affine_t *base_point_precomputed_table;
    /** the comb parameter that influences the size of the comb table */
    uint_t base_point_precomputed_table_width;
    /** optimized scalar multiplication of the base point (uses base_point_tbl) */
    eccp_mul_const_t eccp_mul_base_point;
} eccp_parameters_t;

/** ECDSA signature, with GF(p) elements modulo ecc_parameters_t.order_n_data */
typedef struct {
    gfp_t r;
    gfp_t s;
} ecdsa_signature_t;

#endif /* TYPES_H_ */
