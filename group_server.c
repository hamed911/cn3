#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "utilities.h"


int main(int argn, char** args){
	/*group_info group_ip_[MAX_STR_SIZE];
	clear_group_info(group_ip_);
	test(group_ip_,"11&1.2.3.4&0&server&ss&telegram`1.1.1.1`2.2.2.2`javad*ali*shanbalile&111000&2000"); return;*/
	if(argn!=3){
		printf("Error in using group_server. correct format is: ./group_server name server_ip\n");
		return;
	}
	char* group_name = args[1];
	create_group_file(group_name);
	char multicast_ip[MAX_STR_SIZE];
	clear_buff(multicast_ip,MAX_STR_SIZE);
	char group_ip[MAX_STR_SIZE];
	clear_buff(group_ip,MAX_STR_SIZE);
	char input_buffer[MAX_STR_SIZE];
	clear_buff(input_buffer, MAX_STR_SIZE);

	// while(read(STDINFD, input_buffer, MAX_STR_SIZE) > 0){
	// 	int input_tokens_num;
	// 	char input_tokens[MAX_ARRAY_SIZE][MAX_STR_SIZE];
	// 	tokenizer(input_buffer, " ", &input_tokens_num, input_tokens);
	// 	if(mystrcmp(input_tokens[0], "Set") == 0 && mystrcmp(input_tokens[1], "IP") == 0 && mystrcmp(input_tokens[2], "multicast") == 0 && input_tokens_num == 4){
	// 		strcat(multicast_ip,input_tokens[3]);
	// 		replace_char(multicast_ip,'\n','\0');
	// 		printf("multicast IP set to: %s\n",multicast_ip );
	// 	}else if(mystrcmp(input_tokens[0], "Set") == 0 && mystrcmp(input_tokens[1], "IP") == 0 && input_tokens_num == 3){
	// 		strcat(group_ip,input_tokens[2]);
	// 		replace_char(group_ip,'\n','\0');
	// 		printf("ip set to: %s\n",group_ip );
	// 	}else
	// 		printf("You should Set IP and multicast_ip first\n");
	// 	if(group_ip[0]!='\0' && multicast_ip[0]!='\0')
	// 		break;
	// }
	strcat(multicast_ip,"2.2.2.2");
	strcat(group_ip,"1.1.1.1");
	while(read(STDINFD, input_buffer, MAX_STR_SIZE) > 0)
	{
		//parsing input_buffer
		int input_tokens_num;
		char input_tokens[MAX_ARRAY_SIZE][MAX_STR_SIZE];
		tokenizer(input_buffer, " ", &input_tokens_num, input_tokens);
		
		if(mystrcmp(input_tokens[0], "Connect") == 0 && input_tokens_num == 3)// don't forget to check reconnect!!
		{
			int port_no = atoi(input_tokens[2]);
			int fd = connect_to_port(port_no);
			if(fd==-1)
				continue;
			//sending identity to server
			char frame[MAX_STR_SIZE];
			clear_buff(frame, MAX_STR_SIZE);
			char data[MAX_STR_SIZE];
			clear_buff(data, MAX_STR_SIZE);
			char space[10]=" ";
			concat(argn,args,space,data);
			framing("01","0",group_ip,group_name,data,"gggg",multicast_ip,frame);	//01 is for provider
			strcat(frame, "\0");
			int bytes_written = write(fd, frame, strlength(frame));
			if(bytes_written < 0)
				write(STDOUTFD,"unable to write in router\n", sizeof("unable to write in router\n"));

			//get response from server
			char res_buff[MAX_STR_SIZE];
			clear_buff(res_buff, MAX_STR_SIZE);
			int read_status = read(fd, res_buff, MAX_STR_SIZE);

			//show the response to group_server
			write(STDOUTFD, res_buff, strlength(res_buff));
			while(1)
			{
				char in_buff[MAX_STR_SIZE];
				clear_buff(in_buff, MAX_STR_SIZE);
				int status = read(fd, in_buff, MAX_STR_SIZE);
				// status = write (fd,"provider recieved command\n",sizeof("provider recieved command\n"));
				printf(" %s\n",in_buff );
				write(STDOUTFD,in_buff, strlength(in_buff));				
			}
			close(fd);
		}
		else if(mystrcmp(input_tokens[0], "Exit") == 0)
		{
			return 0;
		}
	}
	return 0;
}