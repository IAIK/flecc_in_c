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

#include "eccp/eccp.h"
#include "gfp/gfp.h"
#include "bi/bi.h"
#include "eccp_ecdlp.h"
#include "utils/rand.h"
#include "io/io_gen.h"

#define NUM_BRANCHES 128
#define NUM_THREADS 4
#define DISTINGUISHING_BITS 6
#define SIZE_FIFO 32
#define MAX_DISTINGUISHED_POINTS 1000000
#define USE_NEGATION_MAP 1
#define LOOP_DETECTION_SIZE 100
#define DEBUG_INSERTION 0


eccp_ecdlp_triple branches[NUM_BRANCHES];
eccp_ecdlp_triple computing[NUM_THREADS];
eccp_ecdlp_triple fifo[SIZE_FIFO];
uint32_t fifo_read_index = 0;
uint32_t fifo_write_index = 0;
uint32_t fifo_size = 0;
pthread_mutex_t fifo_lock;
pthread_t threads[NUM_THREADS];
volatile uint32_t finished_computing = 0;
pthread_mutex_t finished_computing_lock;
pthread_mutex_t num_iterations_lock;
long stats_num_iterations = 0;
eccp_ecdlp_triple* distinguished_triples[MAX_DISTINGUISHED_POINTS];
long stats_num_distinguished_triples = 0;
long stats_num_discarded_triples = 0;
eccp_point_affine_t *globalP, *globalQ;
eccp_parameters_t *param = 0;

long stats_num_pollard_rho_tests = 0;
long stats_pollard_rho_tests_sum_iterations = 0;
long stats_num_branch_additions = 0;

volatile int stats_loops[LOOP_DETECTION_SIZE];
volatile int stats_loops_max = 0;

/**
 * This structure is used within the loop detection mechanism.
 */
typedef struct _eccp_ecdlp_triple_loop_detection_ {
    eccp_ecdlp_triple triple[LOOP_DETECTION_SIZE];
    int index;
    int size;
} eccp_ecdlp_triple_loop_detection;


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
    stats_num_branch_additions++;
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
 * Displays a warning when a loop is detected.
 * @param triple
 * @param loop_detection 
 * @return 1 if loop was detected
 */
int pr_triple_loop_detection(eccp_ecdlp_triple *triple, eccp_ecdlp_triple_loop_detection *loop_detection) {
    int i,j,index;
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
#if (DEBUG_INSERTION == 1)
            printf("WARNING: loop detected\n");
#endif
            if(bigint_compare_var(loop_detection->triple[index].a, triple->a, param->order_n_data.words) != 0) {
                if(bigint_compare_var(loop_detection->triple[index].b, triple->b, param->order_n_data.words) != 0) {
                    printf("WARNING: loop falsely detected ... solution found\n");
//                    pr_send_to_master(&loop_detection->triple[index]);
//                    pr_send_to_master(triple);
                } else {
                    printf("WARNING: loop falsely detected\n");
                }
            }
                
            stats_loops[i]++;
            if(i > stats_loops_max)
                stats_loops_max = i;
            if(i > 7) {
                printf("WARNING: loop detected %u %ld %ld", stats_loops_max, stats_num_branch_additions, stats_num_distinguished_triples);
                for(j=0; j <= stats_loops_max; j++) {
                    printf(" %u", stats_loops[j]);
                }
                printf("-- ");
                double r = (double)stats_num_branch_additions/(double)stats_loops[1];
                printf("%.1f ", r);
                    
                for(j=2; j <= stats_loops_max; j++) {
                    double r = (double)stats_loops[j-2]/(double)stats_loops[j];
                    printf("%.1f ", r);
                }
                printf("\n");
            }
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
        pr_triple_add_branch_index(triple, (j + i + 1) % NUM_BRANCHES);
//        j = rand() % NUM_BRANCHES;
//        pr_triple_add_branch_index(triple, j);
#if (USE_NEGATION_MAP == 1)
        pr_triple_negation_map(triple);
#endif
#else
        pr_triple_generate(triple);
#endif
        pr_triple_send_if_distinguished(triple);
        iterations += pr_triple_loop_detection(triple, loop_detection) + 1;
    }
    
    return iterations;
}

/**
 * Send the triple to the master thread if point is distinguished
 * @param triple
 */
void pr_triple_send_if_distinguished(const eccp_ecdlp_triple *triple) {
    if (bigint_get_msb_var(triple->R.x, param->prime_data.words) < (param->prime_data.bits - DISTINGUISHING_BITS)) {
        pr_send_to_master(triple);
    }
}

/**
 * Send the given triple to the master thread.
 * @param comp
 */
void pr_send_to_master(const eccp_ecdlp_triple *comp) {
    pthread_mutex_lock(&fifo_lock);
    while (fifo_size >= SIZE_FIFO) {
        pthread_mutex_unlock(&fifo_lock);
        usleep(1000);
        if (pthread_mutex_trylock(&finished_computing_lock) == 0) {
            pthread_mutex_unlock(&finished_computing_lock);
            return;
        }
        pthread_mutex_lock(&fifo_lock);
    }

    pr_triple_copy(&fifo[fifo_write_index], comp);

    fifo_write_index = (fifo_write_index + 1) % SIZE_FIFO;
    fifo_size = fifo_size + 1;
    pthread_mutex_unlock(&fifo_lock);
}

/**
 * Called by master thread to receive the latest triple from the fifo.
 * @param computed
 */
void pr_get_latest_point(eccp_ecdlp_triple *computed) {
    pthread_mutex_lock(&fifo_lock);
    while (fifo_size == 0) {
        pthread_mutex_unlock(&fifo_lock);
        usleep(100000);
        pthread_mutex_lock(&fifo_lock);
    }

    pr_triple_copy(computed, &fifo[fifo_read_index]);

    fifo_read_index = (fifo_read_index + 1) % SIZE_FIFO;
    fifo_size = fifo_size - 1;
    pthread_mutex_unlock(&fifo_lock);
}

/**
 * Add the given triple to the data structure, check for duplicates, quit all processing.
 * @param to_insert
 * @param scalar if a collision is found, the scalar is computed
 */
void pr_add_tree(eccp_ecdlp_triple *to_insert, gfp_t scalar) {
    int j = 0;
    gfp_t temp1, temp2;

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

    // check for doubles 
    for (j = 0; j < stats_num_distinguished_triples; j++) {
        if (eccp_affine_point_compare(&to_insert->R, &distinguished_triples[j]->R, param) == 0) {
            if (bigint_compare_var(to_insert->a, distinguished_triples[j]->a, param->order_n_data.words) == 0) {
                // same point already in database --> discard it
#if (DEBUG_INSERTION == 1)
                printf("WARNING(pr_add_tree): discard some triple\n");
#endif
                stats_num_discarded_triples++;
                return;
            }
#if (DEBUG_INSERTION == 1)
            printf("INFO(pr_add_tree): found solution\n");
#endif
            gfp_gen_subtract(temp1, to_insert->b, distinguished_triples[j]->b, &param->order_n_data);
            gfp_binary_euclidean_inverse(temp2, temp1, &param->order_n_data);
            gfp_gen_subtract(temp1, distinguished_triples[j]->a, to_insert->a, &param->order_n_data);
            gfp_mult_two_mont(scalar, temp1, temp2, &param->order_n_data);

            finished_computing = 1;
            pthread_mutex_unlock(&finished_computing_lock);
            break;
        }
    }
#if (DEBUG_INSERTION == 1)
    pthread_mutex_lock(&num_iterations_lock);
    printf("inserted %ld %ld %ld\n", stats_num_distinguished_triples, stats_num_iterations, stats_num_branch_additions);
    pthread_mutex_unlock(&num_iterations_lock);
#endif
    distinguished_triples[stats_num_distinguished_triples++] = to_insert;
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
    loop_detection.size = LOOP_DETECTION_SIZE;
    
    while (pthread_mutex_trylock(&finished_computing_lock) != 0) {
        local_iterations += pr_triple_loop_detection(to_comp, &loop_detection) + 1;
        pr_triple_send_if_distinguished(to_comp);
        pr_triple_add_branch(to_comp);
#if (USE_NEGATION_MAP == 1)
        pr_triple_negation_map(to_comp);
#endif
    }

    pthread_mutex_unlock(&finished_computing_lock);
    pthread_mutex_lock(&num_iterations_lock);
    stats_num_iterations += local_iterations;
    pthread_mutex_unlock(&num_iterations_lock);
    pthread_exit(NULL);
}

/**
 * Master thread that performs the pollard rho attack
 * @param scalar the resulting scalar such that Q = scalar*P
 * @param P
 * @param Q
 * @param param param elliptic curve parameters
 */
void pr_ecdlp_pollard_rho(gfp_t scalar, const eccp_point_affine_t *P, const eccp_point_affine_t *Q, const eccp_parameters_t *local_param) {
    eccp_point_affine_t temp;
    eccp_ecdlp_triple *latest;
    int j;

    srand(time(NULL));
    globalP = (eccp_point_affine_t*) P;
    globalQ = (eccp_point_affine_t*) Q;
    param = (eccp_parameters_t*) local_param;

    if (pthread_mutex_init(&fifo_lock, NULL) != 0) {
        printf("pthread_mutex_init failed\n");
        return;
    }
    if (pthread_mutex_init(&num_iterations_lock, NULL) != 0) {
        printf("pthread_mutex_init failed\n");
        return;
    }
    if (pthread_mutex_init(&finished_computing_lock, NULL) != 0) {
        printf("pthread_mutex_init failed\n");
        return;
    }
    // this mutex is only unlocked when we want the client threads to finish
    pthread_mutex_lock(&finished_computing_lock);
    
    // Prepare all branches
    for (j = 0; j < NUM_BRANCHES; j++) {
        pr_triple_generate(&branches[j]);
    }

    /* Initialize and set thread detached attribute */
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    finished_computing = 0;
    stats_num_distinguished_triples = 0;
    stats_num_iterations = 0;
    stats_num_discarded_triples = 0;
    fifo_size = 0;
    fifo_read_index = 0;
    fifo_write_index = 0;
    stats_num_branch_additions = 0;
    stats_loops_max = 0;
    for(j = 0; j < LOOP_DETECTION_SIZE; j++) {
        stats_loops[j] = 0;
    }
    
    for (j = 0; j < NUM_THREADS; j++) {
        pr_triple_generate(&computing[j]);

        // start the thread ...
        if (pthread_create(&threads[j], &attr, &pr_attack_thread, (void *) &computing[j]) != 0) {
            printf("pthread_create failed\n");
        }
    }

    while (finished_computing == 0) {
        latest = malloc(sizeof (eccp_ecdlp_triple));
//        pr_triple_generate(latest);
        pr_get_latest_point(latest);
        pr_add_tree(latest, scalar);
    }

    // do some cleaning up
    pthread_attr_destroy(&attr);
    for (j = 0; j < NUM_THREADS; j++) {
        pthread_join(threads[j], NULL);
    }
    pthread_mutex_destroy(&fifo_lock);
    pthread_mutex_destroy(&num_iterations_lock);
    pthread_mutex_destroy(&finished_computing_lock);
    for (j = 0; j < stats_num_distinguished_triples; j++) {
        free(distinguished_triples[j]);
    }
    
    // verify result
    eccp_jacobian_point_multiply_L2R_NAF(&temp, P, scalar, param);
    if(eccp_affine_point_compare(&temp, Q, param) != 0) {
        printf("ERROR(pr_ecdlp_pollard_rho): resulting scalar verification failed\n");
    }

    printf("%ld iterations, %ld points, %ld discarded\n", stats_num_iterations, stats_num_distinguished_triples, stats_num_discarded_triples);
    stats_pollard_rho_tests_sum_iterations += stats_num_iterations;
    stats_num_pollard_rho_tests++;
    if(stats_num_pollard_rho_tests > 0) {
        printf("average: %ld, %ld iterations\n", stats_pollard_rho_tests_sum_iterations / stats_num_pollard_rho_tests, stats_num_pollard_rho_tests);
    }
}

