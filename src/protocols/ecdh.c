/*
 * ecdh.c
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
 
#include "../eccp/eccp.h"
#include "../gfp/gfp.h"

/**
 * First phase of a diffie-hellman key exchange
 * @param res resulting point
 * @param scalar ephemeral or static private key (param->order_n_mont_data.prime_data.words large)
 * @param param elliptic curve parameters
 */
void ecdh_phase_one(eccp_point_affine_t* res, const gfp_t scalar, const eccp_parameters_t *param) {

	// base_point is already in Montgomery domain
	eccp_jacobian_point_multiply_L2R_DA(res, &param->base_point, scalar, param);

	// convert to normal basis such that other party has no problem
	if(param->prime_data.montgomery_domain == 1) {
		gfp_montgomery_to_normal(res->x, res->x, &param->prime_data);
		gfp_montgomery_to_normal(res->y, res->y, &param->prime_data);
	}
}

/**
 * First phase of a diffie-hellman key exchange
 * @param res resulting point
 * @param scalar ephemeral or static private key  (param->order_n_mont_data.prime_data.words large)
 * @param other_party_point the (ephemeral) public key of the other party
 * @param param elliptic curve parameters
 */
void ecdh_phase_two(eccp_point_affine_t* res, const gfp_t scalar, eccp_point_affine_t* other_party_point, const eccp_parameters_t *param) {

	// assume that the other party does not use Montgomery domain
	if(param->prime_data.montgomery_domain == 1) {
		gfp_normal_to_montgomery(other_party_point->x, other_party_point->x, &param->prime_data);
		gfp_normal_to_montgomery(other_party_point->y, other_party_point->y, &param->prime_data);
	}

	if(eccp_affine_point_is_valid(other_party_point, param) == 0) {
		// TODO: error handling
	}

	eccp_jacobian_point_multiply_L2R_DA(res, other_party_point, scalar, param);

	// the commonly derived key shall not be in montgomery domain
	if(param->prime_data.montgomery_domain == 1) {
		gfp_montgomery_to_normal(res->x, res->x, &param->prime_data);
		gfp_montgomery_to_normal(res->y, res->y, &param->prime_data);
	}
}

