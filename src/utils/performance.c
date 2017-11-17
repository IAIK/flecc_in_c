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

#include "flecc_in_c/bi/bi_gen.h"
#include "flecc_in_c/eccp/eccp.h"
#include "flecc_in_c/gfp/gfp.h"
#include "flecc_in_c/utils/rand.h"
#include <math.h>
#include <stdio.h>
#include <time.h>

#define NUM_ITERATIONS 1000

void performance_print_statistics( const clock_t *runtime ) {
    clock_t sum, min, max, templ;
    int run_number;
    double temp;
    double average, stddev;

    sum = max = min = runtime[0];
    for( run_number = 1; run_number < NUM_ITERATIONS; run_number++ ) {
        templ = runtime[run_number];
        sum += templ;
        if( max < templ ) {
            max = templ;
        }
        if( min > templ ) {
            min = templ;
        }
    }
    average = (double)sum / (double)NUM_ITERATIONS;

    stddev = 0.0;
    for( run_number = 0; run_number < NUM_ITERATIONS; run_number++ ) {
        temp = (double)runtime[run_number] - average;
        stddev += temp * temp;
    }
    stddev /= (double)NUM_ITERATIONS;
    stddev = sqrt( stddev );

    printf( "min(%lu) avg(%.2f) max(%lu) stddev(%.2f %.2f%%)\n", min, average, max, stddev, stddev / average * 100.0 );
}

/**
 * checks certain operations for their performance
 */
void performance_test_eccp_mul( eccp_parameters_t *param ) {
    eccp_point_affine_t point;
    gfp_t scalar;
    clock_t runtime[NUM_ITERATIONS];
    int run_number;
    clock_t start_time, stop_time;
    eccp_affine_point_copy( &point, &param->base_point, param );

    for( run_number = 0; run_number < NUM_ITERATIONS; run_number++ ) {
        do {
            gfp_rand( scalar, &param->order_n_data );
            bigint_set_bit_var( scalar, param->order_n_data.bits - 1, 1, param->order_n_data.words );
        } while( bigint_compare_var( scalar, param->order_n_data.prime, param->order_n_data.words ) >= 0 );
        start_time = clock();
        param->eccp_mul( &point, &point, scalar, param );
        //        param->eccp_mul_base_point(&point, scalar, param);
        stop_time = clock();
        runtime[run_number] = stop_time - start_time;
    }
    performance_print_statistics( runtime );
}

/**
 * checks certain operations for their performance
 */
void performance_test_gfp_mul( eccp_parameters_t *param ) {
    gfp_t var1, var2;
    clock_t runtime[NUM_ITERATIONS];
    int run_number;
    clock_t start_time, stop_time;

    gfp_rand( var1, &param->prime_data );
    gfp_rand( var2, &param->prime_data );

    bigint_set_bit_var( var1, param->prime_data.bits - 2, 1, param->prime_data.words );
    bigint_set_bit_var( var2, param->prime_data.bits - 2, 1, param->prime_data.words );

    for( run_number = 0; run_number < NUM_ITERATIONS; run_number++ ) {
        start_time = clock();
        gfp_multiply( var2, var2, var1 );
        stop_time = clock();
        runtime[run_number] = stop_time - start_time;
    }
    performance_print_statistics( runtime );
}
