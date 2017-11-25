#include <bios.h>
#include <conio.h>
#include <ctype.h>
#include <dos.h>
#include <string.h>
#include <stdlib.h>

#include "include\d3.h"

static boolean pascal endcmd(void);

void pascal nocr(char *buffer)
{
	*strchr(buffer,'\n')=0; *strchr(buffer,'\r')=0;
}

static boolean pascal endcmd(void)
{
	int cmdsize;

	cmdsize=strlen(cmdbuf)+edcol;
	gorocol(edro,edcol);
	do {
		dputs("\r\n");
		cmdsize-=(winright-winleft+1);
	} while (cmdsize>0);
	SETLINES(curlines);
	return TRUE;
}

boolean pascal cmdproc(void)
{
#if SYNC
	if (keystate==INKEY) return TRUE;
#endif
	if (keystate!=INCMD)
	{
		keystate=INCMD;
		memset(cmdbuf,0,CMDBUFSIZ);
		seted(wy(),promplen,cmdbuf,CMDBUFSIZ);
	}
	switch (edbyt())
	{
		case RET:
			strcpy(lascmd,cmdbuf); keystate=NORM;
			return endcmd();
		case F5: 
			strcpy(lascmd,cmdbuf); 
		case ESC:
			endcmd();
			cmdbuf[0]=0; keystate=NORM;
			return TRUE;
		case F1:
			cmdbuf[edcur]=lascmd[edcur];
			break;
		case F3:
			strcpy(&cmdbuf[edcur],&lascmd[edcur]);
			break;
		default: return FALSE;
	}
	/* This only executes if F1 or F3 is pressed */
	edrite(); edcur=strlen(edst); gorocol(edro,edcur+edcol);
	return FALSE;
}

byte pascal getind(char *buf)
{
	char *chptr;
	boolean special;
	uint *sumptr,mandsum;
	byte i,cmdlen;

	if (isalpha(buf[0]))
	{
		chptr=strpbrk(buf," ,");
		if (chptr==NULL) cmdlen=strlen(buf);
		else { cmdlen=chptr-buf; }
		special=FALSE;
	}
	else { special=TRUE; cmdlen=1; }

	i=buf[cmdlen];
	buf[cmdlen]=0;

	strupr(buf);
	mandsum=chkcrc(buf);

	buf[cmdlen]=i;

	for ( sumptr=mandsums; *sumptr!=0xffff; sumptr++)
	{
		if (*sumptr==mandsum)
		{
			mandind=sumptr-mandsums;
			if (strncmp(buf,cmds[mandind].name,cmdlen)!=0) continue;
			return (special) ? 0 : cmdlen;
		}
	}
	mandind=sumptr-mandsums;
	return (special) ? 0 : cmdlen;
}

void pascal parcmd(void)
{
	byte cmdlen;
	getrocol(&curro,&curcol);

	nocr(cmdbuf);
	if (cmdbuf[0]==0) return;

	cmdlen=getind(cmdbuf);
	if (cmds[mandind].func==NULL) reperror();
	cmds[mandind].func(&cmdbuf[cmdlen]);
}

void pascal concmd(void)
{
#if SYNC
	natolmt=0; strtime=*nowtime;
	curbuf=1; firstblock=TRUE;
	setdestype(act);
#endif

  datain=0;
	f_state=START_R;
	txstate=rxstate=0;

	dispstate=CMDINT;
	dosup=FALSE;
	memset(cmdbuf,0,CMDBUFSIZ);
	if (wintop!=1) gorocol(1,1);

	while (TRUE)
	{
		ei=setjmp(env);
		if (ei!=0) { ep=ei; el=cmdline; }
		dosup=FALSE;

		promp();
		keystate=ATPROMPT;

#if ASYNC
		while (TRUE) { if (KBHIT() && cmdproc()) break; updatedisp();	}
#endif

#if SYNC
		gorcv(TRUE);
#endif

		parcmd();
	}
}

void pascal doex(void)
{
	if (*exfile==0) return;

	clrca();
	if (labtable!=NULL) { free(labtable); labtable=llens=NULL; }
	sclose();

	if (*exfile==':') strcpy(scrnameptr,cfgfilname);
	else
	{
		strcpy(scrnameptr,exfile);
		*exfile=0;
	}

	filecmd(FEXIT);
}

static void pascal runstepline(void)
{
  char dummybuf[CMDBUFSIZ];

	/* Copy current command */
  memset(dummybuf,0,CMDBUFSIZ);
  strcpy(dummybuf,cmdbuf);

	/* Load and run step command */
	strcpy(cmdbuf,stepline);
  parcmd();

	/* Restore original command */
  memset(cmdbuf,0,CMDBUFSIZ);
  strcpy(cmdbuf,dummybuf);
}

void pascal filecmd(byte ctype)
{
	byte stch;

	if (ctype!=FSTRT)
	{
		if (scrbuf!=NULL) scrbufptr=scrbuf;
		else if ( (scrfile=fopen(scrnameptr,"r"))==NULL )
		{
			switch (ctype)
			{
				case FMAIN:
					DROPDTR();
					def_attr=0x07;
					birite("\2\1\1\fUnable to open script: \3");
					birite(scrnameptr);
					cleanexit(200);
				case FEXIT: return;
				default: errlist(BADFILE,scrnameptr);
			}
		}
		dispstate=CMDBAT;
		setllens();
	}
	dispstate=CMDBAT;

	if (lfptr!=NULL) fprintf(lfptr,"\r\nScript: %s\r\n",scrnameptr);

	memset(cmdbuf,0,200);
	cmdline=0;

	if (ctype==FEXIT && *exfile==':')
	{
		PRAM prs;
		prs.evaltype=LABEL;
		prs.pram=exfile;
		prs.addr=exfile;
		dogo(&prs);
		*exfile=0;
	}

	while (!sfeof())
	{
		ei=setjmp(env);
		if (ei!=0) { ep=ei; el=cmdline; }
		if (dosup==5) dosup=0;

		if (sgets(cmdbuf)) { if (ctype==FMAIN) doex(); break; }
		nocr(cmdbuf);
		*strchr(cmdbuf,'\t')=0;
		cmdline++;

		stch='@';
		switch (cmdbuf[0])
		{
			case '!': if (!dosup) dosup=5; stch='!';
			case '@': strcpy(&cmdbuf[0],&cmdbuf[1]);
				if (dispstate!=CMDINT && lfptr!=NULL)
				{
					if (wx()!=1) fprintf(lfptr,"\r\n");
					fprintf(lfptr,"%.3d %s",cmdline,prompt);
					promplen=col;
					fprintf(lfptr,"%c%s\r\n",stch,cmdbuf);
				}
				break;
			default: promp(); dputs("%s\r\n",cmdbuf);
		}

		updatedisp();
		
		if (step) 
		{ 
			byte olro,olcol;
			olro=ro; olcol=col;
			form("\2\1\1\x1D\x50W Script=%s, Line=%d: %s",scrnameptr,cmdline,cmdbuf);
			if (stepline!=NULL) runstepline();
			bioskey(0); 
			form("\2\1\1R");
			ro=olro; col=olcol;
		}
		
		parcmd();
	}

	if (lfptr!=NULL)
	{
		if (wx()!=1) fprintf(lfptr,"\r\n");
		fprintf(lfptr,"Leaving Script: %s\r\n",scrnameptr);
	}

	clrca();
	if (labtable!=NULL) { free(labtable); labtable=llens=NULL; }
	sclose();
}

void pascal reperror(void)
{
	errlist(BADCMD);
}

void pascal stripblanks(char *st)
{
	char *st0,*st1;

	while (*st)
	{
		if (*st=='"')
		{
			st++;
			while (*st!=0)
			{
				if (*st=='"')
				{
					if (*(--st)=='\\') { st+=2; continue; }
					st+=2;
					break;
				}
				st++;
			}
			if (*st==0) break;
		}

		for (st1=st;*st1==' ';st1++); /* advance st1 across blanks. */

		/* Copy string backward, can't rely on strcpy since there is overlap. */
		if (st1!=st)
		{
			st0=st;
			while (*st0) *st0++=*st1++;
			continue;
		}
		st++;
	}
	/* Put additional zeros at end of string. */
	*st++=0; *st++=0; *st++=0;
	return;
};

void pascal dummy(void)
{
	/* This is used to set point function pointers to when I want them to
	be suppressed. */
}