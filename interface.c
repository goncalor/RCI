#include "interface.h"
#include "database.h"
#include "list.h"
#include "UDPlib.h"
#include "inetutils.h"
#include "define.h"
#include "incoming.h"
#include "TCPlib.h"
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

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

	UDPfreemssinfo(received);

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

	personupdate(auth,atoh(IP),UDPport, getpersonTCPport(me), name, surname);
	
	if(dbinsertperson(mydb,auth)==-1)
		return -5;
	Iamtheauth(mydb);
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
			

		Iamnottheauth(mydb);

		char * message= UDPgetmss(LST);
		char buffer[BUF_LEN];
		char * aux;
		unsigned short TCPport;
		person * aux_person;
		sscanf(message,"%s",buffer);
		if(strcmp(buffer,"LST")!=0)
			return -8; /* WTF just happened? 2 */			
		aux=message+strlen(buffer)+1;
		if(aux[0]=='\n')
		{		/*That name is already in use*/
				#ifdef DEBUG
					puts("Name in use;");
				#endif
			personfree(auth);
			UDPclose();
			UDPfreemssinfo(LST);
			return -9;
		}
		Iamnottheauth(mydb);		
		do 
		{
			if(sscanf(aux,"%[^.].%[^;];%[^;];%hu;%hu", name, surname, IP, &TCPport, &UDPport)!=5)
			{
				#ifdef DEBUG
					puts("LST processing error");
				#endif
				break;
			}
			aux_person = personcreate(atoh(IP),UDPport,TCPport,name,surname);
			if(dbinsertperson(mydb,aux_person)==-1)
				return -10;
			aux=strchr(aux,'\n');
			aux++;
		}while((*aux)!='\n');
		UDPfreemssinfo(LST);

		#ifdef DEBUG
			puts("Register myself on everyone's list");
		#endif
		int i;
		list * aux_list;		

		for(i=0;i<255;i++)
		{
			if(mydb->db_table[i]!=NULL)
			{
				for(aux_list = mydb->db_table[i]; aux_list!=NULL; aux_list = LSTfollowing(aux_list))
				{				
					aux_person = LSTgetitem(aux_list); /*Changed from mydb->db_table[i] to aux_list -> test whenever possible*/
					if(personcmp(aux_person,me)!=1&&personcmp(aux_person,auth)!=1)
					{
						if(UDPsend(getpersonIP(aux_person),getpersonUDPport(aux_person),info)==-1)
							return -11;
						if(OK(getpersonIP(aux_person),getpersonUDPport(aux_person))!=0)
							return -12;
					}		
				}
			}
		}
		personfree(auth);
		return fdUDP;
	}
}


/* found is created by find. returns 0 on success*/
int find(unsigned long saIP, unsigned short saport, char *name, char *surname, person **found, person *me, db *mydb)
{
	char str[BUF_LEN];
	UDPmssinfo *received;
	char authname[NAME_LEN], authsurname[NAME_LEN], authIPascii[16], SC_IPascii[16];
	unsigned short authDNSport, talkport;
	unsigned long authIP, SC_IP;
	person *aux;

	if(strcmp(getpersonsurname(me), surname)==0)
	{
		#ifdef DEBUG
		puts("person has same surname as me");
		#endif

		aux = personcreate(0, 0/*DNSport*/, 0, name, surname);
		if(aux==NULL)
			return -11;

		/* find person name.surname in my database */

		*found = dbpersonfindbyname(mydb, aux);
		personfree(aux);	/* not needed anymore */
		if(*found==NULL)
		{
			#ifdef DEBUG
			puts("person not found in my database");
			#endif

			return -12;
		}

		SC_IP = getpersonIP(*found);
		talkport = getpersonTCPport(*found);

		*found = personcreate(SC_IP, 0/*DNSport*/, talkport, name, surname);
		return 0;	/* person found in my database */
	}


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
	printf("auth DNS port: %hu\n", authDNSport);
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

	#ifdef DEBUG
	printf("recv %s\n", UDPgetmss(received));
	#endif

	if(UDPcmpsender(authIP, authDNSport, received)!=0)
		return -8;


	strcpy(str, UDPgetmss(received));

	UDPfreemssinfo(received);

	if(sscanf(str, "RPL %[^.].%[^;];%[^;];%hu", name, surname, SC_IPascii, &talkport)!=4)
	{
		if(strncmp(str, "RPL", 3)==0)	/* name.surname is not registered in SNP */
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

	*found = personcreate(SC_IP, 0/*DNSport*/, talkport, name, surname);

	return 0;	/* person found in another SNP database */
}


int leave(person * me, unsigned long saIP, unsigned short saport, db*mydb)
{
	char buffer[BUF_LEN];
	/*char * name = getpersonname(me);*/
	/*int db_index = (int)name[0];*/
	int alone;
	int i;
	if(AmItheauth(mydb)==1)
	{
		#ifdef DEBUG
			puts("I am leaving and I am the auth");
		#endif
		alone=1; /*first we supose we are alone*/
		for(i=0;i<255;i++)
		{
			if(mydb->db_table[i]!=NULL)
			{
				if(personcmp(me,LSTgetitem(mydb->db_table[i]))!=1 || LSTfollowing(mydb->db_table[i])!=NULL)
				{
					alone=0;	/*We are not alone*/
					break;
				}
			}
		}


		if(alone==1)
		{
			#ifdef DEBUG
				puts("I am the only person with this surname");
			#endif
			sprintf(buffer,"UNR %s.%s",getpersonname(me),getpersonsurname(me));
			if(UDPsend(saIP, saport,buffer)==-1)
			{
				return -1;
			}
			if(OK(saIP,saport)!=0)
				return -1;
			
			/*Free the db*/
			dbclean(mydb);
			UDPclose();
			return 0;

		}
		#ifdef DEBUG
			puts("I am not the only person with this surname");
		#endif

		list * aux_list;
		person * aux_person;
		for(i=0;i<255;i++)
		{
			if(mydb->db_table[i]!=NULL)
			{
				if(personcmp(me,LSTgetitem(mydb->db_table[i]))!=1)
				{
					aux_list = mydb->db_table[i]; 		
					break;
				}
				if(LSTfollowing(mydb->db_table[i])!=NULL)
				{				
					aux_list = LSTfollowing(mydb->db_table[i]);
					break;
				}
			} 
		}

		aux_person = LSTgetitem(aux_list);

		unsigned long newauthIP = htonl(getpersonIP(aux_person)); /*my IP in network byte order*/
		struct in_addr * ip_aux;
		ip_aux = (struct in_addr *) &newauthIP;	
	
		sprintf(buffer,"DNS %s.%s;%s;%hu",getpersonname(aux_person),getpersonsurname(aux_person),inet_ntoa(*ip_aux),getpersonUDPport(aux_person));

		#ifdef DEBUG
			printf("Sending %s to SA \n",buffer);
		#endif				

		if(UDPsend(saIP,saport,buffer)==-1)
			return -1;
		if(OK(saIP,saport)!=0)
			return -1;

		#ifdef DEBUG
			printf("Sending %s to new auth \n",buffer);
		#endif		

		if(UDPsend(getpersonIP(aux_person),getpersonUDPport(aux_person),buffer)==-1)
			return -1;
		if(OK(getpersonIP(aux_person),getpersonUDPport(aux_person))!=0)
			return -1;
		
		sprintf(buffer,"UNR %s.%s",getpersonname(me),getpersonsurname(me));

		#ifdef DEBUG
			printf("Sending %s to everyone on the DB \n",buffer);
		#endif		

		for(i=0;i<255;i++)
		{
			for(aux_list=mydb->db_table[i]; aux_list!=NULL; aux_list = LSTfollowing(aux_list))
			{				
				aux_person = LSTgetitem(aux_list);
				if(personcmp(aux_person,me)!=1)
				{
					if(UDPsend(getpersonIP(aux_person),getpersonUDPport(aux_person),buffer)==-1)
						return -1;
					if(OK(getpersonIP(aux_person),getpersonUDPport(aux_person))!=0)
						return -1;
				}
			}
		}

			/*Free the db*/
		dbclean(mydb);
		UDPclose();
		return 0;


	}
	else
	{
		#ifdef DEBUG
			puts("I am leaving and I am not the auth");
		#endif
		sprintf(buffer,"UNR %s.%s",getpersonname(me),getpersonsurname(me));
		list * aux_list;
		person * aux_person;

		#ifdef DEBUG
			printf("Sending %s to everyone on the DB with my surname \n",buffer);
		#endif		
		for(i=0;i<255;i++)
		{
			for(aux_list = mydb->db_table[i]; aux_list!=NULL; aux_list = LSTfollowing(aux_list))
			{				
				aux_person = LSTgetitem(aux_list);
				if(personcmp(aux_person,me)!=1)
				{
					if(UDPsend(getpersonIP(aux_person),getpersonUDPport(aux_person),buffer)==-1)
						return -1;
					if(OK(getpersonIP(aux_person),getpersonUDPport(aux_person))!=0)
						return -1;
				}
			}
		}

		/*Free the db*/
		dbclean(mydb);
		UDPclose();
		return 0;
	}
}

/* writes message to fd and sends it via TCP. \0 is not sent. returns 0 on successful send. -1 on error and sets errno 
note: the user receiving the message expects it to end in \n */
int message(int fd, char *message, person *me)
{
	int len;
	char header[] = "MSS ";
	char *str, *name, *surname;

	name = getpersonname(me);
	surname = getpersonsurname(me);

	len = strlen(header) + strlen(name) + strlen(surname) + strlen(message) + 3; /* +3 for . ; \n */

	str = malloc(len+1);	/* +1 for \0 */
	if(str==NULL)
		return -2;

	sprintf(str, "%s%s.%s;%s\n", header, name, surname, message);

	#ifdef DEBUG
	printf("sending %s", str);
	#endif

	if(TCPsend(fd, str, len)!=0)	/* \0 is not sent */
	{
		free(str);
		return -1;
	}

	free(str);
	return 0;
}

/* returns a file descriptor for communication on success. returns negative on error */
int Connect(unsigned long saIP, unsigned short saport, char *name, char *surname, person *me, db *mydb)
{
	int err, fd;
	person *found;

	err = find(saIP, saport, name, surname, &found, me, mydb);
	if(err!=0)
		return -1;	/* person not found */

	fd = TCPconnect(getpersonIP(found), getpersonTCPport(found));
	personfree(found);	/* not necessary anymore */
	if(fd<0)
		return -2;	/* unable to connect */

	return fd;
}

