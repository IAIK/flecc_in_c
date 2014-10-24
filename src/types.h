/*
 * types.h
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
typedef struct _gfp_prime_data_t_ {
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
typedef struct _eccp_point_affine_t_ {
    gfp_t x;
    gfp_t y;
    uint8_t identity;
} eccp_point_affine_t;
/** Elliptic curve point using projective (x,y,z) coordinates. */
typedef struct _eccp_point_projective_t_ {
    gfp_t x;
    gfp_t y;
    gfp_t z;
    uint8_t identity;
} eccp_point_projective_t;
/** specifies the used eccp_parameters_t */
typedef enum _curve_type_t { UNKNOWN, SECP192R1, SECP224R1, SECP256R1, SECP384R1, SECP521R1 } curve_type_t;
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
    /** the standardized base point*/
    eccp_point_affine_t base_point;
    /** specifies the used elliptic curve */
    curve_type_t curve_type;
} eccp_parameters_t;
/** ECDSA signature, with GF(p) elements modulo ecc_parameters_t.order_n_data */
typedef struct _ecdsa_signature_t_ {
    gfp_t r;
    gfp_t s;
} ecdsa_signature_t;

#endif /* TYPES_H_ */
