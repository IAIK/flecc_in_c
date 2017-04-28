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

#include "flecc_in_c/bi/bi_const_runtime.h"
#include "flecc_in_c/bi/bi_gen.h"

#include <assert.h>

void bigint_cr_switch( uint_t *var1, uint_t *var2, const int condition, const int length ) {
    assert( length > 0 );
    assert( condition == 0 || condition == 1 );

    uint_t M2 = -condition;
    uint_t M1 = ~M2;

    for( int i = 0; i < length; i++ ) {
        uint_t V1 = var1[i];
        uint_t V2 = var2[i];

        var1[i] = ( V1 & M1 ) | ( V2 & M2 );
        var2[i] = ( V1 & M2 ) | ( V2 & M1 );
    }
}

void bigint_cr_select_2( uint_t *result, const uint_t *var0, const uint_t *var1, const int condition, const int length ) {
    assert( length > 0 );
    assert( condition == 0 || condition == 1 );

    uint_t M1 = -condition;
    uint_t M0 = ~M1;

    for( int i = 0; i < length; i++ ) {
        result[i] = ( var0[i] & M0 ) | ( var1[i] & M1 );
    }
}

int bigint_cr_is_equal_var( const uint_t *a, const uint_t *b, const int length ) {
    assert( length > 0 );

    // compress big integers differences into one word
    uint_t temp = 0;
    for( int i = length - 1; i >= 0; i-- ) {
        temp |= a[i] ^ b[i];
    }
    temp = ~temp;

    // if all bits are anded together, the LSB will be 1 if all bits are equal
    for( int j = 1; j != BITS_PER_WORD; j <<= 1 ) {
        temp &= ( temp >> j );
    }

    return temp & 1;
}

int bigint_cr_is_zero_var( const uint_t *a, const int length ) {
    assert( length > 0 );

    // compress big integer into one word
    uint_t temp = 0;
    for( int i = length - 1; i >= 0; i-- ) {
        temp |= a[i];
    }
    temp = ~temp;

    // if all bits are anded together, the LSB will be 1 the big integer is zero
    for( int j = 1; j != BITS_PER_WORD; j <<= 1 ) {
        temp &= ( temp >> j );
    }

    return temp & 1;
}
