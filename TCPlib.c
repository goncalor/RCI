#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

/* connects IP and port to the socket referred to by the return value. when failing to create a socket returns -1; when failing to connect returns -2. sets errno in both cases. */
int TCPconnect(unsigned long IP, unsigned short port)
{
	int fd;
	struct sockaddr_in address;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd==-1)
	{
		return -1;
	}

	/*memset() not necessary according to 
	http://www.gta.ufrj.br/ensino/eel878/sockets/sockaddr_inman.html */
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(IP);
	address.sin_port = htons(port);

	/* struct sockaddr_in is the same size as struct sockaddr */
	if(connect(fd, (struct sockaddr*)&address, sizeof(address))==-1)
	{
		return -2;
	}

	return fd;
}

/* sends message to the socket referred to by fd. sends only first len bytes. returns 0 on successful send. returns -1 on error and sets errno. */
int TCPsend(int fd, char *message, unsigned int len)
{
	char *ptr = message;
	int nwritten, nleft;

	/* remember to protect write() against SIGPIPE 

	If you call write on a lost connection, write would return -1,
	errno will be set to EPIPE, but the system would raise a
	SIGPIPE signal and, by default, that would kill your process.

	idea: ignore SIGPIPE and then reactivate it after all writes */
	nleft = len;
	while(nleft>0)
	{
		nwritten = write(fd, ptr, nleft);
		if(nwritten<=0)
			return -1;
		nleft -= nwritten;
		ptr += nwritten;
	}

	return 0;
}

/* reads available content from fd and writes at most len bytes to the location starting at str. retuns the number of bytes written to str. returns -1 on unsuccessful read. retuns -2 if connection is closed by peer */
int TCPrecv(int fd, char *str, unsigned int len)
{
	char *ptr = str;
	int nread;

	nread = read(fd, ptr, len);
	if(nread==-1)
		return -1;	/* error while reading */
	else if(nread==0)
		return -2;	/* connection closed by peer */

	return nread;
}

/* closes the file descriptor fd. retuns 0 on success. returns -1 on error and sets errno */
int TCPclose(int fd)
{
	return close(fd);
}

/* binds IP and port to the returned fd. when failing to create a socket returns -1; when failing to bind IP and port to a fd returns -2. sets errno in both cases. */
int TCPcreate(unsigned long IP, unsigned short port)
{
	int fd;
	struct sockaddr_in address;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd==-1)
	{
		return -1;
	}

	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(IP);
	address.sin_port = htons(port);

	/* struct sockaddr_in is the same size as struct sockaddr */
	if(bind(fd, (struct sockaddr*)&address, sizeof(address))==-1)
	{
		return -2;
	}

	return fd;
}
