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

#ifndef GFP_OPT_3_H
#define GFP_OPT_3_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../types.h"

void gfp_opt_3_init( gfp_prime_data_t *prime_data_local );
void gfp_opt_3_add( gfp_t res, const gfp_t a, const gfp_t b );
void gfp_opt_3_subtract( gfp_t res, const gfp_t a, const gfp_t b );
void gfp_opt_3_multiply( gfp_t res, const gfp_t a, const gfp_t b );
void gfp_opt_3_inverse( gfp_t res, const gfp_t a );

#ifdef __cplusplus
}
#endif

#endif /* GFP_OPT_3_H */
