#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>

#define WRITER 0
#define READER 1
#define MASTER 2



#define BUFF_SIZE 4096
/********************************** struct definations *********************//////////////////////

typedef struct node
{
	char* message;
	struct sockaddr_in* client;
	int fd;
	int mode;
	int length;
	struct node* next;
	struct node* prev;
}node;



/********************************** function definations *********************//////////////////////
node* node_init(char* message, struct sockaddr_in* client, int fd,int length, int mode); // pointer upon success , NULL upon failure
int addToList(node** list, node* obj); // 1 on su0ccess, 0 on failure
node* Find_Writer(node** list, int fd); // pointer upon success, NULL upon failure
void free_node(node* node);
void free_nodes(node* node);
int check_input(char* num); // 1 if number, 0 otherwise
void empty_string(char* str);
void to_upper(char* str);
void pc_down();

const char* readmessage= "Server is ready to read\n";
const char* writemessage ="Server is ready to write\n";

node** list = 0; // list init
int fd=0;
int main(int argc, char** argv)
{
	if(argc!=2)
	{
		fprintf(stderr, "Run the software like this ./%s [port to connect]\n", argv[0]);
		return 0;
	}
	////////////////////// ****************** var decleration **************////////////////////	
	int port = 0;
	char buff[BUFF_SIZE];
	fd_set readset;
	fd_set writeset;
	struct sockaddr_in me, shimrit;
	int slen = sizeof(me);
	struct timeval time;


	////////////////////// ****************** program initation **************////////////////////
	if(!check_input(argv[1]) || atoi(argv[1]) < 0)
	{
		fprintf(stderr, "Run the software like this ./%s [port to connect]\nPlease make sure to run it with a POSITIVE integer\n", argv[0]);
		return 0;
	}
	port=atoi(argv[1]);
	empty_string(buff);

	fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	me.sin_family = AF_INET;
	me.sin_port = htons(port);
	me.sin_addr.s_addr = htonl(INADDR_ANY);

	if(fd == -1)
	{
		fprintf(stderr, "Unable to open a new socket\n");
		return 0;
	}
	if(bind(fd, (const struct sockaddr*)&me, slen)==-1)
	{
		close(fd);
		perror("bind");
		return 0;
	}
	list = calloc(1,sizeof(node*));
	list[0] = 0;
	signal(SIGINT,pc_down);
	////////////////////// ****************** DA LOOP **************////////////////////
	while(1)
	{
		FD_ZERO(&readset);
		FD_ZERO(&writeset);
		time.tv_sec=2;
		time.tv_usec=0;
		FD_SET(fd, &readset);
		FD_SET(fd, &writeset);
		int result = select(fd+1,&readset,&writeset,0,&time);
		if(result>0)
		{
			if(FD_ISSET(fd,&readset))
			{
				printf("%s",readmessage);
				empty_string(buff);
				recvfrom(fd,buff,BUFF_SIZE,0,(struct sockaddr *) &shimrit,(socklen_t *) &slen);
				node* new = node_init(buff, &shimrit,fd,slen,WRITER);
				addToList(list,new);
			}
			if(list[0])
			{
				if(FD_ISSET(fd,&writeset))
				{
					printf("%s",writemessage);
					node* old = Find_Writer(list,fd);
					empty_string(buff);
					strcpy(buff,old->message);
					to_upper(buff);
					sendto(old->fd,buff,strlen(buff),0,(struct sockaddr *)old->client, old->length);
					free_node(old);
					old=0;
				}
			}
		}
	}
	close(fd);
	free_nodes(list[0]);
	free(list);
}


node* node_init(char* message, struct sockaddr_in* client, int fd,int length, int mode)
{
	node* n = (node*) malloc (sizeof(node));
	if(!n)
	{
		fprintf(stderr, "malloc\n");
		return 0;
	}
	if(message)
	{
		n->message = (char*) calloc ( ( strlen(message)+1),sizeof(char));
		if(!n->message)
		{
			free(n);
			fprintf(stderr, "malloc\n");
			return 0;
		}
	}
	else
		n->message = 0;
	strcpy(n->message,message);
	if(client)
	{
		n->client = (struct sockaddr_in*) calloc (1, sizeof(struct sockaddr_in));
		if(!n->client)
		{
			free(n->message);
			free(n);
			fprintf(stderr, "malloc\n");
			return 0;
		}
	n->client->sin_addr.s_addr = client->sin_addr.s_addr;
		n->client->sin_family = client->sin_family;
		n->client->sin_port = client->sin_port;
		memset(n->client->sin_zero , '\0',sizeof(n->client->sin_zero));
	}
	else
		n->client = 0;
	n->fd = fd;
	n->next = 0;
	n->prev = 0;
	n->length = length;
	n->mode = mode;
	return n;
}	

int addToList(node** list, node* obj)
{

	node* temp = list[0];
	if(!temp)
	{
		list[0] = obj;
	}
	else
	{
		while(temp->next!=0)
			temp = temp->next;
		temp->next = obj;
		obj->prev = temp;
	}
	return 1;
}

node* Find_Writer(node** list, int fd)
{
	node* temp = list[0];
	if(temp)
	{
		node* prev = temp->prev;
		if(prev)
		{
			prev->next = temp->next;
		}
		else
		{
			list[0] = temp->next;
		}
	}
	return temp;
}

void free_node(node* n)
{
	if(n->message)
		free(n->message);
	if(n->client)
		free(n->client);
	free(n);
	return;
}

void free_nodes(node* node)
{
	if(!node)
		return;
	free_nodes(node->next);
	free_node(node);
	return;
}

int check_input(char* num)
{
	int counter=0;
	while(*num!='\0' && *num!='\n')
	{
		if((*num<'0' || *num>'9') && *num!=' ')
			return -1;
		else if(*num == ' ')
		{

		}
		else
			counter++;
		num++;
	}
	if(!counter)
		return 0;
	return 1;
}

void empty_string(char* str)
{
	int i;
	for(i=0; i<BUFF_SIZE; i++)
	{
		*str = '\0';
		str++;
	}
}

void to_upper(char* str)
{
	if(!str)
		return;
	while(str[0]!='\0')
	{
		if(str[0] >= 'a' && str[0] <='z')
			str[0]-=32;
		str++;
	}
	return;
}
void pc_down()
{
        close(fd);
	free_nodes(list[0]);
	free(list); 
        exit(1);       
}

