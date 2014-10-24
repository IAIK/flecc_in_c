/*
 * eckeygen.c
 *
 *  Created on: 26.03.2014
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
#include "../utils/rand.h"
#include "../gfp/gfp.h"

/**
 * Generate an elliptic curve key pair
 * @param private_key resulting private key
 * @param public_key resulting public key
 * @param param elliptic curve parameters
 */
void eckeygen( gfp_t private_key, eccp_point_affine_t *public_key, eccp_parameters_t *param ) {
    gfp_rand( private_key, &param->order_n_data );
    eccp_jacobian_point_multiply_L2R_DA( public_key, &param->base_point, private_key, param );

    // convert to normal basis such that other party has no problem
    if( param->prime_data.montgomery_domain == 1 ) {
        gfp_montgomery_to_normal( public_key->x, public_key->x, &param->prime_data );
        gfp_montgomery_to_normal( public_key->y, public_key->y, &param->prime_data );
    }
}
