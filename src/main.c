#include <stdio.h>
#include "main_utils.h"
//db reset
//user add
//user delete
//user list
//완료


//user get
//user set = ./db_cli user modify id --name name --age age --gender gender
//get,delete 도 옵션 설정
void main(int argc , char* argv[]) {
    // printf("--------------------\n");
    // printf("argc:%d\n", argc);
    // for(int i = 0 ; i < argc ; i ++) {
    //     printf("%s\n", argv[i]);
    // }
    // printf("--------------------\n");

    if (argc < 2) {
        printf("Usage: %s <command> [<args>]\n", argv[0]);
        printf("Avaliable commands are:\n");
        printf("   db\n");
        printf("   user\n");
        return ;
    }

    if(init() == -1) {
        fprintf(stderr , "Initialize failed.\n");
        clear_mem();
        return;
    }

    if(!strcmp(argv[1] , "db")) {
        db_command(argc - 2, argv);
    } else if(!strcmp(argv[1] , "user")) {
        db_user(argc - 2, argv);
    } else {
        printf("Unknown command : %s\n", argv[1]);
    }
    clear_mem();
    return ;
}