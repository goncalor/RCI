#include "interface.h"
#include "database.h"
#include "list.h"
#include "UDPlib.h"
#include "inetutils.h"
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "define.h"


#define BUF_LEN 1024


int join(person * me, unsigned long saIP, unsigned short saport, db * mydb)
{
	char info[BUF_LEN];
	int fdUDP;
	unsigned long myIPn = htonl(getpersonIP(me)); /*my IP in network byte order*/
	struct in_addr * ip_aux;
	ip_aux = (struct in_addr *) &myIPn;	
/*send info to SA*/
		#ifdef DEBUG
			puts("creating UDP");
		#endif
	if((fdUDP=UDPcreate(getpersonUDPport(me)))==-1)
		return -1;

	sprintf(info,"REG %s.%s;%s;%hu;%hu", getpersonname(me),getpersonsurname(me),inet_ntoa(*ip_aux),getpersonTCPport(me), getpersonUDPport(me));
		#ifdef DEBUG
			printf("Sending: %s\nUDPfd=%d\n",info,fdUDP);
		#endif
	if(UDPsend(saIP,saport,info)==-1)
		return -2;

	/*Now wait for answer from server?*/
			#ifdef DEBUG
			puts("Waiting for answer from server");
		#endif
	/*It will block if no answer*/
	UDPmssinfo * received = UDPrecv();
	if(received==NULL)
		return -3;
	#ifdef DEBUG
			printf("Received:%s\n",UDPgetmss(received));
		#endif

	if(UDPcmpsender(saIP,saport,received)!=0)
		return -4;

	char name[BUF_LEN];
	char surname[BUF_LEN];
	char IP[BUF_LEN];
	unsigned short UDPport;

	if(sscanf(UDPgetmss(received),"DNS %[^.].%[^;];%[^;];%hu",name, surname,IP,&UDPport)!=4)
		return -11;
	person * auth = personcreate(atoh(IP),UDPport,0,name,surname);

	if(personcmp(me,auth)==1)
	{
		#ifdef DEBUG
			puts("I am the auth");
		#endif
		/*I am the auth
		Steps:
		-Insert myself in the Database?
		-Activate the flag saying I am the auth
		-free the auth
		-return
	*/
	
	if(dbinsertperson(mydb,me)==-1)
		return -5;
	Iamtheauth(mydb);
	personfree(auth);
	return fdUDP;	

	}
	else 
	{
		#ifdef DEBUG
			puts("I am not the auth");
		#endif
		/*I am not the auth

		Steps:
		- Go to the auth and REG;
		- Receive the LST
		- free the auth
		- Fill the DataBase;
		- REG yourself in all		
		- Return*/
		if(UDPsend(getpersonIP(auth),getpersonUDPport(auth),info)==-1)
			return -6;
		UDPmssinfo * LST= UDPrecv();	/*Maybe there is a problem here.we could only receive part of the message*/
		if(UDPcmpsender(getpersonIP(auth),getpersonUDPport(auth),LST)!=0)
			return -7; /* WTF just happened?*/
		mydb = dbcreate();			

		Iamnottheauth(mydb);
		personfree(auth);
		char * message= UDPgetmss(LST);
		char buffer[BUF_LEN];
		char * aux;
		unsigned short TCPport;
		person * aux_person;
		sscanf(message,"%s",buffer);
		if(strcmp(buffer,"LST")!=0)
			return -8; /* WTF just happened? 2 */			
		aux=message+strlen(buffer)+1;
		if(aux[1]=='\n')
		{		/*That name is already in use*/
			UDPclose();
			UDPfreemssinfo(LST);
			return -9;
		}
		Iamnottheauth(mydb);		
		do 
		{
			sscanf(aux,"%[^.].%[^;];%[^;];%hu;%hu", name, surname, IP, &TCPport, &UDPport);
			aux_person = personcreate(atoh(IP),UDPport,TCPport,name,surname);
			if(dbinsertperson(mydb,aux_person)==-1)
				return -10;
			aux=strchr(aux,'\n');
		}while(aux[1]!='\n');
		UDPfreemssinfo(LST);

		/*Register myself on everyone's list*/

		int i;
		list * aux_list;		

		for(i=0;i<255;i++)
		{
			if(mydb->db_table[i]!=NULL)
			{
				for(aux_list = mydb->db_table[i]; aux_list!=NULL; aux_list = LSTfollowing(aux_list))
				{				
					aux_person = LSTgetitem(mydb->db_table[i]);
					if(personcmp(aux_person,me)!=1)
					{
						if(UDPsend(getpersonIP(aux_person),getpersonUDPport(aux_person),info)==-1)
							return -11;
					}		
				}
			}
		}
		return fdUDP;
	}
}


/* found is created by find. returns 0 on success*/
int find(unsigned long saIP, unsigned short saport, char *name, char *surname, person *found)
{
	char str[BUF_LEN];
	UDPmssinfo *received;
	char authname[NAME_LEN], authsurname[NAME_LEN], authIPascii[16], SC_IPascii[16];
	unsigned short authDNSport, talkport;
	unsigned long authIP, SC_IP;

	/* query SA */

	sprintf(str, "QRY %s.%s", name, surname);

	if(UDPsend(saIP, saport, str)==-1)
	{
		return -1;
	}

	/* get and process reply from SA */

	received = UDPrecv();
	if(received==NULL)
		return -2;

	if(UDPcmpsender(saIP, saport, received)!=0)
		return -3;

	#ifdef DEBUG
	printf("recv %s\n", UDPgetmss(received));
	#endif

	strcpy(str, UDPgetmss(received));

	UDPfreemssinfo(received);

	if(sscanf(str, "FW %[^.].%[^;];%[^;];%hu", authname, authsurname, authIPascii, &authDNSport)!=4)
	{
		if(strcmp(str, "FW")==0)	/* no such surname in SA */
			return -4;
		else
			return -5;
	}

 	authIP = atoh(authIPascii);	/* in host byte order */

	#ifdef DEBUG
	printf("auth name: %s.%s\n", authname, authsurname);
	printf("authIP: %08lX  (hex)\n", authIP);
//	printf("talkport: %hu\n", talkport);
	printf("auth DNS port: %hu\n", authDNSport);
//	printf("saIP: %08lX  (hex)\n", saIP);
//	printf("saport: %hu\n", saport);
	#endif

	/* query SNP */

	sprintf(str, "QRY %s.%s", name, surname);

	if(UDPsend(authIP, authDNSport, str)==-1)
	{
		return -6;
	}

	/* get and process reply from SNP */

	received = UDPrecv();
	if(received==NULL)
		return -7;

	if(UDPcmpsender(authIP, authDNSport, received)!=0)
		return -8;

	#ifdef DEBUG
	printf("recv %s\n", UDPgetmss(received));
	#endif

	strcpy(str, UDPgetmss(received));

	UDPfreemssinfo(received);

	if(sscanf(str, "RPL %[^.].%[^;];%[^;];%hu", name, surname, SC_IPascii, &talkport)!=4)
	{
		if(strcmp(str, "RPL")==0)	/* name.surname is not registered in SNP */
			return -9;
		else
			return -10;
	}

 	SC_IP = atoh(SC_IPascii);	/* in host byte order */

	#ifdef DEBUG
	printf("found name: %s.%s\n", name, surname);
	printf("SC_IP: %08lX  (hex)\n", SC_IP);
	printf("talkport: %hu\n", talkport);
	#endif

	found = personcreate(SC_IP, 0/*DNSport*/, talkport, name, surname);

	return 0;
}


int leave(person * me, unsigned long saIP, unsigned short saport, db*mydb)
{
	if(AmItheauth(mydb)==1)
	{

	}

	else
	{


	}
}

