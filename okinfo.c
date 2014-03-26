#include "okinfo.h"
#include "list.h"
#include <stdlib.h>


OKinfo * OKinfocreate(unsigned long IP, unsigned short port)
{
	OKinfo * new=malloc(sizeof(OKinfo));
	new->IP=IP;
	new->port=port;
	return new;
}

void OKinfofree(OKinfo * tofree)
{
	free(tofree);
}

int OKcmp(OKinfo * one, OKinfo * two)
{
	return one->IP==two->IP && one->port==two->port;
}

int OKsearchandrm(list ** OKinfolist , OKinfo * received)
{
	list * aux, * aux2=NULL;
	for(aux=*OKinfolist;aux!=NULL;aux=LSTfollowing(aux))
	{
		if(OKcmp(LSTgetitem(aux),received)>0)
			break;
		aux2=aux;
	}
	if(aux==NULL)
	{
		#ifdef DEBUG
			puts("OK not found on list");
		#endif
		return -1;
	}
	aux=LSTremove(aux2, aux,(void (*)(Item))OKinfofree);
	if(aux2==NULL)
		*OKinfolist=aux;
	return 0;
}




