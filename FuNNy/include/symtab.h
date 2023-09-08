#define HASHSIZE 20
#define TRA 0
#define TRI 1
#define SIG 2
#define BEL 3
#define OUT 4
#define INP 6
#define LINVAR 5


struct outlist
{
	struct outlist *next;
	char           *name;
};

struct linnote
{
	struct linnote *nextlin;
	char           *name;
	int             type;
	int             no_of_parm;
	struct outlist *nextout;
	double          *parm;
        char            *adapt;
};

struct symnote
{
	struct symnote *next;
	char           *name;
	int             type;
	int		no_acc;
 	struct linnote *nextlin;
	double          value;
	int		channel;
	double          min;
	double          max;
};


void   printerror(int n);
void   yyerror(const char *msg);
struct symnote *lookup_sym();
struct symnote *insert_sym();
struct linnote *lookup_lin();
struct linnote *insert_lin();
void   init_sym( void );
