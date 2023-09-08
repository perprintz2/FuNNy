void
mk_member_trap(char *fname, char *name,
          char *lin_term,
          struct symnote *sptr,
          struct linnote *lptr);

void
mk_member_tri(char *fname, char *name,
         char *lin_term,
         struct symnote * sptr,
          struct linnote *lptr);
void
mk_member_bell(char *fname, char *name,
          char *lin_term,
          struct symnote * sptr,
           struct linnote *lptr);
void
mk_member_sigmoid(char *fname, char *name,
        char *lin_term,
        struct symnote * sptr,
         struct linnote *lptr);


void
mk_member_output(char *fname, char *name,
		 char *lin_term,
                  double dd, char ch);

void
mk_member_input(char *fname, char *name);

void
mk_handling(char *udsagn, char *name, 
            char *lin_term);

void            mk_rules();
void            mk_slut(float min, float max);
void            mk_init(float min, float max);
void            printsym( void );
void mk_init(float min, float max);
