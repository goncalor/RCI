#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include <arpa/inet.h>

/* returns a 0 terminated vector of addresses associated with host hostname in host byte order. returns NULL if host was not found (errno=1) or could not allocate memory (errno=2)*/
unsigned long *getIPbyname(char *hostname)
{
	unsigned long *IPs;
	struct hostent *hostinfo = gethostbyname(hostname);
	struct in_addr *address;
	int addrno, i;

	if(hostinfo == NULL)
	{
		errno = 1;
		return NULL;
	}

	/*check how many addresses are in h_addr_list */
	addrno=0;
	do
	{
		address = (struct in_addr*)hostinfo->h_addr_list[addrno];
		addrno++;
	}while(address!=NULL);

	IPs = calloc(addrno, sizeof(unsigned int));	/*all bytes set to zero*/

	if(IPs==NULL)
	{
		errno = 2;
		return NULL;
	}

	addrno--;	/* do not access last h_addr_list (NULL) */

	for(i=0; i<addrno; i++)
	{
		address = (struct in_addr*)hostinfo->h_addr_list[i];
		IPs[i] = ntohl(((struct in_addr*)hostinfo->h_addr_list[i])->s_addr);
	}

	return IPs;
}


/* returns the address in ascii_address converted to an unsigned long integer, in host byte order. returns 0 if ascii_address is an unvalid IPv4 address */
unsigned long atoh(char *ascii_address)
{
	unsigned long IP;
	struct in_addr address;

	if(inet_aton(ascii_address, &address)==0)
	{
		errno = 1;
		return 0;
	}

	IP = ntohl(address.s_addr);

	return IP;
}

