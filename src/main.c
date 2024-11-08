#include <stdio.h>
#include "hkdf.h"
#include "sys_utils.h"
#include "user_db.h"
//db reset
//user add
//user delete
//user list
//완료


//user get
//user set = ./db_cli user set age=17/name=park

void db_command(int argc , char * argv[]) {
    if (argc < 1) {
        printf("Usage: %s db <command> [<args>]\n", argv[0]);
        printf("Avaliable sub-commands are:\n");
        printf("   reset\n");
        return ;
    }
    if(!strcmp( argv[2] , "reset") ) {
        if(!remove("/home/ubuntu/file_db_project/build/db/file.db")) {
            printf("Delete db file success.\n");
            return ;
        } else {
            printf("Can not delete db file.\n");
            return ;
        }
    } else {
        printf("Unkown sub command %s.\n", argv[2]);
        return ;
    }
}

void print_user_list(void){        
    int total = db_total_user();
    printf("Total %d users.\n", total);
    if(total == 0) {
        return;
    }
    printf("ID\tname\tage\tgender\n");
    for(int i = 0 ; i < MAX_USER ; i ++) {
        member*user;
        user = db_find_user_by_index(i);
        if(user == NULL) {
            continue;
        } else {
            printf("%d\t%s\t%d\t%c\n", user->id, user->name, user->age, user->gender);
        }
    }
}

void add_user(char *argv[]) {
    if(strlen(argv[0]) > 20) {
        printf("Invalid name length(max 20): %s\n",argv[0]);
        return;
    }
    const char * name = argv[0];
    char * endptr;
    const unsigned long age = strtoul(argv[1], &endptr , 10);
    if (strlen(endptr) != 0) {
        printf("Invalid age: %s[%s?]\n", argv[1], endptr);
        return ;
    }
    if(age > 150) {
        printf("Invalid age(0~150): %ld\n", age);
        return ;
    }
    if(strcmp(argv[2], "M") && strcmp(argv[2], "F")) {
        printf("Invalid gender: 'M'(male) or 'F'(female) required.\n");
        return;
    }
    const char gender = argv[2][0];
    member* user = db_add_user(name, gender, age);
    if(user == NULL) {
        printf("Error occured: failed input user_db.\n");
    } else {
        printf("Create user success: %s\n", name);
        db_export();
    }
}

void delete_user(char * name)  {
    if(db_delete_user((const char*)name)) {
        printf("Not found user: %s\n",name);
        return;
    }
    printf("Delete user: %s\n", name);
    db_export();
}

void db_user(int argc , char * argv[]) {
    if (argc < 1) {
        printf("Usage: %s user <command> [<args>]\n", argv[0]);
        printf("Avaliable sub-commands are:\n");
        printf("   list\n");
        printf("   add <user_name> <user_age> <user_gender>\n");
        printf("   delete <user_name>\n");
        printf("   get <user_name>\n");
        printf("   update <user_name> -age <age(1~150)> -gender <gender(M,F)>\n");
        return ;
    }

    if(!strcmp(argv[2], "list")) {
        print_user_list();
    } else if(!strcmp(argv[2], "add")) {
        if (argc < 4) {
            printf("Usage: %s %s %s <user_name> <user_age(1~150)> <user_gender(M,F)>\n", argv[0], argv[1], argv[2]);
            return ;
        }
        add_user(argv+3);
    } else if(!strcmp(argv[2], "delete")) {
        if(argc < 2) {
            printf("Usage: %s %s %s <user_name>\n", argv[0],argv[1],argv[2]);
            return;
        }
        delete_user(argv[3]);
    }
}

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

    init();

    if(!strcmp(argv[1] , "db")) {
        db_command(argc - 2, argv);
    } else if(!strcmp(argv[1] , "user")) {
        db_user(argc - 2, argv);
    } else {
        printf("Unknown command : %s\n", argv[1]);
        clear_mem();
        return ;
    }
    clear_mem();
    return ;
}