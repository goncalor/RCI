
#define BUF_LEN 1024


struct db {

	void **	db_table;
	int auth; /*has value 1 if it is the authorized SNP, 0 otherwise*/

};

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
	new->db_table=(void **)calloc(255,sizeof(void*));
	new->auth=Auth;

	return new;
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
	free(tofree->surame);
	free(tofree);
}

int personcmp(person * one, person * two)
{
	return one->IP==two->IP && one->DNSport==two->DNSport;

}
