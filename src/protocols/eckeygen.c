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
#include "flecc_in_c/utils/rand.h"

/**
 * Generate an elliptic curve key pair
 * @param private_key resulting private key
 * @param public_key resulting public key
 * @param param elliptic curve parameters
 */
void eckeygen( gfp_t private_key, eccp_point_affine_t *public_key, eccp_parameters_t *param ) {
    gfp_rand( private_key, &param->order_n_data );

    eccp_generic_mul_wrapper( public_key, &param->base_point, private_key, param );

    // convert to normal basis such that other party has no problem
    if( param->prime_data.montgomery_domain == 1 ) {
        gfp_montgomery_to_normal( public_key->x, public_key->x, &param->prime_data );
        gfp_montgomery_to_normal( public_key->y, public_key->y, &param->prime_data );
    }
}
