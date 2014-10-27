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

#ifndef GFP_MONT_H_
#define GFP_MONT_H_

#include "../types.h"

void gfp_normal_to_montgomery( gfp_t res, const gfp_t src, const gfp_prime_data_t *prime_data );
void gfp_montgomery_to_normal( gfp_t res, const gfp_t src, const gfp_prime_data_t *prime_data );

void gfp_mont_compute_R( gfp_t res, gfp_prime_data_t *prime_data );
void gfp_mont_compute_R_squared( gfp_t res, gfp_prime_data_t *prime_data );
uint_t gfp_mont_compute_n0( gfp_prime_data_t *prime_data );
void gfp_mont_multiply_sos( gfp_t res, const gfp_t a, const gfp_t b, const gfp_prime_data_t *prime_data );
void gfp_mont_inverse( gfp_t result, const gfp_t a, const gfp_prime_data_t *prime_data );
void gfp_mont_exponent(
    gfp_t res, const gfp_t a, const uint_t *exponent, const int exponent_length, const gfp_prime_data_t *prime_data );

void gfp_mult_two_mont( gfp_t res, const gfp_t a, const gfp_t b, const gfp_prime_data_t *prime_data );

#define gfp_mont_multiply( res, a, b, prime_data ) gfp_mont_multiply_sos( res, a, b, prime_data )

#endif /* GFP_MONT_H_ */
