/*
 * eccp_jacobian.h
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

#ifndef ECCP_JACOBIAN_H_
#define ECCP_JACOBIAN_H_

#include "../types.h"

int eccp_jacobian_point_is_valid( const eccp_point_projective_t *a, const eccp_parameters_t *param );
int eccp_jacobian_point_equals( const eccp_point_projective_t *a,
                                const eccp_point_projective_t *b,
                                const eccp_parameters_t *param );
void
    eccp_jacobian_point_copy( eccp_point_projective_t *dest, const eccp_point_projective_t *src, const eccp_parameters_t *param );

void eccp_jacobian_to_affine( eccp_point_affine_t *res, const eccp_point_projective_t *a, const eccp_parameters_t *param );
void eccp_affine_to_jacobian( eccp_point_projective_t *res, const eccp_point_affine_t *a, const eccp_parameters_t *param );

void eccp_jacobian_point_double( eccp_point_projective_t *res, const eccp_point_projective_t *a, const eccp_parameters_t *param );
void eccp_jacobian_point_add( eccp_point_projective_t *res,
                              const eccp_point_projective_t *a,
                              const eccp_point_projective_t *b,
                              const eccp_parameters_t *param );
void eccp_jacobian_point_add_affine( eccp_point_projective_t *res,
                                     const eccp_point_projective_t *a,
                                     const eccp_point_affine_t *b,
                                     const eccp_parameters_t *param );
void eccp_jacobian_point_negate( eccp_point_projective_t *res, const eccp_point_projective_t *P, const eccp_parameters_t *param );

void eccp_jacobian_point_multiply_L2R_DA( eccp_point_affine_t *result,
                                          const eccp_point_affine_t *P,
                                          const gfp_t scalar,
                                          const eccp_parameters_t *param );

#endif /* ECCP_JACOBIAN_H_ */
