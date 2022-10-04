#include "params.h"
#include "poly.h"
#include "polyvec.h"
#include <stdint.h>

/*************************************************
* Name:        PQCLEAN_KYBER768_CLEAN_polyvec_du_compress
*
* Description: Compress with factor du=9 and serialize vector of polynomials
*
* Arguments:   - uint8_t *r: pointer to output byte array
*                            (needs space for KYBER_POLY_DU_VECBYTES)
*              - const polyvec *a: pointer to input vector of polynomials
**************************************************/
void PQCLEAN_KYBER768_CLEAN_polyvec_du_compress(uint8_t r[KYBER_POLY_DU_VECBYTES], const polyvec *a) {
    unsigned int i;
    for (i = 0; i < KYBER_K; i++) {
        PQCLEAN_KYBER768_CLEAN_poly_du_compress(r + i * KYBER_POLY_DU_BYTES, &a->vec[i]);
    }
}

/*************************************************
* Name:        PQCLEAN_KYBER768_CLEAN_polyvec_du_decompress
*
* Description: De-serialize and decompress with factor du=9 vector of polynomials;
*              approximate inverse of PQCLEAN_KYBER768_CLEAN_polyvec_du_compress
*
* Arguments:   - polyvec *r:       pointer to output vector of polynomials
*              - const uint8_t *a: pointer to input byte array
*                                  (of length KYBER_POLY_DU_BYTES)
**************************************************/
void PQCLEAN_KYBER768_CLEAN_polyvec_du_decompress(polyvec *r, const uint8_t a[KYBER_POLY_DU_VECBYTES]) {
    unsigned int i;
    for (i = 0; i < KYBER_K; i++) {
        PQCLEAN_KYBER768_CLEAN_poly_du_decompress(&r->vec[i], a + i * KYBER_POLY_DU_BYTES);
    }
}

/*************************************************
* Name:        PQCLEAN_KYBER768_CLEAN_polyvec_dv_compress
*
* Description: Compress with factor dv=3 and serialize vector of polynomials
*
* Arguments:   - uint8_t *r: pointer to output byte array
*                            (needs space for KYBER_POLY_DV_VECBYTES)
*              - const polyvec *a: pointer to input vector of polynomials
**************************************************/
void PQCLEAN_KYBER768_CLEAN_polyvec_dv_compress(uint8_t r[KYBER_POLY_DV_VECBYTES], const polyvec *a) {
    unsigned int i;
    for (i = 0; i < KYBER_K; i++) {
        PQCLEAN_KYBER768_CLEAN_poly_dv_compress(r + i * KYBER_POLY_DV_BYTES, &a->vec[i]);
    }
}

/*************************************************
* Name:        PQCLEAN_KYBER768_CLEAN_polyvec_dv_decompress
*
* Description: De-serialize and decompress with factor dv=3 vector of polynomials;
*              approximate inverse of PQCLEAN_KYBER768_CLEAN_polyvec_dv_compress
*
* Arguments:   - polyvec *r:       pointer to output vector of polynomials
*              - const uint8_t *a: pointer to input byte array
*                                  (of length KYBER_POLY_DV_BYTES)
**************************************************/
void PQCLEAN_KYBER768_CLEAN_polyvec_dv_decompress(polyvec *r, const uint8_t a[KYBER_POLY_DV_VECBYTES]) {
    unsigned int i;
    for (i = 0; i < KYBER_K; i++) {
        PQCLEAN_KYBER768_CLEAN_poly_dv_decompress(&r->vec[i], a + i * KYBER_POLY_DV_BYTES);
    }
}

/*************************************************
* Name:        PQCLEAN_KYBER768_CLEAN_polyvec_tobytes
*
* Description: Serialize vector of polynomials
*
* Arguments:   - uint8_t *r: pointer to output byte array
*                            (needs space for KYBER_POLYVECBYTES)
*              - const polyvec *a: pointer to input vector of polynomials
**************************************************/
void PQCLEAN_KYBER768_CLEAN_polyvec_tobytes(uint8_t r[KYBER_POLYVECBYTES], const polyvec *a) {
    unsigned int i;
    for (i = 0; i < KYBER_K; i++) {
        PQCLEAN_KYBER768_CLEAN_poly_tobytes(r + i * KYBER_POLYBYTES, &a->vec[i]);
    }
}

/*************************************************
* Name:        PQCLEAN_KYBER768_CLEAN_polyvec_frombytes
*
* Description: De-serialize vector of polynomials;
*              inverse of PQCLEAN_KYBER768_CLEAN_polyvec_tobytes
*
* Arguments:   - uint8_t *r:       pointer to output byte array
*              - const polyvec *a: pointer to input vector of polynomials
*                                  (of length KYBER_POLYVECBYTES)
**************************************************/
void PQCLEAN_KYBER768_CLEAN_polyvec_frombytes(polyvec *r, const uint8_t a[KYBER_POLYVECBYTES]) {
    unsigned int i;
    for (i = 0; i < KYBER_K; i++) {
        PQCLEAN_KYBER768_CLEAN_poly_frombytes(&r->vec[i], a + i * KYBER_POLYBYTES);
    }
}

/*************************************************
* Name:        PQCLEAN_KYBER768_CLEAN_polyvec_ntt
*
* Description: Apply forward NTT to all elements of a vector of polynomials
*
* Arguments:   - polyvec *r: pointer to in/output vector of polynomials
**************************************************/
void PQCLEAN_KYBER768_CLEAN_polyvec_ntt(polyvec *r) {
    unsigned int i;
    for (i = 0; i < KYBER_K; i++) {
        PQCLEAN_KYBER768_CLEAN_poly_ntt(&r->vec[i]);
    }
}

/*************************************************
* Name:        PQCLEAN_KYBER768_CLEAN_polyvec_invntt_tomont
*
* Description: Apply inverse NTT to all elements of a vector of polynomials
*              and multiply by Montgomery factor 2^16
*
* Arguments:   - polyvec *r: pointer to in/output vector of polynomials
**************************************************/
void PQCLEAN_KYBER768_CLEAN_polyvec_invntt_tomont(polyvec *r) {
    unsigned int i;
    for (i = 0; i < KYBER_K; i++) {
        PQCLEAN_KYBER768_CLEAN_poly_invntt_tomont(&r->vec[i]);
    }
}

/*************************************************
* Name:        PQCLEAN_KYBER768_CLEAN_polyvec_basemul_acc_montgomery
*
* Description: Multiply elements of a and b in NTT domain, accumulate into r,
*              and multiply by 2^-16.
*
* Arguments: - poly *r: pointer to output polynomial
*            - const polyvec *a: pointer to first input vector of polynomials
*            - const polyvec *b: pointer to second input vector of polynomials
**************************************************/
void PQCLEAN_KYBER768_CLEAN_polyvec_basemul_acc_montgomery(poly *r, const polyvec *a, const polyvec *b) {
    unsigned int i;
    poly t;

    PQCLEAN_KYBER768_CLEAN_poly_basemul_montgomery(r, &a->vec[0], &b->vec[0]);
    for (i = 1; i < KYBER_K; i++) {
        PQCLEAN_KYBER768_CLEAN_poly_basemul_montgomery(&t, &a->vec[i], &b->vec[i]);
        PQCLEAN_KYBER768_CLEAN_poly_add(r, r, &t);
    }

    PQCLEAN_KYBER768_CLEAN_poly_reduce(r);
}

/*************************************************
* Name:        PQCLEAN_KYBER768_CLEAN_polyvec_reduce
*
* Description: Applies Barrett reduction to each coefficient
*              of each element of a vector of polynomials;
*              for details of the Barrett reduction see comments in reduce.c
*
* Arguments:   - polyvec *r: pointer to input/output polynomial
**************************************************/
void PQCLEAN_KYBER768_CLEAN_polyvec_reduce(polyvec *r) {
    unsigned int i;
    for (i = 0; i < KYBER_K; i++) {
        PQCLEAN_KYBER768_CLEAN_poly_reduce(&r->vec[i]);
    }
}

/*************************************************
* Name:        PQCLEAN_KYBER768_CLEAN_polyvec_add
*
* Description: Add vectors of polynomials
*
* Arguments: - polyvec *r: pointer to output vector of polynomials
*            - const polyvec *a: pointer to first input vector of polynomials
*            - const polyvec *b: pointer to second input vector of polynomials
**************************************************/
void PQCLEAN_KYBER768_CLEAN_polyvec_add(polyvec *r, const polyvec *a, const polyvec *b) {
    unsigned int i;
    for (i = 0; i < KYBER_K; i++) {
        PQCLEAN_KYBER768_CLEAN_poly_add(&r->vec[i], &a->vec[i], &b->vec[i]);
    }
}
