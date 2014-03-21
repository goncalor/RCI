#include "database.h"

#ifndef _DATABASE_H
#define _DATABASE_H

int join(person * me, unsigned long saIP, unsigned short saport, db * mydb);
int find(unsigned long saIP, unsigned short saport, char *name, char *surname, person **found, person *me, db *mydb);
int leave(person * me, unsigned long saIP, unsigned short saport, db*mydb);
int message(int fd, char *message, person *me);

#endif
