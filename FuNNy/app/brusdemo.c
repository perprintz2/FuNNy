/* test-applikation */
#include <stdio.h>
#include <math.h>

extern double   hot_water;
extern double   cold_water;
extern double   temp;
extern double   flow;

double fuzzy_set_temp_hot(double val);
double fuzzy_set_temp_middle(double val);
double fuzzy_set_temp_cold(double val);
double fuzzy_set_flow_to_little(double val);
double fuzzy_set_flow_to_much(double val);

double mstst=0.0;
double mstst2=0.0;

main()
{
	int             i;
	float           flow_hot =  0.0;
	float           flow_cold = 0.0;
	float           temp_hot = 55.0;
	float           temp_cold = 8.0;
FILE *ftmp, *fvflow,*fkflow,*mstct,*mstmt,*mstht,*mstlf,*mstmf;
	temp = 0.0;
	flow = 0.0;
ftmp=fopen("temp.dat","w");
fvflow=fopen("vflow.dat","w");
fkflow=fopen("kflow.dat","w");
mstct=fopen("msct.dat","w");
mstmt=fopen("msmt.dat","w");
mstht=fopen("msht.dat","w");
mstlf=fopen("mslf.dat","w");
mstmf=fopen("msmf.dat","w");

	for (i = 0; i < 60; i++) {

		fprintf(mstct,"%f\n", fuzzy_set_temp_cold(mstst));
		fprintf(mstmt,"%f\n", fuzzy_set_temp_middle(mstst));
		fprintf(mstht,"%f\n", fuzzy_set_temp_hot(mstst));
		fprintf(mstlf,"%f\n", fuzzy_set_flow_to_little(mstst2));
		fprintf(mstmf,"%f\n", fuzzy_set_flow_to_much(mstst2));

 mstst+=1.0;
 mstst2+=1.0/60.0;

		if ((flow_hot + flow_cold) != 0.0)
			temp = (flow_hot * temp_hot + flow_cold * temp_cold)
				/ (flow_hot + flow_cold);
		else temp = 0.0;

		flow = flow_hot + flow_cold;

		fuzzy_inferenc();

		flow_hot +=  hot_water;
		flow_cold += cold_water;

		if (flow_hot  < 0.0) flow_hot =  0.0;
		if (flow_cold < 0.0) flow_cold = 0.0;
		if (flow_hot  > 2.0) flow_hot =  2.0;
		if (flow_cold > 2.0) flow_cold = 2.0;

		printf("Vand temperatur: %4.2f [grad C]\n", temp);
		printf("Koldt vandsflow: %4.2f [l/s]\n", flow_cold);
		printf("Varmt vandsflow: %4.2f [l/s]\n\n", flow_hot);
	        fprintf(ftmp,"%4.2f\n", temp);
		fprintf(fkflow,"%4.2f\n", flow_cold);
		fprintf(fvflow,"%4.2f\n", flow_hot);
	}
fclose(ftmp);
fclose(fvflow);
fclose(fkflow);
fclose(mstct);
fclose(mstmt);
fclose(mstht);
fclose(mstlf);
fclose(mstmf);


}
