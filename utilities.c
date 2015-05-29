#include "utilities.h"

void concat(int argn, char** args,char* c,char* concated){
	int i;
	for(i=1; i< argn; i++){
		strcat(concated,args[i]);
		if(i!=argn-1){
			strcat(concated,c);
		}
	}
}

void create_group_file(char* group_name){
	create_directories("./DB");
	create_directories("./DB/Groups");
	printf("Group file %s created\n",group_name );
	char name[MAX_STR_SIZE];
	clear_buff(name, MAX_STR_SIZE);
	strcat(name, "./DB/Groups/");
	strcat(name,group_name);
	int fd= open_or_create_file(name);
	close(fd);
}
/*
void create_service_files(int argn, char** args){
	create_directories("./DB");
	create_directories("./DB/Services");
	int i;
	for(i=1; i<argn; i++){
		printf("service %s created\n",args[i] );
		char name[MAX_STR_SIZE];
		clear_buff(name, MAX_STR_SIZE);
		strcat(name, "./DB/Services/");
		strcat(name,args[i]);
		int fd= open_or_create_file(name);
		close(fd);
	}
}
*/

int open_or_create_file(char* name){
	return open(name, O_RDWR | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH |S_IWUSR);
}

void replace_char(char* string,char old_char,char new_char ){
	int i=0;
	while(string[i]!='\0'){
		if(string[i]==old_char)
			string[i]=new_char;
		i++;
	}
}

bool has_access(char* user,char* service,char* mode){
	char line [MAX_STR_SIZE];
	clear_buff(line,MAX_STR_SIZE);
	FILE* file = fopen("./DB/AC.txt","r");
	while( fgets(line,MAX_STR_SIZE,file) != NULL){
		replace_char(line,'\n','\0');
		int input_tokens_num;
		char input_tokens[MAX_ARRAY_SIZE][MAX_STR_SIZE];
		tokenizer(line, " ", &input_tokens_num, input_tokens);
		if(input_tokens_num!=3){
			printf("Error in number of token in 'AC.txt' in: %s\n",line );
			return false;
		}
		if(strcmp( input_tokens[0],user) ==0 && strcmp( input_tokens[1],service) ==0 && strcmp( input_tokens[2],mode) ==0){
			printf("match hapend in: %s\n",line );
			return true;
		}
	}

	fclose(file);
	return false;
}
// connect to port then return server fd
int connect_to_port(int port){
	int client_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in switch_addr;
	switch_addr.sin_family = AF_INET;
	switch_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	printf("attempt to connect to port : %d\n",port );
	switch_addr.sin_port = htons(port);
	int status1 = connect(client_fd, (struct sockaddr *)&switch_addr, sizeof(switch_addr));
	if(status1 < 0)
	{
		write(STDOUTFD,"Error on connecting to server\n", sizeof("Error on connecting to server\n"));
		return -1;
	}
	else
	{
		printf("sucsessfully connect to port %d with file descriptor '%d'\n",port,client_fd );
	}
	return client_fd;

}

int write_to_fd(int client_fd, char massage[MAX_STR_SIZE],char response[MAX_STR_SIZE]){
	int bytes_written = write(client_fd, massage, strlength(massage));
	if(bytes_written < 0)
	{
		write(STDOUTFD,"Error on writing\n", sizeof("Error on writing\n"));
		return -1;
	}
	printf("chizi ke neveshte  :  %s\n", massage);
	//get response from server
	int read_status = read(client_fd, response, MAX_STR_SIZE);
	return 1;
}

//this function connects to a port and write sth for it ince and read ite response
int write_read_to_port(int to_port, char new_frame[MAX_STR_SIZE], char pasokh[MAX_STR_SIZE])
{
	int client_fd = connect_to_port(to_port);
	write_to_fd(client_fd,new_frame,pasokh);
	//Disconnect from server
	int byts_written = write(client_fd, "DC", strlength("DC"));
	if(byts_written < 0)
		write(STDOUTFD,"Error on writing\n", sizeof("Error on writing\n"));
	close(client_fd);
	return 1;//read_status;
}


void show_table_dst_port(dst_port table [MAX_ARRAY_SIZE],int index){
	int i;
	for(i=0; i<index; i++)
		printf("%d - dst is: %s \tport is:%s\n",i,table[i].dst,table[i].port);
}

void clear_dst_port(dst_port table [MAX_ARRAY_SIZE]){
	int i;
	for(i=0; i<MAX_ARRAY_SIZE; i++)
	{
		strcpy( table[i].dst,"nill");
		strcpy( table[i].port,"nill");
	}
}

int search_dst_port(dst_port table [MAX_ARRAY_SIZE], char * dst){
	int i;
	//mystrcmp(table[i].port,"")<0 && 
	for(i=0; i<MAX_ARRAY_SIZE; i++)
	{
		if(mystrcmp(table[i].dst,dst)==0)
			return i;
	}
	return -1;
}

int search_dst_port_by_port(dst_port table [MAX_ARRAY_SIZE], char * dst, char* port){
	int i;
	//mystrcmp(table[i].port,"")<0 && 
	for(i=0; i<MAX_ARRAY_SIZE; i++)
	{
		if(mystrcmp(table[i].dst,dst)==0 && mystrcmp(table[i].port,port)==0)
			return i;
	}
	return -1;
}

void delete_dst_port(dst_port table [MAX_ARRAY_SIZE],int index){
	strcpy( table[index].dst,"nill");
	strcpy( table[index].port,"nill");
	/*clear_buff(table[index].dst,MAX_STR_SIZE);
	clear_buff(table[index].port,MAX_STR_SIZE);*/
}


void insert_dst_port(dst_port table [MAX_ARRAY_SIZE] ,char* dst,char* port){
	int i;
	for(i=0; i<MAX_ARRAY_SIZE; i++)
	{
		if(mystrcmp(table[i].port,"nill")==0){
			strcpy(table[i].dst,dst);
			strcpy(table[i].port,port);
			break;
		}
	}
}

void clear_group_info(group_info table[MAX_ARRAY_SIZE]){
	int i;
	for(i=0; i<MAX_ARRAY_SIZE; i++)
	{
		printf("here\n");
		strcpy( table[i].ip,"nill");
	}
}

void insert_group_info(group_info table [MAX_ARRAY_SIZE] ,char ip[MAX_STR_SIZE],char port[MAX_STR_SIZE],char multi_ip[MAX_STR_SIZE]){
	int i;
	for(i=0; i< MAX_ARRAY_SIZE; i++){
		if(mystrcmp(table[i].ip,"nill")==0){
			strcpy(table[i].ip,ip);
			strcpy(table[i].port,port);
			strcpy(table[i].multi_ip,multi_ip);
			break;
		}	
	}
}

void show_group_info( group_info table [MAX_ARRAY_SIZE], int limit){
	int i;
	printf("**********multicast table ************\n");
	printf("IP\t\tmulticast_ip\t\tport\t\tmembers\n");
	for(i=0; i< limit; i++){
		printf("%s\t\t%s\t\t%s\t\t",table[i].ip,table[i].multi_ip,table[i].port );
		int j;
		for(j=0; j<limit; j++)
			printf("%s,\t",table[i].members[j] );
		printf("\n");
	}
}

bool file_exist(char * fname){
	if( access( fname, F_OK ) != -1 )
		return true;
	else 
		return false;
}

void read_entire_file(char* name,char data [MAX_ARRAY_SIZE]){
	FILE *f = fopen(name, "rb");
	
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);
	fread(data, fsize, 1, f);
	fclose(f);
	data[fsize] = 0;
}

void show_table_ip_fd(ip_fd table [MAX_ARRAY_SIZE],int index){
	int i;
	for(i=0; i<index; i++)
		printf("%d - ip is: %s \tfd is:%d\n",i,table[i].ip,table[i].fd );
}

void clear_ip_fd(ip_fd table [MAX_ARRAY_SIZE]){
	int i;
	for(i=0; i<MAX_ARRAY_SIZE; i++)
	{
		strcpy( table[i].ip,"");
		table[i].fd=-1;
		// table[i]=NULL;
	}
}

void crc(char* t, char* res){
	//return "$$$$$$";
	int i=0;
	int s = 0;
	while(t[i] != '\0')
	{
		s += t[i++];
	}
	int r = s % 64;
	int_to_str(s, res, 2);
	res[6] = '\0';
}

int search_ip_fd( ip_fd table [MAX_ARRAY_SIZE], char * ip){
	int i;
	for(i=0; i<MAX_ARRAY_SIZE; i++)
		if(table[i].fd !=-1 && mystrcmp(table[i].ip,ip)==0)
			return i;
	return -1;
}

int search_ip_fd_by_fd( ip_fd table [MAX_ARRAY_SIZE], int fd){
	int i;
	for(i=0; i<MAX_ARRAY_SIZE; i++)
		if(table[i].fd ==fd)
			return i;
	return -1;
}
void initial_ip_fd(ip_fd table [MAX_ARRAY_SIZE]){
	int i;
	for (i=0; i<MAX_ARRAY_SIZE;i++){
		strcpy( table[i].ip,"");
		table[i].fd=-1;
	}
}

void delete_all_ip_fd( ip_fd table [MAX_ARRAY_SIZE], int fd){
	int i;
	for(i=0; i<MAX_ARRAY_SIZE; i++)
		if(table[i].fd ==fd)
			table[i].fd=-1;
}

void delete_ip_fd( ip_fd table [MAX_ARRAY_SIZE],int index){
	table[index].fd=-1;
	clear_buff(table[index].ip,MAX_STR_SIZE);
}

void insert_ip_fd( ip_fd table [MAX_ARRAY_SIZE] ,char* ip,int fd){
	int i;
	for(i=0; i<MAX_ARRAY_SIZE; i++)
	{
		if(table[i].fd == -1){
			table[i].fd=fd;
			strcpy(table[i].ip,ip);
			break;
		}
	}
}

void extend_to(char* c, int to,char* ress)
{
	int size = strlen(c);
	int ex = to - size;
	
	int i;
	for (i=0; i<ex; i++)
	{
		strcat(ress, "0");
	}
	strcat(ress, c);
}

void framing(char* type,char* dstAdd,char* srcAdd,char* name,char* data,char* sender,char* extera_data,char* frame){
	strcat(frame, type);
	strcat(frame, "&");
	//dst
	strcat(frame, dstAdd);
	strcat(frame,"&");
	//src
	strcat(frame, srcAdd);
	strcat(frame,"&");
	strcat(frame,name);
	strcat(frame,"&");
	strcat(frame,extera_data);
	strcat(frame, "&");
	//data
	strcat(frame, data);
	char fcrc[20];
	crc(data,fcrc);
	strcat(frame, "&");
	strcat(frame, fcrc);
	strcat(frame, "&");
	//strcat(frame, "&$$$$$$&");
	//port
	strcat(frame, sender);
}

int change_ip_seed(int c)
{
	int file_fd = open("./DB/ip_seed.txt",O_TRUNC | O_RDWR);
	if(file_fd > 0)
		chmod("./DB/ip_seed.txt", S_IRUSR | S_IWUSR );
	else
		write(STDOUTFD, "Error In Opening File to Write!\n", sizeof("Error In Opening File to Write!\n"));
	char res[20];
	clear_buff(res, 20);
	int_to_str(c, res, 10);
	int w_st = write(file_fd, res, strlen(res));
	close(file_fd);
	return w_st;
}

int read_ip_seed()
{
	char line [MAX_STR_SIZE];
	clear_buff(line,MAX_STR_SIZE);
	FILE* file = fopen("./DB/ip_seed.txt","rt");
	fgets(line,MAX_STR_SIZE,file);
	int v = atoi(line);
	fclose( file);
	return v;
}

int create_directories(char path_name[MAX_STR_SIZE])
{
	return mkdir(path_name, S_IRUSR | S_IWUSR | S_IXUSR | S_IROTH | S_IWOTH | S_IXOTH);
}

int strlength(char str[MAX_STR_SIZE])
{
	int i;
	for(i = 0; i < GETMAX(MAX_STR_SIZE, sizeof(str)); i++)
	{
		if(str[i] == '\0') break;
	}
	return i;
}
void int_to_str(int val, char res[20], int base)
{
	clear_buff(res, 20);
	char tmp[20];
	clear_buff(tmp, 20);
	if(val == 0)
	{
		strcpy(res, "0");
		return;
	}
	int index = 0;
	while (val > 0)
	{
		char digit = '0';
		digit = (char) ((val % base) + (int) digit);
		tmp[index] = digit;
		val /= base;
		index ++;
	}
	strcat(tmp, "\0");
	int i;
	int size = strlength(tmp);

	for(i = 0; i<size; i++)
	{
		res[i] = tmp[size-1-i];
	}
	strcat(res, "\0");
}
void print(char str[MAX_STR_SIZE])
{
	write(STDOUTFD, str, strlength(str));
}
void tokenizer(char str[MAX_STR_SIZE], char delim[MAX_STR_SIZE], int* num_of_tokens, char res[MAX_STR_SIZE][MAX_STR_SIZE])
{
	char tmp[MAX_STR_SIZE];
	int i, tmp_index = 0, res_index = 0;
	for(i = 0; i <= strlength(str); i++)
	{
		int cut = 0;
		int j;
		for(j = 0; j < strlength(delim); j++)
		{
			if (str[i] == delim[j] || str[i] == '\0' )
			{
				cut = 1;
				break;
			}
		}
		if(cut == 0)
		{
			tmp[tmp_index] = str[i];
			tmp_index ++;
		}
		else
		{
			int k;
			for(k = 0; k < tmp_index; k++)
				res[res_index][k] = tmp[k];
			res[res_index][k] = '\0';
			res_index ++;
			tmp_index = 0;
		}
	}
	*num_of_tokens = res_index;
}
int clear_buff(char* c, int size)
{
	if (size == 0) size = MAX_STR_SIZE;
	int i;
	for(i = 0; i < size; i++)
	{
		c[i] = 0;
	}
}

int clear_buff_int(int* c, int size)
{
	if (size == 0) size = MAX_STR_SIZE;
	int i;
	for(i = 0; i < size; i++)
	{
		c[i] = 0;
	}
}
int mystrcmp(char const*p, char const *q)
{
	int i = 0;
	for(i=0; q[i]!='\0'; i++)
	{
		if(p[i] != q[i])
			return -1;
	}
	return 0;
}

int mystrcmp_to(char const*p, char const *q, int to)
{
	int i = 0;
	for(i=0; i<to; i++)
	{
		if(p[i] != q[i])
			return -1;
	}
	return 0;
}

