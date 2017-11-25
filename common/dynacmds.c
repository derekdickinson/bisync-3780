#include <alloc.h>
#include <sys\stat.h>
#include <string.h>

#include "include\d3.h"

void pascal allstuf(char *cmd)
{
  PRAM prs[2];
  char *next;
	struct stat buffer;
	FILE *thefile;
	uint length;

  if (*cmd==0) errlist(MUSTPRM,2);
  next=getpoiasn(cmd,&prs[0]);

	switch (mandind)
	{
    case ALLOCI:
    case REALLI:	getscal(next,&prs[1],2,LOWVAL,HIGHVAL);	break;
    case ALLSTI:	getpoint(next,&prs[1]);									break;
    case ALLFBI:	getfilen(next,&prs[1]);									break;
	}	
  if (ADDR(0)!=NULL) { free(ADDR(0)); LENPV(0)=0; }
  switch (mandind)
  {
    case ALLOCI:
      if ((PADDR(0)=malloc(VALUE(1)))==NULL) errlist(NOINTMEM);
			memset(PADDR(0),0,VALUE(1));
      break;
		case ALLSTI:
		  if ((PADDR(0)=malloc(strlen(ADDR(1))+1))==NULL) errlist(NOINTMEM);
		  strcpy(PADDR(0),ADDR(1));
		  LENPV(0)=strlen(ADDR(1))+1;
			return;
		case ALLFBI:
		  if (stat((char *) ADDR(1),&buffer)!=0) errlist(BADFILE,ADDR(1));
			if (( thefile=fopen(ADDR(1),"rb") )==NULL) errlist(BADFILE,ADDR(1));
	    if ((uint) (buffer.st_size >> 16) > 0) errlist(TOOBIG); 
			length=(uint) buffer.st_size+1;
		  if ((PADDR(0)=malloc(length))==NULL) errlist(NOINTMEM);
			memset(PADDR(0),0,length);
			fread(PADDR(0),length,1,thefile);
		  LENPV(0)=length;
			return;
    case REALLI:
      if ( (next=realloc(ADDR(0),VALUE(1)))==NULL ) errlist(NOINTMEM);
      PADDR(0)=next;
      break;
  }
  LENPV(0)=VALUE(1);
}

void pascal dofree(char *cmd)
{
  PRAM prs;
  if (*cmd==0 || *getpoiasn(cmd,&prs)!=0) errlist(MUSTPRM,1);
  if (prs.addr!=NULL) { free(prs.addr); *prs.lenptr=0; }
  *prs.paddr=NULL;
  *prs.lenptr=0;
}

#define UI(p) *( (uint *) (p) )

/* The addr field of the USERV is a pointer to a pointer if POITYP is used.
This macro Pointer Value returns the value of the actual pointer. */
#define PV(p) ( *((char **) p) )

void pascal setmp(char *cmd)
{
  PRAM prs[2];
  char *next;
	char *mem;
	char *pram;
  USERV *uvptr;

  if (*cmd==0) errlist(MUSTPRM,2);
  next=pramset(cmd,&prs[0]);
	if (ASN(0)!=PARASN) errlist(TYPMISS,prs[0].pram);
  getscal(next,&prs[1],2,LOWVAL,HIGHVAL);

  if (ADDR(0)!=NULL) { free(ADDR(0)); LENPV(0)=0; }
  if ((mem=malloc(2*VALUE(1)))==NULL) errlist(NOINTMEM);
	memset(mem,0,2*VALUE(1));

	pram=prs[0].pram;

  uvptr=uv;
	while (UI(uvptr->name)!=UI(&pram[1]) && UI(uvptr->name)!=0xFFFF) uvptr++;
	uvptr->addr=mem;
}
