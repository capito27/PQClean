#ifndef PQCLEAN_KYBER768_CLEAN_PARAMS_H
#define PQCLEAN_KYBER768_CLEAN_PARAMS_H

#define KYBER_N 256
#define KYBER_Q 3329

#define KYBER_SYMBYTES 32   /* size in bytes of hashes, and seeds */
#define KYBER_SSBYTES  32   /* size in bytes of shared key */
// Tweaks kyber-768 internal parameters with the following internal values :
// Du = 9 (was 10)
// Dv = 3 (was 4)
// Dpk = 8 (was 12) // Dpk is the compression value for public key encoding, from lossless to lossy
// Results in much smaller key and cipher sizes, and much higher failure rate (from 2^-164 to ~2^-17)
// without compromising the security of the cryptosystem.

#define KYBER_DU 9
#define KYBER_DV 3

#define KYBER_K 3
#define KYBER_ETA1 2
#define KYBER_ETA2 2

// Lossless poly(vec) serialization sizes
#define KYBER_POLYBYTES     384
#define KYBER_POLYVECBYTES  (KYBER_K * KYBER_POLYBYTES)

// Lossy poly(vec) serialization sizes
#define KYBER_POLY_DU_BYTES (KYBER_N * KYBER_DU / 8)
#define KYBER_POLY_DU_VECBYTES (KYBER_K * KYBER_POLY_DU_BYTES)
#define KYBER_POLY_DV_BYTES (KYBER_N * KYBER_DV / 8)
#define KYBER_POLY_DV_VECBYTES (KYBER_K * KYBER_POLY_DV_BYTES)



#define KYBER_INDCPA_MSGBYTES       (KYBER_SYMBYTES)
#define KYBER_INDCPA_PUBLICKEYBYTES (KYBER_POLYVECBYTES + KYBER_SYMBYTES)
#define KYBER_INDCPA_SECRETKEYBYTES (KYBER_POLYVECBYTES)
// Du compressed polyvec | Dv compressed poly
#define KYBER_INDCPA_BYTES          (KYBER_POLY_DU_VECBYTES + KYBER_POLY_DV_BYTES)

#define KYBER_PUBLICKEYBYTES  (KYBER_INDCPA_PUBLICKEYBYTES)
/* 32 bytes of additional space to save H(pk) */
#define KYBER_SECRETKEYBYTES  (KYBER_INDCPA_SECRETKEYBYTES + KYBER_INDCPA_PUBLICKEYBYTES + 2*KYBER_SYMBYTES)
#define KYBER_CIPHERTEXTBYTES (KYBER_INDCPA_BYTES)

#endif
