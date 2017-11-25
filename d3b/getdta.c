#include <alloc.h>
#include <bios.h>
#include <conio.h>
#include <ctype.h>
#include <dos.h>
#include <stdlib.h>
#include <string.h>

#include "include\d3.h"

static boolean pascal getint(boolean checkall);
static boolean pascal chkmodem(void);
static boolean pascal getbase(boolean checkall);

#define SAFCPY(std,sts) memmove(std,sts,strlen(sts)+1);
#define DSV()			((inportb(contsc) & 0x08) != 0)
#define IODSV()		((inportb(dsrsc) & 0x02) == 0)

boolean pascal chkline(boolean dsr)
{
	if (dsr)
	{
		if (!mdminst) return FALSE;
		if (ioc==3 || dialing) return TRUE;
		if (ioc==1) return IODSV();
		else return DSV();
	}	

	if (!mdminst) return TRUE;
	if (ioc==3 || dialing) return FALSE;
	if (ioc==1) return !IODSV();
	else return !DSV();
}

static int offs[4][6]=
{/*	 A		 B	 cont	 data		dsr	 talk */
	{	0x9,	0x8,	0x9,	0xB,	0x9,	0xD }, /* ioc=0-Standard Sync-Up		*/
	{ 0x1,	0x0,	0x1,	0x3,	0x8,	0x1 }, /* ioc=1-GMM Sync Card				*/
	{ 0x9,	0x8,	0x9,	0xB,	0x9,	0xD }, /* ioc=2-Multyprotocol Card	*/
	{ 0x9,	0x8,	0x8,	0xA,	0x8,	0xC }  /* ioc=3-Sync-Up DTE port		*/
};

static void pascal setbase(void)
{
	int *adds;
	adds=offs[ioc];
	aportsc	=base_address+adds[0];
	bportsc	=base_address+adds[1];
	contsc	=base_address+adds[2];
	datasc	=base_address+adds[3];
	dsrsc		=base_address+adds[4];
	talksc	=base_address+adds[5];
}

uint pascal ahextoi(char *asciihex)
{
  char *stupid;
  return ((uint) strtol(asciihex,&stupid,16)) ;
}

char * pascal convrt(char *str,uint max)
{
  char *st,*endptr,ch,indsave;

  st=str;
  str=&str[max];

  if (*st=='"') SAFCPY(st,&st[1]);
  while (st<str)
  {
    switch (*st)
    {
      case '\\':
        switch (st[1])
        {
          case '0': case '1': case '2': case '3': case '4':
          case '5': case '6': case '7': case '8': case '9': 

          case '-': case '+':
            indsave=(st[1]=='+' || st[1]=='-') ? 5 : 4;
            ch=st[indsave];st[indsave]=0;
            *st=(char) strtol(&st[1],&endptr,10);
            st[indsave]=ch;
            st++; SAFCPY(st,endptr);
            continue;
          case 'x': case 'X':
            ch=st[4]; st[4]=0;
            *((byte *) st)=(byte) strtol(&st[2],&endptr,16);
            st[4]=ch;
            st++; SAFCPY(st,endptr);
            continue;
          case 'a': case 'A': st[1]='\a'; break;
          case 'b': case 'B': st[1]='\b'; break;
          case 'f': case 'F': st[1]='\f'; break;
          case 'n': case 'N': st[1]='\n'; break;
          case 'r': case 'R': st[1]='\r'; break;
          case 't': case 'T': st[1]='\t'; break;
          case 'v': case 'V': st[1]='\v'; break;
        }
        SAFCPY(st,&st[1]);
        break;
      case CR: case LF: case '"': case 0: return st;
    }
    st++;
  }
  return NULL;
}

void pascal mseth(char *cmd)
{
  PRAM prs[2];
  char *next;

  if (*cmd==0)
  {
    VALUE(0)=base_address;
    VALUE(1)=intrupt;
  }
  else
  {
    next=getscal(cmd,&prs[0],1,LOWVAL,HIGHVAL);
    getscal(next,&prs[1],1,2,5);
  }

  if (mdminst)
  {
    outscc(9,0xC0);
    setvect(intrupt+8,int_prev); /* set previous interrupt vector back. */
    outportb(0x21,startmask);
  }
  base_address=VALUE(0);
  intrupt=VALUE(1);
  int_prev=getvect(intrupt+8); /* get previous interrupt vector */

  verimodem(FALSE);
	
  initialize();
  instint();
  if (NODSR()) { statval=OFFLIN; sync=0x16; } 
  else { statval=ONLIN; sync=0x32; } 
  outscc(6,sync); outscc(7,sync);
  mdminst=TRUE;
  modata=INITVAL;
}

void pascal getdiskdta(void)
{
  if ((scrfile=fopen(cfgfilname,"r"))==NULL) return;
  dosup=TRUE;
	scrnameptr=cfgfilname;
  filecmd(FSTRT);
	scrnameptr=scrname;
}

/* -------------------------------------------------------------------------
These functions are for determining the interrupt level.
------------------------------------------------------------------------- */

static boolean pascal getint(boolean checkall)
{
  byte intmask,i;

  setseg();

  startmask=inportb(0x21);
  /* get previous interrupt vectors */
  for(i=0;i<4;i++) int_prevs[i]=getvect(i+10);

	outscc(5,0xe4);
  outscc(12,0x60);
  outscc(13,0x00);
  outscc(14,0x03);
  outscc(15,0x02);    /* Disable all ext. status interrupts except CTS */
  outportb(contsc,0x10); /* Reset any external status ints twice.*/
  outportb(contsc,0x10);
  outscc(1,0x00);     /* Enable External status interrupts. */
  outscc(9,0x09);     /* Enable master interrupt. */

  whichint=0;

  intmask=~(0x01 << intrupt);    /* Disable all ints except expected one.*/
  disable(); outportb(0x21,intmask); enable();

  setvect(intrupt+8,int_detect);    /* install interrupt vector */
  outscc(1,0x01);     /* Enable External status interrupts. */

  outscc(1,0x00);
  setvect(intrupt+8,int_prevs[intrupt-2]); /* set previous interrupt vector back. */
  outportb(0x21,startmask);

  if (whichint==1) return TRUE;
	if (!checkall) return FALSE;

  whichint=0xff;

  intmask=0xc3;       /* Disable 0,1,6,7; Enable 2,3,4,5 */
  disable(); outportb(0x21,intmask); enable();
  for(i=0;i<4;i++) setvect(i+10,int_mine[i]); /* put my interrupt vectors in. */
  outscc(1,0x01);

  outscc(1,0x00); outscc(14,0x00);
  for(i=0;i<4;i++) setvect(i+10,int_prevs[i]); /* set previous interrupt vector back. */
  outportb(0x21,startmask);

  if (whichint==0xff) { outscc(9,0xc0); return FALSE; }

  intrupt=whichint;

  return TRUE;
}

/* -------------------------------------------------------------------------
These functions are for determining the base address.
------------------------------------------------------------------------- */

static boolean pascal chkmodem(void)
{
  int reg;  /* used to read port data into */

  outscc(15,0xAA);
  reg=inscc(15);
  if (reg!=0xAA) return FALSE;

  outscc(15,0);
  reg=inscc(15);
  if (reg!=0x00) return FALSE;

  return TRUE;
}

static boolean pascal getbase(boolean checkall)
{
  byte i;
  setbase();
  if (chkmodem()) return TRUE;
	if (!checkall) return FALSE;
  for(i=0;i<4;i++)
  {
    base_address=def_addrs[i];
    setbase();
    if (chkmodem()) return TRUE;
  }
  return FALSE;
}

void pascal verimodem(boolean checkall)
{
  if (!getbase(checkall)) errlist(NOMODEM);
  if (!getint(checkall)) errlist(NOINTRUPT);
}
