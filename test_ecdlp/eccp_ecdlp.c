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

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "eccp/eccp.h"
#include "gfp/gfp.h"
#include "bi/bi.h"
#include "eccp_ecdlp.h"
#include "utils/rand.h"
#include "io/io_gen.h"
#include "rbtree.h"

#define NUM_SIMUL_INV 50
#define NUM_BRANCHES 128
//#define DISTINGUISHING_BITS 20
#define SIZE_FIFO 32
#define USE_NEGATION_MAP 1
#define LOOP_DETECTION_SIZE 20
#define DEBUG_INSERTION 0


eccp_ecdlp_triple branches[NUM_BRANCHES];
eccp_ecdlp_triple fifo[SIZE_FIFO];
uint32_t fifo_read_index = 0;
uint32_t fifo_write_index = 0;
uint32_t fifo_size = 0;
pthread_mutex_t fifo_lock;
volatile uint32_t finished_computing = 0;
pthread_mutex_t finished_computing_lock;
pthread_mutex_t num_iterations_lock;
long stats_num_iterations = 0;
long stats_num_distinguished_triples = 0;
long stats_num_discarded_triples = 0;
eccp_point_affine_t *globalP, *globalQ;
eccp_parameters_t *param = 0;
rbtree tree = NULL;

long stats_num_pollard_rho_tests = 0;
long stats_pollard_rho_tests_sum_iterations = 0;

pthread_mutex_t stats_loops_lock;
volatile int stats_loops[LOOP_DETECTION_SIZE];
volatile long stats_loops_sum = 0;
volatile int stats_loops_max = 0;

/**
 * This structure is used within the loop detection mechanism.
 */
typedef struct _eccp_ecdlp_triple_loop_detection_ {
    eccp_ecdlp_triple triple[LOOP_DETECTION_SIZE];
    int index;
    int size;
} eccp_ecdlp_triple_loop_detection;

eccp_ecdlp_triple computing[NUM_SIMUL_INV];
eccp_ecdlp_triple_loop_detection loop_detection[NUM_SIMUL_INV];
gfp_t to_invert[NUM_SIMUL_INV];
gfp_t to_invert_T1[NUM_SIMUL_INV];
gfp_t to_invert_T2[NUM_SIMUL_INV];

/**
 * Verifies that the equation X = cP + dQ holds.
 * @param totest
 * @return 1 if valid
 */
int pr_triple_is_valid(const eccp_ecdlp_triple *totest) {
    eccp_point_affine_t temp1, temp2, temp3;
    
    if(eccp_affine_point_is_valid(&totest->R, param) == 0) {
        printf("WARNING(pr_triple_is_valid): point is not valid\n");
        return 0;
    }
    
    eccp_jacobian_point_multiply_L2R_NAF(&temp1, globalP, totest->a, param);
    eccp_jacobian_point_multiply_L2R_NAF(&temp2, globalQ, totest->b, param);
    eccp_affine_point_add(&temp3, &temp1, &temp2, param);

    if (eccp_affine_point_compare(&temp3, &totest->R, param) != 0) {
        printf("WARNING(pr_triple_is_valid): not valid triple\n");
        return 0;
    }
    
    return 1;
}

/**
 * Print the triple
 */
void pr_triple_print(const eccp_ecdlp_triple *triple) {
    io_print_affine_point(&triple->R, param);
    io_print_bigint_var(triple->a, param->order_n_data.words);
    io_print_bigint_var(triple->b, param->order_n_data.words);
    io_print( "\n");
}

/**
 * Copy original triple to new triple.
 * @param dest
 * @param src
 */
void pr_triple_copy(eccp_ecdlp_triple *dest, const eccp_ecdlp_triple *src) {
    eccp_affine_point_copy(&dest->R, &src->R, param);
    bigint_copy_var(dest->a, src->a, param->order_n_data.words);
    bigint_copy_var(dest->b, src->b, param->order_n_data.words);
}

/**
 * Compares two triples.
 * @param dest
 * @param src
 */
int pr_triple_compare(eccp_ecdlp_triple *left, const eccp_ecdlp_triple *right) {
    return eccp_affine_point_compare(&left->R, &right->R, param);
}

/**
 * Double the Point
 */
void pr_triple_double(eccp_ecdlp_triple *triple) {
    eccp_affine_point_double(&triple->R, &triple->R, param);
    gfp_gen_add(triple->a, triple->a, triple->a, &param->order_n_data);
    gfp_gen_add(triple->b, triple->b, triple->b, &param->order_n_data);
}

/**
 * Add two triples
 */
void pr_triple_add(eccp_ecdlp_triple *res, const eccp_ecdlp_triple *opa, const eccp_ecdlp_triple *opb) {
    eccp_affine_point_add(&res->R, &opa->R, &opb->R, param);
    gfp_gen_add(res->a, opa->a, opb->a, &param->order_n_data);
    gfp_gen_add(res->b, opa->b, opb->b, &param->order_n_data);
}

/**
 * Update the triple with a branch
 * @param triple
 * @param index
 */
void pr_triple_add_branch_index(eccp_ecdlp_triple *triple, int index) {
    pr_triple_add(triple, triple, &branches[index]);
}

/**
 * Update the triple with a branch
 * @param triple
 * @return the chosen branch number
 */
int pr_triple_add_branch(eccp_ecdlp_triple *triple) {
    int j = triple->R.x[0] & (NUM_BRANCHES - 1) ;
    pr_triple_add_branch_index(triple, j);
    return j;
}

/**
 * Applies the negation map
 * @param triple
 * @return 1 if negation map was applied
 */
int pr_triple_negation_map(eccp_ecdlp_triple *triple) {
    gfp_t negative_y;
    gfp_gen_negate( negative_y, triple->R.y, &param->prime_data );

    if (bigint_compare_var(negative_y, triple->R.y, param->prime_data.words) < 0) {
//        if (bigint_get_msb(negative_y) < bigint_get_msb(smallest_point->y)) {
        bigint_copy_var(triple->R.y, negative_y, param->prime_data.words);
        gfp_gen_negate(triple->a, triple->a, &param->order_n_data);
        gfp_gen_negate(triple->b, triple->b, &param->order_n_data);
        return 1;
    }
    return 0;
}

/**
 * Compute a new set of triples that can be used for the computation.
 * @param to_comp
 */
void pr_triple_generate(eccp_ecdlp_triple *to_comp) {
    eccp_point_affine_t temp1, temp2;

    gfp_rand(to_comp->a, &param->order_n_data);
    gfp_rand(to_comp->b, &param->order_n_data);
    // R = a P + b Q
    eccp_jacobian_point_multiply_L2R_NAF(&temp1, globalP, to_comp->a, param);
    eccp_jacobian_point_multiply_L2R_NAF(&temp2, globalQ, to_comp->b, param);
    eccp_affine_point_add(&to_comp->R, &temp1, &temp2, param);

#if (USE_NEGATION_MAP == 1)
    pr_triple_negation_map(to_comp);
#endif
}

/**
 * Prints the current loop statistics (does not lock mutex)
 */
void print_loop_stats() {
    int j;
    printf("loop detected %u %ld %ld --", stats_loops_max, stats_num_iterations, stats_num_distinguished_triples);
    for(j=0; j <= stats_loops_max; j++) {
        printf(" %u", stats_loops[j]);
    }
    printf("-- ");
    double r = (double)stats_num_iterations/(double)stats_loops[1];
    printf("%.1f ", r);

    for(j=2; j <= stats_loops_max; j++) {
        double r = (double)stats_loops[j-2]/(double)stats_loops[j];
        printf("%.1f ", r);
    }
    printf("\n");
}

/**
 * Displays a warning when a loop is detected.
 * @param triple
 * @param loop_detection 
 * @return 1 if loop was detected
 */
int pr_triple_loop_detection(eccp_ecdlp_triple *triple, eccp_ecdlp_triple_loop_detection *loop_detection) {
    int i,index;
    int loop_detected = 0;
    int iterations = 0;
    
    for(i=loop_detection->size-1; i >= 0; i--) {
        index = (loop_detection->index + 2*LOOP_DETECTION_SIZE - i - 1) % LOOP_DETECTION_SIZE;
/*
        if(bigint_compare(loop_detection->triple[index].X.x, triple->X.x) == 0) {
            if(bigint_compare(loop_detection->triple[index].X.y, triple->X.y) != 0) {
                debug_print("WARNING: y does not match\n");
            }
*/
        if(eccp_affine_point_compare(&loop_detection->triple[index].R, &triple->R, param) == 0) {
            if(bigint_compare_var(loop_detection->triple[index].a, triple->a, param->order_n_data.words) != 0) {
                if(bigint_compare_var(loop_detection->triple[index].b, triple->b, param->order_n_data.words) != 0) {
                    printf("WARNING: loop falsely detected ... solution found\n");
                } else {
                    printf("WARNING: loop falsely detected\n");
                }
            }
                
            pthread_mutex_lock(&stats_loops_lock);
            stats_loops[i]++;
            stats_loops_sum++;
            if(i > stats_loops_max)
                stats_loops_max = i;
            if(i > 7 && i == stats_loops_max) {
                printf("WARNING: ");
                print_loop_stats();
            }
            pthread_mutex_unlock(&stats_loops_lock);
            loop_detected = i;
            break;
        }
    }
    
    pr_triple_copy(&loop_detection->triple[loop_detection->index], triple);

    if(loop_detection->size < LOOP_DETECTION_SIZE)
        loop_detection->size++;
    loop_detection->index = (loop_detection->index + 1) % LOOP_DETECTION_SIZE;
    
    if(loop_detected != 0) {
#if 1
        int j = triple->R.x[0] & (NUM_BRANCHES - 1);
        if((i + 1) % NUM_BRANCHES == 0) {
            printf("WARNING(loop): cheating\n");
            pr_triple_generate(triple);
        } else {
            pr_triple_add_branch_index(triple, (j + i + 1) % NUM_BRANCHES);
        }
//        j = rand() % NUM_BRANCHES;
//        pr_triple_add_branch_index(triple, j);
#if (USE_NEGATION_MAP == 1)
        pr_triple_negation_map(triple);
#endif
#else
//        pr_triple_generate(triple);
        pr_triple_double(triple);
#endif
        pr_triple_send_if_distinguished(triple);
        iterations += pr_triple_loop_detection(triple, loop_detection) + 1;
    }
    
    return iterations;
}

/**
 * Returns 1 if point is distinguished
 * @param triple
 * @return 1 if distinguished
 */
int pr_triple_is_distinguished(const eccp_ecdlp_triple *triple) {
    int DISTINGUISHING_BITS = param->order_n_data.bits / 4;
    if (bigint_get_msb_var(triple->R.x, param->prime_data.words) < (param->prime_data.bits - DISTINGUISHING_BITS)) {
        return 1;
    }
    return 0;
}

/**
 * Send the triple to the master thread if point is distinguished
 * @param triple
 */
void pr_triple_send_if_distinguished(const eccp_ecdlp_triple *triple) {
    if (pr_triple_is_distinguished(triple) == 1) {
        pr_add_tree(triple, NULL);
    }
}

/**
 * Add the given triple to the data structure, check for duplicates, quit all processing.
 * @param to_insert
 * @param scalar if a collision is found, the scalar is computed
 */
void pr_add_tree(const eccp_ecdlp_triple *to_insert, gfp_t scalar) {
    if (to_insert->R.identity == 1) {
        printf("WARNING(pr_add_tree): Point is identity\n");
        return;
    }
    if (!eccp_affine_point_is_valid(&to_insert->R, param)) {
        printf("WARNING(pr_add_tree): Point is not valid\n");
        return;
    }
    if (!pr_triple_is_valid(to_insert)) {
        printf("WARNING(pr_add_tree): Triple is not valid\n");
        return;
    }

    stats_num_distinguished_triples++;
    
    pr_triple_print(to_insert);
    
#if (DEBUG_INSERTION == 1)
    pthread_mutex_lock(&num_iterations_lock);
    if(stats_num_distinguished_triples % 100 == 0) {
        printf("INFO(inserted %ld %ld)\n", stats_num_distinguished_triples, stats_num_iterations);
    }
    pthread_mutex_unlock(&num_iterations_lock);
#endif
}

/**
 * The client, attack thread.
 * @param _to_comp_
 * @return 
 */
void *pr_attack_thread(void *_to_comp_) {
    long local_iterations = 0;
    eccp_ecdlp_triple_loop_detection loop_detection;
    eccp_ecdlp_triple *to_comp = (eccp_ecdlp_triple*) _to_comp_;

    loop_detection.index = 0;
    loop_detection.size = 0;
    
    while (1 == 1) {
        local_iterations += pr_triple_loop_detection(to_comp, &loop_detection) + 1;
        pr_triple_send_if_distinguished(to_comp);
        pr_triple_add_branch(to_comp);
#if (USE_NEGATION_MAP == 1)
        pr_triple_negation_map(to_comp);
#endif

        if(stats_num_distinguished_triples % 100 == 0) {
//            printf("INFO(inserted %ld %ld)\n", stats_num_distinguished_triples, local_iterations);
        }
    }
}

void attack() {
    int j, index;
    eccp_ecdlp_triple *triple;
    eccp_ecdlp_triple *branch;
    gfp_t temp;
    gfp_t lambda, temp1, temp2;
    
    while(1 == 1) {
        // 1. collect to invert
        for(j = 0; j < NUM_SIMUL_INV; j++) {
            triple = &computing[j];
            index = triple->R.x[0] & (NUM_BRANCHES - 1);
            branch = &branches[index];
            gfp_subtract( to_invert[j], branch->R.x, triple->R.x );
        }    
        // 2. do inverse
        gfp_copy(to_invert_T1[0], to_invert[0] );
        for(j = 1; j < NUM_SIMUL_INV; j++) {
            gfp_multiply( to_invert_T1[j], to_invert_T1[j-1], to_invert[j]);
        }
        gfp_inverse(temp, to_invert_T1[NUM_SIMUL_INV-1]);
        gfp_multiply(to_invert_T2[NUM_SIMUL_INV-1], temp, to_invert_T1[NUM_SIMUL_INV-2]);
        for(j = NUM_SIMUL_INV-2; j >= 1; j--) {
            gfp_multiply( temp, temp, to_invert[j+1]);
            gfp_multiply( to_invert_T2[j], temp, to_invert_T1[j-1]);
        }
        gfp_multiply(to_invert_T2[0], temp, to_invert[1]);

#if 0
        for(j = 0; j < NUM_SIMUL_INV; j++) {
            gfp_multiply(temp, to_invert_T2[j], to_invert[j]);
            if(!bigint_is_equal_var(temp, param->prime_data.gfp_one, param->prime_data.words)) {
                printf("error\n");
            } 
        }
#endif
        
        // 3. point addition, etc...
        for(j = 0; j < NUM_SIMUL_INV; j++) {
            triple = &computing[j];
            index = triple->R.x[0] & (NUM_BRANCHES - 1);
            branch = &branches[index];
            // eccp_affine_point_add(&res->R, &opa->R, &opb->R, param);
            gfp_subtract( temp2, branch->R.y, triple->R.y );
            gfp_multiply( lambda, to_invert_T2[j], temp2 ); // (y2-y1) / (x2-x1)
            gfp_square( temp1, lambda );
            gfp_subtract( temp1, temp1, triple->R.x );
            gfp_subtract( temp1, temp1, branch->R.x ); // L^2 - x1 - x2
            gfp_subtract( temp2, triple->R.x, temp1 ); // (x1 - x3)
            gfp_copy( triple->R.x, temp1 );
            gfp_multiply( temp1, temp2, lambda );
            gfp_subtract( triple->R.y, temp1, triple->R.y ); // L*(x1-x3)-y1
            
            gfp_gen_add(triple->a, triple->a, branch->a, &param->order_n_data);
            gfp_gen_add(triple->b, triple->b, branch->b, &param->order_n_data);

            pr_triple_send_if_distinguished(triple);
        }    
    }
}

#define NUM_ITERATIONS 1000
extern unsigned long perf_get_cycle_counter();

void performance_print_statistics_(unsigned long *runtime) {
    unsigned long sum, min, max, templ;
    int run_number;
    double temp;
    double average, stddev;

    sum = max = min = runtime[0];
    for(run_number = 1; run_number < NUM_ITERATIONS; run_number++) {
        templ = runtime[run_number];
        sum += templ;
        if(max < templ)
            max = templ;
        if(min > templ)
            min = templ;
    }
    average = (double)sum/(double)NUM_ITERATIONS;

    stddev = 0.0;
    for(run_number = 0; run_number < NUM_ITERATIONS; run_number++) {
        temp = (double)runtime[run_number] - average;
        stddev += temp * temp;
    }
    stddev /= (double)NUM_ITERATIONS;
    stddev = sqrt(stddev);
    
    printf("min(%lu) avg(%.2f) max(%lu) stddev(%.2f %.2f%%)\n", min, average, max, stddev, stddev/average*100.0);
}


void performance_test_eccp_mul_(eccp_ecdlp_triple *computing) {
    unsigned long runtime[NUM_ITERATIONS];
    int run_number;
    unsigned long start_time, stop_time;
    eccp_ecdlp_triple_loop_detection loop_detection;
    loop_detection.index = 0;
    loop_detection.size = 0;
    
    for(run_number = 0; run_number < 10000; run_number++) {
        pr_triple_add_branch(computing);
    }

    for(run_number = 0; run_number < NUM_ITERATIONS; run_number++) {
        start_time = perf_get_cycle_counter();
#if 1
        pr_triple_loop_detection(computing, &loop_detection);
        pr_triple_send_if_distinguished(computing);
        pr_triple_add_branch(computing);
        pr_triple_negation_map(computing);
#else
        pr_triple_add_branch(computing);
#endif
        stop_time = perf_get_cycle_counter();
        runtime[run_number] = stop_time - start_time;
    }
#if 0
    for(run_number = 0; run_number < NUM_ITERATIONS; run_number++) {
        printf("%lu\n", runtime[run_number]);
    }
#endif
    performance_print_statistics_(runtime);
}

/**
 * Master thread that performs the pollard rho attack
 * @param scalar the resulting scalar such that Q = scalar*P
 * @param P
 * @param Q
 * @param param param elliptic curve parameters
 */
void pr_ecdlp_pollard_rho(gfp_t scalar, const eccp_point_affine_t *P, const eccp_point_affine_t *Q, const eccp_parameters_t *local_param) {
    int j;

    srand(time(NULL));
    globalP = (eccp_point_affine_t*) P;
    globalQ = (eccp_point_affine_t*) Q;
    param = (eccp_parameters_t*) local_param;

    // Prepare all branches
    for (j = 0; j < NUM_BRANCHES; j++) {
        pr_triple_generate(&branches[j]);
    }

    /* Initialize and set thread detached attribute */
    finished_computing = 0;
    stats_num_distinguished_triples = 0;
    stats_num_iterations = 0;
    stats_num_discarded_triples = 0;
    fifo_size = 0;
    fifo_read_index = 0;
    fifo_write_index = 0;
    stats_loops_max = 0;
    stats_loops_sum = 0;
    for(j = 0; j < LOOP_DETECTION_SIZE; j++) {
        stats_loops[j] = 0;
    }
    tree = rbtree_create();
    for(j = 0; j < NUM_SIMUL_INV; j++) {
        loop_detection[j].size = 0;
        loop_detection[j].index = 0;
    }
    
    srand1(time(NULL));
    
    for(j = 0; j < NUM_SIMUL_INV; j++) {
        pr_triple_generate(&computing[j]);
    }
    
    performance_test_eccp_mul_(&computing[0]);

    attack();
//    pr_attack_thread(&computing[0]);
}

