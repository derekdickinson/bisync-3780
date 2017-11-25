#include <alloc.h>
#include <bios.h>
#include <ctype.h>
#include <conio.h>
#include <dir.h>
#include <dos.h>
#include <stdlib.h>
#include <string.h>
#include <sys\stat.h>

#include "include\d3.h"

static void pascal sseek(long offs);

boolean pascal sgets(char *buf)
{
	char *crptr;

	if (scrbuf==NULL) return (fgets(buf,CMDBUFSIZ,scrfile)==NULL);

	if (*scrbufptr==0) return TRUE;

	crptr=strchr(scrbufptr,CR);
	if (*crptr==0)
	{
		if (crptr[-1]==26) { crptr--; *crptr=0; } /* Check for EOF char */
		strncpy(buf,scrbufptr,CMDBUFSIZ);
		scrbufptr=crptr;
	}
	else
	{
		*crptr=0;
		strncpy(buf,scrbufptr,CMDBUFSIZ);
		*crptr=CR;
		scrbufptr=crptr+1;
		if (*scrbufptr==LF) scrbufptr++;
	}
	return FALSE;
}

static void pascal sseek(long offs)
{
  if (scrbuf==NULL) fseek(scrfile,offs,SEEK_SET);
	else scrbufptr=scrbuf+(uint) offs;
}

long pascal stell(void)
{
	return ( (scrbuf==NULL) ? ftell(scrfile) : scrbufptr-scrbuf );
}

void pascal sclose(void)
{
	scrbuf=scrbufptr=NULL; NUCLOS(scrfile); 
}

boolean pascal sfeof(void)
{
	if (scrbuf==NULL)	return feof(scrfile);
	return (*scrbufptr==0);
}

void pascal setllens(void)
{
  char linbuf[CMDBUFSIZ],*llenstmp,*tabptr;
  int i;
  uint coresize;

  if (labtable!=NULL) free(labtable);
  labtable=llens=NULL;

  if (!mb) return;

  coresize=coreleft();
  if (coresize<10000) return;
  coresize-=10000;
  coresize/=2;
  if ( (labtable=malloc(coresize))==NULL ) errlist(NOINTMEM);

  tabptr=labtable;
  llenstmp=&labtable[coresize-1];

	for (cmdline=1; !sgets(linbuf) ; cmdline++)
  {
		nocr(linbuf);
    if (llenstmp!=NULL) *llenstmp--=strlen(linbuf)+2;
    if (linbuf[0]==':')
    {
			*( (ulng *) tabptr)++=stell();
      *( (uint *) tabptr)++=cmdline;
      strcpy((char *) tabptr,&linbuf[1]);
      tabptr=strchr(tabptr,0);
      tabptr++;
    }
    if (llenstmp<tabptr) llenstmp=NULL;
    if (tabptr>&labtable[coresize-1])
    {
      free(labtable);
      labtable=tabptr=NULL;
      break;
    }
  }
  if (tabptr!=NULL) *tabptr++=0;

  if (llenstmp!=NULL)
  {
    byte temp;

    llens=tabptr;
    llenstmp=&labtable[coresize-1];
    for (i=0; i<cmdline ; i++)
    {
      temp=*llenstmp;
      *llenstmp--=*tabptr;
      *tabptr++=temp;
    }
    *tabptr++=0;
  }
  if (tabptr!=NULL)
  {
    char *temp;
    temp=realloc(labtable,tabptr-labtable);
    if (temp==NULL)
    {
      free(labtable);
      labtable=NULL;
      llens=NULL;
    }
    if (labtable!=temp)
    {
      uint diff;
      diff=llens-labtable;
      labtable=temp;
      llens=diff+labtable;
    }
  }
	sseek(0);
  cmdline=0;
  return;
}

/* The general branching function. */
void pascal dogo(PRAM *prs)
{
  TBLENT *tabptr;
  char linbuf[CMDBUFSIZ];
  uint strtline;

  strtline=cmdline;

  if (dispstate==CMDINT) errlist(CANTINT);

  switch (prs->evaltype)
  {
    case LABEL:
      if (labtable!=NULL)
      {
        tabptr=(TBLENT *) labtable;
        while (TRUE)
        {
          if (strcmp(tabptr->label,&((char *)prs->addr)[1])==0)
          {
						sseek(tabptr->seekval);
            cmdline=tabptr->line;
            break;
          }
          tabptr=(TBLENT *) (strchr(tabptr->label,0)+1);
          if (tabptr->label[0]==0) errlist(BADLBL,prs->pram);
        }
        break;
      }
      while (TRUE)
      {
        if (sgets(linbuf))
        {
          sseek(0);
          if (cmdline==0) errlist(CMDFILRERR);
          cmdline=0;
          continue;
        }
				nocr(linbuf);
        cmdline++;
        if (cmdline==strtline) errlist(BADLBL,prs->pram);
        if ( (linbuf[0]==':') && (strcmp(linbuf,prs->addr)==0) ) break;
      }
      break;
    case SCALAR:
      if (*(prs->pram)==',' || *(prs->pram)==' '|| *(prs->pram)=='=') prs->pram++;
      if (prs->pram[0]=='<') prs->value+=cmdline;
      if (llens!=NULL)
      {
        byte i,*lenptr;
        ulng seekto=0;

        for (i=1, lenptr=(byte *) llens; i<prs->value; i++)
        {
          if (*lenptr==0) errlist(BADLINE,prs->value,prs->pram);
          seekto+=*lenptr++;
        }
        sseek(seekto);
        cmdline=prs->value;
        break;
      }
      if (prs->value<cmdline)
      {
        sseek(0);
        cmdline=0;
      }
      while (TRUE)
      {
        if (sgets(linbuf))
        {
          sseek(0);
          if (cmdline==0) errlist(CMDFILRERR);
          cmdline=0;
          continue;
        }
        cmdline++;
        if (cmdline==prs->value) break;
        if (cmdline==strtline) errlist(BADLINE,prs->value,prs->pram);
      }
      break;
    case POINTER:
      memset(linbuf,0,CMDBUFSIZ);
      strcpy(linbuf,prs->addr);
      memset(cmdbuf,0,CMDBUFSIZ);
      strcpy(cmdbuf,linbuf);
      parcmd();
      break;
    case ISFILE:
      if (*((char*) prs->addr)=='Q') intrp("Q");
      else errlist(MUSTDEST,prs->pram);
  }
}

/* This routine calls a subroutine or another script.  It saves
pointers to the values on the heap.  Heap space could be saved by releasing
the old label table and line length table and then regenerating them on
return.  If calls are pending however the space saved may be fragmented to
the point where it is not that useful. */
void pascal call(char *cmd)
{
  PRAM prs;
  char lastcmdname[FILENAMESIZE];
  uint retlin;
  ulng seekto;
  char *laslabtab,*lasllens,*lascrbuf;
  CALLRET *lascalls,*curcall;

  if (dispstate==CMDINT) errlist(CANTINT);

  cmd++;
  pramset(cmd,&prs);

  /* Load a new batch file and start performing commands out of it. */
  if (prs.evaltype==ISFILE || prs.evaltype==POINTER)
  {
    lascalls=calls;      /* Save subroutine call list. */
    laslabtab=labtable;  /* Save label table */
    lasllens=llens;      /* Save Line length table */
		
    labtable=llens=(void *)calls=NULL; /* Clear call list for new file. */

    if (lfptr!=NULL) 
    {
      if (wx()!=1) fprintf(lfptr,"\r\n");
			fprintf(lfptr,"Leaving Script: %s\r\n",scrnameptr);
    } 

    strcpy(lastcmdname,scrnameptr); 
		lascrbuf=scrbuf;
		
		if (prs.evaltype==POINTER) strcpy(scrnameptr,prs.pram);
		else strcpy(scrnameptr,(char *) prs.addr);
		
    retlin=cmdline; 
		seekto=stell();
		sclose();

		if (prs.evaltype==POINTER) scrbuf=prs.addr;
    filecmd(FCALL); /* Recursive call to filecmd */

		scrbuf=lascrbuf;
    strcpy(scrnameptr,lastcmdname); cmdline=retlin;

		if (lfptr!=NULL) fprintf(lfptr,"\r\nReturned to Script: %s\r\n",scrnameptr);

		if (scrbuf==NULL)
			{ if ((scrfile=fopen(scrnameptr,"r"))==NULL) errlist(NORET,scrnameptr); }
		else scrbufptr=scrbuf;

    calls=lascalls;     /* Switch to subroutine calls in this file. */
    labtable=laslabtab; /* Restore old label table */
    llens=lasllens;     /* Restore line length table */

    sseek(seekto);
    return;
  }

  if ( (curcall=malloc(sizeof(CALLRET)))==NULL ) errlist(NOINTMEM);
  curcall->line=cmdline;
  curcall->seekto=stell();
  curcall->nextret=calls;
  calls=curcall;
  dogo(&prs);
}

void pascal clrca(void)
{
  CALLRET *curcall;

  /* Free any unreturned subroutine calls in called file. */
  while (calls!=NULL)
  {
    curcall=calls->nextret;
    free(calls);
    calls=curcall;
  }
}

void pascal end(void)
{
  if (dispstate==CMDINT) errlist(CANTINT);

  if (scrbuf==NULL) fseek(scrfile,0,SEEK_END);
	else scrbufptr=strchr(scrbuf,0);
}

void pascal go(char *line)
{
  PRAM prs;

  if (dispstate==CMDINT) errlist(CANTINT);

  line++;
  getdest(line,&prs);
  dogo(&prs);
}

void pascal ifcond(char *st)
{
  PRAM prs[2];
  char *next;

  if (dispstate==CMDINT) errlist(CANTINT);

  st++;
  next=getdest(st,&prs[0]);
  getdest(next,&prs[1]);

  if (ei)
  {
    if ( (loopvar!=0) && (--loopvar==0) ) { errlist(LOOPOUT); return; }
    dogo(&prs[0]);
  }
  else dogo(&prs[1]);
}

void pascal ife(char *cmd)
{
  PRAM prs[3];
  char *next;
  struct ffblk afile;

  next=getspec(cmd,&prs[0]);
  if (EVAL(0)==NONE) errlist(LEASTPRM,1);
  if (*next!=0) *next++=0;

  next=getdest(next,&prs[1]);
  getdest(next,&prs[2]);

  if (findfirst(ADDR(0),&afile,0)==0) dogo(&prs[1]);
  else dogo(&prs[2]);
}

void pascal ififs(char *cmd)
{
  PRAM prs[4];
  char *next;

  next=pramset(cmd,&prs[0]);
  if (*next!=0) *next++=0;

  next=pramset(next,&prs[1]);
  if (*next!=0) *next++=0;

  if (EVAL(1)==NONE) errlist(LEASTPRM,3);

  next=getdest(next,&prs[2]);
  getdest(next,&prs[3]);

  if (EVAL(0)!=EVAL(1)) errlist(NOMATCH,prs,2);

	switch (mandind)
	{
	case IFGI:
    if (EVAL(0)==SCALAR)
    {
      if (VALUE(0)>VALUE(1)) dogo(&prs[2]);
      else dogo(&prs[3]);
      return;
    }
    if (strcmp(ADDR(0),ADDR(1))>0) dogo(&prs[2]);
    else dogo(&prs[3]);
    return;
	case IFII:
    if (EVAL(0)==SCALAR)
    {
      if (VALUE(0)==VALUE(1)) dogo(&prs[2]);
      else dogo(&prs[3]);
      return;
    }
    if (strcmp(ADDR(0),ADDR(1))==0) dogo(&prs[2]);
    else dogo(&prs[3]);
    return;
	case IFSI:
	  if (EVAL(0)!=POINTER) errlist(MUSTPOINT,prs[0].pram);

		if (STRLCMP(ADDR(0),ADDR(1))) dogo(&prs[2]);
	  else dogo(&prs[3]);
		return;
	}

}

void pascal ifo(char *st)
{
  PRAM prs[2];
  char *next;

  st++;
  next=getdest(st,&prs[0]);
  getdest(next,&prs[1]);

  if (NODSR()) dogo(&prs[1]);
  else dogo(&prs[0]);
}

void pascal ift(char *cmd)
{
  struct stat buffer;
  unsigned long time0;
  char *next;
  PRAM prs[5];

  next=getfilen(cmd,&prs[0]);
  if (*next!=0) *next++=0;

  next=getfilen(next,&prs[1]);
  if (*next!=0) *next++=0;

  if (EVAL(1)==NONE) errlist(LEASTPRM,3);

  next=getdest(next,&prs[2]);
  next=getdest(next,&prs[3]);
  getdest(next,&prs[4]);

  if (stat((char *) ADDR(0),&buffer)==-1) errlist(BADFILE,ADDR(0));
  time0=buffer.st_atime;

  if (stat((char *) ADDR(1),&buffer)==-1) errlist(BADFILE,ADDR(1));

  if (time0==buffer.st_atime)
  {
    dogo(&prs[2]);
    return;
  }

  if (time0<buffer.st_atime)
  {
    dogo(&prs[3]);
    return;
  }

  dogo(&prs[4]);

}

void pascal il(char *value)
{
  PRAM prs;

  /* Maintain loop variable.  Do checks in if conditionals. */
  getscal(value,&prs,1,LOWVAL,HIGHVAL);
  loopvar=prs.value;
}

void pascal inter(void)
{
  if (dispstate==CMDINT) errlist(CANTINT);
	cls();
	
  if (lfptr!=NULL)     
  {
    if (wx()!=1) fprintf(lfptr,"\r\n");    
		fprintf(lfptr,"Leaving Script: %s\r\n",scrnameptr);
  }  
  clrca();
  if (labtable!=NULL) { free(labtable); labtable=llens=NULL; }
	sclose();
	concmd();
}

void pascal on(char *st)
{
  PRAM prs[4];
  char *next;
  uint value;

  if (dispstate==CMDINT) errlist(CANTINT);

  next=getscal(st,&prs[0],1,LOWVAL,HIGHVAL);
  value=VALUE(0);
  if (*next==0) errlist(LEASTPRM,2);

  while (*next!=0)
  {
    next=getscal(next,&prs[1],0,LOWVAL,HIGHVAL);

    if (*next==';') next=getscal(next,&prs[2],0,LOWVAL,HIGHVAL);
    else VALUE(2)=VALUE(1);

    if (*next=='=') next=getdest(next,&prs[3]);
    else EVAL(3)=NONE;

    if (value>=VALUE(1) && value<=VALUE(2)) { dogo(&prs[3]); return; }
  }

}

void pascal ret(void)
{
  CALLRET *calltop;

  if (dispstate==CMDINT) errlist(CANTINT);
  if (calls==NULL) errlist(RETOVER);

  sseek(calls->seekto);
  cmdline=calls->line;
  calltop=calls;
  calls=calls->nextret;
  free(calltop);
}

void pascal xb(char *filename)
{
  FILE *newfile;
  PRAM prs;

  getstorfil(filename,&prs);

	if (prs.evaltype==POINTER) 
	{
		strcpy(scrnameptr,prs.pram);
		scrbuf=(char *) prs.addr;
	}
  else strcpy(scrnameptr,(char *) prs.addr);

  /* Load a new batch file and start performing commands out of it. */
  if (dispstate==CMDINT)
  {
    filecmd(FCON);
    dispstate=CMDINT;
  }
  else
  {
    /* Open new file */
		if ( prs.evaltype!=POINTER && ((newfile=fopen(prs.addr,"r"))==NULL) )
			errlist(BADFILE,prs.addr);

    NUCLOS(scrfile);   /* Close old file */

    if (lfptr!=NULL)    
    {
      if (wx()!=1) fprintf(lfptr,"\r\n");    
			fprintf(lfptr,"Leaving Script: %s\r\n",scrnameptr);
    }	 
    strcpy(scrnameptr,prs.addr);
		if (lfptr!=NULL) fprintf(lfptr,"Script: %s\r\n",scrnameptr);

    if (prs.evaltype!=POINTER) scrfile=newfile;   /* Set file pointer to new file */

    /* Free any unreturned subroutine calls in called file. */
    clrca();

    setllens();
    cmdline=0;
  }
}
