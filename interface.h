#include "database.h"

#ifndef _DATABASE_H
#define _DATABASE_H

int join(person * me, unsigned long saIP, unsigned short saport, db * mydb);
int find(unsigned long saIP, unsigned short saport, char *name, char *surname, person *to_find);
int leave(person * me, unsigned long saIP, unsigned short saport, db*mydb);

#endif
