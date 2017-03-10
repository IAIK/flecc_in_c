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

#ifndef SHA1_H_
#define SHA1_H_

#include "../types.h"

/** The state of SHA-1. */
typedef struct {
    uint32_t H0;
    uint32_t H1;
    uint32_t H2;
    uint32_t H3;
    uint32_t H4;
} hash_sha1_t;

void hash_sha1_init( hash_sha1_t *state );
void hash_sha1_update( hash_sha1_t *state, const uint8_t *message );
void hash_sha1_final( hash_sha1_t *state, const uint8_t *message, const int remaining_length, const int total_length );

void hash_sha1_to_byte_array( uint8_t *hash, const hash_sha1_t *state );

#endif /* SHA1_H_ */
