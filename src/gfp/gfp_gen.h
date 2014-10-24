/*
 * gfp_gen.h
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

#ifndef GFP_GEN_H_
#define GFP_GEN_H_

#include "../types.h"
#include "../bi/bi.h"

void gfp_gen_add( gfp_t res, const gfp_t a, const gfp_t b, const gfp_prime_data_t *prime_data );
void gfp_gen_subtract( gfp_t res, const gfp_t a, const gfp_t b, const gfp_prime_data_t *prime_data );
void gfp_gen_halving( gfp_t res, const gfp_t a, const gfp_prime_data_t *prime_data );
void gfp_gen_negate( gfp_t res, const gfp_t a, const gfp_prime_data_t *prime_data );
void gfp_gen_multiply_div( gfp_t res, const gfp_t a, const gfp_t b, const gfp_prime_data_t *prime_data );
void gfp_reduce( gfp_t a, const gfp_prime_data_t *prime_data );

#endif /* GFP_GEN_H_ */
