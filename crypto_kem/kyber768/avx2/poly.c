#include "align.h"
#include "cbd.h"
#include "consts.h"
#include "ntt.h"
#include "params.h"
#include "poly.h"
#include "reduce.h"
#include "symmetric.h"
#include <immintrin.h>
#include <stdint.h>
#include <string.h>

/*************************************************
* Name:        PQCLEAN_KYBER768_AVX2_poly_du_compress
*
* Description: Compression with factor du=9 and subsequent serialization of a polynomial
*
* Arguments:   - uint8_t *r: pointer to output byte array
*                            (of length KYBER_POLY_DU_BYTES)
*              - const poly *a: pointer to input polynomial
**************************************************/
void PQCLEAN_KYBER768_AVX2_poly_du_compress(uint8_t r[KYBER_POLY_DU_BYTES], const poly *a) {
    unsigned int i, j;

    uint16_t t[8];
    for (i = 0; i < KYBER_N / 8; i++) {
        for (j = 0; j < 8; j++) {
            t[j]  = a->coeffs[8 * i + j];
            t[j] += ((int16_t)t[j] >> 15) & KYBER_Q;
            t[j]  = ((((uint32_t)t[j] << 9) + KYBER_Q / 2) / KYBER_Q) & 0x1ff;
        }

        r[ 0] = (uint8_t)(t[0] >>  0); // 8
        r[ 1] = (uint8_t)((t[0] >>  8) | (t[1] << 1)); // 1 | 7
        r[ 2] = (uint8_t)((t[1] >>  7) | (t[2] << 2)); // 2 | 6
        r[ 3] = (uint8_t)((t[2] >>  6) | (t[3] << 3)); // 3 | 5
        r[ 4] = (uint8_t)((t[3] >>  5) | (t[4] << 4)); // 4 | 4
        r[ 5] = (uint8_t)((t[4] >>  4) | (t[5] << 5)); // 5 | 3
        r[ 6] = (uint8_t)((t[5] >>  3) | (t[6] << 6)); // 6 | 2
        r[ 7] = (uint8_t)((t[6] >>  2) | (t[7] << 7)); // 7 | 1
        r[ 8] = (uint8_t)(t[7] >>  1); // 8
        r += 9;
    }
}

/*************************************************
* Name:        PQCLEAN_KYBER768_AVX2_poly_du_decompress
*
* Description: De-serialization and subsequent decompression with factor du=9 of a polynomial;
*              approximate inverse of PQCLEAN_KYBER768_AVX2_poly_du_compress
*
* Arguments:   - poly *r: pointer to output polynomial
*              - const uint8_t *a: pointer to input byte array
*                                  (of length KYBER_POLY_DU_BYTES bytes)
**************************************************/
void PQCLEAN_KYBER768_AVX2_poly_du_decompress(poly *r, const uint8_t a[KYBER_POLY_DU_BYTES]) {
    unsigned int i, j;

    uint16_t t[8];
    for (i = 0; i < KYBER_N / 8; i++) {
        t[0] = (a[0] >> 0) | ((uint16_t)a[1] << 8); // 8 | 1
        t[1] = (a[1] >> 1) | ((uint16_t)a[2] << 7); // 7 | 2
        t[2] = (a[2] >> 2) | ((uint16_t)a[3] << 6); // 6 | 3
        t[3] = (a[3] >> 3) | ((uint16_t)a[4] << 5); // 5 | 4
        t[4] = (a[4] >> 4) | ((uint16_t)a[5] << 4); // 4 | 5
        t[5] = (a[5] >> 5) | ((uint16_t)a[6] << 3); // 3 | 6
        t[6] = (a[6] >> 6) | ((uint16_t)a[7] << 2); // 2 | 7
        t[7] = (a[7] >> 7) | ((uint16_t)a[8] << 1); // 1 | 8
        a += 9;

        for (j = 0; j < 8; j++) {
            r->coeffs[8 * i + j] = ((uint32_t)(t[j] & 0x1FF) * KYBER_Q + 256) >> 9;
        }
    }
}

/*************************************************
* Name:        PQCLEAN_KYBER768_AVX2_poly_dv_compress
*
* Description: Compression with factor dv=3 and subsequent serialization of a polynomial
*
* Arguments:   - uint8_t *r: pointer to output byte array
*                            (of length KYBER_POLY_DV_BYTES)
*              - const poly *a: pointer to input polynomial
**************************************************/
void PQCLEAN_KYBER768_AVX2_poly_dv_compress(uint8_t r[KYBER_POLY_DV_BYTES], const poly *a) {
    size_t i, j;
    int16_t u;
    uint8_t t[8];

    for (i = 0; i < KYBER_N / 8; i++) {
        for (j = 0; j < 8; j++) {
            // map to positive standard representatives
            u  = a->coeffs[8 * i + j];
            u += (u >> 15) & KYBER_Q;
            t[j] = ((((uint16_t)u << 3) + KYBER_Q / 2) / KYBER_Q) & 7;
        }

        r[0] = (t[0] >> 0) | (t[1] << 3) | (t[2] << 6);               // 3|3|2
        r[1] = (t[2] >> 2) | (t[3] << 1) | (t[4] << 4) | (t[5] << 7); // 1|3|3|1
        r[2] = (t[5] >> 1) | (t[6] << 2) | (t[7] << 5);               // 2|3|3
        r += 3;
    }
}

/*************************************************
* Name:        PQCLEAN_KYBER768_AVX2_poly_dv_decompress
*
* Description: De-serialization and subsequent decompression with factor dv=3 of a polynomial;
*              approximate inverse of PQCLEAN_KYBER768_AVX2_poly_dv_compress
*
* Arguments:   - poly *r: pointer to output polynomial
*              - const uint8_t *a: pointer to input byte array
*                                  (of length KYBER_POLY_DV_BYTES bytes)
**************************************************/
void PQCLEAN_KYBER768_AVX2_poly_dv_decompress(poly *r, const uint8_t a[KYBER_POLY_DV_BYTES]) {
    size_t i;

    size_t j;
    uint8_t t[8];
    for (i = 0; i < KYBER_N / 8; i++) {
        t[0] = (a[0] >> 0);
        t[1] = (a[0] >> 3);
        t[2] = (a[0] >> 6) | (a[1] << 2);
        t[3] = (a[1] >> 1);
        t[4] = (a[1] >> 4);
        t[5] = (a[1] >> 7) | (a[2] << 1);
        t[6] = (a[2] >> 2);
        t[7] = (a[2] >> 5);
        a += 3;

        for (j = 0; j < 8; j++) {
            r->coeffs[8 * i + j] = ((uint32_t)(t[j] & 15) * KYBER_Q + 4) >> 3;
        }
    }
}

/*************************************************
* Name:        PQCLEAN_KYBER768_AVX2_poly_tobytes
*
* Description: Serialization of a polynomial in NTT representation.
*              The coefficients of the input polynomial are assumed to
*              lie in the invertal [0,q], i.e. the polynomial must be reduced
*              by PQCLEAN_KYBER768_AVX2_poly_reduce(). The coefficients are orderd as output by
*              PQCLEAN_KYBER768_AVX2_poly_ntt(); the serialized output coefficients are in bitreversed
*              order.
*
* Arguments:   - uint8_t *r: pointer to output byte array
*                            (needs space for KYBER_POLYBYTES bytes)
*              - poly *a: pointer to input polynomial
**************************************************/
void PQCLEAN_KYBER768_AVX2_poly_tobytes(uint8_t r[KYBER_POLYBYTES], const poly *a) {
    PQCLEAN_KYBER768_AVX2_ntttobytes_avx(r, a->vec, PQCLEAN_KYBER768_AVX2_qdata.vec);
}

/*************************************************
* Name:        PQCLEAN_KYBER768_AVX2_poly_frombytes
*
* Description: De-serialization of a polynomial;
*              inverse of PQCLEAN_KYBER768_AVX2_poly_tobytes
*
* Arguments:   - poly *r: pointer to output polynomial
*              - const uint8_t *a: pointer to input byte array
*                                  (of KYBER_POLYBYTES bytes)
**************************************************/
void PQCLEAN_KYBER768_AVX2_poly_frombytes(poly *r, const uint8_t a[KYBER_POLYBYTES]) {
    PQCLEAN_KYBER768_AVX2_nttfrombytes_avx(r->vec, a, PQCLEAN_KYBER768_AVX2_qdata.vec);
}

/*************************************************
* Name:        PQCLEAN_KYBER768_AVX2_poly_frommsg
*
* Description: Convert 32-byte message to polynomial
*
* Arguments:   - poly *r: pointer to output polynomial
*              - const uint8_t *msg: pointer to input message
**************************************************/
void PQCLEAN_KYBER768_AVX2_poly_frommsg(poly *restrict r, const uint8_t msg[KYBER_INDCPA_MSGBYTES]) {
    __m256i f, g0, g1, g2, g3, h0, h1, h2, h3;
    const __m256i shift = _mm256_broadcastsi128_si256(_mm_set_epi32(0, 1, 2, 3));
    const __m256i idx = _mm256_broadcastsi128_si256(_mm_set_epi8(15, 14, 11, 10, 7, 6, 3, 2, 13, 12, 9, 8, 5, 4, 1, 0));
    const __m256i hqs = _mm256_set1_epi16((KYBER_Q + 1) / 2);

#define FROMMSG64(i)                        \
    g3 = _mm256_shuffle_epi32(f,0x55*(i));                \
    g3 = _mm256_sllv_epi32(g3,shift);             \
    g3 = _mm256_shuffle_epi8(g3,idx);             \
    g0 = _mm256_slli_epi16(g3,12);                \
    g1 = _mm256_slli_epi16(g3,8);                 \
    g2 = _mm256_slli_epi16(g3,4);                 \
    g0 = _mm256_srai_epi16(g0,15);                \
    g1 = _mm256_srai_epi16(g1,15);                \
    g2 = _mm256_srai_epi16(g2,15);                \
    g3 = _mm256_srai_epi16(g3,15);                \
    g0 = _mm256_and_si256(g0,hqs);  /* 19 18 17 16  3  2  1  0 */ \
    g1 = _mm256_and_si256(g1,hqs);  /* 23 22 21 20  7  6  5  4 */ \
    g2 = _mm256_and_si256(g2,hqs);  /* 27 26 25 24 11 10  9  8 */ \
    g3 = _mm256_and_si256(g3,hqs);  /* 31 30 29 28 15 14 13 12 */ \
    h0 = _mm256_unpacklo_epi64(g0,g1);                \
    h2 = _mm256_unpackhi_epi64(g0,g1);                \
    h1 = _mm256_unpacklo_epi64(g2,g3);                \
    h3 = _mm256_unpackhi_epi64(g2,g3);                \
    g0 = _mm256_permute2x128_si256(h0,h1,0x20);           \
    g2 = _mm256_permute2x128_si256(h0,h1,0x31);           \
    g1 = _mm256_permute2x128_si256(h2,h3,0x20);           \
    g3 = _mm256_permute2x128_si256(h2,h3,0x31);           \
    _mm256_store_si256(&r->vec[0+2*(i)+0],g0);    \
    _mm256_store_si256(&r->vec[0+2*(i)+1],g1);    \
    _mm256_store_si256(&r->vec[8+2*(i)+0],g2);    \
    _mm256_store_si256(&r->vec[8+2*(i)+1],g3)

    f = _mm256_loadu_si256((__m256i *)msg);
    FROMMSG64(0);
    FROMMSG64(1);
    FROMMSG64(2);
    FROMMSG64(3);
}

/*************************************************
* Name:        PQCLEAN_KYBER768_AVX2_poly_tomsg
*
* Description: Convert polynomial to 32-byte message.
*              The coefficients of the input polynomial are assumed to
*              lie in the invertal [0,q], i.e. the polynomial must be reduced
*              by PQCLEAN_KYBER768_AVX2_poly_reduce().
*
* Arguments:   - uint8_t *msg: pointer to output message
*              - poly *a: pointer to input polynomial
**************************************************/
void PQCLEAN_KYBER768_AVX2_poly_tomsg(uint8_t msg[KYBER_INDCPA_MSGBYTES], const poly *restrict a) {
    unsigned int i;
    uint32_t small;
    __m256i f0, f1, g0, g1;
    const __m256i hq = _mm256_set1_epi16((KYBER_Q - 1) / 2);
    const __m256i hhq = _mm256_set1_epi16((KYBER_Q - 1) / 4);

    for (i = 0; i < KYBER_N / 32; i++) {
        f0 = _mm256_load_si256(&a->vec[2 * i + 0]);
        f1 = _mm256_load_si256(&a->vec[2 * i + 1]);
        f0 = _mm256_sub_epi16(hq, f0);
        f1 = _mm256_sub_epi16(hq, f1);
        g0 = _mm256_srai_epi16(f0, 15);
        g1 = _mm256_srai_epi16(f1, 15);
        f0 = _mm256_xor_si256(f0, g0);
        f1 = _mm256_xor_si256(f1, g1);
        f0 = _mm256_sub_epi16(f0, hhq);
        f1 = _mm256_sub_epi16(f1, hhq);
        f0 = _mm256_packs_epi16(f0, f1);
        f0 = _mm256_permute4x64_epi64(f0, 0xD8);
        small = _mm256_movemask_epi8(f0);
        memcpy(&msg[4 * i], &small, 4);
    }
}

/*************************************************
* Name:        PQCLEAN_KYBER768_AVX2_poly_getnoise_eta1
*
* Description: Sample a polynomial deterministically from a seed and a nonce,
*              with output polynomial close to centered binomial distribution
*              with parameter KYBER_ETA1
*
* Arguments:   - poly *r: pointer to output polynomial
*              - const uint8_t *seed: pointer to input seed
*                                     (of length KYBER_SYMBYTES bytes)
*              - uint8_t nonce: one-byte input nonce
**************************************************/
void PQCLEAN_KYBER768_AVX2_poly_getnoise_eta1(poly *r, const uint8_t seed[KYBER_SYMBYTES], uint8_t nonce) {
    ALIGNED_UINT8(KYBER_ETA1 * KYBER_N / 4 + 32) buf; // +32 bytes as required by PQCLEAN_KYBER768_AVX2_poly_cbd_eta1
    prf(buf.coeffs, KYBER_ETA1 * KYBER_N / 4, seed, nonce);
    PQCLEAN_KYBER768_AVX2_poly_cbd_eta1(r, buf.vec);
}

/*************************************************
* Name:        PQCLEAN_KYBER768_AVX2_poly_getnoise_eta2
*
* Description: Sample a polynomial deterministically from a seed and a nonce,
*              with output polynomial close to centered binomial distribution
*              with parameter KYBER_ETA2
*
* Arguments:   - poly *r: pointer to output polynomial
*              - const uint8_t *seed: pointer to input seed
*                                     (of length KYBER_SYMBYTES bytes)
*              - uint8_t nonce: one-byte input nonce
**************************************************/
void PQCLEAN_KYBER768_AVX2_poly_getnoise_eta2(poly *r, const uint8_t seed[KYBER_SYMBYTES], uint8_t nonce) {
    ALIGNED_UINT8(KYBER_ETA2 * KYBER_N / 4) buf;
    prf(buf.coeffs, KYBER_ETA2 * KYBER_N / 4, seed, nonce);
    PQCLEAN_KYBER768_AVX2_poly_cbd_eta2(r, buf.vec);
}

#define NOISE_NBLOCKS ((KYBER_ETA1*KYBER_N/4+SHAKE256_RATE-1)/SHAKE256_RATE)
void PQCLEAN_KYBER768_AVX2_poly_getnoise_eta1_4x(poly *r0,
        poly *r1,
        poly *r2,
        poly *r3,
        const uint8_t seed[32],
        uint8_t nonce0,
        uint8_t nonce1,
        uint8_t nonce2,
        uint8_t nonce3) {
    ALIGNED_UINT8(NOISE_NBLOCKS * SHAKE256_RATE) buf[4];
    __m256i f;
    keccakx4_state state;

    f = _mm256_loadu_si256((__m256i *)seed);
    _mm256_store_si256(buf[0].vec, f);
    _mm256_store_si256(buf[1].vec, f);
    _mm256_store_si256(buf[2].vec, f);
    _mm256_store_si256(buf[3].vec, f);

    buf[0].coeffs[32] = nonce0;
    buf[1].coeffs[32] = nonce1;
    buf[2].coeffs[32] = nonce2;
    buf[3].coeffs[32] = nonce3;

    PQCLEAN_KYBER768_AVX2_shake256x4_absorb_once(&state, buf[0].coeffs, buf[1].coeffs, buf[2].coeffs, buf[3].coeffs, 33);
    PQCLEAN_KYBER768_AVX2_shake256x4_squeezeblocks(buf[0].coeffs, buf[1].coeffs, buf[2].coeffs, buf[3].coeffs, NOISE_NBLOCKS, &state);

    PQCLEAN_KYBER768_AVX2_poly_cbd_eta1(r0, buf[0].vec);
    PQCLEAN_KYBER768_AVX2_poly_cbd_eta1(r1, buf[1].vec);
    PQCLEAN_KYBER768_AVX2_poly_cbd_eta1(r2, buf[2].vec);
    PQCLEAN_KYBER768_AVX2_poly_cbd_eta1(r3, buf[3].vec);
}


/*************************************************
* Name:        PQCLEAN_KYBER768_AVX2_poly_ntt
*
* Description: Computes negacyclic number-theoretic transform (NTT) of
*              a polynomial in place.
*              Input coefficients assumed to be in normal order,
*              output coefficients are in special order that is natural
*              for the vectorization. Input coefficients are assumed to be
*              bounded by q in absolute value, output coefficients are bounded
*              by 16118 in absolute value.
*
* Arguments:   - poly *r: pointer to in/output polynomial
**************************************************/
void PQCLEAN_KYBER768_AVX2_poly_ntt(poly *r) {
    PQCLEAN_KYBER768_AVX2_ntt_avx(r->vec, PQCLEAN_KYBER768_AVX2_qdata.vec);
}

/*************************************************
* Name:        PQCLEAN_KYBER768_AVX2_poly_invntt_tomont
*
* Description: Computes inverse of negacyclic number-theoretic transform (NTT)
*              of a polynomial in place;
*              Input coefficients assumed to be in special order from vectorized
*              forward ntt, output in normal order. Input coefficients can be
*              arbitrary 16-bit integers, output coefficients are bounded by 14870
*              in absolute value.
*
* Arguments:   - poly *a: pointer to in/output polynomial
**************************************************/
void PQCLEAN_KYBER768_AVX2_poly_invntt_tomont(poly *r) {
    PQCLEAN_KYBER768_AVX2_invntt_avx(r->vec, PQCLEAN_KYBER768_AVX2_qdata.vec);
}

void PQCLEAN_KYBER768_AVX2_poly_nttunpack(poly *r) {
    PQCLEAN_KYBER768_AVX2_nttunpack_avx(r->vec, PQCLEAN_KYBER768_AVX2_qdata.vec);
}

/*************************************************
* Name:        PQCLEAN_KYBER768_AVX2_poly_basemul_montgomery
*
* Description: Multiplication of two polynomials in NTT domain.
*              One of the input polynomials needs to have coefficients
*              bounded by q, the other polynomial can have arbitrary
*              coefficients. Output coefficients are bounded by 6656.
*
* Arguments:   - poly *r: pointer to output polynomial
*              - const poly *a: pointer to first input polynomial
*              - const poly *b: pointer to second input polynomial
**************************************************/
void PQCLEAN_KYBER768_AVX2_poly_basemul_montgomery(poly *r, const poly *a, const poly *b) {
    PQCLEAN_KYBER768_AVX2_basemul_avx(r->vec, a->vec, b->vec, PQCLEAN_KYBER768_AVX2_qdata.vec);
}

/*************************************************
* Name:        PQCLEAN_KYBER768_AVX2_poly_tomont
*
* Description: Inplace conversion of all coefficients of a polynomial
*              from normal domain to Montgomery domain
*
* Arguments:   - poly *r: pointer to input/output polynomial
**************************************************/
void PQCLEAN_KYBER768_AVX2_poly_tomont(poly *r) {
    PQCLEAN_KYBER768_AVX2_tomont_avx(r->vec, PQCLEAN_KYBER768_AVX2_qdata.vec);
}

/*************************************************
* Name:        PQCLEAN_KYBER768_AVX2_poly_reduce
*
* Description: Applies Barrett reduction to all coefficients of a polynomial
*              for details of the Barrett reduction see comments in reduce.c
*
* Arguments:   - poly *r: pointer to input/output polynomial
**************************************************/
void PQCLEAN_KYBER768_AVX2_poly_reduce(poly *r) {
    PQCLEAN_KYBER768_AVX2_reduce_avx(r->vec, PQCLEAN_KYBER768_AVX2_qdata.vec);
}

/*************************************************
* Name:        PQCLEAN_KYBER768_AVX2_poly_add
*
* Description: Add two polynomials. No modular reduction
*              is performed.
*
* Arguments: - poly *r: pointer to output polynomial
*            - const poly *a: pointer to first input polynomial
*            - const poly *b: pointer to second input polynomial
**************************************************/
void PQCLEAN_KYBER768_AVX2_poly_add(poly *r, const poly *a, const poly *b) {
    unsigned int i;
    __m256i f0, f1;

    for (i = 0; i < KYBER_N / 16; i++) {
        f0 = _mm256_load_si256(&a->vec[i]);
        f1 = _mm256_load_si256(&b->vec[i]);
        f0 = _mm256_add_epi16(f0, f1);
        _mm256_store_si256(&r->vec[i], f0);
    }
}

/*************************************************
* Name:        PQCLEAN_KYBER768_AVX2_poly_sub
*
* Description: Subtract two polynomials. No modular reduction
*              is performed.
*
* Arguments: - poly *r: pointer to output polynomial
*            - const poly *a: pointer to first input polynomial
*            - const poly *b: pointer to second input polynomial
**************************************************/
void PQCLEAN_KYBER768_AVX2_poly_sub(poly *r, const poly *a, const poly *b) {
    unsigned int i;
    __m256i f0, f1;

    for (i = 0; i < KYBER_N / 16; i++) {
        f0 = _mm256_load_si256(&a->vec[i]);
        f1 = _mm256_load_si256(&b->vec[i]);
        f0 = _mm256_sub_epi16(f0, f1);
        _mm256_store_si256(&r->vec[i], f0);
    }
}
