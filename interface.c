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


int join(person * me, unsigned long saIP, unsigned short saport)
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
		return -1;

	/*Now wait for answer from server?*/
			#ifdef DEBUG
			puts("Waiting for answer from server");
		#endif
	/*It will block if no answer*/
	UDPmssinfo * received = UDPrecv();
	if(received==NULL)
		return -2;
	#ifdef DEBUG
			printf("Received:%s\n\n",UDPgetmss(received));
		#endif

	if(UDPcmpsender(saIP,saport,received)!=0)
		return -2;

	char name[BUF_LEN];
	char surname[BUF_LEN];
	char IP[BUF_LEN];
	unsigned short UDPport;

	sscanf(UDPgetmss(received),"DNS %[^.].%[^;];%[^;];%hu",name, surname,IP,&UDPport);
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
	db * mydb = dbcreate();	
	if(dbinsertperson(mydb,me)==-1)
		return -3;
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
			return -4;
		UDPmssinfo * LST= UDPrecv();
		if(UDPcmpsender(getpersonIP(auth),getpersonUDPport(auth),LST)!=0)
			return -5; /* WTF just happened?*/
		db * mydb = dbcreate();			
		Iamnottheauth(mydb);
		personfree(auth);
		char * message= UDPgetmss(LST);
		char buffer[BUF_LEN];
		char * aux;
		unsigned short TCPport;
		person * aux_person;
		sscanf(message,"%s",buffer);
		if(strcmp(buffer,"LST")!=0)
			return -6; /* WTF just happened? 2 */			
		aux=message+strlen(buffer)+1;
		if(aux[1]=='\n')
		{		/*That name is already in use*/
			UDPclose();
			UDPfreemssinfo(LST);
			return -7;
		}
		Iamnottheauth(mydb);		
		do 
		{
			sscanf(aux,"%[^.].%[^;];%[^;];%hu;%hu", name, surname, IP, &TCPport, &UDPport);
			aux_person = personcreate(atoh(IP),UDPport,TCPport,name,surname);
			if(dbinsertperson(mydb,aux_person)==-1)
				return -3;
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
							return -4;
					}		
				}
			}
		}
		return fdUDP;
	}
}





















