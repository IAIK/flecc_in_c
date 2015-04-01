/* 
 * File:   gfp_const_runtime.h
 * Author: erichwenger
 *
 * Created on March 31, 2015, 5:06 PM
 */

#ifndef GFP_CONST_RUNTIME_H
#define	GFP_CONST_RUNTIME_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "../types.h"
    
void gfp_cr_add( gfp_t res, const gfp_t a, const gfp_t b, const gfp_prime_data_t *prime_data );
void gfp_cr_subtract( gfp_t res, const gfp_t a, const gfp_t b, const gfp_prime_data_t *prime_data );



#ifdef	__cplusplus
}
#endif

#endif	/* GFP_CONST_RUNTIME_H */

