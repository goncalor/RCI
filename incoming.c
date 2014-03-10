#include "define.h"
#include "UDPlib.h"
#include "string.h"
#include <stdio.h>

/* prints incoming message. str is altered to be only the message (without protocol words. returns 0 on success. returns -1 if received message is badly formatted */
int MSS(char *str)
{
	char name[NAME_LEN], surname[NAME_LEN];

	if(sscanf(str, "MSS %[^.]%[^;];%[^\n]", name, surname, str)!=3)
		return -1;

	printf("%s %s: %s\n", name, surname, str);	

	return 0;
}


int OK(unsigned long IP,unsigned short port)
{
	UDPmssinfo * received = UDPrecv();
	if(UDPcmpsender(IP,port, received)!=0)
		return -1;
	if(strncmp("OK",UDPgetmss(received),2)!=0)
		return -1;
	UDPfreemssinfo(received);
	return 0;
}

/*
QRY()
{

}
*/
