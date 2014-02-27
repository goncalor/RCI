#ifndef _UDPLIB_H
#define _UDPLIB_H

typedef struct UDPmssinfo UDPmssinfo;



/*******************************************************************
Creates a socket to use with UDP and binds it to the port specified

Returns 0 on sucess, -1 otherwise and errno is set.
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

/*******************************************************************
can only be called if there is a message to receive in the socket

only receives one message every time it is called

fills UDPmssinfo in host byte order
*******************************************************************/
UDPmssinfo UDPrecv();

/*******************************************************************

*******************************************************************/
char * UDPgetmss(UDPmssinfo *);

/*******************************************************************
Compares the sender info with the IP and port given

Returns 0 on both IP and port match
Returns 1 on IP match
Returns -1 on IP and port missmatch 
*******************************************************************/
int UDPcmpsender(unsigned long IP,unsigned short port, UDPmssinfo *);

/*******************************************************************

*******************************************************************/
void UDPfreemssinfo(UDPmssinfo *);

/*******************************************************************

*******************************************************************/



#endif

