#ifndef HKDF_H
#define HKDF_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <openssl/sha.h>
#define SHA256_BLOCK_SIZE 64
#define SHA256_DIGEST_SIZE 32

int sha256(uint8_t hash[32], int num_args, ...);
void hmac_sha256(const uint8_t *key, size_t key_len, const uint8_t *data, size_t data_len, uint8_t hash[32]);

void hkdf(const uint8_t *initial_key, size_t initial_key_len,
          const uint8_t *salt, size_t salt_len,
          const uint8_t *info, size_t info_len,
          uint8_t *out, size_t out_len);

#endif