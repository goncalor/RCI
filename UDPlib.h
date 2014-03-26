#ifndef _UDPLIB_H
#define _UDPLIB_H

typedef struct UDPmssinfo UDPmssinfo;


/*******************************************************************
Creates a socket to use with UDP and binds it to the port specified

Returns the UDP file descriptor on sucess, -1 otherwise and errno is set.
*******************************************************************/
int UDPcreate(unsigned short port);/*should verify if port>1024*/


/*******************************************************************
Closes the UDP socket

Returns 0 on sucess, -1 otherwise and errno is set.
*******************************************************************/
int UDPclose();

/*******************************************************************
Sends str to IP at port

IP must be in host byte order

Returns 0 on sucess, -1 otherwise and errno is set.
*******************************************************************/
int UDPsend(unsigned long IP,unsigned short port,char * str);


int UDPsendtosender(UDPmssinfo * info,char * str);

/*******************************************************************
Creates UDPmssinfo struct and allocs memory
*******************************************************************/
UDPmssinfo * UDPmssinfocreate(char * message, unsigned short port, unsigned long IP);


/*******************************************************************
can only be called if there is a message to receive in the socket
Creates UDPmssinfo struct and allocs memory
only receives one message every time it is called

fills UDPmssinfo in host byte order
*******************************************************************/
UDPmssinfo * UDPrecv();

/*******************************************************************
Returns a pointer to the message in UDPmssinfo
*******************************************************************/
char * UDPgetmss(UDPmssinfo *);

unsigned long UDPgetIP(UDPmssinfo *);

unsigned short UDPgetport(UDPmssinfo *);

/*******************************************************************
Compares the sender info with the IP and port given

Returns 0 on both IP and port match
Returns 1 on IP match
Returns -1 on IP and port missmatch
*******************************************************************/
int UDPcmpsender(unsigned long IP,unsigned short port, UDPmssinfo *);

/*******************************************************************
Frees the memory used by UDPmssinfo
*******************************************************************/
void UDPfreemssinfo(UDPmssinfo *);


#endif

