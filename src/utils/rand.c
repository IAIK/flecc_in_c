/*
 * rand.c
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

#include <stdlib.h>
#include "rand.h"
#include "../bi/bi.h"

/**
 * Use the deterministic rand() function to initialize dest with random data
 * @param dest
 * @param length the number of words to generate
 */
void bigint_rand_insecure_var( uint_t *dest, const int length ) {
    int word;
    for( word = 0; word < length; word++ ) {
        *dest++ = rand();
    }
}

/**
 * Generate a number that is *smaller* than the given prime and larger than 0
 * @param dest       the number to generate
 * @param prime_data the upper bound
 */
void gfp_rand( gfp_t dest, const gfp_prime_data_t *prime_data ) {
    int msb;
    uint_t mask;
    msb = prime_data->bits & ( BITS_PER_WORD - 1 );
    if( msb == 0 )
        mask = UINT_T_MAX;
    else
        mask = ( 1 << msb ) - 1;
    do {
        // TODO: to be replaced with an external library or something more secure
        bigint_rand_insecure_var( dest, prime_data->words );
        /* speedup of this loop */
        dest[prime_data->words - 1] &= mask;
    } while( ( bigint_compare_var( dest, prime_data->prime, prime_data->words ) >= 0 )
             && ( bigint_is_zero_var( dest, prime_data->words ) == 0 ) );
}
