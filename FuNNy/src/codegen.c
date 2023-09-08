#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"
#include "codegen.h"
#include "fuzzy.h"
#include "fuzzy-code"

extern FILE    *yyin, *yyout;
extern FILE    *tstapp, *matlabapp;

void            dump_code(char **array);

int             done_start_init = 0;
extern struct symnote *symbolarray[];

void
startcode(char *fname)
{
	long            time(), tm = time((long *) 0);
	char           *ctime();
	fprintf(yyout,
	"/*\n Generered af fuzzy-system from src-file: %s \n Date: %s*/\n\n",
		fname, ctime(&tm));
	dump_code(init_code);
	done_start_init = 1;
}

void
mk_member_trap(char *fname, char *name,
	       char *lin_term,
	       struct symnote * sptr,
	       struct linnote * lptr)
{
	if (!done_start_init) {
		startcode(fname);
	}
	fprintf(yyout, "/* Trap. Fuzzy set %s-%s */\n", name, lin_term);
	fprintf(yyout, "double parm_%s_%s[]={%e,%e,%e,%e,%e};\n",
		name, lin_term,
		*(lptr->parm),
		*((lptr->parm) + 1),
		*((lptr->parm) + 2),
		*((lptr->parm) + 3),
		*((lptr->parm) + 4));
	fprintf(yyout, "char adapt_%s_%s[]={'%c','%c','%c','%c','%c'};\n",
		name, lin_term,
		*(lptr->adapt),
		*((lptr->adapt) + 1),
		*((lptr->adapt) + 2),
		*((lptr->adapt) + 3),
		*((lptr->adapt) + 4));
	fprintf(yyout, "double fuzzy_set_%s_%s(double val)\n{\n", name, lin_term);
	fprintf(yyout, "\treturn membertrap(parm_%s_%s,val);\n", name, lin_term);
	fprintf(yyout, "}\n\n");
}

void
mk_member_tri(char *fname, char *name,
	      char *lin_term,
	      struct symnote * sptr,
	      struct linnote * lptr)
{
	if (!done_start_init) {
		startcode(fname);
	}
	fprintf(yyout, "/* Tri. Fuzzy set %s-%s */\n", name, lin_term);


	fprintf(yyout, "double parm_%s_%s[]={%e,%e,%e,%e};\n",
		name, lin_term,
		*(lptr->parm),
		*((lptr->parm) + 1),
		*((lptr->parm) + 2),
		*((lptr->parm) + 3));
	fprintf(yyout, "char adapt_%s_%s[]={'%c','%c','%c','%c'};\n",
		name, lin_term,
		*(lptr->adapt),
		*((lptr->adapt) + 1),
		*((lptr->adapt) + 2),
		*((lptr->adapt) + 3));

	fprintf(yyout, "double fuzzy_set_%s_%s(double val)\n{\n", name, lin_term);
	fprintf(yyout, "\treturn membertri(parm_%s_%s,val);\n", name, lin_term);
	fprintf(yyout, "}\n\n");
}

void
mk_member_bell(char *fname, char *name,
	       char *lin_term,
	       struct symnote * sptr,
	       struct linnote * lptr)
{
	if (!done_start_init) {
		startcode(fname);
	}
	fprintf(yyout, "/* Bell Fuzzy set %s-%s */\n", name, lin_term);

	fprintf(yyout, "double parm_%s_%s[]={%e,%e,%e};\n",
		name, lin_term,
		*(lptr->parm),
		*((lptr->parm) + 1),
		*((lptr->parm) + 2));
	fprintf(yyout, "char adapt_%s_%s[]={'%c','%c','%c'};\n",
		name, lin_term,
		*(lptr->adapt),
		*((lptr->adapt) + 1),
		*((lptr->adapt) + 2));

	fprintf(yyout, "double fuzzy_set_%s_%s(double val)\n{\n", name, lin_term);
	fprintf(yyout, "\treturn memberbell(parm_%s_%s,val);\n", name, lin_term);
	fprintf(yyout, "}\n\n");
}

void
mk_member_sigmoid(char *fname, char *name,
		  char *lin_term,
		  struct symnote * sptr,
		  struct linnote * lptr)
{
	if (!done_start_init) {
		startcode(fname);
	}
	fprintf(yyout, "/* Sigmoid Fuzzy set %s-%s */\n", name, lin_term);

	fprintf(yyout, "double parm_%s_%s[]={%e,%e,%e};\n",
		name, lin_term,
		*(lptr->parm),
		*((lptr->parm) + 1),
		*((lptr->parm) + 2));
	fprintf(yyout, "char adapt_%s_%s[]={'%c','%c','%c'};\n",
		name, lin_term,
		*(lptr->adapt),
		*((lptr->adapt) + 1),
		*((lptr->adapt) + 2));

	fprintf(yyout, "double fuzzy_set_%s_%s(double val)\n{\n", name, lin_term);
	fprintf(yyout, "\treturn membersigmoid(parm_%s_%s,val);\n", name, lin_term);
	fprintf(yyout, "}\n\n");
}

void
mk_member_output(char *fname, char *name,
		 char *lin_term,
		 double dd, char ch)
{
	if (!done_start_init) {
		startcode(fname);
	}
	fprintf(yyout, "/* Output Fuzzy set %s-%s */\n", name, lin_term);


	fprintf(yyout, "double parm_%s_%s[]={%e};\n",
		name, lin_term,
		dd);
	fprintf(yyout, "char adapt_%s_%s[]={'%c'};\n",
		name, lin_term,
		ch);

	fprintf(yyout, "double fuzzy_out_%s_%s()\n{\n", name, lin_term);
	fprintf(yyout, "\treturn parm_%s_%s[0];\n", name, lin_term);
	fprintf(yyout, "}\n");
	fprintf(yyout, "double out_parm_%s_%s= 0.0;\n", name, lin_term);
	fprintf(yyout, "double out_%s_%s= 0.0;\n\n", name, lin_term);
}

void
mk_member_input(char *fname, char *name)
{
	if (!done_start_init) {
		startcode(fname);
	}
	fprintf(yyout, "/* Input %s */\n", name);
	fprintf(yyout, "double %s;\n\n", name);
}

void
mk_handling(char *udsagn, char *name,
	    char *lin_term)
{
	if (!done_start_init) {
		printerror(5);
		exit(1);
	}
	fprintf(yyout, "/* Regl for outputvariabel: %s-%s */\n", name, lin_term);
	fprintf(yyout,
	 "aggregering(out_parm_%s_%s,\n\t%s);\n\n", name, lin_term, udsagn);
}

void
mk_init(float min, float max)
{
	int             i, ant_out = 0, ant_in = 0;
	struct linnote *lptr;
	struct symnote *sptr;
	struct outlist *olptr;

	if (!done_start_init) {
		printerror(5);
		exit(1);
	}
	for (i = 0; i < HASHSIZE; i++) {
		sptr = symbolarray[i];
		while (sptr != NULL) {
			lptr = sptr->nextlin;
			if ((lptr != NULL) && (lptr->type == OUT)) {
				fprintf(yyout, "/* outout %s */\n", sptr->name);
				fprintf(yyout, "double %s;\n", sptr->name);
			}
			sptr = sptr->next;
		}
	}
	fprintf(yyout, "/* Initaliseting af fuzzy system */\n");
	fprintf(yyout, "void fuzzy_init()\n{\n");

	for (i = 0; i < HASHSIZE; i++) {
		sptr = symbolarray[i];
		while (sptr != NULL) {
			lptr = sptr->nextlin;
			if ((lptr != NULL) && (lptr->type == OUT))
				ant_out++;
			sptr = sptr->next;
		}
	}
	fprintf(yyout, "set_ant_out(%d);\n", ant_out);



	ant_in= 0;

	for (i = 0; i < HASHSIZE; i++) {
		sptr = symbolarray[i];
		while (sptr != NULL) {
			if ((sptr->type == LINVAR)|| (sptr->type == INP)){
				lptr = sptr->nextlin;
				if ((lptr != NULL) && (lptr->type != OUT))

					ant_in++;
			}
			sptr = sptr->next;
		}
	}

        fprintf(yyout, "set_ant_in(%d);\n", ant_in);







	for (i = 0; i < HASHSIZE; i++) {
		sptr = symbolarray[i];
		while (sptr != NULL) {
			lptr = sptr->nextlin;
			if ((lptr != NULL) && (lptr->type == OUT))
				fprintf(yyout, "set_out(&%s);\n", sptr->name);
			sptr = sptr->next;
		}
	}
	for (i = 0; i < HASHSIZE; i++) {
		sptr = symbolarray[i];
		while (sptr != NULL) {
			lptr = sptr->nextlin;
			while (lptr != NULL) {
				fprintf(yyout, "add_parm(parm_%s_%s,adapt_%s_%s,\"%s\",\"%s\", %d);\n",
					sptr->name, lptr->name,
					sptr->name, lptr->name,
					sptr->name, lptr->name, lptr->type);
				lptr = lptr->nextlin;
			}
			sptr = sptr->next;
		}
	}
	fprintf(yyout, "}\n\n");
}


void
mk_rules()
{
	int             i;
	struct linnote *lptr;
	struct symnote *sptr;
	struct outlist *olptr;

	if (!done_start_init) {
		printerror(5);
		exit(1);
	}
	fprintf(yyout, "\n/* Reglbase */\n");
	fprintf(yyout, "void fuzzy_inferenc()\n{\n\n");

	for (i = 0; i < HASHSIZE; i++) {
		sptr = symbolarray[i];
		while (sptr != NULL) {
			lptr = sptr->nextlin;
			while (lptr != NULL) {
				if ((lptr != NULL) && (lptr->type == OUT))
					fprintf(yyout, "out_parm_%s_%s=0.0;\n", sptr->name, lptr->name);
				lptr = lptr->nextlin;
			}
			sptr = sptr->next;
		}
	}
	fprintf(yyout, "\n");
}

void
mk_slut(float min, float max)
{
	int             i, k,l=0;
	char            ff = 0;
	struct symnote *sptr;
	struct linnote *lptr;
	struct outlist *olptr;
	for (i = 0; i < HASHSIZE; i++) {
		sptr = symbolarray[i];
		while (sptr != NULL) {
			ff = 0;
			lptr = sptr->nextlin;
			if ((lptr != NULL) && (lptr->type == OUT)) {
				fprintf(yyout, "/* outout %s */\n", sptr->name);
				fprintf(yyout, "%s=\n\t((", sptr->name);
				ff = 1;
			}
			while (lptr != NULL) {
				if (lptr->type == OUT) {
					olptr = lptr->nextout;
					k = 0;
					if (olptr != NULL) {
						fprintf(yyout, "%s*fuzzy_out_%s_%s()+\n\t",
							olptr->name, sptr->name, lptr->name);
					}
				}
				lptr = lptr->nextlin;
			}
			if (ff)
				fprintf(yyout, "0.0)/(");
			lptr = sptr->nextlin;
			while (lptr != NULL) {
				if (lptr->type == OUT) {
					olptr = lptr->nextout;
					k = 0;
					if (olptr != NULL) {
						fprintf(yyout, "%s+\n\t",
							olptr->name);
					}
				}
				lptr = lptr->nextlin;
			}
			if (ff)
				fprintf(yyout, "0.1E-30));\n\n");
			sptr = sptr->next;
		}
	}
	fprintf(yyout, "\n}\n");
	fprintf(yyout, "/* Set min og max */\n");

	fprintf(yyout, "void\nget_min_max(double *min_ptr, double *max_ptr)\n{\n");
        
	for (i = 0; i < HASHSIZE; i++) {
		sptr = symbolarray[i];
		while (sptr != NULL) {
                if ((sptr->type == LINVAR)|| (sptr->type == INP)) {
		lptr = sptr->nextlin;
			if ((lptr != NULL) && (lptr->type != OUT)) {

				fprintf(yyout, "\t*(min_ptr + %d)= %f;\n\t*(max_ptr+ %d)= %f;\n", l,sptr->min, l, sptr->max);l++;
			}			}
			sptr = sptr->next;
		}
	}
	fprintf(yyout, "\n}\n\n");


	fprintf(tstapp, "/* test-applikation */\n");
	fprintf(tstapp, "#include <stdio.h>\n");
	fprintf(tstapp, "#include <math.h>\n");

	for (i = 0; i < HASHSIZE; i++) {
		sptr = symbolarray[i];
		while (sptr != NULL) {
			lptr = sptr->nextlin;
			if ((lptr != NULL) && (lptr->type == OUT))
				fprintf(tstapp, "extern double %s;\n", sptr->name);

			sptr = sptr->next;
		}
	}



	for (i = 0; i < HASHSIZE; i++) {
		sptr = symbolarray[i];
		while (sptr != NULL) {
			if ((sptr->type == LINVAR)|| (sptr->type == INP)){
				lptr = sptr->nextlin;
				if ((lptr != NULL) && (lptr->type != OUT))

				fprintf(tstapp, "extern double %s;\n", sptr->name);
			}
			sptr = sptr->next;
		}
	}






	fprintf(tstapp, "main()");
	fprintf(tstapp, "{\n\tint i;\n");
	for (i = 0; i < HASHSIZE; i++) {
		sptr = symbolarray[i];
		while (sptr != NULL) {
			if ((sptr->type == LINVAR)|| (sptr->type == INP)){
				lptr = sptr->nextlin;
				if ((lptr != NULL) && (lptr->type != OUT))

				fprintf(tstapp, "\t%s= %f;\n", sptr->name, (sptr->min < sptr->max ? sptr->min : sptr->max));
}
			sptr = sptr->next;
		}
	}

	fprintf(tstapp, "\tfuzzy_init();\n");
	fprintf(tstapp, "\tfor (i=0; i<100; i++) { \n");






	for (i = 0; i < HASHSIZE; i++) {
		sptr = symbolarray[i];
		while (sptr != NULL) {
			if ((sptr->type == LINVAR)|| (sptr->type == INP)){
				lptr = sptr->nextlin;
				if ((lptr != NULL) && (lptr->type != OUT))

				fprintf(tstapp, "\t\t%s+= fabs((%f)-(%f))/100.0;\n", sptr->name, sptr->min, sptr->max);
			}
			sptr = sptr->next;
		}
	}





	fprintf(tstapp, "\t\tfuzzy_inferenc();\n");
	for (i = 0; i < HASHSIZE; i++) {
		sptr = symbolarray[i];
		while (sptr != NULL) {
			lptr = sptr->nextlin;
			if ((lptr != NULL) && (lptr->type == OUT)) {
				fprintf(tstapp, "\t\tprintf(\"");
				fprintf(tstapp, "%s:", sptr->name);
				fprintf(tstapp, "%s", " %f\\n\"");
				fprintf(tstapp, ", %s);", sptr->name);
				fprintf(tstapp, "\n");
			}
			sptr = sptr->next;
		}
	}
	fprintf(tstapp, "\t}\n\tprint_parm();\n}\n");


	fprintf(matlabapp, "/* matlab-applikation */\n");
	fprintf(matlabapp, "#include <stdio.h>\n");
	fprintf(matlabapp, "#include <math.h>\n");
	fprintf(matlabapp, "#include \"matx.h\"\n");
	fprintf(matlabapp, "FILE *matfil;\n");

	for (i = 0; i < HASHSIZE; i++) {
		sptr = symbolarray[i];
		while (sptr != NULL) {
			lptr = sptr->nextlin;
			if ((lptr != NULL) && (lptr->type == OUT))
				fprintf(matlabapp, "extern double %s;\n", sptr->name);

			sptr = sptr->next;
		}
	}

	for (i = 0; i < HASHSIZE; i++) {
		sptr = symbolarray[i];
		while (sptr != NULL) {
			lptr = sptr->nextlin;
			while ((lptr != NULL) && (lptr->type != OUT)) {
				fprintf(matlabapp, "double fuzzy_set_%s_%s(double x);\n", sptr->name, lptr->name);
				lptr = lptr->nextlin;
			}
			sptr = sptr->next;
		}
	}

	for (i = 0; i < HASHSIZE; i++) {
		sptr = symbolarray[i];
		while (sptr != NULL) {
			lptr = sptr->nextlin;
			if (!((lptr != NULL) && (lptr->type == OUT)) && (sptr->type == LINVAR))
				fprintf(matlabapp, "matrix *M_%s;\n", sptr->name);
			sptr = sptr->next;
		}
	}

	fprintf(matlabapp, "main()");
	fprintf(matlabapp, "{\n\tint i;\n");




	for (i = 0; i < HASHSIZE; i++) {
		sptr = symbolarray[i];
		while (sptr != NULL) {
			if ((sptr->type == LINVAR)|| (sptr->type == INP)){
				lptr = sptr->nextlin;
				if ((lptr != NULL) && (lptr->type != OUT))

				fprintf(matlabapp, "\tdouble tmp_%s= %f;\n", sptr->name, (sptr->min < sptr->max ? sptr->min : sptr->max));
}
			sptr = sptr->next;
		}
	}

	for (i = 0; i < HASHSIZE; i++) {
		sptr = symbolarray[i];
		while (sptr != NULL) {
			if ((sptr->type == LINVAR)|| (sptr->type == INP)){
				lptr = sptr->nextlin;
				if ((lptr != NULL) && (lptr->type != OUT))

				fprintf(matlabapp, "\tmatrix *M_%s;\n", sptr->name);
}
			sptr = sptr->next;
		}
	}

	for (i = 0; i < HASHSIZE; i++) {
		sptr = symbolarray[i];
		while (sptr != NULL) {
			lptr = sptr->nextlin;
			while ((lptr != NULL) && (lptr->type != OUT)) {
				fprintf(matlabapp, "\tmatrix *M_%s_%s;\n", sptr->name, lptr->name);
				lptr = lptr->nextlin;
			}
			sptr = sptr->next;
		}
	}


	for (i = 0; i < HASHSIZE; i++) {
		sptr = symbolarray[i];
		while (sptr != NULL) {
			if ((sptr->type == LINVAR)|| (sptr->type == INP)){
				lptr = sptr->nextlin;
				if ((lptr != NULL) && (lptr->type != OUT))

				fprintf(matlabapp, "\tinitmat(&M_%s,100,1,0.0);\n", sptr->name);
}
			sptr = sptr->next;
		}
	}

	for (i = 0; i < HASHSIZE; i++) {
		sptr = symbolarray[i];
		while (sptr != NULL) {
			lptr = sptr->nextlin;
			while ((lptr != NULL) && (lptr->type != OUT)) {
				fprintf(matlabapp, "\tinitmat(&M_%s_%s,100,1,0.0);\n", sptr->name, lptr->name);
				lptr = lptr->nextlin;
			}
			sptr = sptr->next;
		}
	}

	for (i = 0; i < HASHSIZE; i++) {
		sptr = symbolarray[i];
		while (sptr != NULL) {
			lptr = sptr->nextlin;
			if (!((lptr != NULL) && (lptr->type == OUT)) && (sptr->type == LINVAR))
				fprintf(matlabapp, "\tinitmat(&M_%s,100,1,0.0);\n", sptr->name);
			sptr = sptr->next;
		}
	}

	fprintf(matlabapp, "\tfor (i=0; i<100; i++) { \n");




	for (i = 0; i < HASHSIZE; i++) {
		sptr = symbolarray[i];
		while (sptr != NULL) {
			if ((sptr->type == LINVAR)|| (sptr->type == INP)){
				lptr = sptr->nextlin;
				if ((lptr != NULL) && (lptr->type != OUT)) {

					fprintf(matlabapp, "\t\ttmp_%s+= fabs((%f)-(%f))/100.0;\n", sptr->name, sptr->min, sptr->max);
					fprintf(matlabapp, "\t\telm(M_%s,i,1)= tmp_%s;\n", sptr->name, sptr->name);
				}

			}
			sptr = sptr->next;
		}
	}





	for (i = 0; i < HASHSIZE; i++) {
		sptr = symbolarray[i];
		while (sptr != NULL) {
			lptr = sptr->nextlin;
			if (!((lptr != NULL) && (lptr->type == OUT)) && (sptr->type == LINVAR)) {
				fprintf(matlabapp, "\t\telm(M_%s,i,1)= tmp_%s;\n", sptr->name, sptr->name);
			}
			sptr = sptr->next;
		}
	}

	for (i = 0; i < HASHSIZE; i++) {
		sptr = symbolarray[i];
		while (sptr != NULL) {
			lptr = sptr->nextlin;
			while ((lptr != NULL) && (lptr->type != OUT)) {
				fprintf(matlabapp, "\t\telm(M_%s_%s,i,1)=fuzzy_set_%s_%s(elm(M_%s,i,1));\n", sptr->name, lptr->name, sptr->name, lptr->name, sptr->name);
				lptr = lptr->nextlin;
			}
			sptr = sptr->next;
		}
	}
	fprintf(matlabapp, "\t}\n\tif ((matfil = fopen(\"tstapp.mat\", \"w\")) == NULL)\n\t\tError(2);\n");




	for (i = 0; i < HASHSIZE; i++) {
		sptr = symbolarray[i];
		while (sptr != NULL) {
			if ((sptr->type == LINVAR)|| (sptr->type == INP)){
				lptr = sptr->nextlin;
				if ((lptr != NULL) && (lptr->type != OUT)) {

					fprintf(matlabapp, "\tsavemat(matfil,\"M_%s\",M_%s);\n", sptr->name, sptr->name);
				}

			}
			sptr = sptr->next;
		}
	}








	for (i = 0; i < HASHSIZE; i++) {
		sptr = symbolarray[i];
		while (sptr != NULL) {
			lptr = sptr->nextlin;
			while ((lptr != NULL) && (lptr->type != OUT)) {
				fprintf(matlabapp, "\tsavemat(matfil,\"M_%s_%s\",M_%s_%s);\n", sptr->name, lptr->name, sptr->name, lptr->name);
				lptr = lptr->nextlin;
			}
			sptr = sptr->next;
		}
	}

	for (i = 0; i < HASHSIZE; i++) {
		sptr = symbolarray[i];
		while (sptr != NULL) {
			lptr = sptr->nextlin;
			if (!((lptr != NULL) && (lptr->type == OUT)) && (sptr->type == LINVAR))
				fprintf(matlabapp, "\tsavemat(matfil,\"M_%s\",M_%s);\n", sptr->name, sptr->name);
			sptr = sptr->next;
		}
	}

	fprintf(matlabapp, "\tfclose(matfil);\n");
	fprintf(matlabapp, "\n\tprint_parm();\n}\n");






}

void
dump_code(char **array)
{
	while (*array)
		fprintf(yyout, "%s\n", *array++);
}
