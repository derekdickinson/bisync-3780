#include <ctype.h>
#include <string.h>

#include "include\d3.h"

/*
cat destptr,sourceptr
  MUSTPRM 2
  1 MUSTBUF 1 pram
  2 MUSTPOINT 2 pram
cpy destptr,sourceptr[,val]
  LEASTPRM 2
  1 MUSTBUF 1 pram
  2 MUSTPOINT 2 pram
  3 MUSTLEN 3 pram
stc deststr,sourcestr,char
  MUSTPRM 3
  1 MUSTPASN
  2 MUSTPOINT
  3 MUSTSCAL
stst deststr,sourcestr,substr
  MUSTPRM 3
  1 MUSTPASN
  2 MUSTPOINT
  3 MUSTPOINT
*/
void pascal strfuncs(char *cmd)
{
  PRAM prs[3];
  char *next;

  switch (mandind)
  {
    case CATI: case CPYI:  next=getbuf(cmd,&prs[0]);    break;
    case STCI: case STSTI: next=getpoiasn(cmd,&prs[0]); break;
  }
  if ( (EVAL(0)==NONE) || (*next==0) )
  {
    switch (mandind)
    {
      case CPYI: errlist(LEASTPRM,2);
      case CATI: errlist(MUSTPRM,2);
      case STCI: case STSTI: errlist(MUSTPRM,3);
    }
  }

  next=getpoint(next,&prs[1]);

  switch (mandind)
  {
    case CATI: 
      getlen(next,&prs[2],&prs[0]);
      strncat(ADDR(0),ADDR(1),LENVA(0)-strlen(ADDR(0))-1 ); 
      break;
    case CPYI:
      getlen(next,&prs[2],&prs[0]);
      memmove(ADDR(0),ADDR(1),VALUE(2));
      break;
    case STCI:
      getscal(next,&prs[2],3,0,0xff);
      PADDR(0)=strchr(ADDR(1),VALUE(2));
      if (PADDR(0)==NULL) PADDR(0)=strchr(ADDR(1),0);
      LENPV(0)=LENVA(1)+PADDR(1)-PADDR(0);
      break;
    case STSTI:
      getpoint(next,&prs[2]);
      PADDR(0)=strstr(ADDR(1),ADDR(2));
      if (PADDR(0)==NULL) PADDR(0)=strchr(ADDR(1),0);
      LENPV(0)=LENVA(1)+PADDR(1)-PADDR(0);
      break;
  }
}

void pascal lwr(char *cmd)
{
  PRAM prs;

  pramset(cmd,&prs);
  switch (prs.assign)
  {
    case STRASN|POIASN:
    case STRASN: strlwr((char *) prs.addr); break;
    case INTASN:
    case CHARASN: *((char *) prs.addr)=tolower((char) prs.value); break;
    default: errlist(MUSTASN,prs.pram);
  }
}

void pascal setbf(char *cmd)
{
  char *next,thech;
  PRAM prs[3];

  if (*cmd==0) errlist(LEASTPRM,1);

  next=getbuf(cmd,&prs[0]);

  if (*next==';')
  {
    next=getscal(next,&prs[2],3,0,0xff);
    thech=(char) VALUE(2);
  } else thech=0;

  getlen(next,&prs[1],&prs[0]);

  memset(ADDR(0),thech,VALUE(1));
}

void pascal slen(char *cmd)
{
  PRAM prs[2];
  char *next;

  next=getscalasn(cmd,&prs[0]);
  getpoint(next,&prs[1]);

  switch (ASN(0))
  {
    case INTASN:  *( (int  *) ADDR(0) )=(int ) strlen(ADDR(1)); break;
    case CHARASN: *( (char *) ADDR(0) )=(char) strlen(ADDR(1)); break;
    default: errlist(MUSTPRM,2); /* NONE */
  }
}

void pascal sptr(char *cmd)
{
  PRAM prs[3];
  char *next;

  next=getscalasn(cmd,&prs[0]);

  next=getpoint(next,&prs[1]);
  getpoint(next,&prs[2]);

  switch (ASN(0))
  {
    case INTASN: *( (int  *) ADDR(0) )=(uint) ADDR(1)-(uint) ADDR(2); break;
    default:     *( (char *) ADDR(0) )=(uint) ADDR(1)-(uint) ADDR(2); break;
  }
}

void pascal srite(char *cmd)
{
  PRAM prs[2];
  int numbytes;
  char buffer[2500];
  char ops[100];
  char *next,*st;

  next=getbuf(cmd,&prs[0]);

  st=buffer;

  while (*next!=0)
  {
    next=setriteops(next,&prs[1],ops);

    if (EVAL(1)==POINTER) sprintf(st,ops,ADDR(1));
    else sprintf(st,ops,VALUE(1));
    st=strchr(st,0);
    if (st>&buffer[2499]) break;
  }
  numbytes=st-buffer+1;
  if (LENVA(0) < numbytes) { numbytes=LENVA(0); buffer[numbytes-1]=0; }
  memmove((char *) ADDR(0),buffer, numbytes);
}

void pascal upr(char *cmd)
{
  PRAM prs;

  pramset(cmd,&prs);
  switch (prs.assign)
  {
    case STRASN|POIASN:
    case STRASN: strupr((char *) prs.addr); break;
    case INTASN:
    case CHARASN: *((char *) prs.addr)=toupper((char) prs.value); break;
    default: errlist(MUSTASN,prs.pram);
  }
}

