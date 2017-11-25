#include <bios.h>
#include <conio.h>
#include <dir.h>
#include <dos.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "include\d3.h"

void pascal dosit(char *dosbuf)
{
	char thebuf[CMDBUFSIZ];
	int errval;

	strncpy(thebuf,dosbuf,CMDBUFSIZ);

	dostype=dosbuf[1];
	
	if (dispstate==CMDINT) xb(" %di\0\0");
	else call(" %di\0\0");

	if (system(&thebuf[1])==-1) errval=errno; else errval=0;

	dostype=thebuf[0];

	getcwd(curdir,FILENAMESIZE);

	if (dispstate==CMDINT) xb(" %do\0\0");
	else call(" %do\0\0");
	
	switch (errval)
	{
		case 0: break;
		case E2BIG:	errlist(ARGOVER);	break;
		case ENOMEM:errlist(NOINTMEM);break;
		default: errlist(64,errval);	break;
	}
}

void pascal intrp(char *cmd)
{
  char dummybuf[CMDBUFSIZ];
  PRAM prs;

  memset(dummybuf,0,CMDBUFSIZ);

  getpoint(cmd,&prs);
  strcpy(dummybuf,prs.addr);
  memset(cmdbuf,0,CMDBUFSIZ);
  strcpy(cmdbuf,dummybuf);
  parcmd();
}

void pascal pa(char *cmd)
{
  pause(cmd,FALSE);
}

void pascal redir(char *cmd)
{
  disp("This function is not supported yet");
  *cmd=0;
}