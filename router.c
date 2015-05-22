#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <errno.h>
#include "utilities.h"

int forward_and_receive(char* my_port, char command[MAX_STR_SIZE], dst_port table[MAX_ARRAY_SIZE], int parents[MAX_ARRAY_SIZE], int pi, ip_fd ip_fd_table[MAX_ARRAY_SIZE], int pack_sender_fd)
{
	printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
	printf("command i ke forward_and_receive avvale kar migire:\n%s\n", command);
	printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");

	int tokens_num;
	char input_tokens[MAX_ARRAY_SIZE][MAX_STR_SIZE];
	tokenizer(command, "&", &tokens_num, input_tokens);
	char* f_type = input_tokens[0];
	char* f_dst_addr = input_tokens[1];
	char* f_src_addr = input_tokens[2];
	char* f_data = input_tokens[5];
	char* f_sndr_port = input_tokens[7];

	if(mystrcmp(f_type, "00")==0)//from client, so should send packet to server, through its parents
	{
		printf("\n**fahmide baste az cliente\n");

		while(1)
		{
			int found_port = search_dst_port(table, f_dst_addr);
			if(found_port < 0)
			{
				write(pack_sender_fd, "\n~~Connecttion Error~~\n", strlen("\n~~Connecttion Error~~\n"));
				return -1;
			}
			else
			{
				char new_frame[MAX_STR_SIZE];
				clear_buff(new_frame, MAX_STR_SIZE);
				char pasokh[MAX_STR_SIZE];
				clear_buff(pasokh, MAX_STR_SIZE);
				printf("MEGHDAARE port baraye ettesal:  %d\n", atoi(table[found_port].port));
				framing(f_type,f_dst_addr,f_src_addr,f_data,my_port,new_frame);
				int conn = connect_to_aport(atoi(table[found_port].port), new_frame, pasokh);
				printf("result of conn in transferring 00 pack is : %d\n", conn);
				printf("pasokhi ke gerefte az girande: %s\n", pasokh);
				if(conn > 0)
					return conn;
				else
				{
					show_table_dst_port(table, 5);
					int f_p = search_dst_port(table, f_dst_addr);
					delete_dst_port(table, f_p);
					show_table_dst_port(table, 5);
					continue;
				}
			}
		}
	}
	else if(mystrcmp(f_type, "10")==0)//from another switch, it's a connection message, connection should be confirmed
	{
		printf("\n**fahmide baste az switche\n");
		//strcpy(res, "connection to this switch for a new swich is confirmed");
		return 1;
	}
	else if(mystrcmp(f_type, "11")==0)//from server, it's a response packet
	{
		printf("\n**fahmide baste az servere\n");
		printf("\n<f-type, dest, src, sender-port> are: <%s, %s, %s, %s>\n", f_type, f_dst_addr, f_src_addr, f_sndr_port);

		int found_fd = search_ip_fd(ip_fd_table, f_dst_addr);

		printf("found_fd is : %d\n", ip_fd_table[found_fd].fd);

		if(found_fd >= 0)
		{

			printf("fahmide be client vasle va fd is ro peida karde\n");
			printf("chizi ke mikhad barash benevise: %s\n", f_data);
			int w_st = write(ip_fd_table[found_fd].fd, command, strlength(command));
			printf("bad az neveshtan e f_data\n");
			if(w_st < 0)
				printf("error in writing the response to found_fd\n");
		}
		else
		{
			while(1)
			{
				//should find appropriate port to send response
				int port_index = search_dst_port(table, f_dst_addr);
				if(port_index >= 0)
				{
					int to_port = atoi(table[port_index].port);
					printf(">>>>>>port index is :  %d\n", port_index);
					printf(">>>>>>table[port_index].port is to resend frame to it is: %s\n", table[port_index].port);

					char new_frame[MAX_STR_SIZE];
					clear_buff(new_frame, MAX_STR_SIZE);	
					printf("\n**for this switch my_port is | ke migzare akhare baste to child\n");
					printf("%s\n", my_port);
					framing(f_type,f_dst_addr,f_src_addr,f_data,my_port,new_frame);
					char pasokh[MAX_STR_SIZE];
					clear_buff(pasokh, MAX_STR_SIZE);
					int sw_conn = connect_to_aport(to_port, new_frame, pasokh);

					if(sw_conn > 0)
					{
						printf("\nResponse from port which is given a frame:\n");
						write(STDOUTFD, pasokh, strlength(pasokh));
						printf("\n");
						break;
					}
					else
					{
						show_table_dst_port(table, 5);
						int f_p = search_dst_port(table, f_dst_addr);
						delete_dst_port(table, f_p);
						show_table_dst_port(table, 5);
						continue;
					}
				}
				else
				{
					write(pack_sender_fd, "\n~~Connecttion Error~~\n", strlen("\n~~Connecttion Error~~\n"));
					return -1;
				}
			}
		}
	}
	else//packet might be corrupted
	{
		printf("packet corrupted or encrypted");
		return -1;
	}
}

int process_command(char* my_port, char command[MAX_STR_SIZE], dst_port table[MAX_ARRAY_SIZE], int parents[MAX_ARRAY_SIZE], int pi, ip_fd ip_fd_table[MAX_ARRAY_SIZE], int it_fd)
{
	int tokens_num;
	char input_tokens[MAX_ARRAY_SIZE][MAX_STR_SIZE];
	tokenizer(command, "&", &tokens_num, input_tokens);
	char* f_type = input_tokens[0];
	char* f_dst_addr = input_tokens[1];
	char* f_src_addr = input_tokens[2];
	char* f_data = input_tokens[5];
	char* f_sndr_port = input_tokens[7];

	//beased on sndr_port and src_addr, table will be updated
	//check that the src_ip is in table or not, if not add src_ip and corr_port to table
	printf("the DST_PORT__table BEFORE update\n");
	show_table_dst_port(table, 5);
	if(search_dst_port_by_port(table,f_src_addr, f_sndr_port)<0 && mystrcmp(f_src_addr, "0000000000009999")<0 && mystrcmp(f_dst_addr, "0000000000009999")<0 && mystrcmp(f_sndr_port, "cccc")<0)
	{
		insert_dst_port(table, f_src_addr, f_sndr_port);
	}
	printf("the dst_port_table AFTER update\n");
	show_table_dst_port(table, 5);

	printf("the IP_FD__table BEFORE update\n");
	show_table_ip_fd(ip_fd_table, 5);
	if(search_ip_fd(ip_fd_table, f_src_addr)<0 && mystrcmp(f_src_addr, "0000000000009999")<0 && mystrcmp(f_sndr_port, "cccc")==0)
		insert_ip_fd( ip_fd_table,f_src_addr,it_fd);
	printf("the ip_fd_table AFTER update\n");	
	show_table_ip_fd(ip_fd_table, 5);				

	//the received_frame should be processed through parents or children
	forward_and_receive(my_port, command, table, parents, pi, ip_fd_table, it_fd);
	return 1;
}

int main(int argn, char** args)
{
	if(argn!=2){
		print("use this format: /Switch port\n");
		return 0;
	}
	char Switch_buffer[MAX_ARRAY_SIZE][MAX_STR_SIZE];
	int buffer_pointer=0;
	int port_number = atoi(args[1]);//to be server
	int port_no;//to be client

	int parents[MAX_ARRAY_SIZE];
	clear_buff_int(parents, MAX_ARRAY_SIZE);
	int parent_i = 0;

	dst_port table[MAX_ARRAY_SIZE];
	clear_dst_port(table);

	ip_fd ip_fd_table[MAX_ARRAY_SIZE];
	clear_ip_fd(ip_fd_table);

	const int num_of_connection = 5;
	char *directory_name = "DB";
	// make directories
	int mkdir_status = create_directories(directory_name);

	if(mkdir_status != 0)
		write(STDOUTFD, "directory exist or problem in creating directory\n", sizeof("directory exist or problem in creating directory\n"));
	else
		write(STDOUTFD, "Directory created\n", sizeof("Directory created\n"));
	//creating socket
	int server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//int client_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port_number);

	struct sockaddr_in client_addr;
	//binding 
	int binding_st = bind(server_fd,(struct sockaddr*) &server_addr, sizeof(server_addr));
	if(binding_st == -1)
	{
		write(STDOUTFD, "binding error\n", sizeof("binding error\n"));
		return 0;
	}
	else
		write(STDOUTFD, "binding successful\n", sizeof("binding successful\n"));
	//listenning
	int listening_st = listen(server_fd, num_of_connection);
	if(listening_st == -1)
	{
		write(STDOUTFD, "listening error\n", sizeof("listening error"));
		return 0;
	}
	else
		write(STDOUTFD, "listening successful\n", sizeof("listening successful"));

	fd_set read_fdset, temp_fdset;
	struct timeval tv;
	int ret_val;
	int new_sock_fd, it_fd;

	/* Watch stdin (fd 0) to see when it has input. */
	FD_ZERO(&read_fdset);
	FD_SET(server_fd, &read_fdset);
	FD_SET(STDINFD, &read_fdset);

	/* Wait up to five seconds. */
	tv.tv_sec = 10 * 60;
	tv.tv_usec = 0;

	unsigned int size_of_client_addr = sizeof(client_addr);
	int status;

	while(1)
	{
		memcpy(&temp_fdset, &read_fdset, sizeof(temp_fdset));
		status = select(FD_SETSIZE, &temp_fdset, (fd_set *)0, (fd_set *)0, (struct timeval*) &tv);
		if(status < 1)
		{
			write(STDOUTFD, "Select Error\n", sizeof("Select Error\n"));
			break;
		}

		for(it_fd = 0; it_fd<FD_SETSIZE; it_fd++)
		{
			if(FD_ISSET(it_fd, &temp_fdset))
			{
				if(it_fd == STDINFD)//command from terminal that a switch want to connect to another switch
				{
					char data [MAX_STR_SIZE];
					clear_buff(data, MAX_STR_SIZE);
					read(it_fd, data, MAX_STR_SIZE-1);
					int input_tokens_num;
					char input_tokens[MAX_ARRAY_SIZE][MAX_STR_SIZE];
					tokenizer(data, " ", &input_tokens_num, input_tokens);//TODO: check that command is connect
					port_no = atoi(input_tokens[2]);
					parents[parent_i] = port_no;
					parent_i++;
					//sending identity to switch
					char iden_buff[MAX_STR_SIZE];
					clear_buff(iden_buff, MAX_STR_SIZE);
					strcat(iden_buff, "10");//10 is for switch
					char frame[MAX_STR_SIZE];
					clear_buff(frame, MAX_STR_SIZE);
					framing(iden_buff, "9999", "9999", data, args[1], frame);

					char res_buff[MAX_STR_SIZE];
					clear_buff(res_buff, MAX_STR_SIZE);
					//send command for server and get response from server
					int conn = connect_to_aport(port_no, frame, res_buff);
					//show the response to terminal
					write(STDOUTFD, res_buff, strlength(res_buff));
					printf("\n");

					if(mystrcmp(res_buff, "Baste ye shoma daryaft shod")<0)
					{
						printf("jaavbi ke bad az ettesal be switch migire: %s\n", res_buff);

						char in_tokens[MAX_ARRAY_SIZE][MAX_STR_SIZE];
						int input_tokens_num;
						tokenizer(res_buff, "#", &input_tokens_num, in_tokens);

						printf("<javab[0], javab[1]> is <%s, %s>\n", in_tokens[0], in_tokens[1]);
						show_table_dst_port(table, 5);
						insert_dst_port(table, "0000000000000000", in_tokens[1]);
						show_table_dst_port(table, 5);
					}

					continue;
				}
				if(it_fd == server_fd)//this case is when switch wants to accept a client
				{	
					new_sock_fd = accept(server_fd, NULL, NULL);
					if(new_sock_fd < 0)
					{
						write(STDOUTFD,"Error on accepting\n", sizeof("Error on accepting\n"));
						break;
					}
					else write(STDOUTFD,"accepting successful\n", sizeof("accepting successful\n"));
					FD_SET(new_sock_fd, &read_fdset);

					printf("accepted fd is: %d\n", new_sock_fd);
				}
				else
				{
					int n, m;
					char buff_read [MAX_STR_SIZE], response_buff[MAX_STR_SIZE];
					clear_buff(buff_read, MAX_STR_SIZE);
					clear_buff(response_buff, MAX_STR_SIZE);

					n = read(it_fd, buff_read, MAX_STR_SIZE-1);
					strcpy(Switch_buffer[buffer_pointer] ,buff_read);
					buffer_pointer++;
					printf("\nDastoore khande shode az tarafe client:");
					print(buff_read);
					printf("\n");

					if(n == 0)
					{
						close(it_fd);
						FD_CLR(it_fd, &read_fdset);
						int ind = search_ip_fd_by_fd( ip_fd_table, it_fd);
						delete_ip_fd( ip_fd_table,ind);
						write(STDOUTFD, "Removing One Client_fd\n", sizeof("Removing One Client_fd\n"));
					}
					else if(n < 0)
					{
						write(STDOUTFD, "Error on reading\n", sizeof("Error on reading\n"));
					}
					//after reading successfully
					else
					{
						if(mystrcmp(buff_read, "DC") < 0 && mystrcmp(buff_read, "get_ip") < 0 && (mystrcmp(buff_read, "11&2000")<0))
						{
							if(process_command(args[1], buff_read, table, parents, parent_i, ip_fd_table, it_fd) < 0)
							{
								int st = write(it_fd, "Invalid command\n", sizeof("Invalid command\n"));
								if(st < 0) write(STDOUTFD, "Error on writing\n", sizeof("Error on writing\n"));
							}
							if(search_dst_port(table, "0000000000000000") >= 0)
							{
								strcpy(response_buff, "connected_to_server");
								strcat(response_buff, "#");
								strcat(response_buff, args[1]);
							}
							else
							{
								strcpy(response_buff, "Baste ye shoma daryaft shod");
							}
							int s = write(it_fd, response_buff, strlength(response_buff));
							if(s < 0) write(STDOUTFD, "Error on writing\n", sizeof("Error on writing\n"));
						}
						else if(mystrcmp(buff_read, "get_ip") == 0)
						{
							int rps = read_ip_seed();
							change_ip_seed(++rps);
							printf("new ip-seed is: %d\n", rps);
							char res[20];
							clear_buff(res, 20);
							int_to_str(rps, res, 10);
							write(it_fd, res,strlen(res));
						}
						else if(mystrcmp(buff_read, "11&2000")==0)
						{
							int tokens_num;
							char input_tokens[MAX_ARRAY_SIZE][MAX_STR_SIZE];
							tokenizer(buff_read, "&", &tokens_num, input_tokens);
							char res[200];
							clear_buff(res, 200);
							strcpy(res, "connection to switch for server is confirmed");

							write(it_fd, res,strlen(res));
							show_table_dst_port(table, 5);
							insert_dst_port(table, "0000000000000000", input_tokens[1]);
							show_table_dst_port(table, 5);
							//connected_to_server = 1;
							//connected_server_port = input_tokens[1];
							//printf("connected_to_server %d\n", connected_to_server);
							//printf("connected_server_port %s\n", connected_server_port);
						}
						else if(mystrcmp(buff_read, "DC") == 0)
						{
							write(it_fd, "Disconnecting in Progress ...\n",sizeof("Disconnecting in Progress ...\n"));
							close(it_fd);
							FD_CLR(it_fd, &read_fdset);
							int ind = search_ip_fd_by_fd( ip_fd_table, it_fd);
							delete_ip_fd( ip_fd_table,ind);
							write(STDOUTFD, "Removing One Client_fd\n", sizeof("Removing One Client_fd\n"));
						}
					}
				}
			}
		}
	}
	close(server_fd);
	return 0;
}