/* 
 * File:   eccp_ecdlp.h
 * Author: erichwenger
 *
 * Created on January 14, 2015, 2:02 PM
 */

#ifndef ECCP_ECDLP_H
#define	ECCP_ECDLP_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "types.h"
#include "test_ser.h"

/**
 * aP + bQ
 */
typedef struct _eccp_ecdlp_triple_ {
    /** element modulo order n */
    gfp_t a;
    /** element modulo order n */
    gfp_t b;
    /** point such that aP + bQ */
    eccp_point_affine_t R;
} eccp_ecdlp_triple;

void pr_add_tree(const eccp_ecdlp_triple *to_insert, gfp_t scalar);
void pr_get_latest_point(eccp_ecdlp_triple *computed);
void pr_send_to_master(const eccp_ecdlp_triple *comp);
int pr_triple_is_distinguished(const eccp_ecdlp_triple *triple);
void pr_triple_send_if_distinguished(const eccp_ecdlp_triple *triple);
void pr_triple_generate(eccp_ecdlp_triple *to_comp);
int pr_triple_negation_map(eccp_ecdlp_triple *triple);
int pr_triple_add_branch(eccp_ecdlp_triple *triple);
void pr_triple_add_branch_index(eccp_ecdlp_triple *triple, int index);
void pr_triple_add(eccp_ecdlp_triple *res, const eccp_ecdlp_triple *opa, const eccp_ecdlp_triple *opb);
void pr_triple_double(eccp_ecdlp_triple *triple);
void pr_triple_copy(eccp_ecdlp_triple *dest, const eccp_ecdlp_triple *src);
int pr_triple_is_valid(const eccp_ecdlp_triple *totest);
void pr_ecdlp_pollard_rho(gfp_t scalar, const eccp_point_affine_t *P, const eccp_point_affine_t *Q, const eccp_parameters_t *local_param);
void pr_triple_print(const eccp_ecdlp_triple *triple);

#ifdef	__cplusplus
}
#endif

#endif	/* ECCP_ECDLP_H */

