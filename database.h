#include "list.h"

#ifndef _DB_H_
#define _DB_H_

typedef struct db {

	list **	db_table;
	int auth; /*has value 1 if it is the authorized SNP, 0 otherwise*/

} db;



typedef struct person person;



/*******************************************************************
Allocs memory to the database; 

Returns the pointer to the database
*******************************************************************/
db* dbcreate();

/*******************************************************************
Sets the database flag to Auth 
*******************************************************************/
void Iamtheauth(db*mydb);

/*******************************************************************
Sets the database flag to not Auth 
*******************************************************************/
void Iamnottheauth(db*mydb);

/*******************************************************************
Returns 1 or 0 if we are the authorized SNP or not
*******************************************************************/
int AmItheauth(db*mydb);

/*******************************************************************
Inserts person in the database
Returns -1 on error, 0 on sucess
*******************************************************************/
int dbinsertperson(db*,person*);

/*******************************************************************
Removes person from the database
Returns -1 on error, 0 on sucess
*******************************************************************/
int dbrmperson(db*,person*);

/*******************************************************************
Creates a person, allocs memory and inicializes it
Returns the pointer to the person
*******************************************************************/
person * personcreate(unsigned long IP, unsigned short DNSport, unsigned short TCPport, char * name, char * surname);

/*******************************************************************
Updates an existing person p
Returns the pointer to the updated person
*******************************************************************/
person *personupdate(person *p, unsigned long IP, unsigned short DNSport, unsigned short TCPport, char *name, char *surname);




/*******************************************************************
Frees the memory of a person
*******************************************************************/
void personfree(person*);

/*******************************************************************
Frees the database and all info on it.
*******************************************************************/
void dbfree(db*mydb);

/*******************************************************************
Returns true if the person has the same IP and same DNSport
Returns false otherwise
*******************************************************************/
int personcmp(person * one, person * two);


/*These are self explanatory*/
unsigned long getpersonIP(person*p);
unsigned short getpersonUDPport(person*p);
unsigned short getpersonTCPport(person*p);
char * getpersonname(person*p);
char * getpersonsurname(person*p);

#endif
