#include "UDPlib.h"
#include "database.h"


#ifndef _INCOMING_H
#define _INCOMING_H



int MSS(char *str);

/******************************************************
Verifies if an "OK" string was received from port at IP
returns 0 on sucess -1 on error
******************************************************/
int OK(unsigned long IP,unsigned short port);
int OKlistrcv(list ** OK_list);
int UDPprocess(db * mydb,person * me);
int REG(db * mydb, UDPmssinfo * received);
int DNS(db * mydb, UDPmssinfo * received, person * me);
int QRY(db * mydb, UDPmssinfo * received);
int UNR(db * mydb, UDPmssinfo * received);

#endif
