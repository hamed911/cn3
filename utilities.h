#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <string.h>
#include <dirent.h>     
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define STDINFD 0
#define STDOUTFD 1
#define STDERRFD 2
#define MAX_STR_SIZE 1024
#define MAX_ARRAY_SIZE 16
#define MAX_LIMITED_STR 32
#define MAX_MEMBER_SIZE 4
#define GETMAX(X,Y) ((X)>(Y)? (X) : (Y))
#define	GETMIN(X,y)	((Y) > (X):(Y))
typedef enum { false, true } bool;

struct linux_dirent {
	long           d_ino;
	off_t          d_off;
	unsigned short d_reclen;
	char           d_name[];
};

typedef struct ip_fd{
	char ip[MAX_STR_SIZE];
	int fd;
} ip_fd;

typedef struct dst_port {
	char dst[MAX_STR_SIZE];
	char port[MAX_STR_SIZE];
} dst_port;

typedef struct group_info
{
	char name[MAX_LIMITED_STR];
	char ip[MAX_LIMITED_STR];
	char multi_ip[MAX_LIMITED_STR];
	char members[MAX_MEMBER_SIZE][MAX_LIMITED_STR];
} group_info;

/* in the maping type-section to nodes:
// 00 is for client
// 01 is for Group_server
// 10 is for router
// 11 is for server
*/

void update_multicast_table( group_info* group_info_table, char * name);
void crc(char* t, char* res);
int connect_to_port(int port);
int write_to_fd(int client_fd, char massage[MAX_STR_SIZE],char response[MAX_STR_SIZE]);
int write_read_to_port(int to_port, char new_frame[MAX_STR_SIZE], char pasokh[MAX_STR_SIZE]);
//void insert_dst_port(dst_port table [MAX_ARRAY_SIZE] ,char* dst,char* port);
void read_entire_file(char* name,char data [MAX_ARRAY_SIZE]);
bool file_exist(char * fname);
void concat(int argn, char** args,char* c,char* concated);
// void create_service_files(int argn, char** args);
int delete_member_group_info(group_info table [MAX_ARRAY_SIZE],int index);
int remove_member_group_info(group_info table [MAX_ARRAY_SIZE],int index, char * member_ip);
void group_info_to_string(group_info* group_info_table,int index,char * result);
void clear_group_info(group_info table[MAX_ARRAY_SIZE]);
int add_member_group_info(group_info table [MAX_ARRAY_SIZE],int index, char * member);
int search_group_info_by_name(group_info table [MAX_ARRAY_SIZE], char * name);
int search_group_info_by_multi_ip(group_info table [MAX_ARRAY_SIZE], char * multi_ip);
int insert_group_info(group_info table [MAX_ARRAY_SIZE] ,char name[MAX_STR_SIZE],char ip[MAX_STR_SIZE],char multi_ip[MAX_STR_SIZE]);
void create_group_file(char* group_name);
void list_groups( group_info* group_info_table, char* result);
int create_directories(char path_name[MAX_STR_SIZE]);
int open_or_create_file(char* name);
bool has_access(char* user,char* file,char* acess);
void initial_ip_fd(ip_fd table [MAX_ARRAY_SIZE]);
void delete_all_ip_fd( ip_fd table [MAX_ARRAY_SIZE], int fd);
void insert_dst_port(dst_port table [MAX_ARRAY_SIZE] ,char* dst,char* port);
void clear_dst_port( dst_port table [MAX_ARRAY_SIZE]);
int search_dst_port( dst_port table [MAX_ARRAY_SIZE], char * dst);
int search_dst_port_by_port(dst_port table [MAX_ARRAY_SIZE], char * dst, char* port);
void delete_dst_port( dst_port table [MAX_ARRAY_SIZE],int index);
void insert_ip_fd( ip_fd table [MAX_ARRAY_SIZE] ,char* ip,int fd);
void clear_ip_fd( ip_fd table [MAX_ARRAY_SIZE]);
int search_ip_fd( ip_fd table [MAX_ARRAY_SIZE], char * ip);
int search_ip_fd_by_fd( ip_fd table [MAX_ARRAY_SIZE], int fd);
void delete_ip_fd( ip_fd table [MAX_ARRAY_SIZE],int index);
void framing(char* type,char* dstAdd,char* srcAdd,char* name,char* data,char* sender,char* extera_data,char* frame);
int change_ip_seed(int c);
int read_ip_seed();
int strlength(char str[MAX_STR_SIZE]);
void tokenizer(char str[MAX_STR_SIZE], char delim[MAX_STR_SIZE], int* num_of_tokens, char res[MAX_ARRAY_SIZE][MAX_STR_SIZE]);
int clear_buff(char* c, int size);
int clear_buff_int(int* c, int size);
int mystrcmp(char const*p, char const *q);
int mystrcmp_to(char const*p, char const *q, int to);
void print(char str[MAX_STR_SIZE]);
void int_to_str(int val, char res[20], int base);
int days_in_month(int month, int year);
void get_date(char time_str [30]);
void list_files(char pathname[MAX_STR_SIZE], char file_names[MAX_ARRAY_SIZE][MAX_STR_SIZE], int* file_num);
#endif
