#include "list.h"

#ifndef _OKinfo_H_
#define _OKinfo_H_


typedef struct OKinfo {

	unsigned long IP;
	unsigned short port;

} OKinfo;

/*typedef list * OKinfolist;*/

int OKcmp(OKinfo * one, OKinfo * two); 

int OKsearchandrm(list ** OKinfolist , OKinfo * received);


OKinfo * OKinfocreate(unsigned long, unsigned short);

void OKinfofree(OKinfo * tofree);

#endif
