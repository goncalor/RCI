#ifndef _MPCHAT_H
#define _MPCHAT_H

#define BUF_LEN 1024

typedef struct connection{
	int fd;
	unsigned long IP;	/* host byte order */
	short int port;	/* HBO */
	char mess[BUF_LEN];
} connection;


int chat_send_WHO(int fd, unsigned long myIP, unsigned short myport);
int chat_WHO(int fd, char *mess, connection **connections);
int chat_send_LST(int fd, connection **connections);
int chat_LST(int *nr_chats, char *mess, connection **connections);
int chat_add(int fd, unsigned long IP, unsigned short port, connection **connections);
int chat_remove(int fd, connection **connections);
int chat_fd_comp(int fd1, int fd2);


#endif
