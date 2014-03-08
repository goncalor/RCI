#ifndef _INCOMING_H
#define _INCOMING_H

int MSS(char *str);

/******************************************************
Verifies if an "OK" string was received from port at IP
returns 0 on sucess -1 on error
******************************************************/
int OK(unsigned long IP,unsigned short port);
#endif
