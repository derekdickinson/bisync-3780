#include <bios.h>
#include <ctype.h>
#include <dir.h>
#include <dos.h>
#include <stdlib.h>
#include <string.h>

#include "include\d3.h"

static char * pascal dovar(char *pram,PRAM *vals);
static void pascal setval(USERV *uvptr,PRAM *vals);
static USERV *douv(char *pram,PRAM *vals);

#define UI(p) *( (uint *) (p) )

/* The addr field of the USERV is a pointer to a pointer if POITYP is used.
This macro Pointer Value returns the value of the actual pointer. */
#define PV(p) ( *((char **) p) )

static USERV *douv(char *pram,PRAM *vals)
{
  USERV *uvptr;

  uvptr=uv;
  while (UI(uvptr->name)!=UI(&pram[1]) && UI(uvptr->name)!=0xFFFF) uvptr++;
  if (UI(uvptr->name)==0xFFFF) errlist(BADVAR,pram);
  vals->assign=uvptr->assign;
  vals->evaltype=uvptr->evaltype;
  vals->addr=uvptr->addr;
  vals->length=uvptr->length;
  vals->lenptr=&uvptr->length;
  return uvptr;
}

static void pascal setval(USERV *uvptr,PRAM *vals)
{
  long dspace;
  struct dfree dfr;

  switch (uvptr->vtype)
  {
    case  STRTYP:                                      break;
    case CHARTYP: vals->value=*( (char *) vals->addr); break;
    case  UINTYP: vals->value=*( (int *) vals->addr);  break;
		case	IARTYP:
		case	PARTYP:
    case  POITYP:
      vals->paddr=uvptr->addr;
      vals->addr=PV(uvptr->addr);
      break;
    case   DSTYP:
      getdfree(0,&dfr);
      dspace=(long) dfr.df_avail * (long) dfr.df_bsec * (long) dfr.df_sclus;
      if (dspace>0x0FFFFL) vals->value=0xffff;
      else vals->value=(uint) dspace;
			break;
  }
}

static char * pascal dovar(char *pram,PRAM *vals)
{
  int ind2;
  static char dolbuf[100];
  static char *dolptr=dolbuf;
  USERV *uvptr;

	updatedisp();

  switch (pram[3])
  {
    case ' ': case ',': case '+': case ';': case '=': case ']': case 0:
      uvptr=douv(pram,vals);
      setval(uvptr,vals);
      switch (*pram)
      {
        case '&': errlist(BADAMP,pram);
        case '$':
          if (uvptr->evaltype!=SCALAR) errlist(BADTYP,pram);
          vals->assign=NOASN; vals->evaltype=POINTER;
          vals->addr=itoa(vals->value,dolptr,10);
          vals->length=strlen(dolptr)+1;
          dolptr+=vals->length;
          if ( (dolptr-dolbuf)>90 ) dolptr=dolbuf;
          break;
        case '#':
          if (uvptr->evaltype!=POINTER || 
							uvptr->vtype==IARTYP ||
							uvptr->vtype==PARTYP) errlist(BADTYP,pram);
          vals->assign=NOASN; vals->evaltype=SCALAR; vals->length=2;
					if (toupper(((char *)vals->addr)[0])=='H')
						vals->value=ahextoi(&((char *)vals->addr)[1]);
          else vals->value=atoi(vals->addr);
          break;
        case '!':
          vals->length=2; vals->evaltype=SCALAR;
          vals->value=uvptr->length; vals->addr=&uvptr->length;
          vals->assign = (uvptr->vtype==POITYP) ? INTASN : NOASN;
          break;
        case '@':
					if (uvptr->evaltype!=POINTER) errlist(BADTYP,pram);
					vals->evaltype=SCALAR;
					switch (uvptr->vtype)
					{
					case IARTYP:
						vals->value=*( (int *) vals->addr);
			      if (uvptr->assign!=NOASN) vals->assign=INTASN;
						vals->lenptr=NULL;
				    vals->length=2;
						break;
					case PARTYP:
			      if (uvptr->assign!=NOASN) vals->assign=POIASN;
						vals->evaltype=POINTER;
				    vals->length=strlen((char *) vals->addr);
						break;
					default:
						vals->value=*( (char *) vals->addr);
	          if (uvptr->assign!=NOASN) vals->assign=CHARASN;
		        vals->length=1;
						break;
					}
          break;
        /* case '%': break; */
      }
      return pram+3;
    case '[':
      {
        PRAM prs;

        pramset(&pram[4],&prs);  /* Solve indexes recursively. */
        if (prs.evaltype!=SCALAR) errlist(BADIND,prs.pram);
        ind2=prs.value;
      }
      uvptr=douv(pram,vals);
      if (uvptr->evaltype!=POINTER && *pram!='$' ) errlist(BADTYP,pram);

			if (uvptr->evaltype==SCALAR) vals->value=*( (int *) vals->addr);
			else
			{
				switch (uvptr->vtype)
				{
				case PARTYP:
					vals->paddr=&(((char **) uvptr->addr)[ind2]);
					vals->addr=*vals->paddr;
					if (vals->assign!=NOASN) vals->assign=POIASN;
					vals->length=strlen((char *) vals->addr);
					break;
				case IARTYP:
					vals->addr=PV(uvptr->addr) + 2*ind2;
					if (vals->assign!=NOASN) vals->assign=INTASN;
					vals->lenptr=NULL;
		      vals->length-=(2*ind2);
			    vals->value=*((int *) vals->addr);
					break;
				case POITYP:
					vals->addr=PV(uvptr->addr)+ind2;
	        vals->assign&=(~POIASN);
			    vals->length-=ind2;
				  vals->value=*((char *) vals->addr);
					break;
				default:
					((char *)vals->addr)+=ind2;
			    vals->length-=ind2;
				  vals->value=*((char *) vals->addr);
				}
			}

      switch (*pram)
      {
        case '%':
					vals->evaltype=SCALAR;
					switch (uvptr->vtype)
					{
					case IARTYP: vals->length=2;					break;
					case PARTYP: vals->evaltype=POINTER;	break;
					default:
						if (vals->assign!=NOASN) vals->assign=CHARASN;
						vals->length=1;
					}
          break;
        case '#':
					if (uvptr->vtype==IARTYP) errlist(BADTYP,pram);
          vals->assign=NOASN; vals->evaltype=SCALAR; vals->length=2;
					if (toupper(((char *)vals->addr)[0])=='H')
						vals->value=ahextoi(&((char *)vals->addr)[1]);
          else vals->value=atoi(vals->addr);
          break;
        case '!':
          vals->assign=NOASN; vals->evaltype=SCALAR;
          vals->value=vals->length; vals->length=2;
          break;
        case '$':
          vals->assign=NOASN; vals->evaltype=POINTER;
          vals->addr=itoa(vals->value,dolptr,10);
					vals->length=strlen(dolptr)+1;
          dolptr+=vals->length;
          if ( (dolptr-dolbuf)>90 ) dolptr=dolbuf;
					if (uvptr->evaltype==SCALAR)
					{
						((char *) vals->addr)+=ind2;
						vals->length-=ind2;
					}
          break;
        case '@': errlist(BADAMP,pram);
      }

      while (*pram!=' ' && *pram!=',' && *pram!=0 && 
             *pram!=';' && *pram!='=' && *pram!='+') pram++;
      break;
    default: errlist(BADEND,pram);
  }
  return pram;
}

char * pascal pramset(char *pram,PRAM *vals)
{
  char *pt,docnv=FALSE;

  if (*pram==' ' || *pram==',' || *pram=='+' ||  
      *pram=='=' || *pram==';') pram++;

  vals->pram=pram;

  switch (*pram)
  {
    case ',': pram++;
    case 0:   vals->evaltype=vals->assign=NONE; vals->value=0; return pram;
    case '\'':
    case '"': break;
    case ':':
      for (pt=pram; *pt!=' ' && *pt!=',' && *pt!='+' &&
                    *pt!='=' && *pt!=';' && *pt!=0; pt++);
      break;
    case '^': pram++; docnv=TRUE;
    default:
      for (pt=pram; *pt!=' ' && *pt!=',' && *pt!='+' &&
                    *pt!='=' && *pt!=';' && *pt!=0; pt++) *pt=toupper(*pt);
  }

  vals->convr=*pram;

  switch (*pram)
  {
    case '%': case '!': case '$': case '#': case '&': case '@':
      pt=dovar(pram,vals); 
      break;
    case '<': pram++;
    case '-':
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
      vals->evaltype=SCALAR; vals->assign=NOASN;
      vals->length=2;
      if (*pram=='H') vals->value=ahextoi(&pram[1]);
      else vals->value=atoi(pram);
      vals->addr=pram;
      break;
      case '"':
      vals->evaltype=POINTER; vals->assign=NOASN;
      vals->addr=pram; 
      while (TRUE)
      {
        if ((&cmdbuf[CMDBUFSIZ]-pram)>=CMDBUFSIZ) pram=convrt(pram,CMDBUFSIZ-10);
				else pram=convrt(pram,&cmdbuf[CMDBUFSIZ]-pram); 
        if (*pram=='"' || *pram==0) { *pram=0; break; }
        else pram++;
      }
      vals->length=pram-vals->addr;
      pt=pram+1;
      break;
    case '\'':
      vals->evaltype=SCALAR; vals->assign=NOASN;
      vals->value=pram[1];   vals->addr=&vals->value;
      vals->length=1;
      pt=pram+3;
      break;
    case ':':
      vals->evaltype=LABEL; vals->assign=LABEL;
      vals->addr=pram;
      break;
    case ']': pram++;
    default:
      vals->evaltype=ISFILE; vals->assign=ISFILE;
      vals->addr=pram;
  }
  if (docnv)
  {
    if (vals->evaltype!=POINTER) errlist(BADCARAT,pram);
    if ( *((char *) vals->addr)==':') { vals->evaltype=vals->assign=LABEL; }
    else { vals->evaltype=vals->assign=ISFILE; }
  } 
  return pt;
}

