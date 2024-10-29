//#include "user_db.h"
#ifndef SYS_UTILS_H
#define SYS_UTILS_H
#include <stdint.h>
typedef struct {
    char * unique_id;
    char * passphrase;
    char * info;
} header_conf;

// init() 함수에서 사용할 정보 저장.
void init_config(header_conf * conf);
int read_user(uint8_t ** db, size_t db_len);
int write_user(const uint8_t *db, size_t db_len);

#endif