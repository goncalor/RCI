#include "utils.h"
#include "inetutils.h"
#include "define.h"
#include "TCPlib.h"
#include "database.h"
#include "interface.h"
#include "incoming.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <sys/select.h>

#define BUF_LEN 1024
#define COMM_LEN 40
#define NR_FDS 4

#define DEBUG

int main(int argc, char **argv)
{
	unsigned short talkport=7000;
	unsigned short dnsport=7000;
	unsigned short saport=58000;
	unsigned long *saIPs = getIPbyname("tejo.ist.utl.pt");
	unsigned long saIP;
	unsigned long ddIP;
	char *username, name[NAME_LEN], surname[NAME_LEN];
	char buf[BUF_LEN], comm[COMM_LEN];
	person *me;
	db * mydb =  dbcreate();

	if(saIPs==NULL)
	{
		exit(1);
	}
	else
	{
		saIP = saIPs[0];
		free(saIPs);
	}


/*-------- check arguments --------*/

	int opt;

	if(argc<3 || argc>11) /* at least two arguments */
	{
		usage(argv[0]);
		puts("Exiting...");
		exit(2);
	}
	else
	{
		username = argv[1];
		if(sscanf(username, "%[^.].%s", name, surname)!=2)
		{
			exit(3);
		}

		ddIP = atoh(argv[2]);
		if(ddIP==0)
		{
			puts("Invalid ddIP.");
			exit(4);
		}



		while((opt=getopt(argc, argv, "t:d:i:p:"))!=-1) /*getopt() returns one arg at a time. sets optarg*/
		{
			switch(opt)
			{
				case 't': talkport = atoi(optarg); break;
				case 'd': dnsport = atoi(optarg); break;
				case 'i':
					saIP = atoh(optarg);
					if(saIP==0)
					{
						puts("Invalid saIP.");
						exit(5);
					}
					break;
				case 'p': saport = atoi(optarg); break;
				case '?': usage(argv[0]); exit(6);
			}
		}
	}


	me = personcreate(ddIP, dnsport, talkport, name, surname);

	#ifdef DEBUG

	printf("username: %s.%s\n", name, surname);
	printf("ddIP: %08lX  (hex)\n", ddIP);
	printf("talkport: %hu\n", talkport);
	printf("dnsport: %hu\n", dnsport);
	printf("saIP: %08lX  (hex)\n", saIP);
	printf("saport: %hu\n", saport);

	#endif

/*-------- END check arguments --------*/

/*-------- parse user commands --------*/

	fd_set rfds;
	FD_ZERO(&rfds);
	int max_fd;
	int i, fds[NR_FDS];
	enum {stdin_fd, TCP_fd, UDP_fd, TCP_fd_chat};
	enum {false, true};
	char connected, chatting;
	int err;
	struct sockaddr_in caller_addr;
	unsigned int caller_addr_size;
	person *interloc=NULL;

	connected = false;
	chatting = false;

	for(i=0; i<NR_FDS; i++)
		fds[i]=-1;

	fds[stdin_fd]=0;	/* 0 is fd for stdin */

	while(1)
	{
		err = 0;
		max_fd = 0;
		for(i=0; i<NR_FDS; i++)
			if(fds[i]>=0)
			{
				FD_SET(fds[i], &rfds);
				if(fds[i]>max_fd)
					max_fd=fds[i];
			}

		i = select(max_fd+1, &rfds, NULL, NULL, NULL);


		if(FD_ISSET(fds[UDP_fd], &rfds))
		{
			#ifdef DEBUG
			puts("UDP connection came in for UDP_fd");
			#endif

		}

		if(FD_ISSET(fds[TCP_fd], &rfds))	/* new chat request */
		{
			#ifdef DEBUG
			puts("TCP connection came in for TCP_fd");
			#endif

			if(chatting==false)
			{
				caller_addr_size = sizeof(caller_addr);
				fds[TCP_fd_chat] = accept(fds[TCP_fd], (struct sockaddr *)&caller_addr, &caller_addr_size);
				chatting = true;
			}
			else
			{
				err = accept(fds[TCP_fd], NULL, NULL);
				if(err==-1)
				{
					/*do something about it*/
				}
				else
				{
					close(err);
				}
			}
		}

		if(FD_ISSET(fds[TCP_fd_chat], &rfds))	/* incoming message on chat */
		{
			#ifdef DEBUG
			puts("TCP connection came in for TCP_fd CHAT");
			#endif

			/*preprocess received string (read till \n)*/

			MSS(buf);
		}

		if(FD_ISSET(fds[stdin_fd], &rfds))	/* something was written */
		{
			#ifdef DEBUG
			puts("fgets() read to buffer");
			#endif

			if(fgets(buf, BUF_LEN, stdin)==0)
			{
				#ifdef DEBUG
				puts("fgets() error");
				#endif
				/*do something about it*/
			}


			if(sscanf(buf, " %s", comm)!=1)	/* no command present */
			{
				comm[0]=0;
			}

			if(strcmp(comm, "join")==0)
			{
				#ifdef DEBUG
				puts("join");
				#endif

				if(connected==true)
				{
					puts("> You have joined already.");
				}
				else
				{
					fds[UDP_fd] = join(me, saIP, saport, mydb);

					if(fds[UDP_fd]<0)
					{
						/*do something about it*/
						#ifdef DEBUG
						printf("join Error:%d",fds[UDP_fd]);
						#endif
					}
					else
					{
						connected = true;
						fds[TCP_fd] = TCPcreate(ddIP, talkport);
						if(listen(fds[TCP_fd], 2)==-1)
						{
							/*do something about it*/
						}
					}
				}

			}
			else if(strcmp(comm, "leave")==0)
			{
				#ifdef DEBUG
				puts("leave");
				#endif
				if(connected==true)
				{
					/*Disconnect;*/

					close(fds[TCP_fd]);	/* accept no more connections */
					fds[TCP_fd] = -1;
					fds[UDP_fd] = -1;

					if(chatting==true)
					{
						close(fds[TCP_fd_chat]);
						fds[TCP_fd_chat] = -1;

						chatting=false;
					}

					if(leave(me, saIP, saport, mydb)!=0)
					{
						/*do something about it*/		
					}
					connected=false;
				}
				else 
				{
					puts("> You are not connected.");
				}

			}
			else if(strcmp(comm, "find")==0)
			{
				#ifdef DEBUG
				puts("find");
				#endif

				if(sscanf(buf, " %*s %[^.].%s", name, surname)!=2)
					puts("> find name.surname");
				else if(find(saIP, saport, name, surname, interloc)!=0)
				{
					#ifdef DEBUG
					puts("find ended abruptly");
					#endif

					/*do something about it*/
				}
				else
				{
					printf("> Found %s.%s\n", getpersonname(interloc), getpersonsurname(interloc));

					#ifdef DEBUG
					printf("Found %s.%s at %0lX with talkport %hu\n", getpersonname(interloc), getpersonsurname(interloc), getpersonIP(interloc), getpersonUDPport(interloc) );
					#endif

					personfree(interloc);
				}
			}
			else if(strcmp(comm, "connect")==0)
			{
				#ifdef DEBUG
				puts("connect");
				#endif

				if(sscanf(buf, " %*s %[^.].%s", name, surname)!=2)
					puts("> connect name.surname");

			}
			else if(strcmp(comm, "disconnect")==0)
			{
				#ifdef DEBUG
				puts("disconnect");
				#endif
			}
			else if(strcmp(comm, "message")==0)
			{
				#ifdef DEBUG
				puts("message");
				#endif

				if(sscanf(buf, " %*s %[^\n]", buf)!=1)
					puts("> message string");
				else
				{
					printf("%s\n", buf);
				}
			}
			else if(strcmp(comm, "exit")==0)
			{
				#ifdef DEBUG
				puts("exit");
				#endif

				/*disconnect, free, etc*/

				exit(0);
			}
			else
			{
				printf("Unknown command '%s'\n", comm);
			}
		} /* FD_ISSET(fds[stdin_fd], &rfds) */
	} /* while */


	exit(0);
}
