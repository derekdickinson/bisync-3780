#include <conio.h>
#include <dos.h>

#include "include\d3.h"

char instdata[]=
"®®¯¯{{„„"
"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C"
"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C"
"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C"
"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C";

boolean dorite;
boolean hidecur=FALSE;
boolean insmode;
boolean mb=TRUE;
boolean mdminst=FALSE;
boolean redisplay=TRUE;
boolean srd;
boolean step=FALSE;

byte actat=0x14;
byte baklmt=10;
byte dimat=0x13;
byte dispstate=STARTING;
byte dosup=FALSE;
byte curcol;
byte curro;
byte edcol;
byte edcur;
byte edln;
byte edro;
byte enqlmt=10;
byte hour;
byte intrupt=3;
byte key_attr=0x0E;
byte lenstr;
byte mandind;
byte mdmerr=0;
byte minute;
byte promplen=2;
byte second;
byte startmask;
byte statval=ONLIN;
byte themask=1;
byte win_attr=0x07;
byte winbottom=25;
byte winleft=1;
byte winright=80;
byte wintop=1;
byte whichint=0;
byte vx=1;
byte vy=1;

char *db=NULL;
char *dosgo=NULL;
char *dosreturn=NULL;
char *edst;
char *exfile=NULL;
char *helpstrt=&instdata[21];
char *labtable=NULL;
char *llens=NULL;
char *packname=&instdata[8];
char *scrbuf=NULL;
char *scrbufptr=NULL;
char *scrnameptr=&instdata[34];
char *stepline=NULL;
char *strtname=&instdata[34];
char *strtscr=&instdata[47];

char abuf[ABUFSIZE];
char cfgfilname[FILENAMESIZE];
char scrname[FILENAMESIZE];
char cmdbuf[CMDBUFSIZ];
char copyright[]              ="Copyright (c) D3 Softdesign 1989,1990";
char curdir[FILENAMESIZE];
char dostype;
char emudir[FILENAMESIZE];
char errmessbuf[100];
char lascmd[CMDBUFSIZ];
char lfname[FILENAMESIZE]     ="NUL";
char prompt[30]               ="?";
char senfilname[FILENAMESIZE] ="°±²Û No File Û²±°";

char *argvg[11]={ abuf,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, NULL };
char *p[10];
char *s[10];

char **messptrs[30];

char * cdecl (*rite)(char *st)=dirite;
char * cdecl (*dsrit)(char *st)=(char * cdecl (*)(char *st)) dummy;
char * cdecl (*svform)(char *st,...)=(char * cdecl (*)(char *st,...)) dummy;

int ei;
int el;
int ep;

int g[10];
int i[10];

int *gptr=g;
int *iptr=i;

jmp_buf env;

uint argcg;
uint blocksiz=512;
uint cmdline;
uint col=1;
uint curbuf=1;
uint curlines;
uint f_state=IDLE;
uint keystate=NORM;
uint loopvar=0;
uint over_char;
uint rblkcnt=0;
uint ro=1;
uint sblkcnt=0;
uint usrmode=0;

uint def_overs[]={ 0x011B, 0x1C0D, 0};
uint *over_chars=def_overs;

uint *n[20]=
{ 
	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL
};

ulng datain=0;
ulng dataout=0;
ulng timeout=1*18;

ulng far *nowtime=MK_FP(0,0x046c); /* The systems clocks location in low memory. */

void interrupt (*old_break)(void);
void interrupt (*int_prev)(void);

volatile uint rxstate=0;
volatile uint txstate=0;

BIGSTR *inptr=&bufs[0];
BIGSTR *outptr=&bufs[0];

BIGSTR bufs[2];

CALLRET *calls=NULL;

FILE *scrfile;
FILE *lfptr=NULL;

VSTF *put=(VSTF *) myputs;
VVF *prin=(VVF *) myprintf;

struct date thedate;
