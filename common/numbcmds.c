#include <string.h>
#include <stdlib.h>

#include "include\d3.h"

/*
dec val
  MUSTPRM 1
  MUSTOPR
inc val
  MUSTPRM 1
  MUSTOPR
mov destval,sourceval
  MUSTPRM 2
  NOMATCH
  1 MUSTOPR
  2 MUSTVAL
opr destval,sourcenum,opchar
  MUSTPRM 3
  NOTYPEMATCH
  1 MUSTOPR
  2 MUSTSCAL
  3 MUSTSCAL
*/

void pascal mathfuncs(char *cmd)
{
  PRAM prs[3];
  char *next;

  next=pramset(cmd,&prs[0]);
  if ((prs->assign & (POIASN|CHARASN|INTASN)) == 0) errlist(MUSTASN,prs[0].pram);

  switch (mandind)
  {
    case DECI:
      switch (ASN(0))
      {
        case CHARASN: (*((char *) ADDR(0)))--; break;
        case INTASN: (*((int *) ADDR(0)))--;   break;
        default: PADDR(0)--; LENPV(0)++;       break; /* POIASN: */
      }
      break;
    case INCI:
      switch (ASN(0))
      {
        case CHARASN: (*((char *) ADDR(0)))++; break;
        case INTASN: (*((int *) ADDR(0)))++;   break;
        default: PADDR(0)++; LENPV(0)--;       break; /* POIASN: */
      }
      break;
    case MOVI:
      if (*next==0) errlist(MUSTPRM,2);
      getval(next,&prs[1]);
      switch (ASN(0))
      {
				case CHARASN:
					if (EVAL(0)!=EVAL(1)) errlist(NOMATCH); 
					*((char *) ADDR(0))=(char) VALUE(1);
					break;
				case INTASN:
					if (EVAL(1)==POINTER) 
					{
						*((int *) ADDR(0))=(int) ADDR(1);
						if (LENPV(0)!=NULL) LENPV(0)=LENVA(1);
					}
					else *((int *) ADDR(0))=VALUE(1);
					break;
				default: /* POIASN, POIASN|STRASN */
					if (EVAL(1)==SCALAR) PADDR(0)=(char *) VALUE(1);
          else PADDR(0)=ADDR(1); 
					LENPV(0)=LENVA(1);
          break;
      }
      break;
    case OPRI:
      next=getscal(next,&prs[1],2,LOWVAL,HIGHVAL);
      if (*next==0) errlist(MUSTPRM,3);
      getscal(next,&prs[2],3,0,0xff);
      switch (ASN(0))
      {
        case CHARASN:
          switch (VALUE(2))
          {
            case '+': *((char *) ADDR(0))+=(char) VALUE(1);  break;
            case '-': *((char *) ADDR(0))-=(char) VALUE(1);  break;
            case '*': *((char *) ADDR(0))*=(char) VALUE(1);  break;
            case '/': *((char *) ADDR(0))/=(char) VALUE(1);  break;
            case '|': *((char *) ADDR(0))|=(char) VALUE(1);  break;
            case '&': *((char *) ADDR(0))&=(char) VALUE(1);  break;
            case '^': *((char *) ADDR(0))^=(char) VALUE(1);  break;
            case '%': *((char *) ADDR(0))%=(char) VALUE(1);  break;
            case '>': *((char *) ADDR(0))>>=(char) VALUE(1); break;
            case '<': *((char *) ADDR(0))<<=(char) VALUE(1); break;
            default: errlist(BADOPR,VALUE(2));
          }
          break;
        case INTASN:
          switch (VALUE(2))
          {
            case '+': *((int *) ADDR(0))+= VALUE(1);  break;
            case '-': *((int *) ADDR(0))-= VALUE(1);  break;
            case '*': *((int *) ADDR(0))*= VALUE(1);  break;
            case '/': *((int *) ADDR(0))/= VALUE(1);  break;
            case '|': *((int *) ADDR(0))|= VALUE(1);  break;
            case '&': *((int *) ADDR(0))&= VALUE(1);  break;
            case '^': *((int *) ADDR(0))^= VALUE(1);  break;
            case '%': *((int *) ADDR(0))%= VALUE(1);  break;
            case '>': *((int *) ADDR(0))>>= VALUE(1); break;
            case '<': *((int *) ADDR(0))<<= VALUE(1); break;
            default: errlist(BADOPR,VALUE(2));
          }
          break;
        default: /* POIASN: */
          switch (VALUE(2))
          {
            case '+': PADDR(0)+=VALUE(1); LENPV(0)-=VALUE(1); break;
            case '-': PADDR(0)-=VALUE(1); LENPV(0)+=VALUE(1); break;
            default: errlist(BADOPRP,VALUE(2));
          }
          break;
      }
      break;
  }

}

void pascal pushpop(char *cmd)
{
	PRAM prs;
	static PARSTACK *pstack=NULL;
	PARSTACK *ptr;
	char *next;
	
	if (*cmd==0) errlist(LEASTPRM,1);
	
	for (next=cmd; *next!=0;)
	{
		next=pramset(next,&prs);
		if (mandind==PUSHI)
		{
			if ((ptr=malloc(sizeof(PARSTACK)))==NULL) errlist(NOINTMEM);
			if (prs.evaltype==SCALAR)
			{
				ptr->value=prs.value;
				ptr->addr=NULL;
				ptr->length=0xFFFF;
			}
			else
			{
				ptr->value=0xFFFF;
				ptr->addr=prs.addr;
				ptr->length=prs.length;
			}
			ptr->next=pstack;
			pstack=ptr;
		}
		else
		{
			if (pstack==NULL) errlist(NOSTACK);
		  if ((prs.assign & (POIASN|CHARASN|INTASN))==0) errlist(MUSTASN,prs.pram);
      switch (prs.assign)
      {
				case CHARASN:
					if (pstack->length!=0xFFFF) errlist(NOMATCH); 
					*((char *) prs.addr)=(char) pstack->value;
					break;
				case INTASN:
					if (pstack->length!=0xFFFF) errlist(NOMATCH);
					*((int *) prs.addr)=pstack->value;
					break;
				default: /* POIASN, POIASN|STRASN */
					if (pstack->length==0xFFFF) errlist(NOMATCH);
					(*(prs.paddr))=pstack->addr;
					(*(prs.lenptr))=pstack->length;
          break;
      }
			ptr=pstack->next;
			free(pstack);
			pstack=ptr;
		}
	}

}

