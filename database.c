#include "database.h"
#define BUF_LEN 1024


struct db {

	void **	db_table;
	unsigned long authIP;
	unsigned short authDNSport
	char authname[BUF_LEN];
	char authsurname[BUF_LEN];

};

struct person {

	unsigned long IP;
        unsigned short DNSport;
	unsigned short TCPport;
        char name[BUF_LEN];
        char surname[BUF_LEN];
};


db * dbcreate(unsigned long authIP, unsigned short authDNSport, )

