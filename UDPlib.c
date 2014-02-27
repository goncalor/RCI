#include <arpa/inet.h>

struct UDPmssinfo {

char * message;
struct sockaddr sender;

}

int UDPfd;

int UDPcreate(unsigned short port)
{
	struct sockaddr_in addr;

	if((UDPfd=socket(AF_INET,SOCK_DGRAM,0))==-1)
		return -1; /*verificar se errno is set*/

	memset((void*)&addr,(int)'\0',sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
	addr.sin_port=htons(port);

	if(bind(UDPfd,(struct sockaddr*)&addr,sizeof(addr))==-1)
		return -1;

	return 0;
}

int UDPclose()
{
	return close(UDPfd);
}

int UDPsend(unsigned long IP,unsigned short port,char * str)
{
	unsigned long IPnet=htonl(IP);
	struct sockaddr_in dest_addr;

	memset((void*)&dest_addr,(int)'\0',sizeof(dest_addr));
	addr.sin_family=AF_INET;
	addr.sin_addr=htonl(IP);
	addr.sin_port=htons(port);

	if(sendto(UDPfd,str,strlen(str)+1,0,(struct sockaddr*)&dest_addr,sizeof(dest_addr))==-1)
		return -1;

	return 0;
}
	

/*

ver: http://stackoverflow.com/questions/15168095/whats-the-addrlen-field-in-recvfrom-used-for

*/
UDPmssinfo UDPrecv()
{









}


char * UDPgetmss(UDPmssinfo *a)
{
	return a.message;
}

int UDPcmpsender(unsigned long IP,unsigned short port, UDPmssinfo *a)
{
	if(IP==a.sender.sin_addr.s_addr)
		if(port==a.sender.sin_port)
			return 0;
		else
			return 1;
	return -1;
}

void UDPfreemssinfo(UDPmssinfo *a)
{
	if(a.message!=NULL)
		free(a.message);
}


