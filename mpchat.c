#include "TCPlib.h"
#include "list.h"
#include "mpchat.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>

#define FIRST_FD 3

#define DEBUG

int chat_send_WHO(int fd)
{
	int err;
	char message[BUF_LEN] = "WHO\n";	/*remember server waits for a \n to recognise EO message */
	int len = strlen(message);

	err = TCPsend(fd, message, len);
	return err;
}

int chat_send_ADD(int fd, unsigned long myIP, unsigned short myport)
{
	int err;
	char message[BUF_LEN];	/*remember server waits for a \n to recognise EO message */
	char str[BUF_LEN];
	int len;

	message[0]='\0';
	strcat(message, "ADD\n");
	sprintf(str, "%lu;%hu\n", myIP, myport);
	strcat(message, str);

	len = strlen(message);
	err = TCPsend(fd, message, len);
	return err;
}

int chat_ADD(int fd, char *mess, connection **connections)
{
	unsigned long IP;
	unsigned short port;
	int i;

	if(sscanf(mess, "ADD\n%lu;%hu\n", &IP, &port)!=2)
	{
		#ifdef DEBUG
		printf("received malformated ADD: %s", mess);
		#endif
		return -1;
	}

	for(i=FIRST_FD; connections[i]!=(connection*)1; i++)
	{
		if(connections[i]!=NULL && connections[i]->fd==fd)
			break;
	}
	if(connections[i]==(connection*)1)
		return -2;	/* noone found to be updated */

	connections[i]->fd=fd;
	connections[i]->IP=IP;
	connections[i]->port=port;

	return 0;
}

int chat_send_LST(int fd, connection **connections)
{
	int err, i, len;
	char message[BUF_LEN], str[BUF_LEN];
	connection *conn;

	message[0]='\0';
	strcpy(message, "LST\n");

	for(i=FIRST_FD; connections[i]!=(connection*)1; i++)
	{
		conn = connections[i];
		if(conn == NULL || connections[i]->fd==fd)	/* A asks for LST. dont send A in that list */
			continue;
		sprintf(str, "%lu;%hu\n", conn->IP, conn->port);	/* IP is in HBO */
		strcat(message, str);
	}
	strcat(message, "\n");	/* double \n to end list */

	#ifdef DEBUG
	printf("going to send connections list: %s", message);
	#endif

	len = strlen(message);
	err = TCPsend(fd, message, len);
	return err;
}

int chat_LST(int *nr_chats, char *mess, unsigned long myIP, unsigned short myport, connection **connections)
{
	char *aux;
	unsigned long IP;
	unsigned short port;
	int fd, err;

	#ifdef DEBUG
	printf("received connections list: %s", mess);
	#endif

	aux = mess;
	aux += strlen("LST\n");

	while((*aux)!='\n')	/* list ends in double \n */
	{
		if(sscanf(aux, "%lu;%hu\n", &IP, &port)!=2)
		{
			#ifdef DEBUG
			puts("error while processing TCP LST");
			#endif
			return -1;
		}

		#ifdef DEBUG
		printf("connecting to %08lX:%d\n", /*(unsigned long)htonl*/(IP), /*htons*/(port));
		#endif

		fd = TCPconnect(IP, port);	/* connect to connection just discovered */
		if(fd<0)
		{
			#ifdef DEBUG
			printf("error connecting in chat_LST(). err = %d\n", fd);
			#endif
			return -1;
		}

		err = chat_send_ADD(fd, myIP, myport);	/* ask to be added to their connections list */
		#ifdef DEBUG
		if(err!=0)
		puts("failed to send ADD in chat_LST()");
		#endif

		chat_add(fd, IP, port, connections);	/* store new connection */
		(*nr_chats)++;

		aux = strchr(aux, '\n');
		aux++;	/* move to next in list*/
	}

	return 0;
}

int chat_add(int fd, unsigned long IP, unsigned short port, connection **connections)
{
	int i;
	connection *conn = malloc(sizeof(connection));
	if(conn==NULL)
		return -1;

	conn->fd = fd;
	conn->IP = IP;
	conn->port = port;
	conn->mess[0]='\0';

	/* add new connection to list */

	for(i=FIRST_FD; connections[i]!=NULL; i++)
		;

	connections[i] = conn;

	return i;
}

int chat_remove(int fd, connection **connections)
{
	int i;

	for(i=FIRST_FD; connections[i]!=(connection*)1; i++)
	{
		if(connections[i]!=NULL && connections[i]->fd==fd)
			break;
	}
	if(connections[i]==(connection*)1)
		return -1;

	free(connections[i]);
	connections[i] = NULL;

	return i;
}

int chat_fd_comp(int fd1, int fd2)
{
	return fd1 == fd2;
}





