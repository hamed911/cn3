#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "utilities.h"

int main(int argn, char** args){
	if(argn!=5){
		print("use this format: ./Client name server_ip router_ip router_port\n");
		return 0;
	}
	char* client_name = args[1];
	char* client_ip=NULL;
	char* server_ip= args[2];
	char* router_port= args[4];
	int ip_input_tokens_num;
	char ip_input_tokens[MAX_ARRAY_SIZE][MAX_STR_SIZE];
	char ip_input_buffer[MAX_STR_SIZE];
	clear_buff(ip_input_buffer, MAX_STR_SIZE);
	while( read(STDINFD, ip_input_buffer, MAX_STR_SIZE) > 0 ){	
		tokenizer(ip_input_buffer, " ", &ip_input_tokens_num, ip_input_tokens);
		if(mystrcmp(ip_input_tokens[0], "Set") == 0 && mystrcmp(ip_input_tokens[1], "IP") == 0 && ip_input_tokens_num == 3){
			client_ip=ip_input_tokens[2];
			replace_char(client_ip,'\n','\0');
			break;
		}else{
			print("You should set your IP first : Set IP 115.2.2.18\n");
		}
	}
	int fd = connect_to_port(atoi(router_port));
	char input_buffer[MAX_STR_SIZE];
	clear_buff(input_buffer, MAX_STR_SIZE);
	while(read(STDINFD, input_buffer, MAX_STR_SIZE) > 0)
	{	
		int input_tokens_num;
		char input_tokens[MAX_ARRAY_SIZE][MAX_STR_SIZE];
		replace_char(input_buffer,'\n','\0');
		char iden_buff[MAX_STR_SIZE];
		clear_buff(iden_buff, MAX_STR_SIZE);
		strcat(iden_buff, "00");//00 is for client
		//parsing input_buffer
		tokenizer(input_buffer," ", &input_tokens_num, input_tokens);
		
		char frame[MAX_STR_SIZE];
		clear_buff(frame, MAX_STR_SIZE);
		char data[MAX_STR_SIZE];
		clear_buff(data, MAX_STR_SIZE);
		
		//send command for server
		if( (mystrcmp(input_tokens[0], "DC") < 0) && (mystrcmp(input_tokens[0], "Logout") < 0) )
		{
			if( strcmp(input_tokens[0],"Send")==0 && strcmp(input_tokens[1],"message")==0  && input_tokens_num==3){
				strcat(data,"Sent message: ");
				strcat(data,input_tokens[2]);
			} else if(strcmp(input_tokens[0],"Send")==0 && strcmp(input_tokens[1],"file")==0 && input_tokens_num==3){
				if(!file_exist(input_tokens[2])){
					printf("file %s doesn't exist! You should create this file near ./Client 's directory\n", input_tokens[2]);
					continue;
				}
				char file_content[MAX_STR_SIZE];
				clear_buff(file_content,MAX_STR_SIZE);
				read_entire_file(input_tokens[2],file_content);
				replace_char(file_content,'\n','^');
				strcat(data,"Sent file ");
				strcat(data,input_tokens[2]);
				strcat(data," contain:\n");
				strcat(data,file_content);
			}else
				strcat(data,input_buffer);

			framing(iden_buff,"0",client_ip,client_name,data,"cccc","cc",frame);
			printf("sent frame is: %s\n",frame );
			int bytes_written = write(fd, frame, strlength(frame));
			if(bytes_written < 0)
				write(STDOUTFD,"Error on writing\n", sizeof("Error on writing\n"));

			//get response from server
			char res_status[MAX_STR_SIZE];
			clear_buff(res_status, MAX_STR_SIZE);
			int read_status = read(fd, res_status, MAX_STR_SIZE);
			//show the response to client
			printf("status of result of first reading ...\n");
			write(STDOUTFD, res_status, strlength(res_status));
			printf("\n");

			char res_buff[MAX_STR_SIZE];
			clear_buff(res_buff, MAX_STR_SIZE);
			read_status = read(fd, res_buff, MAX_STR_SIZE);
			printf("result is:\n%s\n",res_buff );
			int input_tokens_num;
			char input_tokens[MAX_ARRAY_SIZE][MAX_STR_SIZE];
			tokenizer(res_buff, "&", &input_tokens_num, input_tokens);
			//show the response to client
			write(STDOUTFD, input_tokens[5], strlength(input_tokens[5]));
			printf("\n");
		}
		else if(mystrcmp(input_tokens[0], "DC") == 0)
		{
			int bytes_written = write(fd, "DC", strlength("DC"));
			if(bytes_written < 0)
				write(STDOUTFD,"Error on writing\n", sizeof("Error on writing\n"));
			break;
		}
		else if(mystrcmp(input_tokens[0], "Logout") == 0)
		{
			int bytes_written = write(fd,"DC", strlength("DC"));
			if(bytes_written < 0)
				write(STDOUTFD,"Error on writing\n", sizeof("Error on writing\n"));
			close(fd);
			return 0;
		}
	}
	close(fd);
	return 0;
}