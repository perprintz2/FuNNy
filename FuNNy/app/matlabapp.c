/* matlab-applikation */
#include <stdio.h>
#include <math.h>
#include "matx.h"
FILE *matfil;
extern double y;
double fuzzy_set_x_mf8(double x);
double fuzzy_set_x_mf7(double x);
double fuzzy_set_x_mf6(double x);
double fuzzy_set_x_mf5(double x);
double fuzzy_set_x_mf4(double x);
double fuzzy_set_x_mf3(double x);
double fuzzy_set_x_mf2(double x);
double fuzzy_set_x_mf1(double x);
main(){
	int i;
	double tmp_x= -1.200000;
	matrix *M_x;
	matrix *M_x_mf8;
	matrix *M_x_mf7;
	matrix *M_x_mf6;
	matrix *M_x_mf5;
	matrix *M_x_mf4;
	matrix *M_x_mf3;
	matrix *M_x_mf2;
	matrix *M_x_mf1;
	initmat(&M_x,100,1,0.0);
	initmat(&M_x_mf8,100,1,0.0);
	initmat(&M_x_mf7,100,1,0.0);
	initmat(&M_x_mf6,100,1,0.0);
	initmat(&M_x_mf5,100,1,0.0);
	initmat(&M_x_mf4,100,1,0.0);
	initmat(&M_x_mf3,100,1,0.0);
	initmat(&M_x_mf2,100,1,0.0);
	initmat(&M_x_mf1,100,1,0.0);
	for (i=0; i<100; i++) { 
		tmp_x+= fabs((-1.200000)-(1.200000))/100.0;
		elm(M_x,i,1)= tmp_x;
		elm(M_x_mf8,i,1)=fuzzy_set_x_mf8(elm(M_x,i,1));
		elm(M_x_mf7,i,1)=fuzzy_set_x_mf7(elm(M_x,i,1));
		elm(M_x_mf6,i,1)=fuzzy_set_x_mf6(elm(M_x,i,1));
		elm(M_x_mf5,i,1)=fuzzy_set_x_mf5(elm(M_x,i,1));
		elm(M_x_mf4,i,1)=fuzzy_set_x_mf4(elm(M_x,i,1));
		elm(M_x_mf3,i,1)=fuzzy_set_x_mf3(elm(M_x,i,1));
		elm(M_x_mf2,i,1)=fuzzy_set_x_mf2(elm(M_x,i,1));
		elm(M_x_mf1,i,1)=fuzzy_set_x_mf1(elm(M_x,i,1));
	}
	if ((matfil = fopen("tstapp.mat", "w")) == NULL)
		Error(2);
	savemat(matfil,"M_x",M_x);
	savemat(matfil,"M_x_mf8",M_x_mf8);
	savemat(matfil,"M_x_mf7",M_x_mf7);
	savemat(matfil,"M_x_mf6",M_x_mf6);
	savemat(matfil,"M_x_mf5",M_x_mf5);
	savemat(matfil,"M_x_mf4",M_x_mf4);
	savemat(matfil,"M_x_mf3",M_x_mf3);
	savemat(matfil,"M_x_mf2",M_x_mf2);
	savemat(matfil,"M_x_mf1",M_x_mf1);
	fclose(matfil);

	print_parm();
}
