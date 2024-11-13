#include "main_utils.h"

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

void print_user_list(void) {        
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

char * check_string_num(char * str, unsigned long* age) {
    char * endptr;
    *age = strtoul(str, &endptr , 10);
    if(strlen(endptr) != 0) {
        return endptr;
    }
    return NULL;
}

int validate_age(unsigned long age) {
    if(age > 150 || age < 1) {
        return 0;
    }
    return 1;
}

int validate_name(char * name) {
    if(strlen(name) > NAME_LEN_MAX) {
        return 0;
    }
    return 1;
}

int validate_gender(char * gender) {
    if(strcmp(gender, "M") && strcmp(gender , "F")) {
        return 0;
    }
    return 1;
}

void add_user(char *argv[]) {
    if(!validate_name(argv[0])) {
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

    if(!validate_age(age)) {
        printf("Invalid age(1~150): %ld\n", age);
        return ;
    }

    if(!validate_gender(argv[2])) {
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
    if(!validate_name(name)) {
        printf("Invalid name length(max 20): %s\n",name);
        return ;
    }
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

void modify_name(const unsigned long id , char * param) {
    if(!validate_name(param)) {
        printf("Invalid name length(max 20): %s\n",param);
        return;
    }
    member * bakup_user = db_find_user_by_id((uint32_t)id);
    char bakup_name[NAME_LEN_MAX];
    strcpy(bakup_name, bakup_user->name); //이전 이름 백업.

    member * user = db_modify_name((uint32_t)id , param);
    if (user != NULL) {
        printf("Modify success.\n");
        printf("\tInfo: ID %ld name %s -> %s\n\n", id ,bakup_name, user->name);
    } else {
        printf("Modify name failed, somethings wrong.\n\n");
    }
}   

void modify_age(const unsigned long id , char * param) {
    member * bakup_user = db_find_user_by_id((uint32_t)id);
    uint8_t bakup_age = bakup_user->age; // 이전 나이 백업
    unsigned long age;
    char * check_result = check_string_num(param, &age);
    if(check_result != NULL) {
        printf("Invalid age: %s[%s?]\n\n", param, check_result);
        return ;
    }
    if(!validate_age(age)) {
        printf("Invalid age(1~150): %ld\n\n", age);
        return ;
    }
    member * user = db_modify_age((uint32_t)id , age);
    if (user != NULL) {
        printf("Modify success.\n");
        printf("\tInfo: ID %ld age %d -> %d\n\n", id ,bakup_age, user->age);
    } else {
        printf("Modify age failed, somethings wrong.\n\n");
    }
}   

void modify_gender(const unsigned long id , char * param) {
    member * bakup_user = db_find_user_by_id((uint32_t)id);
    char bakup_gender = bakup_user->gender; // 이전 성별 백업
    if(!validate_gender(param)) {
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
}   

void modify_option_handle(const unsigned long id , char * option , char * param) {
    if(!strcmp(option, "--name")){
        modify_name(id,param);
    } else if(!strcmp(option, "--age")) {
        modify_age(id,param);
    } else if(!strcmp(option, "--gender")) {
        modify_gender(id,param);
    } else {
        printf("Invalid option: %s\n", option);
        printf("Available option are:\n");
        printf("\t--name <name>\n");
        printf("\t--age <age(1~150)>\n");
        printf("\t--gender <gender(M,F)>\n\n");
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
        } 
        if(!validate_age(age)) {
            printf("Invalid age(1~150): %ld\n", age);
            free(ages);
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

void print_got_user_by_gender(char * gender) {
    if (!validate_gender(gender)) {
        printf("Invalid gender: 'M'(male) or 'F'(female) required.\n");
        return ;
    }
    printf("---------------------------------\n");
    printf("|  ID\tname\tage\tgender  |\n");
    db_print_user_by_gender(gender[0]);
    printf("---------------------------------\n");
}

void get_option_handle(char * argv[], int argc) {
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
}

void modify_handle(char * argv[], int argc, unsigned long id) {
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
        get_option_handle(argv, argc);

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
        modify_handle(argv, argc, id);
    } else {
        printf("Unknown sub-command: %s\n", argv[2]);
    }
}