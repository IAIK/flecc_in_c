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

#ifndef BI_CONST_RUNTIME_H
#define BI_CONST_RUNTIME_H

#ifdef __cplusplus
extern "C" {
#endif

#include "flecc_in_c/types.h"

/**
 * Switch two big integers with configurable length in constant time and with constant access pattern.
 *
 * @param var1 the first big integer
 * @param var2 the second big integer
 * @param condition if 1, variables are switched; if 0, no switch is performed
 * @param length the number of uint_t elements in the big integers (>= 1)
 */
void bigint_cr_switch( uint_t *var1, uint_t *var2, const int condition, const int length );

/**
 * Copy one of two big integers with configurable length to the destination in constant time.
 *
 * @param result the resulting big integer
 * @param var0 the first big integer
 * @param var1 the second big integer
 * @param condition if 0, selects var0, if 1, selects var1 (0 || 1)
 * @param length the number of uint_t elements in the big integers (>= 1)
 */
void bigint_cr_select_2( uint_t *result, const uint_t *var0, const uint_t *var1, const int condition, const int length );

/**
 * Compares two big integers for equality with configurable length in constant time.
 *
 * @param a the first operand for the comparison
 * @param b the second operand for the comparison
 * @param length the number of uint_t elements in the big integers (>= 1)
 * @return 1 if equal; 0 if not equal
 */
int bigint_cr_is_equal_var( const uint_t *a, const uint_t *b, const int length );

/**
 * Checks if a big integer is zero.
 *
 * @param a the big integer for the check
 * @param length the number of uint_t elements in the big integer (>= 1)
 * @return 1 if zero; otherwise 0.
 */
int bigint_cr_is_zero_var( const uint_t *a, const int length );

#ifdef __cplusplus
}
#endif

#endif /* BI_CONST_RUNTIME_H */
