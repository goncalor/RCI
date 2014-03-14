#include "define.h"
#include "UDPlib.h"
#include "database.h"
#include "inetutils.h"
#include "string.h"
#include <stdio.h>

/* prints incoming message. str is altered to be only the message (without protocol words. returns 0 on success. returns -1 if received message is badly formatted */
int MSS(char *str)
{
	char name[NAME_LEN], surname[NAME_LEN], buf[BUF_LEN];

	if(sscanf(str, "MSS %[^.].%[^;]; %[^\n]\n", name, surname, buf)!=3)
	{
		#ifdef DEBUG
		printf("received malformated string %s", str);
		printf("name: %s\nsurname: %s\nbuf: %s\n", name, surname, buf);
		#endif

		return -1;
	}

	printf("%s %s: %s\n", name, surname, buf);	

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

int UDPprocess(db * mydb)
{
	UDPmssinfo * received = UDPrecv();
	char * mssaux = UDPgetmss(received);

	#ifdef DEBUG
	printf("received %s through UDP\n",mssaux);
	#endif

	/*what kind of message is it:
		-REG : New user, add to db, if we are auth answer LST if not answer OK
		-DNS : new auth, verify if it is me, if it is change the auth flag
		-QRY : looking for some TCP server, respond RPL if not known answer just RPL
		-UNR : someone unregisted, answer OK and delete from db
		- OK : Random Ok, error
		- Unknown command
*/
	if (strncmp("REG",mssaux,3)==0)
	{
		if(REG(mydb, received)!=0)
			return -1;
	}
	else if(strncmp("DNS",mssaux,3)==0)
	{

	}
	else if(strncmp("QRY",mssaux,3)==0)
	{

	}
	else if(strncmp("UNR",mssaux,3)==0)
	{

	}
	else if(strncmp("OK",mssaux,2)==0)
	{
	#ifdef DEBUG
	puts("received random Ok there must have been an error");
	#endif

	}
	else 
	{
		/*Unknown command*/



	}
}

int REG(db * mydb, UDPmssinfo * received)
{
	char name[NAME_LEN];
	char surname[NAME_LEN];
	char IP[IP_LEN];
	unsigned short TCPport;
	unsigned short UDPport;
	unsigned long IPh;
	if(sscanf(UDPgetmss(received),"DNS %[^.].%[^;];%[^;];%hu;%hu",name, surname,IP,&TCPport,&UDPport)!=5)
		return -1;
	IPh=atoh(IP);
	person * new =personcreate(IPh,UDPport,TCPport,name,surname);

	/*Don't know if I should compare who sent the REG with the info on REG*/

	if(UDPcmpsender(IPh,UDPport, received)!=0)
		return -2; /*I have to think about this*/

	/*No matter what we insert them in the db*/

	if(dbinsertperson(mydb,new)!=0)
		return -1;

	if(AmItheauth(mydb)==1)
	{

	}
	else
	{
		if( UDPsend(IPh, UDPport,"OK")==-1)
			return -1;
	}
	return 0;
}

/*
QRY()
{

}
*/
