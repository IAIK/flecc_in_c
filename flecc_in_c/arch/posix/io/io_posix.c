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

#include <stdio.h>
#include "io_posix.h"

FILE *default_output_stream = NULL;
FILE *default_input_stream = NULL;

/**
 * Initializes the POSIX-compatible IO and hereby sets
 * the default input and output streams.
 * @param default_in the input stream to be set to default
 * @param default_out the output stream to be set to default
 */
void io_posix_init( FILE *default_in, FILE *default_out ) {
    default_input_stream = default_in;
    default_output_stream = default_out;
}

/**
 * Reads a byte from the default input stream.
 * @return the byte read
 */
uint8_t io_posix_read_byte() {
    return fgetc( default_input_stream );
}

/**
 * Writes a byte / character to the default output stream.
 * @param byte the byte / character that shall be written
 */
void io_posix_write_byte( uint8_t byte ) {
    fputc( byte, default_output_stream );
}
