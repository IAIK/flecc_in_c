/*
 * assert.c
 *
 *  Created on: 20.03.2014
 *      Author: Thomas Unterluggauer
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

#include "../io/io.h"
#include "../bi/bi.h"

/**
 * Compares two byte arrays of defined length.
 * @param a the first array to be compared
 * @param b the second array to be compared
 * @param length the length of the arrays
 * @return 0 if equal, 1 if the first is larger, -1 otherwise
 */
int compare_bytes( const uint8_t *a, const uint8_t *b, const int length ) {
    int i = length - 1;

    while( i >= 0 ) {
        if( *( a + i ) > *( b + i ) )
            return 1;
        if( *( a + i ) < *( b + i ) )
            return -1;
        i--;
    }

    return 0;
}

/**
 * Checks whether the condition if fulfilled for the
 * given test_id and prints appropriate output.
 * @param test_id the test id
 * @param condition the condition to be checked
 * @return false (=0) on success, true (!=0) on error
 */
int assert( const char *test_id, const int condition ) {
    io_print( "Test " );
    io_print( test_id );
    if( condition ) {
        io_println( ": success" );
        return 0;
    } else {
        io_println( ": error" );
        return 1;
    }
}

/**
 * Checks whether the big integer to_test equals the expected
 * big integer for the given test id and prints appropriate
 * output.
 * @param test_id the test id
 * @param expected the expected bigint
 * @param to_test the bigint to be tested
 * @param length the length of the two big integers
 * @return false (=0) on success, true (!=0) on error
 */
int assert_bigint( const char *test_id, const uint_t *expected, const uint_t *to_test, const int length ) {
    int comp = bigint_compare_var( expected, to_test, length );
    assert( test_id, comp == 0 );
    if( comp != 0 ) {
        uint_t diff[2 * WORDS_PER_GFP];
        bigint_xor_var( diff, expected, to_test, length );
        io_print( "    expected: " );
        io_print_bigint_var( expected, length );
        io_print( "    actual:   " );
        io_print_bigint_var( to_test, length );
        io_print( "    diff:     " );
        io_print_bigint_var( diff, length );
        return 1;
    }
    return 0;
}

/**
 * Checks whether the integer to_test equals the expected integer
 * for the given test id and prints appropriate output.
 * @param test_id the test id
 * @param expected the expected big integer
 * @param to_test the big integer that is being tested
 * @return false (=0) on success, true (!=0) on error
 */
int assert_integer( const char *test_id, const int expected, const int to_test ) {
    int comp = ( expected == to_test );
    assert( test_id, comp );
    if( comp == 0 ) {
        io_print( "    expected: " );
        io_print_integer( expected );
        io_print( "    actual:   " );
        io_print_integer( to_test );
        return 1;
    }
    return 0;
}

/**
 * Checks whether the byte array to_test equals the expected byte array
 * for the given test id and prints appropriate output.
 * @param test_id the test id
 * @param expected the expected byte array
 * @param to_test the byte array that is being tested
 * @param length the length of the two byte arrays
 * @return false (=0) on success, true (!=0) on error
 */
int assert_byte_array( const char *test_id, const uint8_t *expected, const uint8_t *to_test, const int length ) {
    int comp = compare_bytes( expected, to_test, length );
    assert( test_id, comp == 0 );
    if( comp != 0 ) {
        io_print( "    expected: " );
        io_print_bytes_var( expected, length );
        io_print( "    actual:   " );
        io_print_bytes_var( to_test, length );
        return 1;
    }
    return 0;
}
