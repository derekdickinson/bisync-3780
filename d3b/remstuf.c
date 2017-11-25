#include <dir.h>
#include <io.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#include "include\d3.h"

static char * pascal runtrp(char *bufptr)
{
  byte len;
	char lenbuf[3];
	char olcmdbuf[CMDBUFSIZ];

	strncpy(lenbuf,bufptr,2); lenbuf[2]=0; len=atoi(lenbuf); bufptr+=2;

  memcpy(olcmdbuf,cmdbuf,CMDBUFSIZ);
  memset(cmdbuf,0,CMDBUFSIZ);
	strncpy(cmdbuf,bufptr,len); bufptr+=len;

	disp("\r\n°±²Û Received Remote command: %s Û²±°\r\n",cmdbuf);
  if (remcmds) parcmd();
	else disp("Command Ignored\r\n");
	memcpy(cmdbuf,olcmdbuf,CMDBUFSIZ);
  return bufptr;
}

void pascal setremcmd(void)
{
  uint i;
  char *bufptr;

  bufptr=bufs[curbuf].st;

  for(i=0; i<bufs[curbuf].len; i++) (*bufptr++)-=0x40;

	remptr=malloc(bufs[curbuf].len);

	memcpy(remptr,bufs[curbuf].st,bufs[curbuf].len);

	remwaiting=TRUE;
}

void pascal proremcmd(void)
{
	char *bufptr,commandstr[5];
  byte olkeystate;
  uint command;
  boolean done=FALSE;

	if (!remwaiting) return;

  olkeystate=keystate;
  if (keystate==INCMD) keystate=INREMCMD;

	bufptr=&remptr[4];
	strncpy(commandstr,bufptr,4); bufptr+=4;
	commandstr[4]=0;       /* Terminate the string for atoi */
	remver=atoi(commandstr);

  while (!done)
  {
    strncpy(commandstr,bufptr,4); bufptr+=4;
    if (STRLCMP("ZzZz",commandstr)) break;
    command=atoi(commandstr);
		if (command==2) bufptr=runtrp(bufptr);
		else done=TRUE;
  }
  free(remptr);
  remptr=NULL;
  remwaiting=FALSE;
  keystate=olkeystate;
  switch (keystate)
  {
    case INCMD:
      promp();
      edro=wy()+wintop-1;
      { byte dummy=edcur; edcur=0; edrite(); edcur=dummy; }
      gorocol(edro,edcur+edcol);
      break;
    case ATPROMPT:
    case DISPING:
      promp();
      keystate=ATPROMPT;
  }
}

static char *devs[]={	"LPT1",	"LPT2",	"PRN",	"CON", "NUL", "COM1","COM2",NULL };
static char types[]={ LPT1TYP,LPT2TYP,LPT1TYP,CONTYP,SPCTYP,SPCTYP,SPCTYP,NORMTYP };

void pascal setdestype(DEST *what)
{
	char i;
	
  strupr(what->filename);
	
	for (i=0;devs[i]!=NULL;i++) if (strcmp(what->filename,devs[i])==0) break;
	what->destyp=types[i];
  if (what->destyp!=NORMTYP) what->autoname=FALSE;
}