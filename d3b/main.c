#include <bios.h>
#include <conio.h>
#include <ctype.h>
#include <dos.h>
#include <dir.h>
#include <io.h>
#include <stdlib.h>
#include <string.h>

#include "include\d3.h"

static void pascal initdata(void);
static void cdecl endstuff(void);
static boolean setcfgname(char *st);
static boolean setscrname(void);

static void cdecl endstuff(void)
{
  if (mdminst) outscc(1,0x00); /* Disable interrupts */
  fcloseall();
  cleanup();
  if (dce) outportb(base_address,0);
}

int pascal nextname(char lastname[])
{
	struct ffblk ffblk;
  char *lastoccur;
  int num;

  if ((lastoccur=strrchr(lastname,'.'))==NULL) 
	{
		strcat(lastname,".000");
		lastoccur=strrchr(lastname,'.')+1;
		num=-1;
	}	
  else
  {
    lastoccur++;
    num=atoi(lastoccur);
  }
	
	do
	{
    if (++num>999) num=0;
	  sprintf(lastoccur,"%03d",num);
	} while (findfirst(lastname,&ffblk,0)==0);
	
  return num;
}

static void pascal initdata(void)
{
  uint *sumptr;
  COMMAND *cmdptr;

  for (sumptr=mandsums,cmdptr=cmds; cmdptr->func!=(CMDF *)reperror; cmdptr++)
    *sumptr++=chkcrc(cmdptr->name);
  *sumptr=0xffff;
}

static boolean setcfgname(char *st)
{
  struct ffblk ffblk;

  if (findfirst(st,&ffblk,0)==0) { strcpy(cfgfilname,st); return TRUE; }

	sprintf(cfgfilname,"%s\\%s",emudir,st);
	if (findfirst(cfgfilname,&ffblk,0)!=0) return FALSE;
  
	return TRUE;
}

static boolean setscrname(void)
{
  struct ffblk ffblk;
	char st[15];

	if (*strtscr==0) sprintf(scrname,"%s.SCR",packname);
	else sprintf(scrname,strtscr);

  if (findfirst(scrname,&ffblk,0)==0) return TRUE;

	strcpy(st,scrname);
	sprintf(scrname,"%s\\%s",emudir,st);
	if (findfirst(scrname,&ffblk,0)!=0) return FALSE;
  
	return TRUE;
}

#define UI(p) *( (uint *) (p) )

static void setenvp(void)
{
  USERV *uvptr;
  uvptr=uv;
  while (UI(uvptr->name)!=UI("EV")) uvptr++;
  uvptr->addr=environ;
}

void cdecl main(int argc,char *argv[])
{
	memset(abuf,0,ABUFSIZE);
	setenvp();

  for (argcg=0; argcg<argc; argcg++)
	{
		argvg[argcg+1]=stpcpy(argvg[argcg],argv[argcg])+1;
		if (argcg==9) { argcg++; break; }
	}

  if ( ((byte) _version)==2 )
  {
    sprintf(emudir,"%s.EXE",packname);
    strcpy(emudir,searchpath(emudir));
  }
  else 
	{
		strcpy(emudir,argvg[0]);
		strcpy(packname,strrchr(argvg[0],'\\')+1);
		*strrchr(packname,'.')=0;
		if (*strtname==0) sprintf(strtname,"%s.CFG",packname);
	}
  *strrchr(emudir,'\\')=0; /* Note: there is already a 0 at NULL */
	setscrname();

  getvidmode();
  getrocol(&curro,&curcol);
  setseg();
  setup();
  initdata();
	nextname(act->filename);
  atexit(endstuff);

  getcwd(curdir,FILENAMESIZE);

	setcfgname(strtname);
	getdiskdta();
  if (!mdminst)
  {
	  int_prev=getvect(intrupt+8); /* Save previous interrupt vector */
		if ((mdmerr=setjmp(env))==0)
		{
	    verimodem(TRUE);
		  initialize();
	    instint();
		  mdminst=TRUE;
			if (NODSR()) 
	    {
		    statval=OFFLIN; 
				sync=0x16;
	    } 
			else 
	    {
		    statval=ONLIN;
				sync=0x32;
	    } 
		  outscc(6,sync);
			outscc(7,sync);
		}
  }

  switch (usrmode)
  {
    case 0xFFFE:printf(copyright); bioskey(0);
		default:		concmd();
    case 0xFFFF:filecmd(FMAIN);	break;
  }
	doex();
  cleanexit(0);
}
