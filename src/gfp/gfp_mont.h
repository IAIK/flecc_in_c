/*
 * gfp_mont.h
 *
 *  Created on: 18.03.2014
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

#ifndef GFP_MONT_H_
#define GFP_MONT_H_

#include "../types.h"

void gfp_normal_to_montgomery( gfp_t res, const gfp_t src, const gfp_prime_data_t *prime_data );
void gfp_montgomery_to_normal( gfp_t res, const gfp_t src, const gfp_prime_data_t *prime_data );

void gfp_mont_compute_R( gfp_t res, gfp_prime_data_t *prime_data );
void gfp_mont_compute_R_squared( gfp_t res, gfp_prime_data_t *prime_data );
uint_t gfp_mont_compute_n0( gfp_prime_data_t *prime_data );
void gfp_mont_multiply_sos( gfp_t res, const gfp_t a, const gfp_t b, const gfp_prime_data_t *prime_data );
void gfp_mont_inverse( gfp_t result, const gfp_t a, const gfp_prime_data_t *prime_data );
void gfp_mont_exponent(
    gfp_t res, const gfp_t a, const uint_t *exponent, const int exponent_length, const gfp_prime_data_t *prime_data );

void gfp_mult_two_mont( gfp_t res, const gfp_t a, const gfp_t b, const gfp_prime_data_t *prime_data );

#define gfp_mont_multiply( res, a, b, prime_data ) gfp_mont_multiply_sos( res, a, b, prime_data )

#endif /* GFP_MONT_H_ */
