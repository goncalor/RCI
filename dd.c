#include "utils.h"
#include "inetutils.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>

int main(int argc, char **argv)
{
	unsigned short talkport=7000;
	unsigned short dnsport=7000;
	unsigned short saport=58000;
	unsigned long saIP = getIPbyname("tejo.ist.utl.pt", 0);


/*-------- check arguments --------*/

	int opt;

	if(argc<3 || argc>7) /* even number of arguments */
	{
		usage(argv[0]);
		puts("Exiting...");
		exit(1);
	}
	else
	{
		while((opt=getopt(argc, argv, "t:d:i:p:"))!=-1) /*getopt() returns one arg at a time. sets optarg*/
		{
			switch(opt)
			{
				case 't':  break;
				case 'd':  break;
				case 'i':  break;
				case 'p':  break;
				case '?': usage(argv[0]); exit(0);
			}
		}
	}






	exit(0);
}
