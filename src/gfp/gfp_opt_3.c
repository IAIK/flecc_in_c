
#include "types.h"

uint_t *prime;
gfp_prime_data_t *prime_data;

void gfp_opt_3_init(gfp_prime_data_t *prime_data_local) {
    prime_data = prime_data_local;
    prime = prime_data_local->prime;
}

inline void bigint_clear( uint_t *dest ) {
    dest[0] = 0;
    dest[1] = 0;
    dest[2] = 0;
}

inline void bigint_copy( uint_t *dest, const uint_t *source ) {
    dest[0] = source[0];
    dest[1] = source[1];
    dest[2] = source[2];
}

inline int bigint_add( uint_t *res, const uint_t *a, const uint_t *b ) {
    ulong_t temp;
    uint_t a0 = a[0], a1 = a[1], a2 = a[2];
    uint_t b0 = b[0], b1 = b[1], b2 = b[2];
    
    temp = a0 + b0;
    res[0] = temp;
    temp = a1 + b1 + (temp >> BITS_PER_WORD);
    res[1] = temp;
    temp = a2 + b2 + (temp >> BITS_PER_WORD);
    res[2] = temp;
    return temp >> BITS_PER_WORD;
}

inline int bigint_subtract( uint_t *res, const uint_t *a, const uint_t *b ) {
    slong_t temp;
    uint_t a0 = a[0], a1 = a[1], a2 = a[2];
    uint_t b0 = b[0], b1 = b[1], b2 = b[2];
    
    temp = a0 - b0;
    res[0] = temp;
    temp = a1 - b1 + (temp >> BITS_PER_WORD);
    res[1] = temp;
    temp = a2 - b2 + (temp >> BITS_PER_WORD);
    res[2] = temp;
    return temp >> BITS_PER_WORD;
}

inline int bigint_compare( const uint_t *a, const uint_t *b ) {
    slong_t temp;
    
    temp = a[2] - b[2];
    if(temp != 0)
        return temp;
    temp = a[1] - b[1];
    if(temp != 0)
        return temp;
    temp = a[0] - b[0];
    return temp;
}

inline int bigint_is_zero( const uint_t *a ) {
    if( a[2] != 0)
        return 0;
    if( a[1] != 0)
        return 0;
    if( a[0] != 0)
        return 0;
    return 1;
}

inline void bigint_set_bit( uint_t *a, const int bit, const int value ) {
    int iWord, iBit;
    uint_t word;

    iWord = bit >> LD_BITS_PER_WORD;
    iBit = bit & ( BITS_PER_WORD - 1 );

    word = a[iWord];
    word &= ~( 1 << iBit );
    word |= value << iBit;
    a[iWord] = word;
}


/**
 * Shifts a biginteger to the right by one bit
 * @param a the data to shift
 * @param res destination big integer
 * @param length word count which should be shifted
 */
void bigint_shift_right_one( uint_t *res, const uint_t *a ) {
    uint_t a0 = a[0], a1 = a[1], a2 = a[2];

    res[2] = a2 >> 1;
    res[1] = (a2 << (BITS_PER_WORD - 1)) | (a1 >> 1);
    res[0] = (a1 << (BITS_PER_WORD - 1)) | (a0 >> 1);
}


void gfp_opt_3_add( gfp_t res, const gfp_t a, const gfp_t b ) {
    int carry = bigint_add( res, a, b );
    if( carry ) {
        bigint_subtract( res, res, prime );
    } else {
        if( bigint_compare( res, prime_data->prime ) >= 0 ) {
            bigint_subtract( res, res, prime_data->prime );
        }
    }
}

void gfp_opt_3_subtract( gfp_t res, const gfp_t a, const gfp_t b ) {
    int carry = bigint_subtract( res, a, b );
    if( carry ) {
        bigint_add( res, res, prime );
    }
}

/**
 * Montgomery multiplication based on Separated Operand Scanning (SOS) method
 * Koc, ACar, Kaliski "Analyzing and Comparing Montgomery Multiplication
 * Algorithms" IN CONSTANT TIME
 * @param res the result = a * b * R^-1 mod prime
 * @param a first operand
 * @param b second operand
 * @param prime_data the used prime data needed to do the multiplication
 */
void gfp_opt_3_multiply( gfp_t res, const gfp_t a, const gfp_t b ) {
    
    uint_t a0 = a[0], a1 = a[1], a2 = a[2];
    uint_t b0 = b[0], b1 = b[1], b2 = b[2];
    uint_t p0 = prime[0], p1 = prime[1], p2 = prime[2];
    uint_t c0, c1, c2, c3, c4, c5;
    uint_t temp;
    ulong_t product;
    uint_t global_carry;
    
    product = (ulong_t)a0 * (ulong_t)b0;
    c0 = product; product >>= BITS_PER_WORD;
    product += (ulong_t)a0 * (ulong_t)b1;
    c1 = product; product >>= BITS_PER_WORD;
    product += (ulong_t)a0 * (ulong_t)b2;
    c2 = product; product >>= BITS_PER_WORD;
    c3 = product;
    
    product = (ulong_t)a1 * (ulong_t)b0 + c1;
    c1 = product; product >>= BITS_PER_WORD;
    product += (ulong_t)a1 * (ulong_t)b1 + c2;
    c2 = product; product >>= BITS_PER_WORD;
    product += (ulong_t)a1 * (ulong_t)b2 + c3;
    c3 = product; product >>= BITS_PER_WORD;
    c4 = product;
    
    product = (ulong_t)a2 * (ulong_t)b0 + c2;
    c2 = product; product >>= BITS_PER_WORD;
    product += (ulong_t)a2 * (ulong_t)b1 + c3;
    c3 = product; product >>= BITS_PER_WORD;
    product += (ulong_t)a2 * (ulong_t)b2 + c4;
    c4 = product; product >>= BITS_PER_WORD;
    c5 = product;
    
    temp = prime_data->n0 * c0;
    product = (ulong_t)temp * (ulong_t)p0 + c0;
    product >>= BITS_PER_WORD; // c0 = product;
    product += (ulong_t)temp * (ulong_t)p1 + c1;
    c1 = product; product >>= BITS_PER_WORD;
    product += (ulong_t)temp * (ulong_t)p2 + c2;
    c2 = product; product >>= BITS_PER_WORD;
    product += c3;
    c3 = product; product >>= BITS_PER_WORD;
    product += c4;
    c4 = product; product >>= BITS_PER_WORD;
    product += c5;
    c5 = product; product >>= BITS_PER_WORD;
    global_carry = product;
    
    temp = prime_data->n0 * c1;
    product = (ulong_t)temp * (ulong_t)p0 + c1;
    product >>= BITS_PER_WORD; // c1 = product;
    product += (ulong_t)temp * (ulong_t)p1 + c2;
    c2 = product; product >>= BITS_PER_WORD;
    product += (ulong_t)temp * (ulong_t)p2 + c3;
    c3 = product; product >>= BITS_PER_WORD;
    product += c4;
    c4 = product; product >>= BITS_PER_WORD;
    product += c5;
    c5 = product; product >>= BITS_PER_WORD;
    global_carry += product;
    
    temp = prime_data->n0 * c1;
    product = (ulong_t)temp * (ulong_t)p0 + c2;
    product >>= BITS_PER_WORD; // c2 = product;
    product += (ulong_t)temp * (ulong_t)p1 + c3;
    c3 = product; product >>= BITS_PER_WORD;
    product += (ulong_t)temp * (ulong_t)p2 + c4;
    c4 = product; product >>= BITS_PER_WORD;
    product += c5;
    c5 = product; product >>= BITS_PER_WORD;
    global_carry += product;

    res[0] = c3;
    res[1] = c4;
    res[2] = c5;
    
    if(global_carry) {
        bigint_subtract(res, res, prime);
    } else if(bigint_compare(res, prime) >= 0) {
        bigint_subtract(res, res, prime);
    }
}

/** test if a big integer is even */
#define BIGINT_IS_EVEN( a ) ( ( a[0] & 1 ) == 0 )
/** test if a big integer is odd */
#define BIGINT_IS_ODD( a ) ( ( a[0] & 1 ) == 1 )

/**
 * Calculate the montgomery inverse for the given globally defined constants
 * based on Hankerson p. 42
 * @param res the inverse: (a * R)^1 * R^2 mod p
 * @param a the number to invert (within the montgomery domain)
 * @param prime_data the used prime data needed to do the multiplication
 */
void gfp_opt_3_inverse( gfp_t res, const gfp_t a ) {
    gfp_t u;
    gfp_t v;
    gfp_t x1;
    gfp_t x2;
    int k = 0, carry = 0;
    int length = 3;
    bigint_copy( u, a );
    bigint_copy( v, prime );
    bigint_clear( x1 );
    x1[0] = 1;
    bigint_clear( x2 );

    while( !bigint_is_zero( v ) ) {
        if( BIGINT_IS_EVEN( v ) ) {
            bigint_shift_right_one( v, v );
            carry = bigint_add( x1, x1, x1 );
        } else if( BIGINT_IS_EVEN( u ) ) {
            bigint_shift_right_one( u, u );
            bigint_add( x2, x2, x2 );
        } else if( bigint_compare( v, u ) >= 0 ) {
            bigint_subtract( v, v, u );
            bigint_shift_right_one( v, v );
            bigint_add( x2, x2, x1 );
            carry = bigint_add( x1, x1, x1 );
        } else {
            bigint_subtract( u, u, v );
            bigint_shift_right_one( u, u );
            bigint_add( x1, x1, x2 );
            bigint_add( x2, x2, x2 );
        }
        k++;
    }
    if( carry || ( bigint_compare( x1, prime ) >= 0 ) ) {
        bigint_subtract( x1, x1, prime );
    }

    /* at this point x1 = a^1 * 2^k mod prime */
    /* n <= k <= 2*n */

    if( k < ( BITS_PER_WORD * length ) ) {
        bigint_copy( x2, x1 ); /* needed in case of future gfp_mont_multiply optimizations */
        gfp_opt_3_multiply( x1, x2, prime_data->r_squared );
        k += ( BITS_PER_WORD * length );
    }
    /* now k >= Wt */
    gfp_opt_3_multiply( res, x1, prime_data->r_squared );
    if( k > ( BITS_PER_WORD * length ) ) {
        k = ( 2 * BITS_PER_WORD * length ) - k;
        bigint_clear( x2 );
        bigint_set_bit( x2, k, 1 );
        bigint_copy( x1, res ); /* needed in case of future gfp_mont_multiply optimizations */
        gfp_opt_3_multiply( res, x1, x2 );
    }
}

