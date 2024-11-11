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
//user set = ./db_cli user modify id --name name --age age --gender gender


void db_command(int argc , char * argv[]) {
    if (argc < 1) {
        printf("Usage: %s db <command> [<args>]\n", argv[0]);
        printf("Avaliable sub-commands are:\n");
        printf("   reset\n");
        return ;
    }
    if(!strcmp( argv[2] , "reset") ) {
        if(!remove("/etc/file.db")) {
            printf("Delete db file success.\n");
            return ;
        } else {
            printf("Can not delete db file, maybe user doesn't have permission to delete file\n");
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

void print_user(member * user){
    if(user == NULL) {
        return ;
    }
    printf("%d\t%s\t%d\t%c\n", user->id, user->name, user->age, user->gender);
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
        if(db_export()==0) {
            printf("Create user success: %s\n", user->name);
        }
    }
}

void delete_user(char * name)  {
    if(db_delete_user((const char*)name)) {
        printf("Not found user: %s\n",name);
        return;
    }
    if(db_export() == 0) {
        printf("Delete user: %s\n", name);
    }
}

member* get_user(char * name) {
    member * user = db_find_user_by_name(name);
    if (user == NULL) {
        return NULL;
    }
    return user;
}

void print_modify_usage(void) {
    printf("Usage: ./db_cli user modify <user_ID> [<--option> <parameter>] ...\n");
    printf("Available option are:\n");
    printf("\t--name <name>\n");
    printf("\t--age <age(1~150)>\n");
    printf("\t--gender <gender(M,F)>\n");
}

void modify_option_handle(const unsigned long id , char * option , char * param) {

    int option_name = strcmp(option, "--name");
    int option_age = strcmp(option, "--age");
    int option_gender = strcmp(option, "--gender");
    if(option_name && option_age && option_gender) {
        printf("Invalid option: %s\n", option);
        printf("Available option are:\n");
        printf("\t--name <name>\n");
        printf("\t--age <age(1~150)>\n");
        printf("\t--gender <gender(M,F)>\n");
        return;
    }

    member * bakup_user = db_find_user_by_id((uint32_t)id);
    if (bakup_user == NULL) {
        printf("Unkown ID: %ld\n", id);
        return ;
    }

    if(!option_name){
        char bakup_name[NAME_LEN_MAX];
        strcpy(bakup_name, bakup_user->name); //이전 이름 백업.

        member * user = db_modify_name((uint32_t)id , param);
        if (user != NULL) {
            printf("Modify success.\n");
            printf("\tInfo: ID %ld name %s -> %s\n", id ,bakup_name, user->name);
        } else {
            printf("Modify name failed, somethings wrong.\n");
        }
        return ;
    } 

    if(!option_age){
        uint8_t bakup_age = bakup_user->age; // 이전 나이 백업

        char * endptr;
        const unsigned long age = strtoul(param, &endptr , 10); 
        if (strlen(endptr) != 0) {
            printf("Invalid age: %s[%s?]\n", param, endptr);
            return;
        }
        member * user = db_modify_age((uint32_t)id , age);
        if (user != NULL) {
            printf("Modify success.\n");
            printf("\tInfo: ID %ld age %d -> %d\n", id ,bakup_age, user->age);
        } else {
            printf("Modify age failed, somethings wrong.\n");
        }
        return ;
    } 

    if(!option_gender){
        char bakup_gender = bakup_user->gender; // 이전 성별 백업
        if(strcmp("M", param) && strcmp("F", param)) {
            printf("Invalid gender: 'M'(male) or 'F'(female) required.\n");
            return ;
        }
        member * user = db_modify_gender((uint32_t)id , param[0]);
        if (user != NULL) {
            printf("Modify success.\n");
            printf("\tInfo: ID %ld gender %c -> %c\n", id ,bakup_gender, user->gender);
        } else {
            printf("Modify age failed, somethings wrong.\n");
        }
        return ;
    } 
}
void db_user(int argc , char * argv[]) {
    if (argc < 1) {
        printf("Usage: %s user <command> [<args>]\n", argv[0]);
        printf("Available sub-commands are:\n");
        printf("   list\n");
        printf("   add <user_name> <user_age> <user_gender>\n");
        printf("   delete <user_name>\n");
        printf("   get <user_name>\n");
        printf("   modify <user_ID> --name <name> --age <age(1~150)> --gender <gender(M,F)>\n");
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
    } else if(!strcmp(argv[2], "get")) {
        if(argc < 2) {
            printf("Usage: %s %s %s <user_name> <user_name> ...\n", argv[0],argv[1],argv[2]);
            return;
        }
        //printf("%d\n",argc);
        if(argc-1 > MAX_USER) { 
            printf("Invalid argument, max user: %d\n", MAX_USER);
            return ;
        }
        char unknown_names[NAME_LEN_MAX][MAX_USER];
        int cnt = 0;
        printf("ID\tname\tage\tgender\n");
        for(int i = 0; i < argc -1 ; i ++) {
            member* user = get_user(argv[i+3]);
            if (user == NULL) {
                strcpy(unknown_names[cnt++] , argv[i+3]);
            } else {
                printf("%d\t%s\t%d\t%c\n", user->id , user->name, user->age, user->gender);
            }
        }
        for(int i = 0 ; i < cnt ; i ++) {
            printf("User %s not found.\n",unknown_names[i]);
        }
    } else if(!strcmp(argv[2], "modify")) {
        //printf("argc: %d\n", argc);
        if(argc < 4) {
            print_modify_usage();
            return;
        }

        char * endptr;
        const unsigned long id = strtoul(argv[3], &endptr , 10); //argv[3] = id
        if(strlen(endptr) != 0) {
            printf("Invalid user ID: ID is only number. %s[%s?]\n", argv[3],endptr);
            return ;
        }
        switch(argc) {
            case 8:
                modify_option_handle(id , argv[4], argv[5]);
                modify_option_handle(id , argv[6], argv[7]);
                modify_option_handle(id , argv[8], argv[9]);
                break;
            case 6:
                modify_option_handle(id , argv[4], argv[5]);
                modify_option_handle(id , argv[6], argv[7]);
                break;
            case 4:
                modify_option_handle(id , argv[4], argv[5]);
                break;
            default:
                print_modify_usage();
        }
    } else {
        printf("Unkown sub-command: %s\n", argv[2]);
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