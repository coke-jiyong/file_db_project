#include "unity.h"
#include "sys_utils.h"
#include "user_db.h"
void setUp(void) {}

void tearDown(void) {}

void test_init_config(void) {
    header_conf test_conf;
    init_config(&test_conf);
    
    TEST_ASSERT_NOT_EQUAL(0,strlen(test_conf.passphrase));
    TEST_ASSERT_NOT_EQUAL(0,strlen(test_conf.info));
    TEST_ASSERT_NOT_EQUAL(0,strlen(test_conf.unique_id));
}

extern const char * db_filename;

void test_read_write_user(void) {
#define DB_SIZE 10000

    unsigned char dummy[DB_SIZE];
    unsigned char * buf;
    int i ;
    const char * bak = db_filename;
    db_filename = "/home/ubuntu/mytmp/mytest/flexibleMemberTest/build/db/test.db";

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

void hdkf_test(void) {

}

void aes_cbc_test(void) {
    const unsigned char key[16] = "1234567890abcdef";
    const unsigned char iv[16] = "fedcba0987654321";
    unsigned char in[32] = "So Hard Language C ..";
    
    unsigned char enc[32];
    unsigned char dec[32];
    unsigned char enc_data[32] = {0x3d, 0xf6, 0xda, 0xb3, 0x6e, 0x39, 0x52, 0xd6, 0x46, 0x56, 
                                0x16, 0x8a, 0x1e, 0xf0, 0x23, 0x0f, 0x22, 0x90, 0x07, 0x90, 
                                0x4b, 0xa2, 0xa3, 0x28, 0x2c, 0xdf, 0x7f, 0x0d, 0x59, 0xfb, 
                                0x69, 0x8d,}; //"So Hard Language C .." encrypt memory
    size_t out_len = sizeof(enc) + PADDING_SIZE;
    int i = aes128_cbc_encrypt(in , strlen(in), enc, &out_len , key, iv); 
    TEST_ASSERT_EQUAL(0, i);
    TEST_ASSERT_EQUAL(32, out_len);
    TEST_ASSERT_EQUAL_MEMORY(enc_data, enc, sizeof(enc_data));


}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_init_config);
    RUN_TEST(test_read_write_user);
    RUN_TEST(aes_cbc_test);
}