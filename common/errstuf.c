#include <bios.h>
#include <conio.h>
#include <string.h>
#include <stdarg.h>

#include "include\d3.h"

#define ABRTDIFF 20

void cdecl errlist(int errnum,...)
{
  char buffer[300],*pt;
  int index;
  va_list argptr;

  va_start(argptr, errnum); /* Second parameter (errnum) is not used. */

#if SYNC
	dialing=FALSE;
#endif	

	if (mdminst && NODSR()) 
  { 
    if (statval!=ANSWERING) 
    {
      statval=OFFLIN;
      DROPDTR(); 
    }
    initialize(); 
  }

  if (errnum<0)
  {
    /* Note: The argptr stuff comes from checking out the macros in stdarg.h
    It is not at all portable.  The desired string is either the first or
    second value on the stack.  The purpose of this code is to assure that
    the parameter is 0 terminated when the message is displayed. */
    switch (errnum)
    {
      case BADLINE:
      case MUSTINTASN: pt= *((char **) ((uint) argptr+2)); break;
      default: pt=*((char **) argptr);
    }

    if (*pt=='"')
    {
      pt++;
      while (*pt!='"' && *pt!=0) pt++;
      if (*pt==0) *pt++='"';
    }
    else
    {
      for (; *pt!=' ' && *pt!=',' && *pt!='+' &&
             *pt!='=' && *pt!=';' && *pt!=0; pt++);
    }

    *pt=0;
    errnum=-errnum;
  }
  index= (errnum>49) ? (errnum-ABRTDIFF) : errnum;

  vsprintf(buffer,errmessages[index],argptr);
  va_end(argptr); /* This macro does nothing! */

	sprintf(errmessbuf,"%s [%d]\r\n",buffer,errnum);
  disp("%s [%d]\r\n",buffer,errnum);
  longjmp(env,errnum);
}

char * pascal getbuf(char *pram, PRAM *prs)
{
  char *next;
  next=pramset(pram,prs);
  if ((prs->assign & STRASN) == 0) errlist(MUSTASN,prs->pram);
  if (prs->length==0) errlist(MUSTBUF,prs->pram);
  return next;
}

char * pascal getdest(char *pram, PRAM *prs)
{
  char *next;

  switch (*pram)
  {
    case ',': pram++;
    case 0: prs->assign=prs->evaltype=NONE; return pram;
  }

  next=pramset(pram,prs);
  if (*next!=0) *next++=0;
  return next;
}

char * pascal getfilen(char *pram, PRAM *prs)
{
  char *next;
  next=pramset(pram,prs);
  switch (prs->evaltype)
  {
    case ISFILE:  break;
    case POINTER: /* Filename enclosed in double quotes. */
      if (prs->convr!='"') errlist(MUSTFIL,prs->pram);
      prs->evaltype=ISFILE; 
      break;
    case SCALAR: /* Filename that starts with a number. */
      if (prs->assign==NOASN) { prs->evaltype=ISFILE; break; }
    default: errlist(MUSTFIL,prs->pram);
  }
  if (strpbrk(prs->addr,"*?")!=NULL) errlist(NOSPECS);
  return next;
}

#define NOTINRANGE (prs->addr<prsbuf->addr || \
    ( (uint) prs->addr > (uint) prsbuf->addr + prsbuf->length ) )

char * pascal getlen(char *pram, PRAM *prs, PRAM *prsbuf)
{
  char *next;

  next=pramset(pram,prs);
  switch (prs->evaltype)
  {
    case POINTER:
      if (NOTINRANGE) errlist(OUTRANGE,pram);
      prs->value=(uint) prs->addr - (uint) prsbuf->addr;
      break;
    case SCALAR: break;
    case NONE:
      if (prsbuf->evaltype!=POINTER) return next;
      prs->value=prsbuf->length; 
      return next; 
    default: errlist(MUSTLEN,prs->pram); /* case ISFILE: case LABEL: */
  }
  return next;
}

char * pascal getops(char *pram, PRAM *prs)
{
  char *next;
  if (*pram==0) { prs->assign=prs->evaltype=NONE; return pram; }
  next=pramset(pram,prs);
  if (prs->evaltype==NONE) prs->addr="";
  if (prs->evaltype==LABEL) errlist(NOLABEL,prs->pram);
  return next;
}

char * pascal getpoiasn(char *pram, PRAM *prs)
{
  char *next;
  next=pramset(pram,prs);
  if ((prs->assign & POIASN) == 0) errlist(MUSTPASN,prs->pram);
  return next;
}

char * pascal getpoint(char *pram, PRAM *prs)
{
  char *next;
  next=pramset(pram,prs);
  if (prs->evaltype!=POINTER) errlist(MUSTPOINT,prs->pram);
  return next;
}

char * pascal getscal(char *pram, PRAM *prs, byte pnum, int lowval, int highval)
{
  char *next;
  next=pramset(pram,prs);
  if (prs->evaltype==NONE) return next;
  if (prs->evaltype!=SCALAR) errlist(MUSTSCAL,prs->pram);
  if (prs->value<lowval) errlist(ERRLOW,prs->value,pnum,lowval);
  if (prs->value>highval) errlist(ERRHIGH,prs->value,pnum,highval);
  return next;
}

char * pascal getscalasn(char *pram, PRAM *prs)
{
  char *next;
  next=pramset(pram,prs);
  if ((prs->assign & (CHARASN | INTASN)) == 0) errlist(MUSTSCASN,prs->pram);
  return next;
}

char * pascal getspec(char *pram, PRAM *prs)
{
  char *next;
  next=pramset(pram,prs);
  switch (prs->evaltype)
  {
    case ISFILE:
    case NONE: break;
    case SCALAR: if (prs->assign!=NOASN) errlist(MUSTSPEC,prs->pram);
      prs->evaltype=ISFILE;
      break;
    default: errlist(MUSTSPEC,prs->pram);
  }
  return next;
}

char * pascal getstorfil(char *pram, PRAM *prs)
{
  char *next;
  next=pramset(pram,prs);

  switch (prs->evaltype)
  {
    case SCALAR: if (prs->assign!=NOASN) errlist(STRFIL,prs->pram);
      prs->evaltype=ISFILE;
      break;
    case LABEL: errlist(STRFIL,prs->pram);
  }

  if ( prs->evaltype==ISFILE && strpbrk(prs->addr,"*?")!=NULL ) 
    errlist(NOSPECS);
  return next;
}

char * pascal getval(char *pram, PRAM *prs)
{
  char *next;
  next=pramset(pram,prs);
  if ((prs->evaltype & (ISFILE|LABEL)) != 0 ) errlist(MUSTVAL,prs->pram);
  return next;
}
