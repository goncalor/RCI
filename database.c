#include "database.h"
#include "list.h"
#include <string.h>
#include <stdlib.h>

#define BUF_LEN 1024




struct person {

	unsigned long IP;
	unsigned short DNSport;
	unsigned short TCPport;
	char * name;
	char * surname;
};


db * dbcreate(int Auth)
{
	db * new=(db*)calloc(1,sizeof(db));
	new->db_table=(list **)calloc(255,sizeof(void*));/*no need to initialize the List because we use calloc*/
	new->auth=Auth;

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
		mydb->db_table[(int)toremov->name[0]]=NULL;		
	return 0;
}
person * personcreate(unsigned long IP, unsigned short DNSport, unsigned short TCPport, char * name, char * surname)
{
	person * new = (person *)  malloc(sizeof(person));
	new->name= (char *) malloc(sizeof(char)*(strlen(name)+1)); 
	new->surname= (char *) malloc(sizeof(char)*(strlen(surname)+1)); 
	new->IP=IP;
	new->DNSport=DNSport;
	new->TCPport=TCPport;
	
	return new;
}

void personfree(person * tofree)
{
	free(tofree->name);
	free(tofree->surname);
	free(tofree);
}

int personcmp(person * one, person * two)
{
	return one->IP==two->IP && one->DNSport==two->DNSport;

}

unsigned long getpersonIP(person*p)
{
	return p->IP;
}

unsigned short getpersonUDPport(person*p)
{
	return p->DNSport;
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
