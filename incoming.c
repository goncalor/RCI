#include "define.h"
#include "UDPlib.h"
#include "database.h"
#include "inetutils.h"
#include "incoming.h"
#include "okinfo.h"
#include <arpa/inet.h>
#include "string.h"
#include <stdio.h>

#define BUF_LEN 1024


/* prints incoming message. str is altered to be only the message (without protocol words. returns 0 on success. returns -1 if received message is badly formatted */
int MSS(char *str)
{
	char name[NAME_LEN], surname[NAME_LEN], buf[BUF_LEN];

	if(sscanf(str, "MSS %[^.].%[^;]; %[^\n]", name, surname, buf)!=3)
	{
		#ifdef DEBUG
		printf("received malformated string %s", str);
		printf("name: %s\nsurname: %s\nbuf: %s\n", name, surname, buf);
		#endif

		return -1;
	}

	printf("%s.%s: %s\n", name, surname, buf);	

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

int OKlistrcv(list ** OK_list)
{
	OKinfo * OKrcv;
	UDPmssinfo * received;

while(*OK_list!=NULL)
{
	received = UDPrecv();
	if(strncmp("OK",UDPgetmss(received),2)!=0)
		return -1;
	OKrcv=OKinfocreate(UDPgetIP(received), UDPgetport(received));
	if(OKsearchandrm(OK_list , OKrcv)==-1)
		return -2;
	OKinfofree(OKrcv);
	UDPfreemssinfo(received);
}
	return 0;
}

int UDPprocess(db * mydb, person * me)
{
	UDPmssinfo * received = UDPrecv();
	char * mssaux = UDPgetmss(received);
	int i;

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

			i=REG(mydb, received);
			if(i!=0)
			{
				printf("REG Error: %d\n",i);
				return -1;
			}
			
	}
	else if(strncmp("DNS",mssaux,3)==0)
	{
		if(DNS(mydb,received, me)!=0)
			return -2;
	}
	else if(strncmp("QRY",mssaux,3)==0)
	{
		i=QRY(mydb, received);
		if(i<0)
		{
			printf("REG Error: %d\n",i);
			return -3;
		}
	}
	else if(strncmp("UNR",mssaux,3)==0)
	{
		i=UNR(mydb,received);
		if(i!=0)
		{
			printf("UNR Error: %d\n",i);
			return -4;
		}
	}
	else if(strncmp("OK",mssaux,2)==0)
	{
	#ifdef DEBUG
	puts("received random Ok there must have been an error");
	#endif
		return -5;
	}
	else 
	{
		/*Unknown command*/
		return -6;
	}
	UDPfreemssinfo(received);
	return 0;
}

int REG(db * mydb, UDPmssinfo * received)
{
	char name[NAME_LEN];
	char surname[NAME_LEN];
	char IP[IP_LEN];
	unsigned short TCPport;
	unsigned short UDPport;
	unsigned long IPh;
	if(sscanf(UDPgetmss(received),"REG %[^.].%[^;];%[^;];%hu;%hu",name, surname,IP,&TCPport,&UDPport)!=5)
		return -1;
	IPh=atoh(IP);
	person * new =personcreate(IPh,UDPport,TCPport,name,surname);

	/*Don't know if I should compare who sent the REG with the info on REG*/

	if(UDPcmpsender(IPh,UDPport, received)!=0)
		return -2; /*I have to think about this*/

	/*Verificar se o utilizador Já está registado.*/
	if(dbpersonfind(mydb,new)==NULL)	
	{	
		if(dbinsertperson(mydb,new)!=0)
			return -1;
		if(AmItheauth(mydb)==1)
		{

			#ifdef DEBUG
			printf("I am the auth, preparing the LST\n");
			#endif

			int i;
			list * aux_list;		
			person * aux_person;
			char LSTstr[BUF_LEN];
			char aux_str[BUF_LEN];
			strcpy(LSTstr, "LST\n");
			unsigned long myIPn; /* my IP in network byte order */
			struct in_addr * ip_aux;
			ip_aux = (struct in_addr *) &myIPn;	
		


			for(i=0;i<255;i++)
			{
				if(mydb->db_table[i]!=NULL)
				{
					for(aux_list = mydb->db_table[i]; aux_list!=NULL; aux_list = LSTfollowing(aux_list))
					{				
						aux_person = LSTgetitem(aux_list); /* Changed from mydb->db_table[i] to aux_list -> test whenever possible */
						myIPn = htonl(getpersonIP(aux_person));
						sprintf(aux_str,"%s.%s;%s;%hu;%hu\n",getpersonname(aux_person),getpersonsurname(aux_person),inet_ntoa(*ip_aux), getpersonTCPport(aux_person), getpersonUDPport(aux_person));
						strcat(LSTstr, aux_str);
							#ifdef DEBUG
							printf("LST building:%s\n",LSTstr);
							#endif
					}
				}
			}
			strcat(LSTstr, "\n");

			#ifdef DEBUG
			printf("Going to send:%s",LSTstr);
			#endif

			if(UDPsend(IPh, UDPport, LSTstr)==-1)
				return -1;

			return 0;
		}
	}
	else{
		puts("Already Registered");
	}
	if( UDPsend(IPh, UDPport,"OK")==-1)
			return -1;
	

	return 0;
}

int DNS(db * mydb, UDPmssinfo * received, person * me)
{
	char name[NAME_LEN];
	char surname[NAME_LEN];
	char IP[IP_LEN];
	person * new;
	unsigned short UDPport;
	unsigned long IPh;
	if(sscanf(UDPgetmss(received),"DNS %[^.].%[^;];%[^;];%hu",name, surname,IP,&UDPport)!=4)
		return -1;
	IPh=atoh(IP);
	new =personcreate(IPh,UDPport,0,name,surname);
	if(personcmpbyname(new, me)==1)
	{
		Iamtheauth(mydb);
			#ifdef DEBUG
			puts("I am the auth");
			#endif
	}
	if(UDPsendtosender(received,"OK")==-1)
			return -1;
	personfree(new);
	return 0;
}


int QRY(db * mydb, UDPmssinfo * received)
{
	char name[NAME_LEN];
	char surname[NAME_LEN];
	char RPL_str[BUF_LEN];
	person * new, * person_aux;
	unsigned long IPn;
	struct in_addr * ip_aux;
	ip_aux = (struct in_addr *) &IPn;	

	if(sscanf(UDPgetmss(received), "QRY %[^.].%s", name, surname)!=2)
		return -1;

	new = personcreate(0,0,0,name,surname);
	person_aux = dbpersonfindbyname(mydb,new);
	if(person_aux==NULL)	/* person does not exist in database */
	{
		#ifdef DEBUG
		printf("%s.%s not found in my database\n", name, surname);
		#endif

		personfree(new);
		if(UDPsendtosender(received,"RPL")==-1)
			return -2;
		else
			return 1;
	}

	IPn = htonl(getpersonIP(person_aux));
	sprintf(RPL_str,"RPL %s.%s;%s;%hu", getpersonname(person_aux), getpersonsurname(person_aux), inet_ntoa(*ip_aux), getpersonTCPport(person_aux));

	if(UDPsendtosender(received, RPL_str)==-1)
		return -4;
	personfree(new);
	return 0;
}

int UNR(db * mydb, UDPmssinfo * received)
{
	char name[NAME_LEN];
	char surname[NAME_LEN];
	person * new;

	if(sscanf(UDPgetmss(received),"UNR %[^.].%s",name, surname)!=2)
		return -1;
	new=personcreate(0,0,0,name,surname);
	if(dbrmpersonbyname(mydb, new)!=0)
		return -2; /*person not found*/

	if(UDPsendtosender(received,"OK")==-1)
			return -1;
	personfree(new);
	return 0;
}



