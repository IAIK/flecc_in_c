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

#ifndef ECCP_STD_PROJECTIVE_H_
#define ECCP_STD_PROJECTIVE_H_

#include "../types.h"

int eccp_std_projective_point_is_valid( const eccp_point_projective_t *a, const eccp_parameters_t *param );
int eccp_std_projective_point_equals( const eccp_point_projective_t *a,
                                      const eccp_point_projective_t *b,
                                      const eccp_parameters_t *param );
void eccp_std_projective_point_copy( eccp_point_projective_t *dest,
                                     const eccp_point_projective_t *src,
                                     const eccp_parameters_t *param );

void eccp_std_projective_to_affine( eccp_point_affine_t *res, const eccp_point_projective_t *a, const eccp_parameters_t *param );
void eccp_affine_to_std_projective( eccp_point_projective_t *res, const eccp_point_affine_t *a, const eccp_parameters_t *param );
void eccp_std_projective_point_negate( eccp_point_projective_t *res,
                                       const eccp_point_projective_t *P,
                                       const eccp_parameters_t *param );

#endif /* ECCP_STD_PROJECTIVE_H_ */
