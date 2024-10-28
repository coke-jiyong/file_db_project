#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sys_utils.h"
#include "user_db.h"

const char *db_filename = "/home/ubuntu/mytmp/mytest/flexibleMemberTest/filedb.db";
static char unique_id[24]; 

static char *get_default_interface()
{
    FILE *f;
    char line[100], *p, *c, *g, *saveptr;
    char *iface = NULL;

    f = fopen("/proc/net/route", "r");

    while (fgets(line, 100, f))
    {
        p = strtok_r(line, " \t", &saveptr);
        c = strtok_r(NULL, " \t", &saveptr);
        g = strtok_r(NULL, " \t", &saveptr);

        if (p != NULL && c != NULL)
        {
            if (strcmp(c, "00000000") == 0)
            {
                // Default route found
                if (iface != NULL)
                {
                    free(iface);
                }
                iface = strdup(p);
                break;
            }
        }
    }
    fclose(f);
    return iface;
}


static void fill_unique_id(void)
{
    int fd;
    struct ifreq ifr;
    char *iface = get_default_interface();
    unsigned char *mac;

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, iface, IFNAMSIZ - 1);
    free(iface);

    ioctl(fd, SIOCGIFHWADDR, &ifr);

    close(fd);

    mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;
    // printf("%s\n",mac);
    sprintf(unique_id, "%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

static header_conf config = {
    .unique_id = NULL,
    .passphrase = "AES key for encrypting user database is made from this passphrase and unique_id",
    .info = "My file db project",
};

void init_config(header_conf * conf){
    fill_unique_id();
    config.unique_id = unique_id;
    *conf = config;
}

int write_user(const uint8_t *db, size_t db_len) {
    char temp_file[strlen(db_filename) + 20];
    int fd;

    sprintf(temp_file , "%s.XXXXXX", db_filename);
    fd = mkstemp(temp_file); //템프파일생성 XXXXXXX 는 고유한 랜덤문자열로 치환됨.
    if (fd == -1) {
        fprintf(stderr, "make temp file failed.\n");
        return -1;
    }

    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; //rw-r--r-- 로 파일권한 설정
    if (fchmod(fd, mode) == -1) {
        fprintf(stderr, "change tempfile permission failed.\n");
        close(fd);
        return -1;
    }

    FILE *file = fdopen(fd, "wb");
    if (file == NULL) {
        fprintf(stderr, "open tempfile failed.\n");        
        close(fd);
        return -1;
    }

    fwrite(db , db_len , 1 , file);

    if (fsync(fd) != 0) {
        fprintf(stderr, "fsync failed.\n"); //fsync함수가 0을 반환하지 않았다는것은 디스크에 파일이 제대로 쓰이지 않았다는 뜻.
        close(fd);
        return -1;
    }
    fclose(file);

    if(rename(temp_file , db_filename) != 0) {
        fprintf(stderr, "tempfile -> db_filename failed.\n");
        return -1;
    }

    return 0;

}

int read_user(uint8_t ** db, size_t db_len){
    FILE *file;
    int file_size;

    file = fopen(db_filename , "rb"); //read바이너리 모드로 파일 오픈
    if(file == NULL) {
        fprintf(stderr , "fopen failed.\n");
        return -1;
    }

    fseek(file , 0 , SEEK_END); //파일포인터를 파일의 마지막으로 옮김.
    file_size = ftell(file); //파일포인터의 현재 포지션. (파일사이즈)
    rewind(file); // 파일포인터 파일의 시작으로 다시 옮김. fseek(file , 0 , SEEK_SET)
    printf("file_size : %d\n", file_size);

    if(file_size > db_len) {
        fprintf(stderr , "db len too small.\n");
        fclose(file);
        return -1;
    }

    *db = (uint8_t*)malloc(db_len);
    if (db == NULL) {
        fprintf(stderr ,"malloc failed.\n");
        fclose(file);
        return -1;
    }

    if(fread(*db , file_size , 1 , file) != 1) {// file_size 바이트크키로 1번 읽음. 성공 시 읽은 횟수 (1) 반환. 
        fprintf(stderr ,"fread failed.\n");
        fclose(file);
        return -1;
    }

    fclose(file);
    return file_size;
}
// int main(void) {
//     header_conf conf;
//     init_config(&conf);
//     printf("%s\n%s\n%s\n" , conf.info, conf.passphrase , conf.unique_id);
// }