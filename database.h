#include "list.h"

#ifndef _DB_H_
#define _DB_H_

typedef struct db {

	list **	db_table;
	int auth; /*has value 1 if it is the authorized SNP, 0 otherwise*/

} db;



typedef struct person person;


int personcmp(person * one, person * two);
db* dbcreate(int Auth);
void Iamtheauth(db*mydb);
void Iamnottheauth(db*mydb);
int dbinsertperson(db*,person*);
int dbrmperson(db*,person*);
person * personcreate(unsigned long, unsigned short, unsigned short, char*,char*);
void personfree(person*);
int personcmp(person*,person*);
unsigned long getpersonIP(person*p);
unsigned short getpersonUDPport(person*p);
unsigned short getpersonTCPport(person*p);
char * getpersonname(person*p);
char * getpersonsurname(person*p);







#endif
