struct parm_note
{
	struct parm_note *next;
	int             type;
        char            *name;
        char            *value;
	int             size;
	double          *parm;
	char            *adapt;
	double          *dy_dw;
};

struct index_note 
{ char init;
  int index;
};

void set_out(double *ptr);
void add_parm(double *dptr, char *adapt, char *name, char *value, int type);
void print_parm(void);
void print_tmp_parm(void);
void print_memfunk(void);
void calc_dy_dw(void);
void update_w(double *Dud, double *alpha);
void print_dy_dw(void);
void rand_parm(double sigma, matrix *xl, matrix *yl, double *x, double *y);
void init_index(int nr,struct index_note **index_array);
void rand_index(struct index_note *index_array);
void print_index(struct index_note *index_array);
