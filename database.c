#include "database.h"
#define BUF_LEN 1024


struct db {

	void **	db_table;
	int auth; /*has value 1 if it is the authorized SNP, 0 otherwise*/

};

struct person {

	unsigned long IP;
        unsigned short DNSport;
	unsigned short TCPport;
        char name[BUF_LEN];
        char surname[BUF_LEN];
};


db * dbcreate(int Auth)
{
	db * new=(db*)calloc(1,sizeof(db));
	new->db_table=(void **)calloc(255,sizeof(void*));
	new->auth=Auth;

	return new;
}

int personcmp(person * one, person * two)
{
	return one->IP==two->IP && one->DNSport==two->DNSport;

}
