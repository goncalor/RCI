#ifndef _TCPlib_H
#define _TCPlib_H

int TCPconnect(unsigned long IP, unsigned short port);
int TCPsend(int fd, char *message, unsigned int len);

int TCPclose(int fd);

#endif
