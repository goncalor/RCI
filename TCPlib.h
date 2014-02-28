#ifndef _TCPlib_H
#define _TCPlib_H

int TCPconnect(unsigned long IP, unsigned short port);
int TCPsend(int fd, char *message, unsigned int len);
int TCPrecv(int fd, char *str, unsigned int len);
int TCPclose(int fd);
int TCPcreate(unsigned long IP, unsigned short port);

#endif
