/*---------------------------------------------------------------------
This file has been adapted from the implementation
(available at, Public Domain https://github.com/pq-crystals/kyber)
of "CRYSTALS – Kyber: a CCA-secure module-lattice-based KEM"
by : Joppe Bos, Leo Ducas, Eike Kiltz, Tancrede Lepoint,
Vadim Lyubashevsky, John M. Schanck, Peter Schwabe & Damien stehle
----------------------------------------------------------------------*/
#include "SABER_params.h"
#include "cbd.h"
#include "fips202.h"
#include "poly.h"

void PQCLEAN_SABER_CLEAN_GenSecret(uint16_t r[SABER_K][SABER_N], const unsigned char *seed) {


    uint32_t i;

    int32_t buf_size = SABER_MU * SABER_N * SABER_K / 8;

    uint8_t buf[buf_size];

    shake128(buf, buf_size, seed, SABER_NOISESEEDBYTES);

    for (i = 0; i < SABER_K; i++) {
        PQCLEAN_SABER_CLEAN_cbd(r[i], buf + i * SABER_MU * SABER_N / 8);
    }
}
