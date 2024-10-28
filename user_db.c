#include "user_db.h"
#include "hkdf.h"
#include "sys_utils.h"
#include <assert.h>
header *user_db = NULL;
uint8_t aes_key[AES128_KEY_LEN];
uint8_t test_aes_key[AES128_KEY_LEN];
header_conf conf;

static int aes128_cbc_decrypt(const uint8_t *in, size_t in_len, uint8_t *out, size_t *out_len, const uint8_t key[16], const uint8_t iv[16])
{
    AES_KEY aes_key;
    uint8_t iv_int[16];
    memcpy(iv_int, iv, 16);
    AES_set_decrypt_key(key, 128, &aes_key);
    AES_cbc_encrypt(in, out, in_len, &aes_key, iv_int, AES_DECRYPT);

    size_t padding_len = out[in_len - 1];
    if (padding_len > 16) {
        // Invalid padding
        return -1;
    }
    *out_len = in_len - padding_len;
    return 0;
}

static int aes128_cbc_encrypt(const uint8_t *in, size_t in_len, uint8_t *out, size_t *out_len, const uint8_t key[16], const uint8_t iv[16])
{
    AES_KEY aes_key;
    uint8_t iv_int[16];
    uint8_t padding[16];
    size_t padding_len;
    memcpy(iv_int, iv, 16);

    if (in_len + 16 > *out_len) {
        // Output buffer is too small
        return -1;
    }
    AES_set_encrypt_key(key, 128, &aes_key);

    // Encrypt all full blocks 
    size_t full_block_len = in_len - (in_len % 16);
    AES_cbc_encrypt(in, out, full_block_len, &aes_key, iv_int, AES_ENCRYPT);

    // Prepare and encrypt the last block with PKCS#7 padding
    padding_len = 16 - (in_len % 16);
    memcpy(padding, in + full_block_len, in_len % 16);
    memset(padding + (in_len % 16), padding_len, padding_len);
    AES_cbc_encrypt(padding, out + full_block_len, 16, &aes_key, iv_int, AES_ENCRYPT);

    *out_len = full_block_len + 16;
    return 0;    
}

//conf 불러오기 , aes_key 생성
int init() {
    
    init_config(&conf);
    if (conf.info == NULL || conf.passphrase == NULL || conf.unique_id == NULL) {
        return -1;
    }
    hkdf(
        (uint8_t *)conf.passphrase , strlen(conf.passphrase) , 
        (uint8_t *)conf.unique_id , strlen(conf.unique_id),
        (uint8_t *)conf.info , strlen(conf.info),
        aes_key , sizeof(aes_key)
    );

    db_init();
    // //test
    // for(int i = 0 ; i < 100 ; i ++) {
    //     hkdf(
    //         (uint8_t *)conf.passphrase , strlen(conf.passphrase) , 
    //         (uint8_t *)conf.unique_id , strlen(conf.unique_id),
    //         (uint8_t *)conf.info , strlen(conf.info),
    //         test_aes_key , sizeof(test_aes_key)
    //     );

    //     assert(memcmp(aes_key, test_aes_key, sizeof(aes_key)) == 0);
    // }
}


int cprng(uint8_t *buf, size_t len)
{
    if (RAND_bytes(buf, len) != 1) {
        // Error occurred during random number generation
        return -1;
    }
    return 0;
}

// user_db변수 초기화.
void db_new(void) {
    if(user_db != NULL) {
        free(user_db);
        user_db = NULL;
    }

    size_t size = sizeof(header) + MAX_USER * sizeof(member);
    user_db = (header*)malloc(size);
    user_db->magic = USER_DB_MAGIC;
    user_db->user_id = START_USER_ID;
    cprng(user_db->iv + sizeof(uint64_t) , sizeof(user_db->iv) - sizeof(uint64_t));
}

int db_export(){
    if(user_db == NULL) {
        return -1;
    }

    //iv 업데이트
    *(uint64_t*)user_db->iv = *(uint64_t*)user_db->iv + 1;

    //암호화
    uint8_t *enc;
    size_t size = sizeof(header) + MAX_USER * sizeof(member);
    enc = (uint8_t*)malloc(size); // +패딩?

    memset(enc , 0 , size);
    memcpy(enc , user_db , sizeof(header));

    size_t enc_off = offsetof(header, user_id); //암호화 하지 않을 offset
    size_t out_len = size - enc_off;

    int res = aes128_cbc_encrypt((uint8_t*)user_db + enc_off , out_len , //out_len + 16 > *out_len
                                enc + enc_off ,&out_len , 
                                (uint8_t*)aes_key , (uint8_t*)user_db->iv);

    if(res != 0) {
        fprintf(stderr, "db encrypt error.\n");
        free(enc);
        return -2;
    }

    //hmac업데이트
    size_t iv_offs = offsetof(header,iv);
    hmac_sha256(aes_key, AES128_KEY_LEN ,
                enc + iv_offs , size - iv_offs ,
                enc + offsetof(header , hmac));

    res = write_user(enc , size);
    if (res != 0) {
        fprintf(stderr, "write user db error, maybe user doesn't have permission to write file\n");
        free(enc);
        exit(1);    
    }


    free(enc);
    return 0;

}

//db파일 user_db로 읽어옴.
int db_import() {
    uint8_t *enc;
    size_t size = sizeof(header) + MAX_USER * sizeof(member); //패딩을 ?
    int res;
    uint8_t hash[32];

    if (user_db != NULL) {
        free(user_db);
        user_db = NULL;
    }

    res = read_user(&enc , size);
    if(res <= 0) {
        return -1;
    }

    //magic 검사
    header * check_header = (header*)enc;
    if (check_header->magic != USER_DB_MAGIC) {
        fprintf(stderr, "db magin error.\n");
        free(enc);
        return -2;
    }
    //hmac검사 iv~ hash 한 값
    size_t iv_off = offsetof(header , iv);
    hmac_sha256(aes_key, AES128_KEY_LEN , check_header + iv_off , res - iv_off , hash);
    if (memcmp(check_header->hmac , hash , sizeof(hash)) != 0) {
        fprintf(stderr, "db hmac error.\n");
        free(enc);
        return -3;
    }

    //user_db 에 read_user로 읽은 파일 복사 및 복호화
    size_t enc_off = offsetof(header , user_id);
    user_db = (header*)malloc(res);
    memset(user_db , 0 , res);
    memcpy(user_db , enc , enc_off); // 암호화 되지 않은 파일은 그대로 복사.
    size_t out_len = res - (int)enc_off; // 읽어드린 파일 크기에서 암호화 되지 않은 구조체의 크기만큼 뺌. 곧 암호화 된 크기.

    res = aes128_cbc_decrypt(enc + enc_off , out_len ,
                            (uint8_t*)user_db + enc_off, &out_len ,
                            (const uint8_t *)aes_key , user_db->iv);

    if(res != 0) {
        fprintf(stderr, "db decrypt error.\n");
        free(enc);
        free(user_db);
        user_db = NULL;
        return -4;
    }

    free(enc);
    return 0;
}

//user_db로 파일db 가져옴.
int db_init(void)
{
    int i = db_import();
    //printf("db_import : %d\n", i);
    if (i != 0) {
        db_new();
    }
    return 0;
}

// member *db_add_user(char *name , char gender, uint8_t age) {
//     if (name == NULL || user_db == NULL) {
//         return NULL;
//     }

//     //중복 검사

//     for(int i = 0 ; i < MAX_USER ; i ++) {
//         if(user_db->user[i].name[0] == '\0')
//             continue;

//         if (strcmp(name , user_db->user[i].name) == 0)
//     }
// }
int main(void) {
    // db_new();
    // uint64_t res = *(uint64_t*)user_db->iv + 8; //iv[16]중 뒤 8바이트는 cprng()함수로 인해 0이면 에러.
    // if(res == 0) {
    //     fprintf(stderr,"error.\n");
    //     free(user_db);
    //     return 0;
    // }
    
    // res = *(uint64_t*)user_db->iv; // iv[16] 중 앞의 8바이트는 초기화하지않았기때문에 0이여야함. de_new()직후만 한정.
    // if(res != 0) {
    //     fprintf(stderr,"error.\n");
    //     free(user_db);
    //     return 0;
    // }  


    // db_new();
    // printf("%d\n", *(uint64_t*)user_db->iv);
    // *(uint64_t*)user_db->iv = *(uint64_t*)user_db->iv + 1;
    // printf("%d\n", *(uint64_t*)user_db->iv);
    // free(user_db);
    


    init();

    user_db->user[0].name = strdup("park");
    user_db->user[0].age = 27;
    user_db->user[0].gender = MALE;

    db_export();
    free(user_db);
    return 0;
}