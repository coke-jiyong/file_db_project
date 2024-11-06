#include "unity.h"
#include "sys_utils.h"
#include "user_db.h"
#include "hkdf.h"
#include <string.h>
#include <sys/stat.h>
#include <openssl/evp.h>
#include <openssl/kdf.h>
#include <openssl/hmac.h>
void setUp(void) {}

void tearDown(void) {}

// static void openssl_hmac_sha256(const unsigned char *key, int key_len, const unsigned char *data, size_t data_len, unsigned char hash[32])
// {
//     unsigned int len = 32;
//     HMAC_CTX *ctx = HMAC_CTX_new();

//     HMAC_Init_ex(ctx, key, key_len, EVP_sha256(), NULL);
//     HMAC_Update(ctx, data, data_len);
//     HMAC_Final(ctx, hash, &len);
//     HMAC_CTX_free(ctx);
// }

static void openssl_hmac_sha256(const unsigned char *key, int key_len, const unsigned char *data, size_t data_len, unsigned char hash[32])
{   
    if ((key == NULL && key_len != 0) || (key != NULL && key_len == 0)) {
        fprintf(stderr, "Invalid key or key length\n");
        return;
    }

    unsigned int len = 32;
    HMAC_CTX *ctx = HMAC_CTX_new();
    if (ctx == NULL) {
        fprintf(stderr , "HMAC_CTX_new() failed.");
        return;
    }
    if (HMAC_Init_ex(ctx, key, key_len, EVP_sha256(), NULL) != 1) {
        fprintf(stderr , "HMAC_Init_ex() failed.");
        HMAC_CTX_free(ctx);
        return;
    }
    
    if (HMAC_Update(ctx, data, data_len) != 1) {
        fprintf(stderr , "HMAC_Update() failed.");
        HMAC_CTX_free(ctx);
        return;
    }
    
    if (HMAC_Final(ctx, hash, &len) != 1) {
        fprintf(stderr , "HMAC_Final() failed.");
        HMAC_CTX_free(ctx);
        return;
    }

    HMAC_CTX_free(ctx);
}

static int openssl_hkdf(const unsigned char *initial_key, size_t initial_key_len,
                        const unsigned char *salt, size_t salt_len,
                        const unsigned char *info, size_t info_len,
                        unsigned char *out, size_t out_len)
{
    EVP_PKEY_CTX *pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_HKDF, NULL);

    if (EVP_PKEY_derive_init(pctx) <= 0)
        return -1;
    if (EVP_PKEY_CTX_set_hkdf_md(pctx, EVP_sha256()) <= 0)
        return -1;
    if (EVP_PKEY_CTX_set1_hkdf_salt(pctx, salt, salt_len) <= 0)
        return -1;
    if (EVP_PKEY_CTX_set1_hkdf_key(pctx, initial_key, initial_key_len) <= 0)
        return -1;
    if (EVP_PKEY_CTX_add1_hkdf_info(pctx, info, info_len) <= 0)
        return -1;
    if (EVP_PKEY_derive(pctx, out, &out_len) <= 0)
        return -1;

    EVP_PKEY_CTX_free(pctx);

    return 0; // Success
}

int evp_aes128_cbc_encrypt(const unsigned char *in, size_t in_len, unsigned char *out, size_t *out_len, const unsigned char key[16], const unsigned char iv[16])
{
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    int len;
    int ciphertext_len;
    EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv);
    EVP_EncryptUpdate(ctx, out, &len, in, in_len);
    ciphertext_len = len;
    EVP_EncryptFinal_ex(ctx, out + len, &len);
    ciphertext_len += len;
    EVP_CIPHER_CTX_free(ctx);
    *out_len = ciphertext_len;
    return 0;
}

void test_init_config(void) {
    
    init_config(&conf);
    
    TEST_ASSERT_NOT_EQUAL(0,strlen(conf.passphrase));
    TEST_ASSERT_NOT_EQUAL(0,strlen(conf.info));
    TEST_ASSERT_NOT_EQUAL(0,strlen(conf.unique_id));

}

extern const char * db_filename;

void test_read_write_user(void) {
#define DB_SIZE 10000

    unsigned char dummy[DB_SIZE];
    unsigned char * buf;
    int i ;
    const char * bak = db_filename;
    db_filename = "/home/ubuntu/file_db_project/build/db/test.db";

    remove(db_filename);

    buf = NULL;
    i = read_user(&buf , DB_SIZE); //-1 반환되어야 함. fopen failed. 출력
    TEST_ASSERT_EQUAL(-1 , i);
    TEST_ASSERT_EQUAL(NULL , buf);

    cprng(dummy, sizeof(dummy));
    i = write_user(dummy , sizeof(dummy));
    TEST_ASSERT_EQUAL(0 , i);

    i = read_user(&buf, DB_SIZE -1); //db len too small. 출력.
    TEST_ASSERT_EQUAL(-1 , i);

    i = read_user(&buf , DB_SIZE); // 성공
    TEST_ASSERT_EQUAL(10000, i);
    TEST_ASSERT_NOT_EMPTY(buf);

    TEST_ASSERT_EQUAL_MEMORY(dummy, buf, DB_SIZE);
    free(buf);

    db_filename = bak;
}

void test_hamc(void) {

    const unsigned char *key_short = "keyshort";
    unsigned char key_long[128];
    const unsigned char *data = "dummy data short";
    unsigned char hash1[32];
    unsigned char hash2[32];

    cprng(key_long, sizeof(key_long));

    hmac_sha256(key_short, strlen(key_short), data, strlen(data), hash1);
    openssl_hmac_sha256(key_short, strlen(key_short), data, strlen(data), hash2);
    TEST_ASSERT_EQUAL_MEMORY(hash1, hash2, sizeof(hash1));

    hmac_sha256(key_long, strlen(key_long), data, strlen(data), hash1);
    openssl_hmac_sha256(key_long, strlen(key_long), data, strlen(data), hash2);
    TEST_ASSERT_EQUAL_MEMORY(hash1, hash2, sizeof(hash1));
}

void test_hkdf(void) {
    unsigned char key[] = "test key";
    unsigned char salt[] = "test salt";
    unsigned char info[] = "test info";
    unsigned char out1[16];
    unsigned char out2[16];
    hkdf(key , strlen(key), salt , strlen(salt), info , strlen(info) , out1 , sizeof(out1));
    openssl_hkdf(key , strlen(key), salt , strlen(salt), info , strlen(info) , out2 , sizeof(out2));
    TEST_ASSERT_EQUAL_MEMORY(out1 , out2 , sizeof(out1));

}


void test_aes_cbc(void) {
    const unsigned char key[16] = "1234567890abcdef";
    const unsigned char iv[16] = "fedcba0987654321";
    unsigned char in[32] = "So Hard Language C ..";
    
    unsigned char enc[32];
    unsigned char dec[32];
    unsigned char enc_data[32] = {0x3d, 0xf6, 0xda, 0xb3, 0x6e, 0x39, 0x52, 0xd6, 0x46, 0x56, 
                                0x16, 0x8a, 0x1e, 0xf0, 0x23, 0x0f, 0x22, 0x90, 0x07, 0x90, 
                                0x4b, 0xa2, 0xa3, 0x28, 0x2c, 0xdf, 0x7f, 0x0d, 0x59, 0xfb, 
                                0x69, 0x8d,}; //"So Hard Language C .." encrypt memory
    
    //fail required. out_len 패딩 필요.
    size_t out_len = sizeof(enc);
    int i = aes128_cbc_encrypt(in , strlen(in), enc, &out_len , key, iv); 
    TEST_ASSERT_EQUAL(-1, i); 

    memset(enc,0,sizeof(enc));
    out_len = sizeof(enc) + PADDING_SIZE;
    //success required.
    i = aes128_cbc_encrypt(in , strlen(in), enc, &out_len , key, iv); 
    TEST_ASSERT_EQUAL(0, i); 
    TEST_ASSERT_EQUAL(32, out_len);
    TEST_ASSERT_EQUAL_MEMORY(enc_data, enc, sizeof(enc_data));

    memset(enc,0,sizeof(enc));
    evp_aes128_cbc_encrypt(in, strlen(in), enc , &out_len, key , iv);
    TEST_ASSERT_EQUAL_MEMORY(enc_data, enc, sizeof(enc_data));
    
    i = aes128_cbc_decrypt(enc, out_len, dec, &out_len, key, iv);
    TEST_ASSERT_EQUAL(0 , i);
    TEST_ASSERT_EQUAL(strlen(in), out_len);
    TEST_ASSERT_EQUAL_MEMORY(in, dec, out_len);

    memset(enc , 0, sizeof(enc));
    char in2[16] ;
    cprng(in2 , sizeof(in2));
    out_len = sizeof(enc) + 1;
    i = aes128_cbc_encrypt(in2 , sizeof(in2), enc , &out_len , key, iv);
    TEST_ASSERT_EQUAL(0 , i);
    TEST_ASSERT_EQUAL(32, out_len);
    
    unsigned char enc2[32];
    out_len = sizeof(enc2) + 1;
    evp_aes128_cbc_encrypt(in2 , sizeof(in2), enc2, &out_len, key, iv);
    TEST_ASSERT_EQUAL(32, out_len);
    TEST_ASSERT_EQUAL_MEMORY(enc, enc2, sizeof(enc));

    memset(dec , 0 , sizeof(dec));
    i = aes128_cbc_decrypt(enc , out_len, dec, &out_len , key, iv);
    TEST_ASSERT_EQUAL(0, i);
    TEST_ASSERT_EQUAL(sizeof(in2), out_len);
    TEST_ASSERT_EQUAL_MEMORY(in2 , dec , out_len);
}

extern header *user_db ;

void test_db_new(void) {
    db_new();
    TEST_ASSERT_NOT_NULL(user_db);
    TEST_ASSERT_EQUAL(USER_DB_MAGIC,user_db->magic);
    TEST_ASSERT_EQUAL( START_USER_ID, user_db->user_id);
    TEST_ASSERT_EQUAL(0, *(uint64_t*)user_db->iv);
    TEST_ASSERT_NOT_EQUAL(0, *(user_db->iv + sizeof(uint64_t)));
    free(user_db);
    user_db = NULL;
    TEST_ASSERT_NULL(user_db);
}

void test_db_import_export(void) {

    // hkdf(
    //     (uint8_t *)conf.passphrase , strlen(conf.passphrase) , 
    //     (uint8_t *)conf.unique_id , strlen(conf.unique_id),
    //     (uint8_t *)conf.info , strlen(conf.info),
    //     aes_key , sizeof(aes_key)
    // );
    //printf("%s\n", aes_key);
    const char *bak = db_filename;
    db_filename = "/home/ubuntu/file_db_project/build/db/test.db";
    remove(db_filename);
    TEST_ASSERT_EQUAL(-1 , db_import());

    size_t size = sizeof(header) + sizeof(member) * MAX_USER;
    uint8_t * buf = (uint8_t*)malloc(size+16);
    memset(buf , 0 , size+16);
    write_user(buf,size); //test db에 파일 쓰기
    TEST_ASSERT_EQUAL(-2 , db_import()); //db magic error 출력

    header* h = (header*)buf;
    h->magic = USER_DB_MAGIC;
    write_user(buf,size); //test db에 파일 쓰기
    TEST_ASSERT_EQUAL(-3 , db_import()); //db hmac error 출력
    
    if(user_db) {
        free(user_db);
        user_db = NULL;
    }
    TEST_ASSERT_EQUAL(-1 , db_export()); // NULL error
    db_new();
    TEST_ASSERT_NOT_EQUAL(NULL, user_db);
    TEST_ASSERT_EQUAL(USER_DB_MAGIC, user_db->magic);
    TEST_ASSERT_EQUAL(START_USER_ID, user_db->user_id);
    TEST_ASSERT_EQUAL(0 , *(uint64_t*)user_db->iv);
    TEST_ASSERT_NOT_EQUAL(0 , *(uint64_t*)(user_db->iv + sizeof(uint64_t)));

    
    //printf("%d\n", st.st_size);
    TEST_ASSERT_EQUAL(0, db_export()); //<- 이부분에서 st_size가 292
    TEST_ASSERT_EQUAL(1, *(uint64_t*)user_db->iv);
    TEST_ASSERT_EQUAL(0, db_export());
    TEST_ASSERT_EQUAL(2, *(uint64_t*)user_db->iv);

    memset(buf , 0 , size);
    memcpy(buf , user_db, size); //buf 에 user_db백업 후 db_import
    TEST_ASSERT_EQUAL(0, db_import());
    TEST_ASSERT_EQUAL_MEMORY(buf + sizeof(header) , user_db->user, sizeof(member)* MAX_USER);
    TEST_ASSERT_EQUAL_MEMORY(buf + offsetof(header, iv) , user_db->iv , sizeof(user_db->iv));
    TEST_ASSERT_EQUAL_MEMORY(buf + offsetof(header, user_id) , &user_db->user_id , sizeof(user_db->user_id)); 
    //주의! user_db->user_id 로 비교하면 segmentation error.
    //메모리 비교하는데 user_db->user_id 로 비교하면 값이 비교됨. TEST_ASSERT_EQUAL(1, user_db->user_id);
    
    
    struct stat st;
    stat(db_filename, &st);
    FILE *fp = fopen(db_filename , "rb");
    TEST_ASSERT_NOT_EQUAL(NULL, fp);
    fread(buf, 1, st.st_size ,fp);
    fclose(fp);
    
    TEST_ASSERT_EQUAL(USER_DB_MAGIC, h->magic); //header * h = (header*)buf;
    TEST_ASSERT_EQUAL(2, *(uint64_t*)h->iv);

    // hkdf(
    //     (uint8_t *)conf.passphrase , strlen(conf.passphrase) , 
    //     (uint8_t *)conf.unique_id , strlen(conf.unique_id),
    //     (uint8_t *)conf.info , strlen(conf.info),
    //     aes_key , sizeof(aes_key)
    // );
    
    //hmac 검증
    uint8_t hash[32];
    hmac_sha256(aes_key, AES128_KEY_LEN , buf + offsetof(header , iv), st.st_size - offsetof(header , iv), hash);
    TEST_ASSERT_EQUAL_MEMORY(hash , buf + offsetof(header, hmac) , sizeof(hash)); 
    //첫 export 시 aes_key를 생성하지 않았기때문에 이전 export에서 빈 메모리를 key로 hmac update. 그래서 에러가 나는거였음.
    

    free(buf);
    free(user_db); //hmac 검사까지 완료돼야 free() 가능
    user_db = NULL; // NULL 처리하지 않으면 db_import 시 double free
    db_filename = bak;
}

void test_db_user(void) {
    const char * bak = db_filename;
    db_filename = "/home/ubuntu/file_db_project/build/db/test.db";

    remove(db_filename);
    TEST_ASSERT_EQUAL(-1 ,db_import());
    init();

    //test add_user
    for(int i = 0 ; i < MAX_USER ; i ++) {
        char name[20];
        sprintf(name , "User%d", i);
        member * user;
        if(i % 2 ==0) {
            user = db_add_user(name, MALE , i + 20);    
        } else {
            user = db_add_user(name, FEMALE , i + 20);    
        }
        TEST_ASSERT_NOT_EQUAL(NULL, user);
        TEST_ASSERT_EQUAL(START_USER_ID + i , user->id);
        TEST_ASSERT_EQUAL_STRING(name , user->name);
        TEST_ASSERT_EQUAL_PTR(&user_db->user[i] , user);
        TEST_ASSERT_EQUAL(i + 1, db_total_user());
    }
    TEST_ASSERT_EQUAL(NULL , db_add_user("User10", MALE , 30));

    //test delete_user
    TEST_ASSERT_EQUAL(0 , db_delete_user("User5"));
    TEST_ASSERT_EQUAL(-1 , db_delete_user("User5"));
    TEST_ASSERT_EQUAL(MAX_USER-1 , db_total_user());

    TEST_ASSERT_EQUAL(0 , db_delete_user("User0"));
    TEST_ASSERT_EQUAL(MAX_USER-2 , db_total_user());

    TEST_ASSERT_EQUAL(-1 , db_delete_user("User15"));
    TEST_ASSERT_EQUAL(MAX_USER-2 , db_total_user());

    // test find
    for(int i = 0 ; i < MAX_USER ; i ++) {
        char name[20];
        sprintf(name , "User%d", i);

        member * user = db_find_user_by_name(name);
        if (i == 0 || i == 5) {
            TEST_ASSERT_EQUAL(NULL, user);
        } else {
            TEST_ASSERT_NOT_EQUAL(NULL , user);
            TEST_ASSERT_EQUAL_PTR(&user_db->user[i], user);
        }

        user = db_find_user_by_id(START_USER_ID + i);
        if (i == 0 || i == 5) {
            TEST_ASSERT_EQUAL(NULL, user);
        } else {
            TEST_ASSERT_NOT_EQUAL(NULL , user);
            TEST_ASSERT_EQUAL_PTR(&user_db->user[i], user);
        }
    }

    size_t size = sizeof(header) + sizeof(member) * MAX_USER;
    uint8_t * buf = (uint8_t *)malloc(size);
    TEST_ASSERT_NOT_EQUAL(NULL, buf);
    memcpy(buf, user_db, size); //buf 에 memcpy 후 export , import

    TEST_ASSERT_EQUAL(0 , db_export());
    TEST_ASSERT_EQUAL(0 , db_import());
    size_t offset = offsetof(header, user_id);
    TEST_ASSERT_EQUAL_MEMORY(buf + offset , &user_db->user_id, size-offset);
    TEST_ASSERT_EQUAL(*(uint32_t*)(buf + offset) , user_db->user_id);
    TEST_ASSERT_EQUAL(0 , db_export());
    free(user_db);
    user_db = NULL;
    TEST_ASSERT_EQUAL(0 , db_import());
    TEST_ASSERT_EQUAL_MEMORY(buf + offset , &user_db->user_id, size-offset);

    free(buf);
    free(user_db);
    user_db = NULL;
    db_filename = bak;
}


int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_init_config);
    RUN_TEST(test_read_write_user);
    RUN_TEST(test_hamc);
    RUN_TEST(test_hkdf);
    RUN_TEST(test_aes_cbc);
    
    RUN_TEST(test_db_new);
    RUN_TEST(test_db_import_export);
    RUN_TEST(test_db_user);
    return UNITY_END();
}