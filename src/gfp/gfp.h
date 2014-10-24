/*
 * gfp.h
 *
 *  Created on: 14.03.2014
 *      Author: Erich Wenger
 *
 *  The following defines gfp_* are assuming that there exists a local
 *  variable ecc_parameters_t *param. The defines beautify the eccp code
 *  and make it easier to exchange underlying gfp_* functions.
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

#ifndef GFP_H_
#define GFP_H_

#include "../bi/bi.h"
#include "gfp_gen.h"
#include "gfp_mont.h"

#define gfp_add( res, a, b ) gfp_gen_add( res, a, b, &param->prime_data )
#define gfp_subtract( res, a, b ) gfp_gen_subtract( res, a, b, &param->prime_data )
#define gfp_halving( res, a ) gfp_gen_halving( res, a, &param->prime_data )
#define gfp_negate( res, a ) gfp_gen_negate( res, a, &param->prime_data )
#define gfp_multiply( res, a, b ) gfp_mont_multiply_sos( res, a, b, &param->prime_data )
#define gfp_square( res, a ) gfp_mont_multiply_sos( res, a, a, &param->prime_data )
#define gfp_inverse( res, a ) gfp_mont_inverse( res, a, &param->prime_data )
#define gfp_exponent( res, a, exponent, exponent_length )                                                                        \
    gfp_mont_exponent( res, a, exponent, exponent_length, &param->prime_data )

#define gfp_clear( dest ) bigint_clear_var( dest, param->prime_data.words )
#define gfp_copy( dest, src ) bigint_copy_var( dest, src, param->prime_data.words )
#define gfp_compare( a, b ) bigint_compare_var( a, b, param->prime_data.words )
#define gfp_is_zero( a ) bigint_is_zero_var( a, param->prime_data.words )

// #define gfp_compare(a,b,prime_data)
// bigint_compare_var(a,b,(prime_data)->words)

#endif /* GFP_H_ */
