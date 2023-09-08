#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "matx.h"

//#include "mat.h"

#define MATLAB 1000

typedef struct
{
	long type;   /* type */
	long mrows;  /* row dimension */
	long ncols;  /* column dimension */
	long imagf;  /* flag indicating imag part */
	long namlen; /* name length (including NULL) */
} Fmatrix;

string navn1, navn2;

matrix *tmp;

FILE *pmat;

int rand(void);

/************************************************************************/
/*                                                                      */
/* Retunere en TEXT-string der svare til det paag. fejl nr . <n>        */
/*                                                                      */
/* Eks:   printf("%s\n",Errstr(3));                                     */
/*                                                                      */
/************************************************************************/
char *
Errstr(int n)
{
	static char *Err[] = {
		"Ukendt fejl. Der er kuk i koden",
		"Kan ikke allocere plads til matrisen",
		"Matrisen findes ikke i filen",
		"<GETMAT> variabel findes ikke i filen",
		"<GETMAT> kan ikke allokere plads til matrisen",
		"<MATADD> Forskellig antal COLS i matriserne",
		"<MATADD> Forskellig antal ROWS i matriserne",
		"<MATSUB> Forskellig antal COLS i matriserne",
		"<MATSUB> Forskellig antal ROWS i matriserne",
		"<MATTRANS>  Antallet af COLS og ROWS i matriserne passer ikke sammen",
		"<MATMULREAL> Forskellig antal COLS i matriserne",
		"<MATMULREAL> Forskellig antal ROWS i matriserne",
		"<MATMUL>  Antallet af COLS og ROWS i matriserne passer ikke sammen",
		"<MATDIAGO>  Antallet af COLS og ROWS i matriserne passer ikke sammen",
		"<MATBACKSUB>  Antallet af COLS og ROWS i matriserne passer ikke sammen"};
	return ((n < 1 || n > 14) ? Err[0] : Err[n]);
}

/************************************************************************/
/*                                                                      */
/* Udskriver en fejlmeddelse paa <stderr> som svare til fejl nr. <n>    */
/*                                                                      */
/* Eks:  if (!ok) Error(Notok_error_nr);                                */
/*                                                                      */
/************************************************************************/
void Error(int n)
{
	fprintf(stderr, "Error nr.%d : %s\n", n, Errstr(n));
	exit(1);
}

/* char Mopen(char *FileName, char *RW)
{
	printf("Creating file %s...\n\n", FileName);
	pmat = matOpen(FileName, RW);
	if (pmat == NULL)
	{
		printf("Error creating file %s\n", FileName);
		printf("(Do you have write permission in this directory?)\n");
		return 0;
	}
	return 1;
} */

/************************************************************************/
/*                                                                      */
/* Gemmer et array af <double> i en standart <MATLAB>.mat fil.          */
/*                                                                      */
/* Eks:  double data[400];                                              */
/* SaveMat(datfile,"kurt",20,20,data);                                  */
/*                                                                      */
/************************************************************************/
/* void SaveMat(char *pname, int nrows, int ncols, double *preal)
{
	mxArray *pa1;
	int status;
	pa1 = mxCreateDoubleMatrix(nrows, ncols, mxREAL);
	if (pa1 == NULL)
	{
		printf("%s : Out of memory on line %d\n", __FILE__, __LINE__);
		printf("Unable to create mxArray.\n");
	}
	memcpy((void *)(mxGetPr(pa1)), (void *)preal, sizeof(double) * nrows * ncols);

	status = matPutVariable(pmat, "LocalDouble", pa1);
	if (status != 0)
	{
		printf("%s :  Error using matPutVariable on line %d\n", __FILE__, __LINE__);
	}
	mxDestroyArray(pa1);
	if (matClose(pmat) != 0)
	{
		printf("Error closing file\n");
	}
}

void closeMat()
{
	if (matClose(pmat) != 0)
	{
		printf("Error closing file\n");
	}
} */

/************************************************************************/
/*                                                                      */
/* Laeser en <MATLAB>.mat fil.                                          */
/*                                                                      */
/* Eks:  double *data;                                                  */
/* LoadMat(datfile,navn,ant_rows,ant_cols,&data);                       */
/*                                                                      */
/************************************************************************/
char LoadMat(FILE *fp, char *pname, int *mrows, int *ncols, double **preal)
{
	Fmatrix x;
	int mn, namlen;
	if (fread((char *)&x, sizeof(Fmatrix), 1, fp) != 1)
		return (1);
	*mrows = x.mrows;
	*ncols = x.ncols;
	namlen = x.namlen;
	mn = x.mrows * x.ncols;

	if (fread(pname, sizeof(char), namlen, fp) != namlen)
		return (1);
	if ((*preal = (double *)malloc(mn * sizeof(double))) == NULL)
	{
		Error(1);
	}
	if (fread(*preal, sizeof(double), mn, fp) != mn)
	{
		free(*preal);
		return (1);
	}
	return (0);
}

/************************************************************************/
/*                                                                      */
/* Frigiver det allocerede lager.                                       */
/*                                                                      */
/* Eks: matrix *m1;                                                     */
/* killmat(&m1);                                                        */
/*                                                                      */
/************************************************************************/
void killmat(matrix **mat)
{
	free((*mat)->vaerdi);
	free((*mat)->rowoffset);
	free(*mat);
}

/************************************************************************/
/*                                                                      */
/* Udskriver en <matrix> paa skaermenen.                                */
/*                                                                      */
/* Eks: matrix *m1;                                                     */
/* writemat("vigo",m1);                                                 */
/*                                                                      */
/************************************************************************/
void writemat(char *navn, matrix *mat)
{
	int i, j;
	printf("\n\n%s=\n", navn);
	for (i = 0; i < mat->rows; i++)
	{
		for (j = 0; j < mat->cols; j++)
			printf("  %10.3e", elm(mat, i, j));
		printf("\n");
	}
	printf("\n");
}

/************************************************************************/
/*                                                                      */
/* Udskriver en <matrix> til fil csv format.                                */
/*                                                                      */
/* Eks: matrix *m1;                                                     */
/* writemat("vigo",m1);                                                 */
/*                                                                      */
/************************************************************************/
void writematToFile(char *navn, matrix *mat)
{
	char * str;

	str= (char *)malloc(strlen(navn+8));
	str[0]=0;
	strcat(str,navn);
	strcat(str,".csv");
	FILE *f= fopen(str,"w");
	int i, j;
	// printf("\n\n%s=\n", navn);
	// printf("\n\n%s\n", str);
	for (i = 0; i < mat->rows; i++)
	{
		fprintf(f,"%10.3e", elm(mat, i, 0));
		for (j = 1; j < mat->cols; j++)
			fprintf(f,",%10.3e", elm(mat, i, j));
		fprintf(f,"\n");
	}
	fclose(f);
}


/************************************************************************/
/*                                                                      */
/* Udskriver col og row for <matrix> paa skaermenen.                    */
/*                                                                      */
/* Eks: matrix *m1;                                                     */
/* whomat("vigo",m1);                                                   */
/*                                                                      */
/************************************************************************/
void whomat(char *navn, matrix *mat)
{
	printf("%s:", navn);
	printf(" size  %dx%d\n", mat->rows, mat->cols);
}

/************************************************************************/
/*                                                                      */
/* Opretter en <matrix> og initalisere alle elementer til <val>.        */
/*                                                                      */
/* Eks: matrix *mny;                                                    */
/* initmat(&mny,10,10,0.0);                                             */
/*                                                                      */
/************************************************************************/
char initmat(matrix **mat, int row, int col, float val)
{
	int i, j;
	if ((*mat = (matrix *)malloc(sizeof(matrix))) == NULL)
	{
		Error(1);
	}
	if (((*mat)->vaerdi = (float *)malloc(col * row * sizeof(float))) == NULL)
	{
		Error(1);
	}
	for (i = 0; i < col * row; i++)
		*((*mat)->vaerdi + i) = val;
	(*mat)->rows = row;
	(*mat)->cols = col;
	if ((((*mat)->rowoffset) = (int *)malloc(col * sizeof(int))) == NULL)
	{
		Error(1);
	}
	for (i = 0; i < col; i++)
	{
		*((*mat)->rowoffset + i) = i * row;
	}
	return (0);
}

/************************************************************************/
/*                                                                      */
/* Henter <matrix> med navn <navn> fra en <MATLAB>.mat fil.             */
/*                                                                      */
/* Eks: matrix *m1;                                 FILE *fil,                     */
/* getmat(datfil,"vigo",&m1);                                           */
/*                                                                      */
/************************************************************************/
char getmat(FILE *fil, char *navn, matrix **vdi)
{
	double *elmvaerdi;
	int cols, rows, i, notfound;
	notfound = 1;
	i = 0;
	rewind(fil);
	while (notfound == 1)
	{
		if (0 == LoadMat(fil, navn1, &rows, &cols, &elmvaerdi))
		{
			if (strcmp(navn, navn1) == 0)
				notfound = 0;
			if (notfound == 1)
				free(elmvaerdi);
			i++;
		}
		else
		{
			Error(3);
		}
	}
	if (notfound == 0)
	{
		if ((*vdi = (matrix *)malloc(sizeof(matrix))) == NULL)
		{
			Error(4);
		}
		if (((*vdi)->vaerdi = (float *)malloc(sizeof(float) * rows * cols)) == NULL)
		{
			Error(4);
		}
		for (i = 0; i < rows * cols; i++)
		{
			*((*vdi)->vaerdi + i) = (float)*(elmvaerdi + i);
		}
		free(elmvaerdi);
		(*vdi)->rows = rows;
		(*vdi)->cols = cols;
		if ((((*vdi)->rowoffset) = (int *)malloc(cols * sizeof(int))) == NULL)
		{
			Error(4);
		}
		for (i = 0; i < cols; i++)
		{
			*((*vdi)->rowoffset + i) = i * rows;
		}
	}
	return (0);
}

/************************************************************************/
/*                                                                      */
/* Gemmet <matrix> paa Matlab.mat fil.                                  */
/*                                                                      */
/* Eks: matrix *m1;                                                     */
/* savemat(datafil,"vigo",m1);                                          */
/*                                                                      */
/************************************************************************/
void savemat(char *navn, matrix *mat)
{
	int i;
	double *elmvaerdi;
	if ((elmvaerdi = (double *)malloc(sizeof(double) * mat->rows * mat->cols)) == NULL)
	{
		Error(4);
	}
	for (i = 0; i < mat->rows * mat->cols; i++)
	{
		*(elmvaerdi + i) = (double)*((mat)->vaerdi + i);
	}
	//SaveMat(navn, mat->rows, mat->cols, elmvaerdi);
	free(elmvaerdi);
}

/************************************************************************/
/*                                                                      */
/* Addere matriserne <a> og <b> og plasere resultatet i <c>.            */
/*                                                                      */
/* Eks: matrix *m1,*m2,*sum;                                            */
/* matadd(m1,m2,&sum);                                                  */
/*                                                                      */
/************************************************************************/
void matadd(matrix *a, matrix *b, matrix **c)
{
	int i, j;
	if ((a->rows == b->rows) & (a->rows == (*c)->rows))
	{
		if ((a->cols == b->cols) & (a->cols == (*c)->cols))
		{

			for (i = 0; i < a->rows; i++)
				for (j = 0; j < a->cols; j++)
					elm(*c, i, j) = elm(a, i, j) + elm(b, i, j);
		}
		else
			Error(5);
	}
	else
		Error(6);
}

/************************************************************************/
/*                                                                      */
/* Nulstiller matriserne <a>.                                           */
/*                                                                      */
/* Eks: matrix *m1;                                                     */
/* matzero(&m1);                                                        */
/*                                                                      */
/************************************************************************/
void matzero(matrix **a)
{
	int i, j;

	for (i = 0; i < (*a)->rows; i++)
		for (j = 0; j < (*a)->cols; j++)
			elm(*a, i, j) = 0.0;
}

int matlength(matrix *a)
{
	return (a->rows * a->cols);
}

/************************************************************************/
/*                                                                      */
/* Tilskriver en matris <b> til <a>.                                    */
/*                                                                      */
/* Eks: matrix *m1,*lig_m1;                                             */
/* matassign(m1,&lig_m1);                                               */
/*                                                                      */
/************************************************************************/
void matassign(matrix *a, matrix **b)
{
	int i, j;
	if (a->rows == (*b)->rows)
	{
		if (a->cols == (*b)->cols)
		{

			for (i = 0; i < a->rows; i++)
				for (j = 0; j < a->cols; j++)
					elm(*b, i, j) = elm(a, i, j);
		}
		else
			Error(5);
	}
	else
		Error(6);
}

/************************************************************************/
/*                                                                      */
/* Subtra matriserne <a> og <b> og plasere resultatet i <c>.            */
/*                                                                      */
/* Eks: matrix *m1,*m2,*dif;                                            */
/* matsub(m1,m2,&dif);                                                  */
/*                                                                      */
/************************************************************************/
void matsub(matrix *a, matrix *b, matrix **c)
{
	int i, j;
	if ((a->rows == b->rows) & (a->rows == (*c)->rows))
	{
		if ((a->cols == b->cols) & (a->cols == (*c)->cols))
		{

			for (i = 0; i < a->rows; i++)
				for (j = 0; j < a->cols; j++)
					elm(*c, i, j) = elm(a, i, j) - elm(b, i, j);
		}
		else
			Error(7);
	}
	else
		Error(8);
}

/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
void mattrans(matrix *a, matrix **b)
{
	int i, j;
	if (a->rows == (*b)->cols)
	{
		if (a->cols == (*b)->rows)
		{

			for (i = 0; i < a->rows; i++)
				for (j = 0; j < a->cols; j++)
					elm(*b, j, i) = elm(a, i, j);
		}
		else
			Error(9);
	}
	else
		Error(9);
}

/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
void matmulreal(matrix *a, float val, matrix **b)
{
	int i, j;
	if (a->rows == (*b)->rows)
	{
		if (a->cols == (*b)->cols)
		{

			for (i = 0; i < a->rows; i++)
				for (j = 0; j < a->cols; j++)
					elm(*b, i, j) = elm(a, i, j) * val;
		}
		else
			Error(10);
	}
	else
		Error(11);
}

/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
void matmul(matrix *a, matrix *b, matrix **c)
{
	int i, j, l;
	if (a->rows == (*c)->rows)
	{
		if ((a->cols == b->rows) & (b->cols == (*c)->cols))
		{
			for (i = 0; i < a->rows; i++)
				for (j = 0; j < b->cols; j++)
				{
					elm(*c, i, j) = 0.0;
					for (l = 0; l < a->cols; l++)
						elm(*c, i, j) = elm(*c, i, j) + elm(a, i, l) * elm(b, l, j);
				}
		}
		else
			Error(12);
	}
	else
		Error(12);
}

/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
void matdiago(matrix **a)
{
	int i, j, l, m;
	float c, tp[100];

	if ((*a)->rows <= (*a)->cols)
	{
		for (i = 0; i < (*a)->rows; i++)
		{
			for (j = i + 1; j < (*a)->rows; j++)
			{
				if (elm(*a, i, i) == 0.0)
				{
					for (l = i; l < (*a)->rows + 1; l++)
					{
						tp[l] = elm(*a, i, l);
						elm(*a, i, l) = elm(*a, j, l);
						elm(*a, j, l) = tp[l];
					}
				}
				else
				{
					if (elm(*a, i, i) != 0.0)
						c = -elm(*a, j, i) / elm(*a, i, i);
					else
					{
						c = -elm(*a, j, i) / 0.0e-38;
						printf("Matrix n�r singulaer\n");
					}
					for (l = i; l < (*a)->cols; l++)
						elm(*a, j, l) = elm(*a, j, l) + elm(*a, i, l) * c;
					for (m = 0; m < i; m++)
						elm(*a, j, m) = 0.0;
				}
			}
		}
	}
	else
		Error(13);
}

/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
void matbacksub(matrix *a, matrix **b)
{
	int i, j;

	if ((a->rows) == ((*b)->rows) & (a->cols) == (((*b)->rows) + 1))
	{
		for (i = a->rows - 1; i >= 0; i--)
		{
			elm(*b, i, 1) = elm(a, i, a->cols - 1) / elm(a, i, i);
			for (j = i + 1; j < a->cols - 1; j++)
			{
				elm(*b, i, 1) = elm(*b, i, 1) - elm(*b, j, 1) * elm(a, i, j) / elm(a, i, i);
			}
		}
	}
	else
		Error(14);
}

/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
void matsolve(matrix *x, matrix *y, matrix **c)
{
	int i, j, l;
	initmat(&tmp, x->rows, (x->cols) + 1, 0.0);
	for (i = 0; i < x->rows; i++)
		for (j = 0; j < (x->cols); j++)
			elm(tmp, i, j) = elm(x, i, j);
	for (i = 0; i < x->rows; i++)
		elm(tmp, i, (x->cols) + 1) = elm(y, i, 1);
	matdiago(&tmp);
	/*writemat("tmp", tmp);*/
	matbacksub(tmp, c);
	killmat(&tmp);
}

/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
float l_rand(void)
{
	return ((float)(rand() / 32767.0));
}

/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
float g_rand(float my, float sigma)
{
	float x;
	int k;
	x = 0.0;
	for (k = 1; k <= 12; k++)
		x += l_rand();
	return (my + (x - 6.0) * sigma);
}

/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
char gaussinitmat(matrix **mat, int row, int col, float my, float sigma)
{
	int i, j;

	if ((*mat = (matrix *)malloc(sizeof(matrix))) == NULL)
	{
		Error(1);
	}
	if (((*mat)->vaerdi = (float *)malloc(col * row * sizeof(float))) == NULL)
	{
		Error(1);
	}
	for (i = 0; i < col * row; i++)
		*((*mat)->vaerdi + i) = g_rand(my, sigma);
	(*mat)->rows = row;
	(*mat)->cols = col;
	if ((((*mat)->rowoffset) = (int *)malloc(col * sizeof(int))) == NULL)
	{
		Error(1);
	}
	for (i = 0; i < col; i++)
	{
		*((*mat)->rowoffset + i) = i * row;
	}
	return (0);
}

/************************************************************************/
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*                                                                      */
/*                                                                      */
/************************************************************************/
char randinitmat(matrix **mat, int row, int col, float min, float max)
{
	int i, j;

	if ((*mat = (matrix *)malloc(sizeof(matrix))) == NULL)
	{
		Error(1);
	}
	if (((*mat)->vaerdi = (float *)malloc(col * row * sizeof(float))) == NULL)
	{
		Error(1);
	}
	for (i = 0; i < col * row; i++)
		*((*mat)->vaerdi + i) = (max - min) * l_rand() + min;
	;
	(*mat)->rows = row;
	(*mat)->cols = col;
	if ((((*mat)->rowoffset) = (int *)malloc(col * sizeof(int))) == NULL)
	{
		Error(1);
	}
	for (i = 0; i < col; i++)
	{
		*((*mat)->rowoffset + i) = i * row;
	}
	return (0);
}

/************************************************************************/
/*                                                                      */
/* Normering af en matris <b> til <a>.                                  */
/*                                                                      */
/* Eks: matrix *m1,*lig_m1. ext;                                        */
/* matmormer(m1, &lig_m1, &ext, 0);                                      */
/*                                                                      */
/************************************************************************/
void matnormer(matrix *a, matrix **b, matrix **ext, int ori)
{
	int i, j;
	if (ori == 0)
	{

		if (a->rows == (*b)->rows)
		{
			if (a->cols == (*b)->cols)
			{

				for (j = 0; j < a->rows; j++)
				{
					elm(*ext, j, 0) = elm(a, j, 0);
					elm(*ext, j, 1) = elm(a, j, 0);
				}

				for (i = 0; i < a->rows; i++)
					for (j = 0; j < a->cols; j++)
					{
						if (elm(*ext, i, 0) < elm(a, i, j))
							elm(*ext, i, 0) = elm(a, i, j);
						if (elm(*ext, i, 1) > elm(a, i, j))
							elm(*ext, i, 1) = elm(a, i, j);
					}
				for (i = 0; i < a->rows; i++)
					for (j = 0; j < a->cols; j++)
					{
						elm(*b, i, j) = (elm(a, i, j) - elm(*ext, i, 1)) / (1.25 * (elm(*ext, i, 0) - elm(*ext, i, 1))) + 0.1;
					}
			}
			else
				Error(5);
		}
		else
			Error(6);
	}
	else
	{

		if (a->rows == (*b)->rows)
		{
			if (a->cols == (*b)->cols)
			{

				for (j = 0; j < a->cols; j++)
				{
					elm(*ext, j, 0) = elm(a, 0, j);
					elm(*ext, j, 1) = elm(a, 0, j);
				}

				for (j = 0; j < a->cols; j++)
					for (i = 0; i < a->rows; i++)
					{
						if (elm(*ext, j, 0) < elm(a, i, j))
							elm(*ext, j, 0) = elm(a, i, j);
						if (elm(*ext, j, 1) > elm(a, i, j))
							elm(*ext, j, 1) = elm(a, i, j);
					}
				for (i = 0; i < a->rows; i++)
					for (j = 0; j < a->cols; j++)
					{
						elm(*b, i, j) = (elm(a, i, j) - elm(*ext, j, 1)) / (1.25 * (elm(*ext, j, 0) - elm(*ext, j, 1))) + 0.1;
					}
			}
			else
				Error(5);
		}
		else
			Error(6);
	}
}

/************************************************************************/
/*                                                                      */
/* Normering af en matris <b> til <a> med extrenun.                     */
/*                                                                      */
/* Eks: matrix *m1,*lig_m1. *ext;                                        */
/* matmormer(m1, &lig_m1, ext, 0);                                     */
/*                                                                      */
/************************************************************************/
void matnormerext(matrix *a, matrix **b, matrix *ext, int ori)
{
	int i, j;
	if (ori == 0)
	{

		if (a->rows == (*b)->rows)
		{
			if (a->cols == (*b)->cols)
			{

				for (i = 0; i < a->rows; i++)
					for (j = 0; j < a->cols; j++)
					{
						elm(*b, i, j) = (elm(a, i, j) - elm(ext, i, 1)) / (1.25 * (elm(ext, i, 0) - elm(ext, i, 1))) + 0.1;
					}
			}
			else
				Error(5);
		}
		else
			Error(6);
	}
	else
	{

		if (a->rows == (*b)->rows)
		{
			if (a->cols == (*b)->cols)
			{

				for (i = 0; i < a->rows; i++)
					for (j = 0; j < a->cols; j++)
					{
						elm(*b, i, j) = (elm(a, i, j) - elm(ext, j, 1)) / (1.25 * (elm(ext, j, 0) - elm(ext, j, 1))) + 0.1;
					}
			}
			else
				Error(5);
		}
		else
			Error(6);
	}
}

/************************************************************************/
/*                                                                      */
/* Normering af en matris <b> til <a>.                                  */
/*                                                                      */
/* Eks: matrix *m1,*lig_m1. ext;                                        */
/* matmormp(m1, &lig_m1, &ext, 0);                                      */
/*                                                                      */
/************************************************************************/
void matnormp(matrix *a, matrix **b, matrix **ext, int ori)
{
	int i, j;
	if (ori == 0)
	{

		if (a->rows == (*b)->rows)
		{
			if (a->cols == (*b)->cols)
			{

				for (j = 0; j < a->rows; j++)
				{
					elm(*ext, j, 0) = 0.0;
					elm(*ext, j, 1) = 0.0;
				}

				for (i = 0; i < a->rows; i++)
					for (j = 0; j < a->cols; j++)
					{
						elm(*ext, i, 0) += elm(a, i, j);
					}
				for (i = 0; i < a->rows; i++)
				{
					elm(*ext, i, 0) += elm(*ext, i, 0) / (float)a->cols;
				}

				for (i = 0; i < a->rows; i++)
					for (j = 0; j < a->cols; j++)
					{
						elm(*ext, i, 1) += (elm(a, i, j) - elm(*ext, i, 0)) * (elm(a, i, j) - elm(*ext, i, 1));
					}

				for (i = 0; i < a->rows; i++)
				{
					elm(*ext, i, 1) += elm(*ext, i, 1) / (float)a->cols;
				}

				for (i = 0; i < a->rows; i++)
					for (j = 0; j < a->cols; j++)
					{
						elm(*b, i, j) = (elm(a, i, j) - elm(*ext, i, 0)) / (2 * elm(*ext, i, 1));
					}
			}
			else
				Error(5);
		}
		else
			Error(6);
	}
	else
	{

		if (a->rows == (*b)->rows)
		{
			if (a->cols == (*b)->cols)
			{

				for (j = 0; j < a->cols; j++)
				{
					elm(*ext, j, 0) = 0.0;
					elm(*ext, j, 1) = 0.0;
				}

				for (j = 0; j < a->cols; j++)
					for (i = 0; i < a->rows; i++)
					{
						elm(*ext, j, 0) += elm(a, i, j);
					}
				for (j = 0; j < a->cols; j++)
				{
					elm(*ext, j, 0) = elm(*ext, j, 0) / (float)a->rows;
				}

				for (j = 0; j < a->cols; j++)
					for (i = 0; i < a->rows; i++)
					{
						elm(*ext, j, 1) += (elm(a, i, j) - elm(*ext, j, 0)) * (elm(a, i, j) - elm(*ext, j, 0));
					}

				for (j = 0; j < a->cols; j++)
				{
					elm(*ext, j, 1) = sqrt(elm(*ext, j, 1) / (float)a->rows);
				}

				for (i = 0; i < a->rows; i++)
					for (j = 0; j < a->cols; j++)
					{
						elm(*b, i, j) = (elm(a, i, j) - elm(*ext, j, 0)) / (2 * elm(*ext, j, 1));
					}
			}
			else
				Error(5);
		}
		else
			Error(6);
	}
}

/************************************************************************/
/*                                                                      */
/* Normering af en matris <b> til <a>.                                  */
/*                                                                      */
/* Eks: matrix *m1,*lig_m1. ext;                                        */
/* matmormpext(m1, &lig_m1, &ext, 0);                                      */
/*                                                                      */
/************************************************************************/
void matnormpext(matrix *a, matrix **b, matrix *ext, int ori)
{
	int i, j;
	if (ori == 0)
	{

		if (a->rows == (*b)->rows)
		{
			if (a->cols == (*b)->cols)
			{

				for (i = 0; i < a->rows; i++)
					for (j = 0; j < a->cols; j++)
					{
						elm(*b, i, j) = (elm(a, i, j) - elm(ext, i, 0)) / (2 * elm(ext, i, 1));
					}
			}
			else
				Error(5);
		}
		else
			Error(6);
	}
	else
	{

		if (a->rows == (*b)->rows)
		{
			if (a->cols == (*b)->cols)
			{

				for (i = 0; i < a->rows; i++)
					for (j = 0; j < a->cols; j++)
					{
						elm(*b, i, j) = (elm(a, i, j) - elm(ext, j, 0)) / (2 * elm(ext, j, 1));
					}
			}
			else
				Error(5);
		}
		else
			Error(6);
	}
}

/************************************************************************/
/*                                                                      */
/* Normering af en matris <b> til <a>.                                  */
/*                                                                      */
/* Eks: matrix *m1,*lig_m1. ext;                                        */
/* matrenormpext(m1, &lig_m1, &ext, 0);                                      */
/*                                                                      */
/************************************************************************/
void matrenormp(matrix *a, matrix **b, matrix *ext, int ori)
{
	int i, j;
	if (ori == 0)
	{

		if (a->rows == (*b)->rows)
		{
			if (a->cols == (*b)->cols)
			{

				for (i = 0; i < a->rows; i++)
					for (j = 0; j < a->cols; j++)
					{
						elm(*b, i, j) = (elm(a, i, j) * 2 * elm(ext, i, 2)) + elm(ext, j, 1);
					}
			}
			else
				Error(5);
		}
		else
			Error(6);
	}
	else
	{

		if (a->rows == (*b)->rows)
		{
			if (a->cols == (*b)->cols)
			{

				for (i = 0; i < a->rows; i++)
					for (j = 0; j < a->cols; j++)
					{
						elm(*b, i, j) = (elm(a, i, j) * 2 * elm(ext, j, 2)) + elm(ext, j, 1);
					}
			}
			else
				Error(5);
		}
		else
			Error(6);
	}
}
