all: client server router group_server
client: client.c utils
	gcc client.c utilities.o -o Client
group_server: group_server.c utils
	gcc group_server.c utilities.o -o Group_server
router: router.c utils
	gcc router.c utilities.o -o Router
server: server.c utils
	gcc server.c utilities.o -o Server
utils: utilities.c utilities.h
	gcc -c  utilities.c -o utilities.o
.PHONY: clean
clean:
	rm *.o Client Server Router Group_server
