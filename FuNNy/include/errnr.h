static char    *Errtxt[]={
/*  0 */		"Unknown error. KUK I KODEN",     
/*  1 */		"Missing variable in symboltab",
/*  2 */		"Lin-trem for thes lin-var is define twice",
/*  3 */		"Lin-variable define ",
/*  4 */		"Malloc error",
/*  5 */		"No membership functions define",
/*  6 */		"Lin-trem for thes lin-var is not define"
};

#define errortxt(n) ((n < 1 || n > 6) ? Errtxt[0] : Errtxt[n])
