#ifndef USER_DB_H
#define USER_DB_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <openssl/aes.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include "sys_utils.h"
#define FEMALE F
#define MALE M
#define USER_DB_MAGIC 0x4341544f
#define START_USER_ID 1
#define MAX_USER 10
#define AES128_KEY_LEN 16
typedef struct 
{
    uint8_t age;
    char gender;
    char *name;
} member;

typedef struct 
{
    uint32_t magic;
    uint8_t hmac[32];
    uint8_t iv[16];

    uint32_t user_id;
    member user[];
} header;



extern uint8_t aes_key[AES128_KEY_LEN];
extern header_conf conf;

#endif