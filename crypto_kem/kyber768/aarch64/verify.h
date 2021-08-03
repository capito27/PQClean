#ifndef PQCLEAN_KYBER768_AARCH64_VERIFY_H
#define PQCLEAN_KYBER768_AARCH64_VERIFY_H
#include "params.h"
#include <stddef.h>
#include <stdint.h>

int PQCLEAN_KYBER768_AARCH64_verify(const uint8_t *a, const uint8_t *b, size_t len);

void PQCLEAN_KYBER768_AARCH64_cmov(uint8_t *r, const uint8_t *x, size_t len, uint8_t b);

#endif
