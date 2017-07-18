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

#include <flecc_in_c/bi/bi.h>
#include <flecc_in_c/io/io.h>

#include <stdio.h>

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
        if( *( a + i ) > *( b + i ) ) {
            return 1;
        }
        if( *( a + i ) < *( b + i ) ) {
            return -1;
        }
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
    fputs( "Test ", stdout );
    fputs( test_id, stdout );
    if( condition ) {
        fputs( ": success\n", stdout );
        return 0;
    }
    fputs( ": error\n", stdout );
    return 1;
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
        fputs( "    expected: ", stdout );
        io_print_bigint_var( expected, length );
        fputs( "    actual:   ", stdout );
        io_print_bigint_var( to_test, length );
        fputs( "    diff:     ", stdout );
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
        fputs( "    expected: ", stdout );
        io_print_integer( expected );
        fputs( "    actual:   ", stdout );
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
        fputs( "    expected: ", stdout );
        io_print_bytes_var( expected, length );
        fputs( "    actual:   ", stdout );
        io_print_bytes_var( to_test, length );
        return 1;
    }
    return 0;
}
