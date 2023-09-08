%{
#include "fuzzy.tab.h"
#include <string.h>
#include "symtab.h"
#include "errnr.h"
#include <math.h>
extern int lineno;

%}

whitespace   [ \t]+
comment      #[^#\n]*
realval      ([-+]?[0-9]+|[-+]?([0-9]*\.[0-9]+)([eE][-+]?[0-9]+)?)
variabel     [a-zA-Z][a-zA-Z0-9_]*
nl           \n

%%
{whitespace} ;
{comment} ;

trapez       {return TRAPEZOID;}
bell         {return BELL;}
triangle     {return TRIANGEL;}
sigmoid      {return SIGMOID;}
output       {return OUTPUT;}
input        {return INPUT;}
[AaCc]       {yylval.ch=*yytext;
              return ADAPT;}

rules        {return RULES;}

if           {return IF;}
is           {return IS;}
then         {return THEN;}
":"          {return MEANS;}

and          {return AND;}
or           {return OR;}
not          {return NOT;}

very         {return VERY;}
more-or-less {return MORE_OR_LESS;}
extremely    {return EXTREMELY;}
about        {return ABOUT;}
to           {return TO;}

{realval}    {yylval.fval= atof(yytext);
              return REAL;}

{variabel}   {if ((yylval.symptr= lookup_sym(yytext)) == NULL)
                yylval.symptr= insert_sym(yytext);
              return VARIABEL;}

{nl}         {lineno++;}

.            {return *yytext;}
%%

extern struct symnote *symbolarray[];

void
printerror(int n)
{
   yyerror(errortxt(n));
}

void
yyerror(const char *msg)
{
   printf("Line# %d: %s at < %s > \n", lineno, msg, yytext);
}

int
hash(char *s)
{
   int             hv = 0;
   int             i;
   for (i = 0; s[i] != '\0'; i++) {
      int             v = (hv >> 28) ^ (s[i] & 0xf);
      hv = (hv << 4) | v;
   }
   hv = hv & 0x7fffffff;
   return hv % HASHSIZE;
}

struct symnote *
lookup_sym(char *s)
{
   struct symnote *sptr;
   int             index = hash(s);
   sptr = symbolarray[index];

   while ((sptr != NULL)) {
      if (strcmp(sptr->name, s) == 0)
         return sptr;
      sptr = sptr->next;
   }
   return sptr;
}


struct symnote *
insert_sym(char *s)
{

   struct symnote *sptr;
   int             index = hash(s);
   sptr = symbolarray[index];
   symbolarray[index] = (struct symnote *) malloc(sizeof(struct symnote));

   if (sptr == NULL)
      symbolarray[index]->next = NULL;
   else
      symbolarray[index]->next = sptr;
   symbolarray[index]->nextlin = NULL;
   symbolarray[index]->name = strdup(s);
   symbolarray[index]->type = 0;
   symbolarray[index]->max = -1E30;
   symbolarray[index]->min = 1E30;
   return symbolarray[index];
}

struct linnote *
lookup_lin(struct symnote * sptr, char *s)
{
   struct linnote *lptr;
   lptr = sptr->nextlin;

   while ((lptr != NULL)) {
      if (strcmp(lptr->name, s) == 0)
         return lptr;
      lptr = lptr->nextlin;
   }
   return lptr;
}


struct linnote *
insert_lin(struct symnote * sptr, char *s)
{
   struct linnote *lptr;
   lptr = sptr->nextlin;
   sptr->nextlin = (struct linnote *) malloc(sizeof(struct linnote));

   if (lptr == NULL)
      sptr->nextlin->nextlin = NULL;
   else
      sptr->nextlin->nextlin = lptr;
   sptr->nextlin->name = strdup(s);
   sptr->nextlin->nextout = NULL;
   return sptr->nextlin;
}


void
init_sym( void )
{
   int             i;

   for (i = 0; i < HASHSIZE; i++){
      symbolarray[i] = NULL;
   }
}
