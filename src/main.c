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
//get,delete 도 옵션 설정


void print_user(member * user){
    if(user == NULL) {
        return ;
    }
    printf("|  %d\t%s\t%d\t%c\t|\n", user->id, user->name, user->age, user->gender);
}

extern const char * db_filename;
void db_command(int argc , char * argv[]) {
    if (argc < 1) {
        printf("Usage: %s db <command> [<args>]\n", argv[0]);
        printf("Avaliable sub-commands are:\n");
        printf("   reset\n");
        return ;
    }
    if(!strcmp( argv[2] , "reset") ) {
        if(!remove(db_filename)) {
            printf("Delete db file success.\n");
        } else {
            printf("Can not delete db file.\n");
        }
    } else {
        printf("Unknown sub command %s.\n", argv[2]);
    }
}

void print_user_list(void){        
    int total = db_total_user();
    printf("Total %d users.\n", total);
    if(total == 0) {
        return;
    }
    printf("---------------------------------\n");
    printf("|  ID\tname\tage\tgender  |\n");
    for(int i = 0 ; i < MAX_USER ; i ++) {
        member*user;
        user = db_find_user_by_index(i);
        if(user == NULL) {
            continue;
        } else {
            print_user(user);
        }
    }
    printf("---------------------------------\n");
}

char * check_string_num(char * str, unsigned long* age){
    char * endptr;
    *age = strtoul(str, &endptr , 10);
    if(strlen(endptr) != 0) {
        return endptr;
    }
    return NULL;
}

void add_user(char *argv[]) {
    if(strlen(argv[0]) > 20) {
        printf("Invalid name length(max 20): %s\n",argv[0]);
        return;
    }
    const char * name = argv[0];

    unsigned long age;
    char * check_result = check_string_num(argv[1], &age);
    if(check_result != NULL) {
        printf("Invalid age: %s[%s?]\n", argv[1], check_result);
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

    member * bakup_user = db_find_user_by_id((uint32_t)id);
    if(!strcmp(option, "--name")){
        char bakup_name[NAME_LEN_MAX];
        strcpy(bakup_name, bakup_user->name); //이전 이름 백업.

        member * user = db_modify_name((uint32_t)id , param);
        if (user != NULL) {
            printf("Modify success.\n");
            printf("\tInfo: ID %ld name %s -> %s\n\n", id ,bakup_name, user->name);
        } else {
            printf("Modify name failed, somethings wrong.\n\n");
        }
    } else if(!strcmp(option, "--age")) {
        uint8_t bakup_age = bakup_user->age; // 이전 나이 백업
        unsigned long age;
        char * check_result = check_string_num(param, &age);
        if(check_result != NULL) {
            printf("Invalid age: %s[%s?]\n\n", param, check_result);
            return ;
        }
        if(age > 150) {
            printf("Invalid age(0~150): %ld\n\n", age);
            return ;
        }
        member * user = db_modify_age((uint32_t)id , age);
        if (user != NULL) {
            printf("Modify success.\n");
            printf("\tInfo: ID %ld age %d -> %d\n\n", id ,bakup_age, user->age);
        } else {
            printf("Modify age failed, somethings wrong.\n\n");
        }
    } else if(!strcmp(option, "--gender")) {
        char bakup_gender = bakup_user->gender; // 이전 성별 백업
        if(strcmp("M", param) && strcmp("F", param)) {
            printf("Invalid gender: 'M'(male) or 'F'(female) required.\n\n");
            return ;
        }
        member * user = db_modify_gender((uint32_t)id , param[0]);
        if (user != NULL) {
            printf("Modify success.\n");
            printf("\tInfo: ID %ld gender %c -> %c\n\n", id ,bakup_gender, user->gender);
        } else {
            printf("Modify age failed, somethings wrong.\n\n");
        }
    } else {
        printf("Invalid option: %s\n", option);
        printf("Available option are:\n");
        printf("\t--name <name>\n");
        printf("\t--age <age(1~150)>\n");
        printf("\t--gender <gender(M,F)>\n\n");
    }
}

int check_option(char * param , char ** str) {
    char * ptr = strtok(param , "=");
    if (!strcmp(ptr, "--name")) {
        ptr = strtok(NULL , " ");
        if(strlen(ptr) > NAME_LEN_MAX) { // buf overflow 방지
            printf("Invalid name length(max 20): %s\n",ptr);
            return -1;
        }
        *str = malloc(strlen(ptr)+1);
        strcpy(*str, ptr);
        return 1;
    } else if (!strcmp(ptr , "--age")) {
        return 2;
    } else if (!strcmp(ptr, "--gender")) {
        return 3;
    } else {
        return 0;
    }
}

void print_got_user_by_name(char ** argv , int len) {
    char unknown_names[NAME_LEN_MAX][MAX_USER];
    int cnt = 0;
    printf("---------------------------------\n");
    printf("|  ID\tname\tage\tgender  |\n");
    for(int i = 0; i < len ; i ++) {
        char * name = *(argv + i);
        member* user = get_user(name);
        if (user == NULL) {
            strcpy(unknown_names[cnt++] , name);
        } else {
            print_user(user);
        }
    }
    printf("---------------------------------\n");
}

void print_got_user_by_age(char ** argv , int len) {
    
    unsigned long *ages = (unsigned long*)malloc(sizeof(unsigned long) * len);
    if (ages == NULL) {
        ERROR_MSG("Allocation failed.\n");
        return ;
    }
    // check age strings
    for(int i = 0 ; i < len ; i ++) {
        unsigned long age;
        char * invalid_str = check_string_num(*(argv+i),&age);
        if(invalid_str != NULL) {
            printf("Invalid age: %s[%s?]\n", *(argv+i), invalid_str);
            free(ages);
            return;
        } else if(age > 150) {
            printf("Invalid age(0~150): %ld\n", age);
            return ;
        }
        ages[i] = age;
    }
    printf("---------------------------------\n");
    printf("|  ID\tname\tage\tgender  |\n");
    for(int i = 0 ; i < MAX_USER ; i ++) {
        member * user = db_find_user_by_age((uint8_t)ages[i]);
        if (user != NULL) {
            print_user(user);
        }
    }
    printf("---------------------------------\n");
    free(ages);
    
}

void print_got_user_by_gender(char * gender){
    if (strcmp(gender, "M") && strcmp(gender,"F")) {
        printf("Invalid gender: 'M'(male) or 'F'(female) required.\n");
        return ;
    }
    printf("---------------------------------\n");
    printf("|  ID\tname\tage\tgender  |\n");
    db_print_user_by_gender(gender[0]);
    printf("---------------------------------\n");
}

void db_user(int argc , char * argv[]) {
    if (argc < 1) {
        printf("Usage: %s user <command> [<args>]\n", argv[0]);
        printf("Available sub-commands are:\n");
        printf("   list\n");
        printf("   add <name> <user_age> <user_gender>\n");
        printf("   delete <name>\n");
        printf("   get --name=<name> or --age=<age> or --gender=<gender>\n");
        printf("   modify <user_ID> --name <name> --age <age(1~150)> --gender <gender(M,F)>\n");
        return ;
    }

    if(!strcmp(argv[2], "list")) {
        print_user_list();
    } else if(!strcmp(argv[2], "add")) {
        if (argc < 4) {
            printf("Usage: %s %s %s <name> <user_age(1~150)> <user_gender(M,F)>\n", argv[0], argv[1], argv[2]);
            return ;
        }

        add_user(argv+3);
    } else if(!strcmp(argv[2], "delete")) {
        if(argc < 2) {
            printf("Usage: %s %s %s <name>\n", argv[0],argv[1],argv[2]);
            return;
        }

        delete_user(argv[3]);
    } else if(!strcmp(argv[2], "get")) {
        if(argc < 2) {
            printf("Usage: ./db_lic user get --name=<name> <name> ...\n");
            printf("\t\t\t --age=<age> <age> ...\n");
            printf("\t\t\t --gender=<gender>\n");
            return;
        }
        //printf("%d\n",argc);
        if(argc-1 > MAX_USER) { 
            printf("Invalid argument, max user: %d\n", MAX_USER);
            return ;
        }
        char * p = strtok(argv[3], "=");
        if (!strcmp(p,"--name")) {
            p = strtok(NULL," ");
            if(p == NULL) {
                printf("Usage: ./db_lic user get --name=<name> <name> ...\n");
                return;
            }
            strcpy(argv[3], p);
            print_got_user_by_name(argv+3, argc-1);
        } else if(!strcmp(p,"--age")) {
            p = strtok(NULL," ");
            if(p == NULL) {
                printf("Usage: ./db_lic user get --age=<age> <age> ...\n");
                return;
            }
            strcpy(argv[3], p);
            print_got_user_by_age(argv+3, argc-1);
        } else if (!strcmp(p , "--gender")) {
            p = strtok(NULL," ");
            if(p == NULL) {
                printf("Usage: ./db_lic user get --gender=<gender>\n");
                return;
            }
            print_got_user_by_gender(p);
        } else {
            printf("Invalid option :%s\n", p);
        }
        
    
    } else if(!strcmp(argv[2], "modify")) {
        //printf("argc: %d\n", argc);
        if(argc < 4) {
            print_modify_usage();
            return;
        }
        unsigned long id;
        char * check_result = check_string_num(argv[3], &id);
        if(check_result != NULL) {
            printf("Invalid user ID: ID is only number. %s[%s?]\n", argv[3],check_result);
            return ;
        }
        //id check
        member * user = db_find_user_by_id((uint32_t)id);
        if (user == NULL) {
            printf("Unknown ID: %ld\n", id);
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
        printf("Unknown sub-command: %s\n", argv[2]);
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

    if(init() == -1) {
        fprintf(stderr , "Initialize failed.\n");
        clear_mem();
        exit(1);
    }

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