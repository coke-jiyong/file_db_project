#ifndef MAIN_UTILS_H
#define MAIN_UTILS_H
#include "hkdf.h"
#include "sys_utils.h"
#include "user_db.h"


void print_user(member *user);
void db_command(int argc, char *argv[]);
void print_user_list(void);
char *check_string_num(char *str, unsigned long *age);
int validate_age(unsigned long age);
int validate_name(char *name);
int validate_gender(char *gender);
void add_user(char *argv[]);
void delete_user(char *name);
member *get_user(char *name);
void print_modify_usage(void);
void modify_name(const unsigned long id, char *param);
void modify_age(const unsigned long id, char *param);
void modify_gender(const unsigned long id, char *param);
void modify_option_handle(const unsigned long id, char *option, char *param);
void print_got_user_by_name(char **argv, int len);
void print_got_user_by_age(char **argv, int len);
void print_got_user_by_gender(char *gender);
void get_option_handle(char *argv[], int argc);
void modify_handle(char *argv[], int argc, unsigned long id);
void db_user(int argc, char *argv[]);


#endif