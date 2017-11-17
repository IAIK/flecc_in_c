#include "flecc_in_c/utils/param.h"
#include "flecc_in_c/utils/performance.h"

#include <stdio.h>
#include <string.h>

void run_benchmarks(curve_type_t curve_type) {
    eccp_parameters_t curve_params;
    param_load( &curve_params, curve_type );
    if( curve_params.curve_type == UNKNOWN ) {
        printf("Warning: Curve %d is not supported in the current configuration and has been skipped.\n", curve_type);
        return;
    }
    printf("Benchmarking curve %d:\n", curve_type);
    printf("gfp_mul : ");
    performance_test_gfp_mul( &curve_params );

    printf("eccp_mul: ");
    performance_test_eccp_mul( &curve_params );
    printf("\n");
}

int main(int argc, char** argv) {
    if(argc > 1) {
        // process curves from the command line
        int return_value = 0;
        for( int idx = 1; idx < argc; ++idx) {
            curve_type_t curve_type = param_get_curve_type_from_name( argv[idx], strlen( argv[idx] ) );
            if( curve_type == UNKNOWN ) {
                fprintf(stderr, "ERROR: Curve \"%s\" is unkown and has been skipped.\n", argv[idx]);
                if(return_value == 0)
                    return_value = -1;
                continue;
            }
            run_benchmarks(curve_type);
        }
        return return_value;
    }
    // benchmark a hardcoded set
    curve_type_t curves[] = {SECP192R1, SECP224R1, SECP256R1, SECP384R1, SECP521R1};

    for( unsigned idx = 0; idx < sizeof(curves)/sizeof(curve_type_t); ++idx) {
        run_benchmarks(curves[idx]);
    }

    return 0;
}
