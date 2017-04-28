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

/**
 *  @file gfp.h
 *
 *  The following defines gfp_* are assuming that there exists a local
 *  variable ecc_parameters_t *param. The defines beautify the eccp code
 *  and make it easier to exchange underlying gfp_* functions.
 */

#ifndef GFP_H_
#define GFP_H_

#include "../bi/bi.h"
#include "gfp_const_runtime.h"
#include "gfp_gen.h"
#include "gfp_mont.h"

/* performance optimized vs. constant runtime implementations */
#if 0

#define gfp_add( res, a, b ) gfp_gen_add( res, a, b, &param->prime_data )
#define gfp_subtract( res, a, b ) gfp_gen_subtract( res, a, b, &param->prime_data )
#define gfp_halving( res, a ) gfp_gen_halving( res, a, &param->prime_data )
#define gfp_negate( res, a ) gfp_gen_negate( res, a, &param->prime_data )
#define gfp_multiply( res, a, b ) gfp_mont_multiply( res, a, b, &param->prime_data )
#define gfp_square( res, a ) gfp_mont_multiply( res, a, a, &param->prime_data )
#define gfp_inverse( res, a ) gfp_mont_inverse( res, a, &param->prime_data )
#define gfp_exponent( res, a, exponent, exponent_length )                                                                        \
    gfp_mont_exponent( res, a, exponent, exponent_length, &param->prime_data )

#define gfp_clear( dest ) bigint_clear_var( dest, param->prime_data.words )
#define gfp_copy( dest, src ) bigint_copy_var( dest, src, param->prime_data.words )
#define gfp_is_equal( a, b ) bigint_is_equal_var( a, b, param->prime_data.words )
#define gfp_is_zero( a ) bigint_is_zero_var( a, param->prime_data.words )

#else

#define gfp_add( res, a, b ) gfp_cr_add( res, a, b, &param->prime_data )
#define gfp_subtract( res, a, b ) gfp_cr_subtract( res, a, b, &param->prime_data )
#define gfp_halving( res, a ) gfp_cr_halving( res, a, &param->prime_data )
#define gfp_negate( res, a ) gfp_cr_negate( res, a, &param->prime_data )
#define gfp_multiply( res, a, b ) gfp_mont_multiply( res, a, b, &param->prime_data )
#define gfp_square( res, a ) gfp_mont_multiply( res, a, a, &param->prime_data )
#define gfp_inverse( res, a ) gfp_mont_inverse_fermat( res, a, &param->prime_data )
#define gfp_exponent( res, a, exponent, exponent_length )                                                                        \
    gfp_mont_exponent( res, a, exponent, exponent_length, &param->prime_data )

#define gfp_clear( dest ) bigint_clear_var( dest, param->prime_data.words )
#define gfp_copy( dest, src ) bigint_copy_var( dest, src, param->prime_data.words )
#define gfp_is_equal( a, b ) bigint_cr_is_equal_var( a, b, param->prime_data.words )
#define gfp_is_zero( a ) bigint_cr_is_zero_var( a, param->prime_data.words )

#endif

#endif /* GFP_H_ */
