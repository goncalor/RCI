#define BUF_LEN 1024







int join(char * name,char * surname, unsigned long saIP,unsigned long ddIP, unsigned short saport, unsigned short dnsport, unsigned short talkport)
{
	char info[BUF_LEN];
/*send info to SA*/
	if(UDPcreate(dnsport)==-1)
		return -1;

	sprintf(info,"REG %s.%s;%s;%hu;%hu", name, surname,inet_ntoa(htonl(ddIP)),talkport,dnsport);
	if(UDPsend(saIP,saport,info)==-1)
		return -1;

	/*Now wait for answer from server?*/

	/*It will block if no answer*/
	UDPmssinfo * received = UDPrcv();

	if(UDPcmpsender(saIP,saport,received)!=0)
		return -2;

	char authname[BUF_LEN];
	char authsurname[BUF_LEN];
	unsigned long authIP;
	unsigned short authdnsport;

	sscanf(UDPgetmss,"DNS %[^.].%[^;];%[^;];%s",authname, authsurname, &authIP, &authdnsport);
