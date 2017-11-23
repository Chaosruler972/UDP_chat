#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <netdb.h>
#include <memory.h>

#define BUFFER_SIZE 4096

int allUpperCase(char msg[])
{
    int iter = 0;
    for(;msg[iter] != '\0';iter++)
        if('a' <= msg[iter] && msg[iter] <= 'z')
            return 0;
    return 1;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr,"./client <server-host-name> <port>\n(if 'server-host-name' is unknown, use 'localhost')\n");
        exit(1);
    }
    int socketFD = 0, serverlen = 0, port = atoi(argv[2]), upperCaseReceived = 0;
    ssize_t reponseCode = 0;
    char buffer[BUFFER_SIZE];
    char *hostname = argv[1];
    struct sockaddr_in serveraddr;
    struct hostent *server;
    socketFD = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketFD < 0)
    {
        fprintf(stderr,"Error with socket creation.\n");
        return 1;
    }
    server = gethostbyname(hostname);
    if (server == NULL)
    {
        fprintf(stderr,"Error with finding the host.\n");
        return 1;
    }
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,(char *)&serveraddr.sin_addr.s_addr,(size_t)server->h_length);
    serveraddr.sin_port = htons((uint16_t) port);
    serverlen = sizeof(serveraddr);
    printf("Established connection to the server through port %d, enter 'qqq' to exit this client :D\n",port);
    printf("(PLEASE NOTE: this is a UDP connection base client,\nso the server might not be reachable even after 'successful' connection).\n\n");
    while(1)
    {
        bzero(buffer, BUFFER_SIZE);
        printf("Enter message to sent to the server: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        if(!strcmp("qqq\n",buffer))
            break;
        reponseCode = sendto(socketFD, buffer, BUFFER_SIZE, 0, (const struct sockaddr *)&serveraddr,(socklen_t)serverlen);
        if (reponseCode < 0)
        {
            fprintf(stderr,"Well... that's awkward, there was something wrong with the sending to the server :|\n");
            return 1;
        }
        reponseCode = recvfrom(socketFD, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &serveraddr, (socklen_t *) &serverlen);
        if (reponseCode < 0)
        {
            fprintf(stderr,"Well... that's awkward, there was something wrong with the reading from the server :|\n");
            return 1;
        }
        upperCaseReceived = allUpperCase(buffer);
        printf("Echo from server: %s(%s)\n", buffer,upperCaseReceived ? "all upper-case" : "not all upper case");
    }
    return 0;
}
