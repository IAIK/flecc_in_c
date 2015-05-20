/* 
 * File:   main.c
 * Author: erichwenger
 *
 * Created on January 14, 2015, 1:36 PM
 */

#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "gfp/gfp.h"
#include "eccp/eccp.h"
#include "utils/rand.h"
#include "eccp_ecdlp.h"
#include "io/io.h"

/**
 *         add("itsec2012_30", "", ec_type.GF_p,
            "3fffffd7", // <- prime
            "3fffffd4", // <- a
            "11308ecb", // <- b
            "1e150fa7", // <- P.x
            "259af", // <- P.y
            "3fff7589", // <- order
 * @param param
 */
void init_param_itsec2012_30(eccp_parameters_t *param) {
    param->curve_type = CUSTOM;
    param->eccp_mul = &eccp_protected_point_multiply;
    param->eccp_mul_base_point = NULL;
    param->base_point_precomputed_table = NULL;
    param->base_point_precomputed_table_width = 0;
    
    // set prime data
    param->prime_data.bits = 30;
    param->prime_data.words = 1;
    param->prime_data.montgomery_domain = 1;
    param->prime_data.prime[0] = 0x3fffffd7;

    // compute Montgomery constants
    gfp_mont_compute_R( param->prime_data.gfp_one, &( param->prime_data ) );
    gfp_mont_compute_R_squared( param->prime_data.r_squared, &( param->prime_data ) );
    param->prime_data.n0 = gfp_mont_compute_n0( &( param->prime_data ) );

    // set prime data (group order)
    param->order_n_data.bits = 30;
    param->order_n_data.words = 1;
    param->order_n_data.montgomery_domain = 0;
    param->order_n_data.prime[0] = 0x3fff7589;

    // compute Montgomery constants (group order)
    gfp_mont_compute_R( param->order_n_data.gfp_one, &( param->order_n_data ) );
    gfp_mont_compute_R_squared( param->order_n_data.r_squared, &( param->order_n_data ) );
    param->order_n_data.n0 = gfp_mont_compute_n0( &( param->order_n_data ) );

    // copy ECC parameters a, b, and cofactor h
    param->param_a[0] = 0x3fffffd4;
    param->param_b[0] = 0x11308ecb;
    param->h = 1;
    gfp_normal_to_montgomery(param->param_a, param->param_a, &param->prime_data);
    gfp_normal_to_montgomery(param->param_b, param->param_b, &param->prime_data);

    // copy ECC base point
    param->base_point.x[0] = 0x1e150fa7;
    param->base_point.y[0] = 0x259af;
    gfp_normal_to_montgomery(param->base_point.x, param->base_point.x, &param->prime_data);
    gfp_normal_to_montgomery(param->base_point.y, param->base_point.y, &param->prime_data);
    param->base_point.identity = 0;
}

/**
 * returns 1 if the given set of parameters is valid
 * @param param the parameters to check
 * @return 1 if valid, 0 otherwise
 * 1. check base point fits curve equation (y^2 = x^3 + ax + b)
 * 2. check order of base point
 */
int param_is_valid(const eccp_parameters_t *param) {
    eccp_point_affine_t temp;
    if(eccp_affine_point_is_valid(&param->base_point, param) != 1) {
        printf("ERROR: base point not valid\n");
        return 0;
    }
    eccp_jacobian_point_multiply_L2R_NAF(&temp, &param->base_point, param->order_n_data.prime, param);
    if(temp.identity != 1) {
        printf("ERROR: base point not correct order\n");
        return 0;
    }
    return 1;
}
#define READ_BUFFER_SIZE 10000

/*
 * 
 */
int main(int argc, char** argv) {
    char buffer[READ_BUFFER_SIZE];
    eccp_point_affine_t P, Q;
    gfp_t scalar;
    eccp_parameters_t curve_params;
    eccp_parameters_t *param = &curve_params;

    // disable buffering of stdout
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("ecdlp_test started\n");

    srand(0);
    printf("seed: %u\n", rand());
    
    io_init( stdin, stdout );
    gfp_opt_3_init(&param->prime_data);
    read_elliptic_curve_parameters(buffer, READ_BUFFER_SIZE, param);

    if(param_is_valid(param) == 0)
        return -1;

    read_gfp( buffer, READ_BUFFER_SIZE, Q.x, &param->prime_data, 1 );
    read_gfp( buffer, READ_BUFFER_SIZE, Q.y, &param->prime_data, 1 );
    Q.identity = 0;
    
    eccp_affine_point_copy(&P, &param->base_point, param);

    pr_ecdlp_pollard_rho(scalar, &P, &Q, param);
    
    return (EXIT_SUCCESS);
}

