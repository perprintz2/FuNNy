%{
#include <stdio.h>
#include <string.h>
#include "symtab.h"
#include "codegen.h"
#include <stdlib.h>
#include <math.h>


#define mx(A,B) (A) > (B) ? (A) : (B)
#define mi(A,B) (A) > (B) ? (B) : (A)

struct simpelterm {
   char           *linterm;
   int             not;
   int             hedge;
   int             nr_about;
   double          a_val[2];
};

extern FILE    *yyin, *yyout;
char           *indfile;

struct symnote *sptr;
struct linnote *lptr;
struct outlist *olptr;
char            string[40];


float           maxv = -1E30, minv = 1E30;
int yylex();
%}
%union {
   struct symnote *symptr;
   struct simpelterm *sterm;
   double fval;
   char *string;
   char ch;
}

%token <symptr> VARIABEL 
%token <fval> REAL
%token <ch> ADAPT
%token IF IS THEN AND OR NOT MEANS
%token TRAPEZOID BELL TRIANGEL SIGMOID OUTPUT INPUT
%token VERY MORE_OR_LESS EXTREMELY 
%token ABOUT TO RULES

%type <string> regellist regler udsagn simpeludsagn 
%type <sterm> saetning 
%type <sterm> hedge_primary primary hedge fuzzy_number

%left OR
%left AND
%right NOT

%start program

%%

program:     erklaering start_rules regellist { mk_slut(minv,maxv);};

erklaering:  membervar | erklaering membervar ;

start_rules: RULES {mk_init(minv,maxv);mk_rules();};

regellist:   regler | regellist regler;

membervar:   VARIABEL VARIABEL MEANS TRAPEZOID 
      '(' REAL ',' ADAPT ',' REAL ',' ADAPT ',' REAL ',' ADAPT ',' REAL ',' ADAPT ',' REAL ',' ADAPT ')' ';'
         {   
            if ((sptr=lookup_sym($2->name)) == NULL) {
               printerror(1);exit(1);
            } 
            sptr->type=LINVAR;
            if ((lptr=lookup_lin(sptr,$1->name)) != NULL) {
               printerror(2);exit(1);
            } else lptr= insert_lin(sptr,$1->name);
            if ((lptr->parm= (double *)malloc(sizeof(double)*5)) == NULL) {
               printerror(4); exit(1);
            } 
            if ((lptr->adapt= (char *)malloc(sizeof(char)*5)) == NULL) {
               printerror(4); exit(1);
            }
            sptr->max=mx($6,sptr->max);sptr->min=mi($6,sptr->min);
            sptr->max=mx($10,sptr->max);sptr->min=mi($10,sptr->min);
            sptr->max=mx($14,sptr->max);sptr->min=mi($14,sptr->min);
            sptr->max=mx($18,sptr->max);sptr->min=mi($18,sptr->min);
            *(lptr->parm)=$6;
            *((lptr->parm)+1)=$10;
            *((lptr->parm)+2)=$14;
            *((lptr->parm)+3)=$18;
            *((lptr->parm)+4)=$22;
            *(lptr->adapt)=$8;
            *((lptr->adapt)+1)=$12;
            *((lptr->adapt)+2)=$16;
            *((lptr->adapt)+3)=$20;
            *((lptr->adapt)+4)=$24;
            lptr->type=TRA;
            mk_member_trap(indfile,$2->name,$1->name,sptr,lptr);
         }

   | VARIABEL VARIABEL MEANS BELL '(' REAL ',' ADAPT ',' REAL ',' ADAPT ',' REAL ',' ADAPT ')' ';'
         {   
            if ((sptr=lookup_sym($2->name)) == NULL) {
               printerror(1);exit(1);
            }
            if ((lptr=lookup_lin(sptr,$1->name)) != NULL) {
            sptr->type=LINVAR;
               printerror(2);exit(1);
            } else lptr= insert_lin(sptr,$1->name);
            if ((lptr->parm= (double *)malloc(sizeof(double)*3)) == NULL) {
               printerror(4); exit(1);
            } 
            if ((lptr->adapt= (char *)malloc(sizeof(char)*3)) == NULL) {
               printerror(4); exit(1);
            }
            sptr->max=mx($6+$10,sptr->max);
	    sptr->min=mi($6-$10,sptr->min);
            sptr->max=mx($6-$10,sptr->max);
	    sptr->min=mi($6+$10,sptr->min);
            *(lptr->parm)=$6;
            *((lptr->parm)+1)=$10;
	    *((lptr->parm)+2)=$14;
	    *(lptr->adapt)=$8;
            *((lptr->adapt)+1)=$12;
	    *((lptr->adapt)+2)=$16;
            lptr->type=BEL;
            mk_member_bell(indfile,$2->name,$1->name,sptr,lptr);
         }

   | VARIABEL VARIABEL MEANS TRIANGEL '(' REAL ',' ADAPT ',' REAL ',' ADAPT ',' REAL ',' ADAPT ',' REAL ',' ADAPT ')' ';'
         {   
            if ((sptr=lookup_sym($2->name)) == NULL) {
               printerror(1);exit(1);
            }
            sptr->type=LINVAR;
            if ((lptr=lookup_lin(sptr,$1->name)) != NULL) {
               printerror(2);exit(1);
            } else lptr= insert_lin(sptr,$1->name);
            if ((lptr->parm= (double *)malloc(sizeof(double)*4)) == NULL) {
               printerror(4); exit(1);
            } 
            if ((lptr->adapt= (char *)malloc(sizeof(char)*4)) == NULL) {
               printerror(4); exit(1);
            }

            sptr->max=mx($6,sptr->max);sptr->min=mi($6,sptr->min);
            sptr->max=mx($10,sptr->max);sptr->min=mi($10,sptr->min);
            sptr->max=mx($14,sptr->max);sptr->min=mi($14,sptr->min);
            *(lptr->parm)=$6;
            *((lptr->parm)+1)=$10;
            *((lptr->parm)+2)=$14;
	    *((lptr->parm)+3)=$18;
	    *(lptr->adapt)=$8;
            *((lptr->adapt)+1)=$12;
	    *((lptr->adapt)+2)=$16;
	    *((lptr->adapt)+3)=$20;
            lptr->type=TRI;
            mk_member_tri(indfile,$2->name,$1->name,sptr,lptr);
         }

   | VARIABEL VARIABEL MEANS SIGMOID '(' REAL ',' ADAPT ',' REAL ',' ADAPT ',' REAL ',' ADAPT ')' ';'
         {   
            if ((sptr=lookup_sym($2->name)) == NULL) {
               printerror(1);exit(1);
            }
            sptr->type=LINVAR;
            if ((lptr=lookup_lin(sptr,$1->name)) != NULL) {
                    printerror(2);exit(1);
            } else lptr= insert_lin(sptr,$1->name);
            if ((lptr->parm= (double *)malloc(sizeof(double)*3)) == NULL) {
               printerror(4); exit(1);
            }   
	    if ((lptr->adapt= (char *)malloc(sizeof(char)*3)) == NULL) {
               printerror(4); exit(1);
            }
            sptr->max=mx($6,sptr->max);sptr->min=mi($6,sptr->min);
            sptr->max=mx($10,sptr->max);sptr->min=mi($10,sptr->min);
            *(lptr->parm)=$6;
            *((lptr->parm)+1)=$10;
	    *((lptr->parm)+2)=$14;
	    *(lptr->adapt)=$8;
            *((lptr->adapt)+1)=$12;
	    *((lptr->adapt)+2)=$16;
            lptr->type=SIG;
            mk_member_sigmoid(indfile,$2->name,$1->name,sptr,lptr);
         }   
   
   | VARIABEL VARIABEL MEANS OUTPUT '(' REAL ',' ADAPT ')' ';'
         {   
            if ((sptr=lookup_sym($2->name)) == NULL) {
               printerror(1);exit(1);
            }
            sptr->type=LINVAR;
            if ((lptr=lookup_lin(sptr,$1->name)) != NULL) {
               printerror(2);exit(1);
            } else lptr= insert_lin(sptr,$1->name);
            lptr->type=OUT;
            mk_member_output(indfile,$2->name,$1->name,$6,$8);
         }


| VARIABEL MEANS INPUT ';'
         {   
            if ((sptr=lookup_sym($1->name)) == NULL) {
               printerror(1);exit(1);
            }
            sptr->type=INP;
	    // printf("input\n");
            mk_member_input(indfile,$1->name);
         };

regler: IF udsagn THEN VARIABEL IS VARIABEL 
         {
            mk_handling($2,$4->name,$6->name); 
            if ((lptr=lookup_lin($4,$6->name)) == NULL) {
               printerror(6); exit(1);
            }
            olptr=lptr->nextout;
            if ((lptr->nextout=(struct outlist *)malloc(sizeof(struct outlist))) ==  NULL) {
               printerror(4); exit(1);
            }
            if ((lptr->nextout->name=(char *)malloc(strlen($4->name)+strlen($6->name)+11)) ==NULL) {
               printerror(4); exit(1);
            }
            strcpy(lptr->nextout->name,"out_parm_");
            strcat(lptr->nextout->name,$4->name);
            strcat(lptr->nextout->name,"_");
            strcat(lptr->nextout->name,$6->name);
            lptr->nextout->next=olptr;
            free($2);
         };


udsagn: udsagn AND  udsagn 
         {  if (($$=(char *)malloc(strlen($1)+strlen($3)+ 2)) == NULL) {
              printerror(4); exit(1);
            }
            strcpy($$,$1);
            strcat($$,"*");
            strcat($$,$3);
            free($1);free($3);
         }
   | udsagn OR udsagn 
         {  if (($$=(char *)malloc(strlen($1)+strlen($3)+ 2)) == NULL) {
              printerror(4); exit(1);
            }
            strcpy($$,$1);
            strcat($$,"+");
            strcat($$,$3);
            free($1);free($3);
         }
   | '(' udsagn ')' 
         {  if (($$=(char *)malloc(strlen($2)+ 3)) == NULL) {
              printerror(4); exit(1);
            }
            strcpy($$,"(");
            strcat($$,$2);
            strcat($$,")");
            free($2);
         }
   | simpeludsagn 
         {  if (($$=(char *)malloc(strlen($1)+1)) == NULL) {
              printerror(4); exit(1);
            }
            strcpy($$,$1);
            free($1);
         }; 

simpeludsagn: VARIABEL IS saetning 
         {switch ($3->hedge) {
            case 0: 
               if ((lptr=lookup_lin($1,$3->linterm)) == NULL) {
                  printerror(6); exit(1);
               }
               if (!($3->not)) {
                  if (($$ = (char *) malloc(2*strlen($1->name) + strlen($3->linterm) + 30)) == NULL) {
                     printerror(4); exit(1);
                  }
                  strcpy($$, "fuzzy_set_");
                  strcat($$, $1->name);
                  if ((lptr = lookup_lin($1, $3->linterm)) == NULL) {
                     printerror(6); exit(1);
                  }
                  strcat($$, "_");
                  strcat($$, $3->linterm);
                  strcat($$, "(");
                  strcat($$, $1->name);
                  strcat($$, ")");
//printf("sim: %s", $$);
               } else {
                  if (($$ = (char *) malloc(strlen($1->name) + strlen($3->linterm) + 30)) == NULL) {
                     printerror(4); exit(1);
                  }
                  strcpy($$, "(1.0-fuzzy_set_");
                  strcat($$, $1->name);
                  if ((lptr = lookup_lin($1, $3->linterm)) == NULL) {
                     printerror(6); exit(1);
                  }
                  strcat($$, "_");
                  strcat($$, $3->linterm);
                  strcat($$, "(");
                  strcat($$, $1->name);
                  strcat($$, "))");
               }
               break; 
            case VERY: 
               if ((lptr=lookup_lin($1,$3->linterm)) == NULL) {
                  printerror(6); exit(1);
               }
               if (!($3->not)) {
                  if (($$ = (char *) malloc(2*strlen($1->name) + strlen($3->linterm) + 30)) == NULL) {
                     printerror(4); exit(1);
                  }
                  strcpy($$, "very(fuzzy_set_");
                  strcat($$, $1->name);
                  if ((lptr = lookup_lin($1, $3->linterm)) == NULL) {
                     printerror(6); exit(1);
                  }
                  strcat($$, "_");
                  strcat($$, $3->linterm);
                  strcat($$, "(");
                  strcat($$, $1->name);
                  strcat($$, "))");
               } else {
                  if (($$ = (char *) malloc(strlen($1->name) + strlen($3->linterm) + 30)) == NULL) {
                     printerror(4); exit(1);
                  }
                  strcpy($$, "(1.0-very(fuzzy_set_");
                  strcat($$, $1->name);
                  if ((lptr = lookup_lin($1, $3->linterm)) == NULL) {
                     printerror(6); exit(1);
                  }
                  strcat($$, "_");
                  strcat($$, $3->linterm);
                  strcat($$, "(");
                  strcat($$, $1->name);
                  strcat($$, ")))");
               }
               break;
            case MORE_OR_LESS: 
               if ((lptr=lookup_lin($1,$3->linterm)) == NULL) {
                  printerror(6); exit(1);
               }
               if (!($3->not)) {
                  if (($$ = (char *) malloc(2*strlen($1->name) + strlen($3->linterm) + 30)) == NULL) {
                     printerror(4); exit(1);
                  }
                  strcpy($$, "morl(fuzzy_set_");
                  strcat($$, $1->name);
                  if ((lptr = lookup_lin($1, $3->linterm)) == NULL) {
                     printerror(6); exit(1);
                  }
                  strcat($$, "_");
                  strcat($$, $3->linterm);
                  strcat($$, "(");
                  strcat($$, $1->name);
                  strcat($$, "))");
               } else {
                  if (($$ = (char *) malloc(strlen($1->name) + strlen($3->linterm) + 30)) == NULL) {
                     printerror(4); exit(1);
                  }
                  strcpy($$, "(1.0-morl(fuzzy_set_");
                  strcat($$, $1->name);
                  if ((lptr = lookup_lin($1, $3->linterm)) == NULL) {
                     printerror(6); exit(1);
                  }
                  strcat($$, "_");
                  strcat($$, $3->linterm);
                  strcat($$, "(");
                  strcat($$, $1->name);
                  strcat($$, ")))");
               }
               break;
            case EXTREMELY: 
               if ((lptr=lookup_lin($1,$3->linterm)) == NULL) {
                  printerror(6); exit(1);
               }
               if (!($3->not)) {
                  if (($$ = (char *) malloc(2*strlen($1->name) + strlen($3->linterm) + 30)) == NULL) {
                     printerror(4); exit(1);
                  }
                  strcpy($$, "extremly(fuzzy_set_");
                  strcat($$, $1->name);
                  if ((lptr = lookup_lin($1, $3->linterm)) == NULL) {
                     printerror(6); exit(1);
                  }
                  strcat($$, "_");
                  strcat($$, $3->linterm);
                  strcat($$, "(");
                  strcat($$, $1->name);
                  strcat($$, "))");
               } else {
                  if (($$ = (char *) malloc(strlen($1->name) + strlen($3->linterm) + 30)) == NULL) {
                     printerror(4); exit(1);
                  }
                  strcpy($$, "(1.0-extremly(fuzzy_set_");
                  strcat($$, $1->name);
                  if ((lptr = lookup_lin($1, $3->linterm)) == NULL) {
                     printerror(6); exit(1);
                  }
                  strcat($$, "_");
                  strcat($$, $3->linterm);
                  strcat($$, "(");
                  strcat($$, $1->name);
                  strcat($$, ")))");
               }
               break;
            case ABOUT:
               if (!($3->not)) {
                  if ($3->nr_about == 1) {
                     if (($$ = (char *) malloc(strlen($1->name) + 40)) == NULL) {
                        printerror(4); exit(1);
                     }
                     strcpy($$, "about_set_1(");
                     strcat($$, $1->name);
                     strcat($$, ",");
                     sprintf(string, "%e", $3->a_val[0]);
                     strcat($$, string);
                     strcat($$, ")");
                  }
                  if ($3->nr_about == 2) {
                     if (($$ = (char *) malloc(strlen($1->name) + 50)) == NULL) {
                        printerror(4); exit(1);
                     }
                     strcpy($$, "about_set_2(");
                     strcat($$, $1->name);
                     strcat($$, ",");
                     sprintf(string, "%e", $3->a_val[0]);
                     strcat($$, string);
                     strcat($$, ",");
                     sprintf(string, "%e", $3->a_val[1]);
                     strcat($$, string);
                     strcat($$, ")");
                  }
               } else {
                  if ($3->nr_about == 1) {
                     if (($$ = (char *) malloc(strlen($1->name) + 40)) == NULL) {
                        printerror(4); exit(1);
                     }
                     strcpy($$, "(1.0-about_set_1(");
                     strcat($$, $1->name);
                     strcat($$, ",");
                     sprintf(string, "%e", $3->a_val[0]);
                     strcat($$, string);
                     strcat($$, "))");
                  }
                  if ($3->nr_about == 2) {
                     if (($$ = (char *) malloc(strlen($1->name) + 50)) == NULL) {
                        printerror(4); exit(1);
                     }
                     strcpy($$, "(1.0-about_set_2(");
                     strcat($$, $1->name);
                     strcat($$, ",");
                     sprintf(string, "%e", $3->a_val[0]);
                     strcat($$, string);
                     strcat($$, ",");
                     sprintf(string, "%e", $3->a_val[1]);
                     strcat($$, string);
                     strcat($$, "))");
                  }
               }
               break;
            default:
               printerror(0);
            }
            free($3);
            };

saetning: NOT hedge_primary 
            {   $$= $2; $$->not = 1;
            }
        | hedge_primary 
            {   $$=$1;
            };


hedge_primary: primary {$$=$1;} | fuzzy_number {$$=$1;} | hedge {$$=$1;};


primary: VARIABEL          
            {  if (($$=(struct simpelterm *)malloc(sizeof(struct simpelterm))) == NULL) {
                  printerror(4); exit(1);
               }
//printf("name: %s\n",$1->name);
               $$->linterm=$1->name;
               $$->hedge = 0;
               $$->nr_about= 0;
               $$->not = 0;
               $$->a_val[0]= 0.0;
               $$->a_val[1]= 0.0;
           };

fuzzy_number: ABOUT REAL TO ABOUT REAL 
           {   if (($$=(struct simpelterm *)malloc(sizeof(struct simpelterm))) == NULL) {
                  printerror(4); exit(1);
               }
               $$->linterm=NULL;
               $$->hedge = ABOUT;
               $$->not = 0;
               $$->nr_about= 2;
               $$->a_val[0]= $2;
               $$->a_val[1]= $5;
           }
      | ABOUT REAL       
           {   if (($$=(struct simpelterm *)malloc(sizeof(struct simpelterm))) == NULL) {
                  printerror(4); exit(1);
               }
               $$->linterm=NULL;
               $$->hedge = ABOUT;
               $$->nr_about= 1;
               $$->not = 0;
               $$->a_val[0]= $2;
           };

hedge: VERY VARIABEL         
           {   if (($$=(struct simpelterm *)malloc(sizeof(struct simpelterm))) == NULL) {
                  printerror(4); exit(1);
               }
               $$->linterm=$2->name;
               $$->hedge = VERY;
               $$->not = 0;
               $$->nr_about= 0;
               $$->a_val[0]= 0.0;
               $$->a_val[1]= 0.0;
           }

   | MORE_OR_LESS VARIABEL      
           {   if (($$=(struct simpelterm *)malloc(sizeof(struct simpelterm))) == NULL) {
                  printerror(4); exit(1);
               }
               $$->linterm=$2->name;
               $$->hedge = MORE_OR_LESS;
               $$->not = 0;
               $$->nr_about= 0;
               $$->a_val[0]= 0.0;
               $$->a_val[1]= 0.0;
           }


   | EXTREMELY VARIABEL      
           {   if (($$=(struct simpelterm *)malloc(sizeof(struct simpelterm))) == NULL) {
                  printerror(4); exit(1);
               }
               $$->linterm=$2->name;
               $$->hedge = EXTREMELY;
               $$->nr_about= 0;
               $$->not = 0;
               $$->a_val[0]= 0.0;
               $$->a_val[1]= 0.0;
           };
 
%%

char *progname;
int  lineno = 1;
char *usage= "%s: usage [infile] [outfile] fx: fuzzy prog1.fuz fuzout.c\n";
char *infilerr= "%s: cannot open %s for input\n";
char *outfilerr= "%s: cannot open %s for output\n";
char *tstfilerr= "%s: cannot open %s for output\n";
char *cmakeerr= "%s: cannot open %s for output\n";
FILE *tstapp, *matlabapp, *cmake;

struct symnote *symbolarray[HASHSIZE];

int main(int argc, char **argv)
{
   char  *outfile;
   progname = argv[0];
   if (argc > 3) {
      fprintf(stderr, usage, progname);
      exit(1);
   }
   if (argc > 1) {
      indfile = argv[1];
      yyin = fopen(indfile, "r");
      if (yyin == NULL) {
         fprintf(stderr, infilerr, progname, indfile);
         exit(1);
      }
   }
   if (argc > 2) {
      outfile = argv[2];
      yyout = fopen(outfile, "w");
 
   } else yyout = fopen("fuzout.c", "w");
   if (yyout == NULL) {
      fprintf(stderr, outfilerr, progname, outfile);
      exit(1);
   }
   tstapp = fopen("tstapp.c", "w");
   if (tstapp == NULL) {
      fprintf(stderr, tstfilerr, progname, "tstapp.c");
      exit(1);
   }
  matlabapp = fopen("matlabapp.c", "w");
   if (matlabapp == NULL) {
      fprintf(stderr, tstfilerr, progname, "matlabapp.c");
      exit(1);
   }
   init_sym();
   yyparse();
   /* printsym(); */
   fclose(yyout);
   fclose(tstapp);

   cmake = fopen("CMakeLists.txt", "w");
   if (tstapp == NULL) {
      fprintf(stderr, tstfilerr, progname, "CMakeLists.txt");
      exit(1);
   }
   fprintf(cmake, "cmake_minimum_required(VERSION 2.8)\n");
	fprintf(cmake, "include_directories(/home/ppm/Documents/demos/pyFuNNy/FuNNy/include)\n");
	fprintf(cmake, "project(tstapp)\n");
	fprintf(cmake, "add_executable(tstapp fuzout.c tstapp.c /home/ppm/Documents/demos/pyFuNNy/FuNNy/matlib.c /home/ppm/Documents/demos/pyFuNNy/FuNNy/adapt.c)\n");
	fprintf(cmake, "target_link_libraries(tstapp m)\n");
   fclose(cmake);
   fclose(matlabapp);
}

void
printsym( void )
{
   int             i;
   struct symnote *sptr;
   struct linnote *lptr;
   for (i = 0; i < HASHSIZE; i++) {
      printf("symtab[%d]\t", i);
      sptr = symbolarray[i];
      while (sptr != NULL) {
         printf(" -> %s", sptr->name);
         lptr = sptr->nextlin;
         while (lptr != NULL) {
            printf(" & %s", lptr->name);
            lptr = lptr->nextlin;
         }
         sptr = sptr->next;
      }
      printf(" -> NULL\n");
   }
}
