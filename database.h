#ifndef _DB_H_
#define _DB_H_

typedef struct db db;

typedef struct person person;


int personcmp(person * one, person * two);
db* dbcreate(int Auth);
int dbinsertperson(db*,person*);
int dbrmperson(db*,person*);
person * personcreate(unsigned long, unsigned short, unsigned short, char*,char*);
void personfree(person*);
int personcmp(person*,person*);







#endif
