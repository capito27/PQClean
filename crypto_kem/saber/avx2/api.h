#ifndef PQCLEAN_SABER_AVX2_API_H
#define PQCLEAN_SABER_AVX2_API_H

// Update with dagger params
#define PQCLEAN_SABER_AVX2_CRYPTO_ALGNAME "Saber"
#define PQCLEAN_SABER_AVX2_CRYPTO_BYTES 32
#define PQCLEAN_SABER_AVX2_CRYPTO_CIPHERTEXTBYTES 960
#define PQCLEAN_SABER_AVX2_CRYPTO_PUBLICKEYBYTES 896
#define PQCLEAN_SABER_AVX2_CRYPTO_SECRETKEYBYTES 2208

int PQCLEAN_SABER_AVX2_crypto_kem_keypair(unsigned char *pk, unsigned char *sk);

int PQCLEAN_SABER_AVX2_crypto_kem_enc(unsigned char *ct, unsigned char *k, const unsigned char *pk);

int PQCLEAN_SABER_AVX2_crypto_kem_dec(unsigned char *k, const unsigned char *ct, const unsigned char *sk);


#endif /* PQCLEAN_SABER_AVX2_API_H */
