#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>

/* returns the the IP number entryno associated with host hostname in host byte order. returns 0 on error. sets errno to 1 if host was not found. sets errno to 2 if entryno is greater than the number of IPs known for the host*/
unsigned long getIPbyname(char *hostname, short entryno)
{
	unsigned long IP;
	struct hostent* hostinfo = gethostbyname(hostname);
	struct in_addr* address;
	int addrno;

	if(hostinfo == NULL)
	{
		errno = 1;
		return (int)NULL;
	}

	/*check how many addresses are in h_addr_list */
	for(addrno=0; address!=NULL; addrno++)
	{
		address = (struct in_addr*)hostinfo->h_addr_list[addrno];
	}

	addrno--;

	if(entryno>=addrno)
	{
		errno=2;
		return (int)NULL;
	}
	
	IP = ntohl(((struct in_addr*)hostinfo->h_addr_list[entryno])->s_addr);

	return IP;
}
