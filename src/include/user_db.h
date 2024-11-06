#ifndef USER_DB_H
#define USER_DB_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <openssl/aes.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include "sys_utils.h"
#define FEMALE 'F'
#define MALE 'M'
#define USER_DB_MAGIC 0x64404883
#define START_USER_ID 1
#define MAX_USER 10
#define AES128_KEY_LEN 16
#define PADDING_SIZE AES128_KEY_LEN
#define NAME_LEN_MAX 20
typedef struct 
{
    uint8_t age;
    char gender;
    char name[NAME_LEN_MAX + 1];
    uint32_t id;
} member;

typedef struct 
{
    uint32_t magic;
    uint8_t hmac[32];
    uint8_t iv[16];
    uint32_t user_id;
    member user[0];
} header;



extern uint8_t aes_key[AES128_KEY_LEN];
extern header_conf conf;

int init(void);
int db_init(void);
int db_import(void);
int db_export(void);
int cprng(uint8_t *buf, size_t len);
int aes128_cbc_encrypt(const uint8_t *in, size_t in_len, uint8_t *out, size_t *out_len, const uint8_t key[16], const uint8_t iv[16]);
int aes128_cbc_decrypt(const uint8_t *in, size_t in_len, uint8_t *out, size_t *out_len, const uint8_t key[16], const uint8_t iv[16]);
member* db_add_user(const char * name, char gender , uint8_t age);
int db_total_user(void);
int db_delete_user(const char * name);
member * db_find_user_by_name(const char * name);
member * db_find_user_by_id(uint32_t id);

#endif