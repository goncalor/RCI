#define BUF_LEN 1024







int join(person * me, unsigned long saIP, unsigned short saport)
{
	char info[BUF_LEN];
/*send info to SA*/
	if(UDPcreate(me->DNSport)==-1)
		return -1;

	sprintf(info,"REG %s.%s;%s;%hu;%hu", me->name,me->surname,inet_ntoa(htonl(me->IP)),me->TCPport,me->UDPport);
	if(UDPsend(saIP,saport,info)==-1)
		return -1;

	/*Now wait for answer from server?*/

	/*It will block if no answer*/
	UDPmssinfo * received = UDPrcv();

	if(UDPcmpsender(saIP,saport,received)!=0)
		return -2;

	char name[BUF_LEN];
	char surname[BUF_LEN];
	char IP[BUF_LEN];
	unsigned short port;

	sscanf(UDPgetmss(received),"DNS %[^.].%[^;];%[^;];%d",name, surname,IP,&port);
	person * auth = personcreate(ntohl(inet_aton(IP)),port,0,name,surname);

	if(personcmp(me,&auth)==1)
	{
		/*I am the auth
		Steps:
		-Insert myself in the Database?
		-Activate the flag saying I am the auth
		-free the auth
		-return
	*/
	}
	else 
	{
		/*I am not the auth*/
		/*
		Steps:
		- Go to the auth and REG;
		- Receive the LST
		- Fill the DataBase;
		- REG yourself in all		
		- free the auth
		- Return

	}
