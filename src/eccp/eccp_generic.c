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

#include "flecc_in_c/eccp/eccp_generic.h"
#include "flecc_in_c/eccp/eccp_affine.h"
#include <stdlib.h> // for the definition of the NULL pointer

/**
 * wraps a generic point/scalar multiplication to one of the optimized
 * methods (param->eccp_mul_base_point or param->eccp_mul)
 * @param result the resulting point
 * @param P the point to multiply with the scalar
 * @param scalar the scalar to multiply with the point P
 * @param param a set of parameters, necessary for the multiplication of P with scalar
 */
void eccp_generic_mul_wrapper( eccp_point_affine_t *result,
                               const eccp_point_affine_t *P,
                               const gfp_t scalar,
                               const eccp_parameters_t *param ) {
    if( ( param->eccp_mul_base_point != NULL ) && ( param->base_point_precomputed_table != NULL )
        && ( param->base_point_precomputed_table_width != 0 )
        && ( eccp_affine_point_compare( &param->base_point, P, param ) == 0 ) ) {
        param->eccp_mul_base_point( result, scalar, param );
    } else {
        param->eccp_mul( result, P, scalar, param );
    }
}
