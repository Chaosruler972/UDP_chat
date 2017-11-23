server: server.c
		gcc -Wall -Wvla -g server.c -o server
client: client.c
		gcc -Wall -Wvla -g client.c -o client
clear:
		rm client server
