#include "flecc_in_c/bi/bi.h"
#include "flecc_in_c/eccp/eccp.h"
#include "flecc_in_c/gfp/gfp.h"
#include "flecc_in_c/hash/sha2.h"
#include "flecc_in_c/io/io.h"
#include "flecc_in_c/protocols/protocols.h"
#include "flecc_in_c/utils/param.h"
#include "flecc_in_c/utils/parse.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

const char *curve_type_str = "secp256r1";

const char *priv_key_str = "66948777AE2CBD0760AEC5809D49AFC92BA7FB68B7EE93FF37DA7046E72B424B"; // private key
const char *pub_aff_x = "47DA6BF2E8D7A0BB0A5994C5790C75BC735679B0C437C2626E70C07AD542DA83";    // public key x
const char *pub_aff_y = "807328B5E0BF5FF1596266B746F6EDE01525AF3AA23CD437EFACF02F2108BD06";    // public key y

const char *msg_str = "This is the message, where the sha256 hash is generated from.";

/**
 * Parses a hex encoded bigint from a zero terminated string.
 * @param string the string to parse
 * @param big_int the big integer to write into
 * @param bi_length the length of the big integer
 */
void parse_bigint( const char *string, uint_t *big_int, const int bi_length ) {
    int len = strlen( string );
    bigint_parse_hex_var( big_int, bi_length, string, len );
}

int main() {
    // seed rand which is used internally
    srand( time( 0 ) );

    // load curve
    printf( "Curve:\n%s\n", curve_type_str );
    eccp_parameters_t curve_params;
    curve_params.curve_type = param_get_curve_type_from_name( curve_type_str, strlen( curve_type_str ) );
    param_load( &curve_params, curve_params.curve_type );

    // parse private key
    gfp_t priv_key;
    parse_bigint( priv_key_str, priv_key, curve_params.order_n_data.words );
    printf( "\nPrivate Key: (MSB->LSB)\n" );
    io_print_bigint_var( priv_key, curve_params.order_n_data.words );

    // parse public key
    eccp_point_affine_t public_key;
    parse_bigint( pub_aff_x, public_key.x, curve_params.order_n_data.words );
    parse_bigint( pub_aff_y, public_key.y, curve_params.order_n_data.words );
    public_key.identity = 0;
    printf( "\nPublic Key: (MSB->LSB)\n" );
    io_print_affine_point( &public_key, &curve_params );

    // calculate hash
    printf( "\nMessage:\n%s\n", msg_str );
    hash_sha224_256_t sha2_state;
    gfp_t hash_array;
    int message_length = strlen( msg_str );
    hash_sha256_init( &sha2_state );
    hash_sha2_final( &sha2_state, (uint8_t *)msg_str, message_length, message_length );
    hash_sha256_to_byte_array( (uint8_t *)hash_array, &sha2_state );
    printf( "\nSHA-256 Hash interpreted as number: (MSB->LSB)\n" );
    io_print_bigint_var( hash_array, curve_params.order_n_data.words );

    // calculate signature
    ecdsa_signature_t signature;
    ecdsa_sign( &signature, hash_array, priv_key, &curve_params );
    printf( "\nSignature:\nr = " );
    io_print_bigint_var( signature.r, curve_params.order_n_data.words );
    printf( "s = " );
    io_print_bigint_var( signature.s, curve_params.order_n_data.words );

    // verify signature
    int is_valid = ecdsa_is_valid( &signature, hash_array, &public_key, &curve_params );
    printf( "\nVerification %s\n", is_valid ? "succeed" : "failed" );

    return !is_valid;
}
