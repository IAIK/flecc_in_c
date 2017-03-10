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

#include "flecc_in_c/hash/sha2.h"

const uint32_t K256[64]
    = {0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL, 0x3956c25bUL, 0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL,
       0xd807aa98UL, 0x12835b01UL, 0x243185beUL, 0x550c7dc3UL, 0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL, 0xc19bf174UL,
       0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL, 0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL,
       0x983e5152UL, 0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL, 0xc6e00bf3UL, 0xd5a79147UL, 0x06ca6351UL, 0x14292967UL,
       0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL, 0x53380d13UL, 0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
       0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL, 0xd192e819UL, 0xd6990624UL, 0xf40e3585UL, 0x106aa070UL,
       0x19a4c116UL, 0x1e376c08UL, 0x2748774cUL, 0x34b0bcb5UL, 0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL, 0x682e6ff3UL,
       0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL, 0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL};

#define ROTATE( a, n ) ( ( ( a ) >> ( n ) ) + ( ( a ) << ( 32 - ( n ) ) ) )

#define Sigma0( x ) ( ROTATE( ( x ), 2 ) ^ ROTATE( ( x ), 13 ) ^ ROTATE( ( x ), 22 ) )
#define Sigma1( x ) ( ROTATE( ( x ), 6 ) ^ ROTATE( ( x ), 11 ) ^ ROTATE( ( x ), 25 ) )
#define sigma0( x ) ( ROTATE( ( x ), 7 ) ^ ROTATE( ( x ), 18 ) ^ ( ( x ) >> 3 ) )
#define sigma1( x ) ( ROTATE( ( x ), 17 ) ^ ROTATE( ( x ), 19 ) ^ ( ( x ) >> 10 ) )

#define Ch( x, y, z ) ( ( ( x ) & ( y ) ) ^ ( ( ~( x ) ) & ( z ) ) )
#define Maj( x, y, z ) ( ( ( x ) & ( y ) ) ^ ( ( x ) & ( z ) ) ^ ( ( y ) & ( z ) ) )

/**
 * Initialize the state with the SHA224 initialization vector
 * @param state the hash state
 */
void hash_sha224_init( hash_sha224_256_t *state ) {
    state->hash[0] = 0xc1059ed8UL;
    state->hash[1] = 0x367cd507UL;
    state->hash[2] = 0x3070dd17UL;
    state->hash[3] = 0xf70e5939UL;
    state->hash[4] = 0xffc00b31UL;
    state->hash[5] = 0x68581511UL;
    state->hash[6] = 0x64f98fa7UL;
    state->hash[7] = 0xbefa4fa4UL;
}

/**
 * Initialize the state with the SHA256 initialization vector
 * @param state the hash state
 */
void hash_sha256_init( hash_sha224_256_t *state ) {
    state->hash[0] = 0x6a09e667UL;
    state->hash[1] = 0xbb67ae85UL;
    state->hash[2] = 0x3c6ef372UL;
    state->hash[3] = 0xa54ff53aUL;
    state->hash[4] = 0x510e527fUL;
    state->hash[5] = 0x9b05688cUL;
    state->hash[6] = 0x1f83d9abUL;
    state->hash[7] = 0x5be0cd19UL;
}

/**
 * Process a 512-bit block (w) to update the current hash state
 * @param state the hash state to update
 * @param w already prepared uint32_t[16] array used to update the state (is overwritten during the calculation)
 */
void hash_sha2_process_block( hash_sha224_256_t *state, uint32_t *message ) {
    uint32_t a, b, c, d, e, f, g, h;
    uint32_t s0, s1, T1, T2;
    int i;

    a = state->hash[0];
    b = state->hash[1];
    c = state->hash[2];
    d = state->hash[3];
    e = state->hash[4];
    f = state->hash[5];
    g = state->hash[6];
    h = state->hash[7];

    for( i = 0; i < 16; i++ ) {
        T1 = message[i];
        T1 += h + Sigma1( e ) + Ch( e, f, g ) + K256[i];
        T2 = Sigma0( a ) + Maj( a, b, c );
        h = g;
        g = f;
        f = e;
        e = d + T1;
        d = c;
        c = b;
        b = a;
        a = T1 + T2;
    }

    for( ; i < 64; i++ ) {
        s0 = message[( i + 1 ) & 0x0f];
        s0 = sigma0( s0 );
        s1 = message[( i + 14 ) & 0x0f];
        s1 = sigma1( s1 );

        T1 = message[i & 0xf] += s0 + s1 + message[( i + 9 ) & 0xf];
        T1 += h + Sigma1( e ) + Ch( e, f, g ) + K256[i];
        T2 = Sigma0( a ) + Maj( a, b, c );
        h = g;
        g = f;
        f = e;
        e = d + T1;
        d = c;
        c = b;
        b = a;
        a = T1 + T2;
    }

    state->hash[0] += a;
    state->hash[1] += b;
    state->hash[2] += c;
    state->hash[3] += d;
    state->hash[4] += e;
    state->hash[5] += f;
    state->hash[6] += g;
    state->hash[7] += h;
}

/**
 * Update the hash state with a 512 bit large message.
 * @param state the hash state to update
 * @param message a 64 bytes long byte array
 */
void hash_sha2_update( hash_sha224_256_t *state, const uint8_t *message ) {
    uint32_t message_ui[16];
    uint32_t temp;
    int i;

    for( i = 0; i < 16 * 4; i += 4 ) {
        temp = ( (uint32_t)message[i] ) << 24;
        temp |= ( (uint32_t)message[i + 1] ) << 16;
        temp |= ( (uint32_t)message[i + 2] ) << 8;
        temp |= (uint32_t)message[i + 3];
        message_ui[i / 4] = temp;
    }

    hash_sha2_process_block( state, message_ui );
}

/**
 * Finalizes the sha2 hash calculation.
 * @param state the current state of the hash calculation --> will contain the final hash value.
 * @param message the remaining message to be signed.
 * @param remaining_length the number of remaining bytes within "message"
 * @param total_length the total number of bytes signed
 *
 * (total_length - remaining_length) should already been processed using sha1_process_message or sha1_process_block.
 * (total_length - remaining_length) must be a multiple of 64!!
 */
void hash_sha2_final( hash_sha224_256_t *state, const uint8_t *message, const int remaining_length, const int total_length ) {
    int i, remaining_length_ = remaining_length;
    uint32_t message_ui[16];
    uint32_t temp;

    /* debug_assert(total_length >= remaining_length, "total_length >
     * remaining_length"); */
    /* debug_assert((total_length - remaining_length) % 64 == 0, "(total_length -
     * remaining_length) % 64 == 0"); */

    while( remaining_length_ >= 64 ) {
        /* copy message into the message block w */
        for( i = 0; i < 16 * 4; i += 4 ) {
            temp = ( ( (uint32_t)message[i] ) & 0xFF ) << 24;
            temp |= ( ( (uint32_t)message[i + 1] ) & 0xFF ) << 16;
            temp |= ( ( (uint32_t)message[i + 2] ) & 0xFF ) << 8;
            temp |= ( (uint32_t)message[i + 3] ) & 0xFF;
            message_ui[i / 4] = temp;
        }

        hash_sha2_process_block( state, message_ui );
        message += 64;
        remaining_length_ -= 64;
    }

    /* copy message into the message block w */
    for( i = 0; i < 16; i++ ) {
        message_ui[i] = 0;
    }
    for( i = 0; i < remaining_length_; i++ ) {
        message_ui[i / 4] |= ( ( (uint32_t)message[i] ) & 0xFF ) << ( ( ~i & 3 ) * 8 );
    }

    /* do message padding */
    message_ui[remaining_length_ >> 2] |= 0x80L << ( ( ~remaining_length_ & 3 ) * 8 );

    if( remaining_length_ < 56 ) {
        message_ui[15] = total_length * 8;
        hash_sha2_process_block( state, message_ui );
    } else {
        hash_sha2_process_block( state, message_ui );

        for( i = 0; i < 15; i++ ) {
            message_ui[i] = 0;
        }

        message_ui[15] = total_length * 8;
        hash_sha2_process_block( state, message_ui );
    }
}

/**
 *  Convert the hash state into a 224/8=28 byte long byte array.
 *  @param hash the 28 byte long array
 *  @param state the hash state
 */
void hash_sha224_to_byte_array( uint8_t *hash, const hash_sha224_256_t *state ) {
    int i;
    for( i = 0; i < 7; i++ ) {
        *hash++ = state->hash[i] >> 24;
        *hash++ = ( state->hash[i] >> 16 ) & 0xFF;
        *hash++ = ( state->hash[i] >> 8 ) & 0xFF;
        *hash++ = state->hash[i] & 0xFF;
    }
}

/**
 *  Convert the hash state into a 256/8=32 byte long byte array.
 *  @param hash the 32 byte long array
 *  @param state the hash state
 */
void hash_sha256_to_byte_array( uint8_t *hash, const hash_sha224_256_t *state ) {
    int i;
    for( i = 0; i < 8; i++ ) {
        *hash++ = state->hash[i] >> 24;
        *hash++ = ( state->hash[i] >> 16 ) & 0xFF;
        *hash++ = ( state->hash[i] >> 8 ) & 0xFF;
        *hash++ = state->hash[i] & 0xFF;
    }
}
