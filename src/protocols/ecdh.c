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

#include "flecc_in_c/eccp/eccp.h"
#include "flecc_in_c/gfp/gfp.h"

/**
 * First phase of a diffie-hellman key exchange
 * @param res resulting point
 * @param scalar ephemeral or static private key (param->order_n_mont_data.prime_data.words large)
 * @param param elliptic curve parameters
 */
void ecdh_phase_one( eccp_point_affine_t *res, const gfp_t scalar, const eccp_parameters_t *param ) {

    // base_point is already in Montgomery domain
    eccp_generic_mul_wrapper( res, &param->base_point, scalar, param );

    // convert to normal basis such that other party has no problem
    if( param->prime_data.montgomery_domain == 1 ) {
        gfp_montgomery_to_normal( res->x, res->x, &param->prime_data );
        gfp_montgomery_to_normal( res->y, res->y, &param->prime_data );
    }
}

/**
 * First phase of a diffie-hellman key exchange
 * @param res resulting point
 * @param scalar ephemeral or static private key (param->order_n_mont_data.prime_data.words large)
 * @param other_party_point the (ephemeral) public key of the other party
 * @param param elliptic curve parameters
 */
void ecdh_phase_two( eccp_point_affine_t *res,
                     const gfp_t scalar,
                     eccp_point_affine_t *other_party_point,
                     const eccp_parameters_t *param ) {

    // assume that the other party does not use Montgomery domain
    if( param->prime_data.montgomery_domain == 1 ) {
        gfp_normal_to_montgomery( other_party_point->x, other_party_point->x, &param->prime_data );
        gfp_normal_to_montgomery( other_party_point->y, other_party_point->y, &param->prime_data );
    }

    if( eccp_affine_point_is_valid( other_party_point, param ) == 0 ) {
        // TODO: error handling
    }

    eccp_generic_mul_wrapper( res, other_party_point, scalar, param );

    // the commonly derived key shall not be in montgomery domain
    if( param->prime_data.montgomery_domain == 1 ) {
        gfp_montgomery_to_normal( res->x, res->x, &param->prime_data );
        gfp_montgomery_to_normal( res->y, res->y, &param->prime_data );
    }
}
