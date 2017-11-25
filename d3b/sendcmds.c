#include <bios.h>
#include <conio.h>
#include <ctype.h>
#include <dir.h>
#include <stdlib.h>
#include <string.h>

#include "include\d3.h"

static void pascal doparms(char *st,SND *snd);
static char * pascal getparms(char *st,char *set);

/* File SEND.C */

void pascal sctrl(char *cmd)
{
	PRAM prs[2];
	BIGSTR mess;

	getpoint(cmd,&prs[0]);

	mess.len=stpcpy(mess.st,ADDR(0))-mess.st;

	ctrlptr=(LITSTR *) &mess;
	txstate=CTRLSEND;
	send();
	while (txstate!=0) if (ABRTE()) errlist(USRABRT);
}

void pascal freesndlist(void)
{
	SND *sndptr0,*sndptr1;

	sndptr0=sndlist;
	sndptr1=sndlist->nextsnd;

	while (sndptr0!=NULL)
	{
		free(sndptr0);
		sndptr0=sndptr1;
		sndptr1=sndptr0->nextsnd;
	}
	sndlist=NULL;
}

static void pascal doparms(char *st,SND *snd)
{
	char gotype=0;

	if (*st!=0)
	{
		for (; *st!=0; st++)
		{
			switch (toupper(*st))
			{
				case 'S': snd->buftype=VARY;				 gotype='S'; break;
				case 'T': snd->buftype=TRAN | FIXED; gotype='T'; break;
				case 'V':
					if (gotype=='T') snd->buftype=TRAN | VARY;
					else snd->buftype=VARY;
					break;
				case 'F':
					if (gotype=='S') snd->buftype=FIXED;
					else snd->buftype=TRAN | FIXED;
					break;
				case 'B':
					if (gotype=='S') snd->buftype=BINARY;
					else snd->buftype=TRAN | BINARY;
					break;
				case 'H':
					if (gotype=='T') snd->buftype=TRAN | HEXPAIR;
					else snd->buftype=HEXPAIR;
					break;
				case 'N': snd->prindest=PUNCH;				 break;
				case 'P': snd->prindest=PRINTER;			 break;
				case 'C': snd->catype=ETB;						 break;
				case 'X': snd->catype=ETX;						 break;
			}
			*st=0;
		}
	}

}

/* Searches backward for character from set.	Returns pointer to character
if it exists and a pointer to the terminating 0 at the end of the string
otherwize. */
static char * pascal getparms(char *st,char *set)
{
	char *srchset,*stend;

	for (stend=strchr(st,0)-1; stend>st; stend--)
	{
		if (*stend=='"') break;
		for (srchset = set; *srchset; srchset++)
			if (*stend == *srchset) return(stend);
	}
	return (strchr(st,0));
}

#define FREERR1(uno) { freesndlist(); errlist(uno); }
#define FREERR2(uno,dos) { freesndlist(); errlist(uno,dos); }
#define CADDR(a) ((char *) ADDR(a))

void pascal sendf(char *cmd)
{
	PRAM prs[2];
	SND *stack=NULL,*curptr,*lasptr,defsnds;
	char *next,*opptr,drive[3],directory[66],name[9],ext[5];
	struct ffblk dirinfo;

	while (*cmd==' ') cmd++; /* Get rid of extra blanks. */

	sndlist=NULL;

	defsnds.prindest=prindest;
	defsnds.buftype=buftype;
	defsnds.catype=ETX;
	defsnds.nextsnd=NULL;

	EVAL(0)=POINTER;

	if (statval<ONLIN) errlist(CANTOFF);

	opptr=getparms(cmd," ,");
	if (*opptr!=0) *opptr++=0;
	doparms(opptr,&defsnds);

	prindest=defsnds.prindest;

	stripblanks(cmd);

	/* Send a file or message.	Do a syntactic check to determine which. */
	/* allow any combination of punch, binary and concatenation to be used. */
	for (next=cmd; *next!=0;)
	{
		next=pramset(next,&prs[0]);
		if ((curptr=malloc(sizeof(SND)))==NULL) FREERR1(NOINTMEM);
		memcpy(curptr,&defsnds,sizeof(SND));
		if (*next==';')
		{
			*next++=0;
			next=pramset(next,&prs[1]);
			if (EVAL(1)==LABEL) FREERR2(NOLABEL,prs[1].pram);
			*next++=0;
			doparms(CADDR(1),curptr);
		} else *next++=0;

		switch (EVAL(0))
		{
		case POINTER:	curptr->sensrc=MESBUF; curptr->mesbuf=CADDR(0);	break;
		case SCALAR : if (ASN(0)!=NOASN) FREERR2(STRFIL,prs[0].pram);
		case ISFILE :
			if (findfirst(CADDR(0),&dirinfo,0)==0)
			{
				curptr->sensrc=SENFILE;
				fnsplit(CADDR(0),drive,directory,NULL,NULL);
				fnsplit(dirinfo.ff_name,NULL,NULL,name,ext);
				fnmerge(curptr->filename,drive,directory,name,ext);
				lasptr=curptr;
				while (findnext(&dirinfo)==0) /* Support wildcards */
				{
					curptr->nextsnd=stack;
					stack=curptr;
					if ((curptr=malloc(sizeof(SND)))==NULL) FREERR1(NOINTMEM);
					memcpy(curptr,lasptr,sizeof(SND));
					fnsplit(dirinfo.ff_name,NULL,NULL,name,ext);
					fnmerge(curptr->filename,drive,directory,name,ext);
				}
			}	else FREERR2(BADFILE,CADDR(0));
			break;
		default: FREERR2(STRFIL,prs[0].pram);
		}

		/* Move to next structure in linked list. */
		curptr->nextsnd=stack;
		stack=curptr;
	}

	/* Reverse linked list so items are transmitted from left to right. */
	lasptr=NULL; sndlist=curptr=stack;
	while (curptr->nextsnd!=NULL)
	{
		sndlist=curptr->nextsnd;
		curptr->nextsnd=lasptr;
		lasptr=curptr;
		curptr=sndlist;
	}
	curptr->nextsnd=lasptr;
	
	if (f_state>12 && f_state<21) { natolmt=0xFFFF; gorcv(FALSE); }

	listsend();

	freesndlist();

	disp("°±²Û Send Completed Û²±°\r\n");
}

void pascal rem(char *cmd)
{
	PRAM prs;
	char *next,rembuf[2000],*remptr;

	if (statval<ONLIN) errlist(CANTOFF);

	if (sndlist!=NULL) errlist(CURSEND);

	if ( (sndlist=malloc(sizeof(SND)))==NULL ) errlist(NOINTMEM);

	memset(rembuf,0,2000);
	remptr=rembuf;

	sndlist->sensrc=REMCMD;
	sndlist->mesbuf=rembuf;
	sndlist->prindest=prindest;
	sndlist->buftype=buftype;
	sndlist->catype=cmend;
	sndlist->nextsnd=NULL;

	stripblanks(cmd);
	next=cmd;
	if (*next==0) FREERR2(LEASTPRM,1);

	/* Send a file or message.	Do a syntactic check to determine which. */
	/* allow any combination of punch, binary and concatenation to be used. */
	while (*next!=0)
	{
		next=pramset(next,&prs);
		if (prs.evaltype!=POINTER) FREERR2(MUSTPOINT,prs.pram);

		sprintf(remptr,"0002%.2d%s",strlen((char *) prs.addr),prs.addr);
		remptr=strchr(rembuf,0);
	}

	if (f_state>12 && f_state<21) { natolmt=0xFFFF; gorcv(FALSE); }

	listsend();

	freesndlist();

	disp("°±²Û Remote Command Completed Û²±°\r\n");
}
