#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "UDPlib.h"
#include "define.h"

#define BUF_LEN_UDP 5120 /*see max size of UDP Package*/

struct UDPmssinfo {

char * message;
struct sockaddr_in sender;

};

int UDPfd;

int UDPcreate(unsigned short port)
{
	struct sockaddr_in addr;

	if((UDPfd=socket(AF_INET,SOCK_DGRAM,0))==-1)
		return -1; /*verificar se errno is set*/

	memset((void*)&addr,(int)'\0',sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=htonl(INADDR_ANY); /* socket will be bound to all local interfaces */
	addr.sin_port=htons(port);

	if(bind(UDPfd,(struct sockaddr*)&addr,sizeof(addr))==-1)
		return -1;

	return UDPfd;
}

int UDPclose()
{
	return close(UDPfd);
}

int UDPsend(unsigned long IP,unsigned short port,char * str)
{
	struct sockaddr_in dest_addr;

	memset((void*)&dest_addr,(int)'\0',sizeof(dest_addr));
	dest_addr.sin_family=AF_INET;
	dest_addr.sin_addr.s_addr=htonl(IP);
	dest_addr.sin_port=htons(port);

	#ifdef DEBUG
	printf("Sending through UDP=%s\n",str);
	#endif

	if(sendto(UDPfd,str,strlen(str)+1,0,(struct sockaddr*)&dest_addr,sizeof(dest_addr))==-1)
		return -1;

	return 0;
}

int UDPsendtosender(UDPmssinfo * info,char * str)
{
	struct sockaddr_in dest_addr;
	memset((void*)&dest_addr,(int)'\0',sizeof(dest_addr));
	dest_addr.sin_family=AF_INET;
	dest_addr.sin_addr.s_addr=htonl(info->sender.sin_addr.s_addr);
	dest_addr.sin_port=htons(info->sender.sin_port);
	#ifdef DEBUG
	printf("Sending through UDP=%s\n",str);
	#endif
	if(sendto(UDPfd,str,strlen(str)+1,0,(struct sockaddr*)&dest_addr,sizeof(dest_addr))==-1)
		return -1;

	return 0;
}


UDPmssinfo * UDPmssinfocreate(char * message, unsigned short port, unsigned long IP)
{
	UDPmssinfo * new = (UDPmssinfo *) calloc(1, sizeof(UDPmssinfo));
	new->message = (char *)	malloc((strlen(message)+1)*sizeof(char));
	strcpy(new->message, message);
	new->message[strlen(message)]='\0';
	new->sender.sin_family=AF_INET;
	new->sender.sin_addr.s_addr=ntohl(IP);
	new->sender.sin_port=ntohs(port);
	return new;
}


/*

ver: http://stackoverflow.com/questions/15168095/whats-the-addrlen-field-in-recvfrom-used-for

*/
UDPmssinfo * UDPrecv()
{
	struct sockaddr_in src_addr;
	unsigned int addrlen=sizeof(src_addr);
	char buffer[BUF_LEN_UDP];
	int mess_len;

	mess_len = recvfrom(UDPfd,buffer,BUF_LEN_UDP,0,(struct sockaddr*)&src_addr,&addrlen);
	if(mess_len==-1)
		return NULL;

	buffer[mess_len]='\0';

	#ifdef DEBUG
	printf("Received through UDP=%s\n",buffer);
	#endif

	return UDPmssinfocreate(buffer, src_addr.sin_port, src_addr.sin_addr.s_addr);

}


char * UDPgetmss(UDPmssinfo *a)
{
	return a->message;
}
unsigned long UDPgetIP(UDPmssinfo *a)
{
	return a->sender.sin_addr.s_addr;
}

unsigned short UDPgetport(UDPmssinfo *a)
{
	return a->sender.sin_port;
}


int UDPcmpsender(unsigned long IP,unsigned short port, UDPmssinfo *a)
{
	if(IP==a->sender.sin_addr.s_addr){
		if(port==a->sender.sin_port)
			return 0;
		else
			return 1;}
	return -1;
}

void UDPfreemssinfo(UDPmssinfo * to_free)
{
	if(to_free!=NULL)
		if(to_free->message!=NULL)
			free(to_free->message);
	free(to_free);
}


