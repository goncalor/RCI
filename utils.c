#include <stdio.h>

void usage(char *exename)
{
	printf("Usage: %s name.surname IP [-t talkport] [-d dnsport] [-i saIP] [-p saport]\n", exename);
	putchar('\n');
}

void listcommands()
{
	puts("Command list:");
	puts("\t\tjoin");
	puts("\t\tleave");
	puts("\t\tfind name.surname");
	puts("\t\tconnect name.surname");
	puts("\t\tdisconnect");
	puts("\t\tmessage message");
	puts("\t\tlist");
	puts("\t\texit");
}

void version(char *nr)
{
	printf("dd version %s\n", nr);
}
