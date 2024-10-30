#include "hkdf.h"

int sha256(uint8_t hash[32], int num_args, ...)
{
    SHA256_CTX ctx;
    SHA256_Init(&ctx);

    va_list valist;
    va_start(valist, num_args);

    for (int i = 0; i < num_args; i++)
    {
        uint8_t *data = va_arg(valist, uint8_t *);
        size_t data_len = va_arg(valist, size_t);
        SHA256_Update(&ctx, data, data_len);
    }
    va_end(valist);
    SHA256_Final(hash, &ctx);
    return 0;
}

void hmac_sha256(const uint8_t *key, size_t key_len, const uint8_t *data, size_t data_len, uint8_t hash[32])
{
    uint8_t i_key_pad[SHA256_BLOCK_SIZE];
    uint8_t o_key_pad[SHA256_BLOCK_SIZE];
    uint8_t key_hash[32];
    uint8_t temp_hash[32];

    if (key_len > SHA256_BLOCK_SIZE)
    {
        sha256(key_hash, 1, key, key_len);
        key = key_hash;
        key_len = 32;
    }

    memset(i_key_pad, 0, sizeof(i_key_pad));
    memset(o_key_pad, 0, sizeof(o_key_pad));
    memcpy(i_key_pad, key, key_len);
    memcpy(o_key_pad, key, key_len);

    for (int i = 0; i < sizeof(i_key_pad); i++)
    {
        i_key_pad[i] ^= 0x36;
        o_key_pad[i] ^= 0x5c;
    }

    sha256(temp_hash, 2, i_key_pad, SHA256_BLOCK_SIZE, data, data_len);
    sha256(hash, 2, o_key_pad, SHA256_BLOCK_SIZE, temp_hash, 32);
}

void hkdf(const uint8_t *initial_key, size_t initial_key_len,
          const uint8_t *salt, size_t salt_len,
          const uint8_t *info, size_t info_len,
          uint8_t *out, size_t out_len)
{
    uint8_t prk[SHA256_DIGEST_SIZE];

    // If salt is not provided, set it to a string of SHA256_DIGEST_SIZE zeros.
    if (salt == NULL)
    {
        salt_len = SHA256_DIGEST_SIZE;
        uint8_t zero_salt[SHA256_DIGEST_SIZE] = {0};
        salt = zero_salt;
    }

    // Step 1: HKDF-Extract
    hmac_sha256(salt, salt_len, initial_key, initial_key_len, prk);

    // Step 2: HKDF-Expand
    size_t hash_len = SHA256_DIGEST_SIZE;
    size_t N = (out_len + hash_len - 1) / hash_len;
    uint8_t T[SHA256_DIGEST_SIZE];
    uint8_t Tn[SHA256_DIGEST_SIZE];

    for (size_t i = 0; i < N; i++)
    {
        size_t info_len_n = info_len + ((i == 0) ? 0 : hash_len) + 1;
        uint8_t info_n[info_len_n];
        memcpy(info_n, info, info_len);
        if (i > 0)
        {
            memcpy(info_n + info_len, T, hash_len);
        }
        info_n[info_len_n - 1] = i + 1;

        hmac_sha256(prk, hash_len, info_n, info_len_n, Tn);

        memcpy(T, Tn, hash_len);
        memcpy(out + i * hash_len, T, (i == N - 1) ? out_len - i * hash_len : hash_len);
    }
}
