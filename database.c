#include "database.h"
#include "list.h"
#include <string.h>
#include <stdlib.h>

#define BUF_LEN 1024


struct person {

	unsigned long IP;
	unsigned short UDPport;
	unsigned short TCPport;
	char * name;
	char * surname;
};



db * dbcreate()
{
	db * new=(db*)calloc(1,sizeof(db));
	new->db_table=(list **)calloc(255,sizeof(void*));/*no need to initialize the List because we use calloc*/

	return new;
}



void Iamtheauth(db*mydb)
{
	mydb->auth=1;
}

void Iamnottheauth(db*mydb)
{
	mydb->auth=0;
}

int AmItheauth(db*mydb)
{
	return mydb->auth;
}


int dbinsertperson(db * mydb, person * toinsert)
{
	mydb->db_table[(int)toinsert->name[0]]=LSTadd(mydb->db_table[(int)toinsert->name[0]],(person *)toinsert);
	if(mydb->db_table[(int)toinsert->name[0]]==NULL)
		return -1;
	return 0;
}

int dbrmperson(db * mydb, person * toremov)
{
	list * aux, *  aux2=NULL;
	for(aux=mydb->db_table[(int)toremov->name[0]]; !LSTapply(aux,(Item(*)(Item, Item)) personcmp, toremov) && aux!=NULL; aux=LSTfollowing(aux))
	{
		aux2=aux;
	}
	if(aux==NULL)
		return -1; /*person not found*/
	aux=LSTremove(aux2,aux, (void (*)(Item))personfree);
	if(aux2==NULL)
		mydb->db_table[(int)toremov->name[0]]=aux;		
	return 0;
}

int dbrmpersonbyname(db * mydb, person * toremov)
{
	list * aux, *  aux2=NULL;
	for(aux=mydb->db_table[(int)toremov->name[0]]; aux!=NULL && !LSTapply(aux,(Item(*)(Item, Item)) personcmpbyname, toremov); aux=LSTfollowing(aux))
	{
		aux2=aux;
	}
	if(aux==NULL)
		return -1; /*person not found*/
	aux=LSTremove(aux2,aux, (void (*)(Item))personfree);
	if(aux2==NULL)
		mydb->db_table[(int)toremov->name[0]]=aux;		
	return 0;
}


person * dbpersonfind(db * mydb, person * tofind)
{
	list * aux;
	for(aux=mydb->db_table[(int)tofind->name[0]]; aux!=NULL && !LSTapply(aux,(Item(*)(Item, Item)) personcmp, tofind); aux=LSTfollowing(aux));
	if(aux==NULL)
		return NULL;
	return LSTgetitem(aux);
}

person * personcreate(unsigned long IP, unsigned short UDPport, unsigned short TCPport, char * name, char * surname)
{
	person * new = (person *)  malloc(sizeof(person));
	new->name= (char *) malloc(sizeof(char)*(strlen(name)+1)); 
	strcpy(new->name,name);
	new->surname= (char *) malloc(sizeof(char)*(strlen(surname)+1)); 
	strcpy(new->surname,surname);
	new->IP=IP;
	new->UDPport=UDPport;
	new->TCPport=TCPport;
	
	return new;
}

person *personupdate(person *p, unsigned long IP, unsigned short UDPport, unsigned short TCPport, char *name, char *surname)
{
	strcpy(p->name, name);
	strcpy(p->surname, surname);
	p->IP=IP;
	p->UDPport=UDPport;
	p->TCPport=TCPport;

	return p;
}

void personfree(person * tofree)
{
	free(tofree->name);
	free(tofree->surname);
	free(tofree);
}

void dbclean(db*mydb)
{
	int i;
	for(i=0;i<255;i++)
		{
			if(mydb->db_table[i]!=NULL)
			{
				LSTdestroy(mydb->db_table[i], (void (*)(Item))personfree);
				mydb->db_table[i]=NULL;
			}
		}
}

void dbfree(db*mydb)
{
	free(mydb->db_table);
	free(mydb);
}

int personcmp(person * one, person * two)
{
	return one->IP==two->IP && one->UDPport==two->UDPport;

}

int personcmpbyname(person * one, person * two)
{
	if(strcmp(one->name,two->name)==0 && strcmp(one->surname,two->surname)==0 )
		return 1;
	else
		return 0;
}

person * dbpersonfindbyname(db * mydb, person * tofind)
{
	list * aux;
	for(aux=mydb->db_table[(int)tofind->name[0]]; aux!=NULL && !LSTapply(aux,(Item(*)(Item, Item)) personcmpbyname, tofind); aux=LSTfollowing(aux))
	;
	if(aux==NULL)
		return NULL;
	return LSTgetitem(aux);
}

unsigned long getpersonIP(person*p)
{
	return p->IP;
}

unsigned short getpersonUDPport(person*p)
{
	return p->UDPport;
}

unsigned short getpersonTCPport(person*p)
{
	return p->TCPport;
}

char * getpersonname(person*p)
{
	return p->name;
}

char * getpersonsurname(person*p)
{
	return p->surname;
}
