#include "include\d3.h"

void pascal rbufs(char *cmd)
{
  BUFPTR *b;
  PRAM prs[3];
  char *next;

  next=getbuf(cmd,&prs[0]);
  if (EVAL(0)==NONE) errlist(LEASTPRM,1);

  switch (*next)
  {
    case 0:
    case ';': getlen("",&prs[1],&prs[0]);        break;
    default:  next=getlen(next,&prs[1],&prs[0]); break;
  }
  getops(next,&prs[2]);
  if (EVAL(2)!=NONE && *((char *)ADDR(2))!='L') errlist(OPMUST,'L');

  switch (mandind)
  {
    case BUFI: b=&rbuf;   break;
    case BDPI: b=&ptrbuf; break;
    case BDNI: b=&punbuf; break;
  }

  if (VALUE(1)==0)
  {
    b->avail=FALSE;
    b->strt=b->end=b->bufbeg=NULL;
    b->left=b->buflen=0;
    b->wrap=FALSE;
    b->len=0;
    return;
  }

  b->avail=TRUE;
  b->strt=b->end=b->bufbeg=(char *) ADDR(0);
  b->left=b->buflen=VALUE(1);
  if (ASN(2)==ISFILE) b->wrap=( *( (char *) ADDR(2) )=='W' );
  else b->wrap=FALSE;
  b->len=0;
}
