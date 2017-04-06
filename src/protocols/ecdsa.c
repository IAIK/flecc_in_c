/****************************************************************************
**
** Copyright (C) 2014 Stiftung Secure Information and
**                    Communication Technologies SIC and
**                    Graz University of Technology
** Contact: http://opensource.iaik.tugraz.at
**
** This file is part of <product_name>.
**
** $BEGIN_LICENSE:DEFAULT$
** Commercial License Usage
** Licensees holding valid commercial licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and SIC. For further information
** contact us at http://opensource.iaik.tugraz.at.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** This software is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this software. If not, see http://www.gnu.org/licenses/.
**
** $END_LICENSE:DEFAULT$
**
****************************************************************************/

#include "flecc_in_c/bi/bi.h"
#include "flecc_in_c/eccp/eccp.h"
#include "flecc_in_c/gfp/gfp.h"
#include "flecc_in_c/utils/rand.h"

/**
 * Sign the given hash of a message.
 * @param signature the resulting signature
 * @param hash_of_message the hash of the message (smaller than param->order_n_data.prime)
 * @param private_key the used private key
 * @param param elliptic curve parameters
 */
void ecdsa_sign( ecdsa_signature_t *signature,
                 const gfp_t hash_of_message,
                 const gfp_t private_key,
                 const eccp_parameters_t *param ) {
    gfp_t ephemeral_key;
    eccp_point_affine_t ephemeral_point;

    do {
        do {
            gfp_rand( ephemeral_key, &param->order_n_data );
            eccp_generic_mul_wrapper( &ephemeral_point, &param->base_point, ephemeral_key, param );

            // in case order n and prime have a different length
            signature->r[param->order_n_data.words - 1] = 0;
            if( param->prime_data.montgomery_domain == 1 ) {
                // convert to normal basis is necessary
                gfp_montgomery_to_normal( signature->r, ephemeral_point.x, &param->prime_data );
            } else {
                bigint_copy_var( signature->r, ephemeral_point.x, param->prime_data.words );
            }

            // TODO: using a loop here is not pretty (gfp_reduce)
            // however prime and order_n are quite similar in size (in all
            // standardized elliptic curves)
            // therefore not many iterations are to be expected
            gfp_reduce( signature->r, &param->order_n_data );
        } while( bigint_is_zero_var( signature->r, param->order_n_data.words ) == 1 );

        // reuse ephemeral_key and ephemeral_point to safe memory
        // compute k^-1 e + (k^-1 d) r
        // because private_key, signature->r, and hash_of message are not in
        // Montgomery domain,
        // the conversion back to normal domain is performed implicitly.
        gfp_normal_to_montgomery( ephemeral_key, ephemeral_key, &param->order_n_data );
        gfp_mont_inverse( ephemeral_key, ephemeral_key, &param->order_n_data );                         //  k^-1
        gfp_normal_to_montgomery( ephemeral_point.y, private_key, &param->order_n_data );               // d * R^2 * R^-1
        gfp_mont_multiply( ephemeral_point.x, ephemeral_point.y, ephemeral_key, &param->order_n_data ); // d R * k^-1 R * R^-1
        gfp_mont_multiply( ephemeral_point.x, ephemeral_point.x, signature->r, &param->order_n_data );  // d k^-1 R * r * R^-1
        gfp_mont_multiply( ephemeral_point.y, ephemeral_key, hash_of_message, &param->order_n_data );   // k^-1 R * e * R^-1
        gfp_gen_add( signature->s, ephemeral_point.x, ephemeral_point.y, &param->order_n_data );        // k^-1 (e+d*r)

    } while( bigint_is_zero_var( signature->s, param->order_n_data.words ) == 1 );
}

/**
 * Returns 1 if the given ECDSA signature is valid.
 * @param signature the signature to verify
 * @param hash_of_message the hash of the message (smaller thanparam->order_n_data.prime)
 * @param public_key the public key used for the verification (assumed to be not in montgomery domain)
 * @param param elliptic curve parameters
 * @return 1 if signature is valid, 0 if signature is invalid
 */
int ecdsa_is_valid( const ecdsa_signature_t *signature,
                    const gfp_t hash_of_message,
                    const eccp_point_affine_t *public_key,
                    const eccp_parameters_t *param ) {
    gfp_t w, u1;
    eccp_point_affine_t P1, P2;

    /* Verify that r and s are integers in the interval [1,n-1]. If any
     * verification fails then reject the signature */
    if( bigint_is_zero_var( signature->r, param->order_n_data.words ) == 1
        || bigint_is_zero_var( signature->s, param->order_n_data.words ) == 1
        || bigint_compare_var( signature->r, param->order_n_data.prime, param->order_n_data.words ) >= 0
        || bigint_compare_var( signature->s, param->order_n_data.prime, param->order_n_data.words ) >= 0 ) {
        return 0;
    }

    /* Verify the validity of the public key (just to be sure) */
    eccp_affine_point_copy( &P1, public_key, param );
    if( param->prime_data.montgomery_domain == 1 ) {
        gfp_normal_to_montgomery( P1.x, public_key->x, &param->prime_data );
        gfp_normal_to_montgomery( P1.y, public_key->y, &param->prime_data );
    }
    if( eccp_affine_point_is_valid( &P1, param ) == 0 || P1.identity == 1 ) {
        return 0;
    }

    gfp_normal_to_montgomery( w, signature->s, &param->order_n_data ); // s*R
    gfp_mont_inverse( w, w, &param->order_n_data );                    // s^-1*R
    gfp_mont_multiply( u1, hash_of_message, w, &param->order_n_data ); // u1 = e*s^-1*R*R^-1
    gfp_mont_multiply( w, signature->r, w, &param->order_n_data );     // u2 = r*s^-1*R*R^-1

    // TODO: replace by joint sparse form simultaneous point multiplication
    // TODO: safe memory
    eccp_generic_mul_wrapper( &P2, &P1, w, param );
    eccp_generic_mul_wrapper( &P1, &param->base_point, u1, param );
    eccp_affine_point_add( &P1, &P1, &P2, param );

    if( P1.identity == 1 ) {
        return 0;
    }

    // in case order n and prime have a different length
    w[param->order_n_data.words - 1] = 0;
    if( param->prime_data.montgomery_domain == 1 ) {
        // convert to normal basis is necessary
        gfp_montgomery_to_normal( w, P1.x, &param->prime_data );
    } else {
        bigint_copy_var( w, P1.x, param->prime_data.words );
    }
    gfp_reduce( w, &param->order_n_data );

    if( bigint_compare_var( w, signature->r, param->order_n_data.words ) == 0 ) {
        return 1;
    }
    return 0;
}

/**
 * Converts the computed hash into a number smaller than order n. (usable for
 * ECDSA)
 * @param element the resulting number.
 * @param hash the given hash (upper(bits/8) long array)
 * @param hash_bits in bits
 * @param prime the prime data
 */
void ecdsa_hash_to_gfp( gfp_t element, const uint8_t *hash, const int hash_bits, const gfp_prime_data_t *prime ) {
    uint32_t hash_bytes = BYTES_PER_BITS( hash_bits );
    uint32_t prime_bytes;
    uint32_t i;

    if( prime->bits >= (uint32_t)hash_bits ) {
        bigint_clear_var( element, prime->words );
        for( i = 0; i < hash_bytes; i++ ) {
            bigint_set_byte_var( element, prime->words, hash_bytes - 1 - i, hash[i] );
        }
    } else {
        // prime->bits smaller than hash_bits
        prime_bytes = BYTES_PER_BITS( prime->bits );
        for( i = 0; i < prime_bytes; i++ ) {
            bigint_set_byte_var( element, prime->words, prime_bytes - 1 - i, hash[i] );
        }
        bigint_shift_right_var( element, element, ( hash_bits - prime->bits ) % 8, prime->words );
    }
    gfp_reduce( element, prime );
}
