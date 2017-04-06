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

#ifndef SHA1_C_
#define SHA1_C_

#include "flecc_in_c/hash/sha1.h"

#define leftrotate( value, bits ) ( ( ( value ) << ( bits ) ) | ( ( value ) >> ( 32 - ( bits ) ) ) )

/**
 * Initialize the state variable.
 * @param state the state to initialize
 */
void hash_sha1_init( hash_sha1_t *state ) {
    state->H0 = 0x67452301;
    state->H1 = 0xEFCDAB89;
    state->H2 = 0x98BADCFE;
    state->H3 = 0x10325476;
    state->H4 = 0xC3D2E1F0;
}

/**
 * Process a 512-bit block (w) to update the current hash state
 * @param state the hash state to update
 * @param w already prepared uint32_t[16] array used to update the state (is
 * overwritten during the calculation)
 */
void hash_sha1_process_block( hash_sha1_t *state, uint32_t *w ) {
    int i;
    uint32_t ST_A, ST_B, ST_C, ST_D, ST_E, ST_F, ST_K, ST_T;

    /* init state */
    ST_A = state->H0;
    ST_B = state->H1;
    ST_C = state->H2;
    ST_D = state->H3;
    ST_E = state->H4;

    /* perform 80 rounds */
    for( i = 0; i <= 79; i++ ) {
        if( i <= 19 ) {
            ST_F = ST_D ^ ( ST_B & ( ST_C ^ ST_D ) );
            ST_K = 0x5A827999;
        } else if( i <= 39 ) {
            ST_F = ST_B ^ ST_C ^ ST_D;
            ST_K = 0x6ED9EBA1;
        } else if( i <= 59 ) {
            ST_F = ( ST_B & ST_C ) | ( ST_D & ( ST_B | ST_C ) );
            ST_K = 0x8F1BBCDC;
        } else {
            ST_F = ST_B ^ ST_C ^ ST_D;
            ST_K = 0xCA62C1D6;
        }

        if( i >= 16 ) {
            ST_T = leftrotate( w[( i + 13 ) & 15] ^ w[( i + 8 ) & 15] ^ w[( i + 2 ) & 15] ^ w[i & 15], 1 );
            w[i & 15] = ST_T;
        }

        ST_T = leftrotate( ST_A, 5 ) + ST_F + ST_E + ST_K + w[i & 15];
        ST_E = ST_D;
        ST_D = ST_C;
        ST_C = leftrotate( ST_B, 30 );
        ST_B = ST_A;
        ST_A = ST_T;
    }

    state->H0 += ST_A;
    state->H1 += ST_B;
    state->H2 += ST_C;
    state->H3 += ST_D;
    state->H4 += ST_E;
}

/**
 * Converts the message to a block and calls sha1_process_block
 * @param state the hash state to update
 * @param message a 64 byte long message block
 */
void hash_sha1_update( hash_sha1_t *state, const uint8_t *message ) {
    int i;
    uint32_t w[16];
    uint32_t temp;

    /* copy message into the message block w */
    for( i = 0; i < 16 * 4; i += 4 ) {
        temp = ( (uint32_t)message[i] ) << 24;
        temp |= ( (uint32_t)message[i + 1] ) << 16;
        temp |= ( (uint32_t)message[i + 2] ) << 8;
        temp |= (uint32_t)message[i + 3];
        w[i / 4] = temp;
    }

    hash_sha1_process_block( state, w );
}

/**
 * Finalizes the sha1 hash calculation.
 * @param state the current state of the hash calculation --> will contain the final hash value.
 * @param message the remaining message to be signed.
 * @param remaining_length the number of remaining bytes within "message"
 * @param total_length the total number of bytes signed
 *
 * (total_length - remaining_length) should already been processed using sha1_process_message or sha1_process_block.
 * (total_length - remaining_length) must be a multiple of 64!!
 */
void hash_sha1_final( hash_sha1_t *state, const uint8_t *message, const int remaining_length, const int total_length ) {
    int i, remaining_length_ = remaining_length;
    uint32_t w[16];
    uint32_t temp;

    /* debug_assert(total_length >= remaining_length, "total_length >
     * remaining_length"); */
    /* debug_assert((total_length - remaining_length) % 64 == 0, "(total_length -
     * remaining_length) % 64 == 0"); */

    while( remaining_length_ >= 64 ) {
        /* copy message into the message block w */
        for( i = 0; i < 16 * 4; i += 4 ) {
            temp = ( (uint32_t)message[i] ) << 24;
            temp |= ( (uint32_t)message[i + 1] ) << 16;
            temp |= ( (uint32_t)message[i + 2] ) << 8;
            temp |= (uint32_t)message[i + 3];
            w[i / 4] = temp;
        }

        hash_sha1_process_block( state, w );
        message += 64;
        remaining_length_ -= 64;
    }

    /* copy message into the message block w */
    for( i = 0; i < 16; i++ ) {
        w[i] = 0;
    }
    for( i = 0; i < remaining_length_; i++ ) {
        w[i / 4] |= ( (uint32_t)message[i] ) << ( ( ~i & 3 ) * 8 );
    }

    /* do message padding */
    w[remaining_length_ >> 2] |= ( (uint32_t)0x80 ) << ( ( ~remaining_length_ & 3 ) * 8 );

    if( remaining_length_ < 56 ) {
        w[15] = total_length * 8;
        hash_sha1_process_block( state, w );
    } else {
        hash_sha1_process_block( state, w );

        for( i = 0; i < 15; i++ ) {
            w[i] = 0;
        }

        w[15] = total_length * 8;
        hash_sha1_process_block( state, w );
    }
}

/**
 * Convert the hash state into a 160/8=20 byte long byte array.
 * @param hash a 20 byte long array
 * @param state the hash state
 */
void hash_sha1_to_byte_array( uint8_t *hash, const hash_sha1_t *state ) {
    hash[0] = state->H0 >> 24;
    hash[1] = state->H0 >> 16;
    hash[2] = state->H0 >> 8;
    hash[3] = state->H0;
    hash[4] = state->H1 >> 24;
    hash[5] = state->H1 >> 16;
    hash[6] = state->H1 >> 8;
    hash[7] = state->H1;
    hash[8] = state->H2 >> 24;
    hash[9] = state->H2 >> 16;
    hash[10] = state->H2 >> 8;
    hash[11] = state->H2;
    hash[12] = state->H3 >> 24;
    hash[13] = state->H3 >> 16;
    hash[14] = state->H3 >> 8;
    hash[15] = state->H3;
    hash[16] = state->H4 >> 24;
    hash[17] = state->H4 >> 16;
    hash[18] = state->H4 >> 8;
    hash[19] = state->H4;
}

#endif /* SHA1_C_ */
