all: server. client.c
	gcc -Wall -Wvla -g server.c -o server
	gcc -Wall -wvla -g client.c -o client
server: server.c
		gcc -Wall -Wvla -g server.c -o server
client: client.c
		gcc -Wall -Wvla -g client.c -o client
clear:
		rm client server
