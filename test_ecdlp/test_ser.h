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

/*
 * test_ser.h
 *
 *  Created on: 14.03.2014
 *      Author: Erich Wenger
 */

#ifndef TEST_SER_H_
#define TEST_SER_H_

void read_eccp_affine_point( char *buffer,
                             const int buf_length,
                             eccp_point_affine_t *ec_point,
                             const gfp_prime_data_t *mont_data,
                             uint8_t montgomery_domain );
void read_gfp(
    char *buffer, const int buf_length, uint_t *big_int, const gfp_prime_data_t *prime_data, uint8_t montgomery_domain );
void read_elliptic_curve_parameters (char *buffer, const int buf_length, eccp_parameters_t *param);
unsigned test_ser();

#endif /* TEST_SER_H_ */
