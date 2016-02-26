/* 
 * File:   gfp_opt_3.h
 * Author: erichwenger
 *
 * Created on May 20, 2015, 4:25 PM
 */

#ifndef GFP_OPT_3_H
#define	GFP_OPT_3_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "../types.h"
    
void gfp_opt_3_init(gfp_prime_data_t *prime_data_local);
void gfp_opt_3_add( gfp_t res, const gfp_t a, const gfp_t b );
void gfp_opt_3_subtract( gfp_t res, const gfp_t a, const gfp_t b );
void gfp_opt_3_multiply( gfp_t res, const gfp_t a, const gfp_t b );
void gfp_opt_3_inverse( gfp_t res, const gfp_t a );

#ifdef	__cplusplus
}
#endif

#endif	/* GFP_OPT_3_H */

