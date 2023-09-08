#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <time.h>
#include "../include/matx.h"
#include "../include/adapt.h"
#include "../include/symtab.h"
#define dydw(r,c) parm_ptr->dy_dw[parm_size[parm_ptr->type]*r+c]
void get_min_max(double *min_ptr, double *max_ptr);
void fuzzy_inferenc();

int             parm_size[] = {5, 4, 3, 3, 1};
double          delta = 0.0;
struct parm_note *parm_start = (struct parm_note *) 0;
struct parm_note *tmp_parm_start = (struct parm_note *) 0;
double        **outputs;
int             ocnt = 0;

int             ant_out,ant_in;
double          *min, *max;

int nr_of_data= 0;
void
set_ant_out(int nr)
{
	ant_out = nr;
	outputs = (double **) malloc(sizeof(double **) * ant_out);
}


void init_index(int nr, struct index_note **index_array)
{
  int i=0;
  if (nr_of_data >0) free(*index_array);
  nr_of_data=nr;
 
  printf("nr: %d\n",nr);
  *index_array = (struct index_note *) malloc(sizeof(struct index_note) * nr);

  for (i=0;i<nr;i++) (*index_array+i)->init = 0;

}

void rand_index(struct index_note *index_array)
{
  int index,i;
  for (i=0;i<nr_of_data;i++) (index_array+i)->init = 0;
  for (i=0;i<nr_of_data;i++) { 
    //printf("nr: %d",i);

    index= ((int)(nr_of_data*(float)rand()) / (float)RAND_MAX)-1;
    //printf("ind: %d",index);
    while ((index_array+(index%nr_of_data))->init == 1) index++;
    (index_array+(index%nr_of_data))->init = 1;
    (index_array+(index%nr_of_data))->index = i;
  }
}

void print_index(struct index_note *index_array)
{
  int index,i;
  for (i=0;i<nr_of_data;i++) {
    printf("Index: %d er %d reftil: %d\n",i,  (index_array+(i))->init,(index_array+(i))->index);

  }
}

void
set_ant_in(int nr)
{
	int             i;
	ant_in = nr;


	min = (double *) malloc(sizeof(double) * ant_in);
	max = (double *) malloc(sizeof(double) * ant_in);
	get_min_max(min, max);
	delta = 1.0e32;

	for (i = 0; i < ant_in; i++)
		delta = (*(max + i) - *(min + i)) < (delta) ? (*(max + i) - *(min + i)) : (delta);

	delta = (max - min) / 200.0;
}

void
set_out(double *ptr)
{
	outputs[ocnt++] = ptr;
}

void
add_parm(double *dptr, char *adapt, char *name, char *value, int type)
{
	int             i;
	struct parm_note *parm_ptr = (struct parm_note *) 0;

	parm_ptr = (struct parm_note *) malloc(sizeof(struct parm_note));
	parm_ptr->parm = dptr;

	parm_ptr->type = type;
	parm_ptr->name = (char *) malloc(strlen(name) + 1);
	strcpy(parm_ptr->name, name);
	parm_ptr->value = (char *) malloc(strlen(value) + 1);
	strcpy(parm_ptr->value, value);
	parm_ptr->size = parm_size[type];
	parm_ptr->next = parm_start;
	parm_ptr->dy_dw = (double *) malloc(sizeof(double) * parm_size[type] *ant_out);

	parm_ptr->adapt = (char *) malloc(sizeof(char) * parm_size[type]);
	for (i = 0; i < parm_size[type]; i++)
		if ((*(adapt+i) == 'A') || (*(adapt+i) == 'a'))
			*(parm_ptr->adapt+i) = 1;
		else
			*(parm_ptr->adapt+i) = 0;

	parm_start = parm_ptr;
}


void
print_parm(void)
{
	struct parm_note *parm_ptr = (struct parm_note *) 0;
	int             i = 0;
	parm_ptr = parm_start;

	while (parm_ptr) {
		printf("\n%s\t %s: ", parm_ptr->value, parm_ptr->name);
		switch (parm_ptr->type) {
		case TRA:
			printf("trapetzoid(");
			break;
		case TRI:
			printf("triangle(");
			break;
		case SIG:
			printf("sigmoid(");
			break;
		case BEL:
			printf("bell(");
			break;
		case INP:
			printf("input(");
			break;
		case LINVAR:
			printf(" xxx ");
			break;
		case OUT:
			printf("output(");
		default: ;
		}

		for (i = 0; i < (parm_ptr->size - 1); i++){
			printf("%f,", *(parm_ptr->parm + i));
		if (*(parm_ptr->adapt + i) == 1)
				printf("a, ");
		else printf("c, ");}
		printf("%f,", *(parm_ptr->parm + i));
		if (*(parm_ptr->adapt + i) == 1)
				printf("a ");
		else printf("c ");
		printf(");");
		parm_ptr = parm_ptr->next;
	}
	printf("\n");
}

void
print_tmp_parm(void)
{
	struct parm_note *tmp_parm_ptr = (struct parm_note *) 0;
	struct parm_note *parm_ptr = (struct parm_note *) 0;
	int             i = 0;
	tmp_parm_ptr = tmp_parm_start;
	parm_ptr = parm_start;

	while (parm_ptr) {
		printf("\n%s\t %s: ", parm_ptr->value, parm_ptr->name);
		switch (parm_ptr->type) {
		case TRA:
			printf("trapetzoid(");
			break;
		case TRI:
			printf("triangle(");
			break;
		case SIG:
			printf("sigmoid(");
			break;
		case BEL:
			printf("bell(");
			break;
		case INP:
			printf("input(");
			break;
		case LINVAR:
			printf(" xxx ");
			break;
		case OUT:
			printf("output(");
		default: ;
		}

		for (i = 0; i < (parm_ptr->size - 1); i++){
			printf("%f,", *(tmp_parm_ptr->parm + i));
		if (*(parm_ptr->adapt + i) == 1)
				printf("a, ");
		else printf("c, ");}
		printf("%f,", *(tmp_parm_ptr->parm + i));
		if (*(parm_ptr->adapt + i) == 1)
				printf("a ");
		else printf("c ");
		printf(");");

		parm_ptr = parm_ptr->next;
                tmp_parm_ptr = tmp_parm_ptr->next;
	}
	printf("\n");
}

void
print_memfunk(void)
{
	FILE           *memFile;
	struct parm_note *parm_ptr = (struct parm_note *) 0;
	int             i = 0;
	parm_ptr = parm_start;
	memFile = fopen("NewMemFunk.fuz", "w");

	while (parm_ptr) {
		fprintf(memFile, "\n%s\t %s: ", parm_ptr->value, parm_ptr->name);
		switch (parm_ptr->type) {
		case TRA:
			fprintf(memFile, "trapez(");
			break;
		case TRI:
			fprintf(memFile, "triangle(");
			break;
		case SIG:
			fprintf(memFile, "sigmoid(");
			break;
		case BEL:
			fprintf(memFile, "bell(");
			break;
		case INP:
			fprintf(memFile, "input(");
			break;
		case LINVAR:
			fprintf(memFile, " xxx ");
			break;
		case OUT:
			fprintf(memFile, "output(");
		default: ;
		}

		for (i = 0; i < (parm_ptr->size - 1); i++) {
			fprintf(memFile, "%f,", *(parm_ptr->parm + i));
			if (*(parm_ptr->adapt + i) == 1)
				fprintf(memFile, "a, ");
			else fprintf(memFile, "c, ");
		}
		fprintf(memFile, "%f,", *(parm_ptr->parm + i));
		if (*(parm_ptr->adapt + i) == 1)
				fprintf(memFile, "a ");
			else fprintf(memFile, "c ");
		fprintf(memFile, ");");
		parm_ptr = parm_ptr->next;
	}
	fclose(memFile);
}


void
calc_dy_dw(void)
{

	struct parm_note *parm_ptr = (struct parm_note *) 0;
	int             i = 0, k = 0;
	double         *tmp;
	double          tmp_parm;
	tmp = (double *) malloc(sizeof(double) * ant_out);
	parm_ptr = parm_start;

	
/*  	  fuzzy_inferenc(); */
/*  	  for (k = 0; k < ant_out; k++) */
/*  	  tmp[k] = *outputs[k];  */
	



	while (parm_ptr) {
		for (i = 0; i < parm_ptr->size; i++) {
			if (*(parm_ptr->adapt + i)) {
				tmp_parm = *(parm_ptr->parm + i);

				*(parm_ptr->parm + i) -= delta;
				fuzzy_inferenc();
				for (k = 0; k < ant_out; k++)
				tmp[k] = *outputs[k]; 

				*(parm_ptr->parm + i) += delta;
				fuzzy_inferenc();
				for (k = 0; k < ant_out; k++) {
					dydw(k, i) = (*outputs[k] - tmp[k]) / (2*delta);
				}
				*(parm_ptr->parm + i) = tmp_parm;
			}
		}
		parm_ptr = parm_ptr->next;
	}
	free(tmp);
}




void
update_w(double *Dud, double *alpha)
{

	struct parm_note *parm_ptr = (struct parm_note *) 0;
	int             i = 0, k = 0;

	parm_ptr = parm_start;

	while (parm_ptr) {
		for (i = 0; i < parm_ptr->size; i++) {
			if (*(parm_ptr->adapt+i)) {
				for (k = 0; k < ant_out; k++) {
					*(parm_ptr->parm + i) += dydw(k, i) * *(Dud + k) * *alpha;
				}
			}
		}
		parm_ptr = parm_ptr->next;
	}
}



void
rand_parm(double sigma, matrix *xl, matrix *yl, double *x, double *y)
{
	struct parm_note *parm_ptr = (struct parm_note *) 0;	
	struct parm_note *tmp_parm_ptr = (struct parm_note *) 0;	
        struct parm_note *c_parm_ptr = (struct parm_note *) 0;

	int            f=0, i = 0, l, m, n;
	double err=0.0, err_start=0.0;
	parm_ptr = parm_start;
	for (l = 0; l < xl->rows; l++) {
	  for (m = 0; m < xl->cols; m++) {
	    *(x+m)=elm(xl,l,m);
	    //printf("elm %d, %d = %f\n",l,m,*(x+m));
	  }

	  fuzzy_inferenc();
	  
	  for (m = 0; m < ant_out; m++) err+= (elm(yl,l,m)-*outputs[m])*(elm(yl,l,m)-*outputs[m]);	
	}
	err_start=err;
        //printf("start err: %f",err_start);
	// print_parm();
	tmp_parm_start= tmp_parm_ptr = (struct parm_note *) malloc(sizeof(struct parm_note));	
	  do {
	    tmp_parm_ptr->parm = (double *)malloc(sizeof(double)*(parm_size[parm_ptr->type]+1));
	    
	    for (i = 0; i < parm_size[parm_ptr->type]; i++) {
	      *(tmp_parm_ptr->parm+i)= *(parm_ptr->parm+i); }
	    c_parm_ptr= tmp_parm_ptr;  
	    c_parm_ptr->next= tmp_parm_ptr = (struct parm_note *) malloc(sizeof(struct parm_note));
	    parm_ptr = parm_ptr->next;
	  } while (parm_ptr != (struct parm_note *) 0);
	  
	
	  for (n = 0; n < 10; n++) {
            //printf("n= %d\n",n);
            tmp_parm_ptr= tmp_parm_start;
	    parm_ptr = parm_start;

	    do {
	        
	      for (i = 0; i < parm_size[parm_ptr->type]; i++) {
		//*(tmp_parm_ptr->parm+i)= *(parm_ptr->parm+i);
		if (*(parm_ptr->adapt + i) == 1) 
		  *(parm_ptr->parm + i) = *(tmp_parm_ptr->parm + i)+ (double)g_rand((float)0.0,(float)sigma);
	      }
	     
	      tmp_parm_ptr = tmp_parm_ptr->next;

	      parm_ptr = parm_ptr->next;
	    } while (parm_ptr != (struct parm_note *) 0);
	    
	    //print_parm();

	    err=0.0;
	    for (l = 0; l < xl->rows; l++) {
	      for (m = 0; m < xl->cols; m++) {
		*(x+m)=elm(xl,l,m);
		//printf("elm %d, %d = %f\n",l,m,*(x+m));
	      }
	      fuzzy_inferenc();
	      
	      for (m = 0; m < yl->cols; m++) err+= (elm(yl,l,m)-*outputs[m])*(elm(yl,l,m)-*outputs[m]);	
	    }
	        
	    //printf("err=  %f\n",err);
	    if (err < err_start) { 
	      //printf("nye parm: %d\n",n);
	      err_start=err;
	      tmp_parm_ptr= tmp_parm_start;
	      parm_ptr = parm_start;
	      do {
		
		for (i = 0; i < parm_size[parm_ptr->type]; i++) {
		  *(tmp_parm_ptr->parm+i)= *(parm_ptr->parm+i);
		  
		  
		}
		
		parm_ptr = parm_ptr->next;
		tmp_parm_ptr = tmp_parm_ptr->next;
		
	      } while (parm_ptr != (struct parm_note *) 0); 
	    }
	  }
	  
	  tmp_parm_ptr= tmp_parm_start;
	  parm_ptr = parm_start;
	  do {
	    
	    
	    for (i = 0; i < parm_size[parm_ptr->type]; i++) {
	      *(parm_ptr->parm+i)= *(tmp_parm_ptr->parm+i);
	      
	    }
	    
	    c_parm_ptr= tmp_parm_ptr;
	    tmp_parm_ptr = tmp_parm_ptr->next;
	    free(c_parm_ptr->parm);
	    free(c_parm_ptr);
	    parm_ptr = parm_ptr->next;
	    
	  } while (parm_ptr != (struct parm_note *) 0);
}


void
print_dy_dw(void)
{

	struct parm_note *parm_ptr = (struct parm_note *) 0;
	int             i = 0, k = 0;
	double         *tmp;
	//tmp = (double *) malloc(sizeof(double) * ant_out);
	parm_ptr = parm_start;

	while (parm_ptr) {
		printf("size: %d  type: %d\n", parm_ptr->size, parm_ptr->type);
		for (i = 0; i < parm_ptr->size; i++) {
			if (*(parm_ptr->adapt + i)) {
				for (k = 0; k < ant_out; k++) {


					printf("dydw(%d,%d): %e\n", k, i, dydw(k, i));

				}
			}
		}
		parm_ptr = parm_ptr->next;
	}
	printf("delta: %f\n", delta);
}
