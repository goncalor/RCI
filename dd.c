#include "utils.h"
#include "inetutils.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

#define NAME_LEN 128

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


	#ifdef DEBUG

	printf("username: %s.%s\n", name, surname);
	printf("ddIP: %08lX  (hex)\n", ddIP);
	printf("talkport: %hu\n", talkport);
	printf("dnsport: %hu\n", dnsport);
	printf("saIP: %08lX  (hex)\n", saIP);
	printf("saport: %hu\n", saport);

	#endif

/*-------- END check arguments --------*/




	exit(0);
}
