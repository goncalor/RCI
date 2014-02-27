#define BUF_LEN 1024







int join(char * name,char * surname, unsigned long saIP,unsigned long ddIP, unsigned short saport, unsigned short dnsport, unsigned short talkport)
{
	char info[BUF_LEN];
/*send info to SA*/

	sprintf(info,"REG %s.%s;%s;%hu;%hu", name, surname,inet_ntoa(htonl(ddIP)),talkport,dnsport);
	if(UDPsend(saIP,saport,info)==-1)
		return -1;
	
	/*Now wait for answer from server?*/

	/*It will block if no answer*/



