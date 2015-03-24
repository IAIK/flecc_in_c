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

#include "bi_const_runtime.h"
#include "bi_gen.h"

/**
 * Accesses all elements in table and moves one entry to the destination memory.
 * @param result the destination buffer
 * @param table pointer to two-dimensional array
 * @param index the index in the table to access
 * @param tbl_entries the number of entries in the table
 * @param words_per_entry the number of words per table entry
 * @param words_result the number of words copied to result
 */
void bigint_cr_tbl_access(uint_t *result, const uint_t *table, const int index, 
        const int tbl_entries, const int words_per_entry, const int words_result) {
    int i, j, k, right_entry, index2;
    uint_t mask;
    uint_t *table_ptr = (uint_t *)table;
    
    bigint_clear_var(result, words_result);
    
    index2 = -index;
    for(i = 0; i < tbl_entries; i++) {
        right_entry = index2;
        j = 1;
        while(j != (sizeof(int) * 8)) {
            right_entry |= (right_entry >> j);
            j <<= 1;
        }
        right_entry &= 1;
        mask = -right_entry;
        mask = ~mask;
        
        for(k = 0; k < words_result; k++) {
            result[k] |= table_ptr[k] & mask;
        }
        table_ptr += words_per_entry;
        index2++;
    }
}

/**
 * Switch two variables in constant time and with constant access pattern.
 * @param var1
 * @param var2
 * @param switch_vars if 1, variables are switched; if 0, no switch is performed
 * @param words number of words to represent the two variables
 */
void bigint_cr_switch(uint_t *var1, uint_t *var2, const int switch_vars, const int words) {
    uint_t M1, M2;
    uint_t V1, V2, V3, V4;
    int i;
    
    M2 = -switch_vars;
    M1 = ~M2;
    
    for(i = 0; i < words; i++) {
        V1 = var1[i];
        V2 = var2[i];
        V3 = (V1 & M1) | (V2 & M2);
        V4 = (V1 & M2) | (V2 & M1);
        var1[i] = V3;
        var2[i] = V4;
    }
}

