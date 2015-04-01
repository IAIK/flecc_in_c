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

#include "gfp_gen.h"
#include "../bi/bi.h"

/**
 * Adds two numbers a,b and stores the result in res IN CONSTANT TIME. 
 * A finite field reduction is automatically performed.
 * @param res the result
 * @param a the first parameter to add
 * @param b the second parameter to add
 * @param prime_data the prime number data to reduce the result
 */
void gfp_cr_add( gfp_t res, const gfp_t a, const gfp_t b, const gfp_prime_data_t *prime_data ) {
    gfp_t temp;
    int carry = bigint_add_var( res, a, b, prime_data->words );
    int carry2 = -bigint_subtract_var(temp, res, prime_data->prime, prime_data->words);
    
    bigint_cr_select_2(res, res, temp, carry | (1-carry2), prime_data->words);
}


/**
 * Subtract b from a, store it in res, use the modulo. res = (a - b) mod modulo.
 * @param a the minuend
 * @param b the subtrahend
 * @param res the difference
 * @param prime_data the prime number data to reduce the result
 */
void gfp_cr_subtract( gfp_t res, const gfp_t a, const gfp_t b, const gfp_prime_data_t *prime_data ) {
    gfp_t temp;
    int carry = bigint_subtract_var( res, a, b, prime_data->words );
    bigint_add_var( temp, res, prime_data->prime, prime_data->words );
    
    bigint_cr_select_2(res, res, temp, carry, prime_data->words);
}
