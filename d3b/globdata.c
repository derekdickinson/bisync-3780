#include <conio.h>
#include <dos.h>

#include "include\d3.h"

#include "include\commands.h"
#include "include\dispvars.h"
#include "include\ragstrs.h"
#include "include\scrns.h"
#include "include\tables.h"
#include "include\uservars.h"

boolean as=TRUE;
boolean co=TRUE;
boolean comsend=FALSE;
boolean destsent=FALSE;
boolean dce=FALSE;
boolean dialing=FALSE;
boolean firstblock=TRUE;
boolean fixedblanks=FALSE;
boolean hasp=FALSE;
boolean modata=0;
boolean pb=FALSE;
boolean remwaiting=FALSE;
boolean remcmds=TRUE;
boolean sendmess=FALSE;
boolean sendrsp=FALSE;
boolean skipone=FALSE;
boolean std;
boolean tb=TRUE;
boolean vff=TRUE;
boolean vfs=TRUE;

byte buftype=VARY;
byte cmend=ENQ;
byte ioc=0;
byte lastdest=NONEYET;
byte lastrcv=STANTYPE;
byte naklmt=10;
byte numcmds=sizeof(cmds)/sizeof(COMMAND);
byte numdisps=sizeof(dv)/sizeof(DISPVAR);
byte prindest=PRINTER;
byte rcvtype=0;
byte sync=0x32;

byte defbytes[MAXBYTES];
byte rcvends[2]={0,0};
byte sendends[2]={0,0};
byte strtstate[]={ STANSEND,TRANSEND };

char *crlf="\r\n";
char *eschar="\x61\xE2\xE3\xC1\xC2";
char *messptr;
char *remptr=NULL;

char messtring[]              ="°±²Û Message Û²±°";
char moname[30]               ="Unavailable";
char remcmdstring[]           ="°±²Û Command Û²±°";

uint aportsc=0;
uint base_address=0x300;
uint bportsc=0;
uint contsc=0;
uint datasc=0;
uint dsrsc=0;
uint idllmt=60*18+60/5;
uint memleft;
uint natolmt=0;
uint preclen=80;
uint remver=0;
uint renqcnt=0;
uint rnakcnt=0;
uint rreclen=80;
uint senqcnt=0;
uint snakcnt=0;
uint strtenq;
uint strtnak;
uint talksc=0;

uint def_addrs[]={ 0x300, 0x310, 0x340, 0x360 };
uint mandsums[sizeof(cmds)/sizeof(COMMAND)];

ulng beltimeout=8*18+8/5;
ulng sendtime;
ulng strtime;

void interrupt (*int_mine[])(void)={ int2, int3, int4, int5 };
void interrupt (*int_prevs[4])(void);

volatile byte ctrlbyt=0;
volatile byte error=0;
volatile byte rxtyp;

BUFPTR ptrbuf={ FALSE, NULL };
BUFPTR punbuf={ FALSE, NULL };
BUFPTR rbuf  ={ FALSE, NULL };

DEST ptr={"DATARECV",NORMTYP,VARY,BINARY,TRUE,NULL};
DEST pun={"CON",CONTYP,VARY,BINARY,TRUE,NULL};

DEST *act=&ptr;

DISPVAR *cnvspos[]= {
  &dv[PTRSTDDV], &dv[PTRTRANDV], &dv[PUNSTDDV], &dv[PUNTRANDV]
};

DISPVAR *namepos[]={
  &dv[PTRNAMEDV], &dv[PUNNAMEDV], &dv[SNDNAMEDV]
};

DISPVAR *updisps[]={
&dv[BUFDV],        &dv[DIRDV],         &dv[INDV],          &dv[OUTDV],
&dv[PTRAUTODV],    &dv[PTRNAMEDV],     &dv[PTRSTDDV],      &dv[PTRTRANDV],
&dv[PUNAUTODV],    &dv[PUNNAMEDV],     &dv[PUNSTDDV],      &dv[PUNTRANDV],
&dv[RBLKDV],       &dv[RENQDV],        &dv[RNAKDV],        &dv[SBLKDV],
&dv[ACTPRINDV],    &dv[SNDNAMEDV],     &dv[SENQDV],        &dv[SNAKDV],
&dv[SRVARDV],      &dv[STATMDV],       &dv[STDV],          NULL
};

FILE *sendfile;

LITSTR ack0={ 2, { 0x10, 0x70} };
LITSTR ack1={ 2, { 0x10, 0x61} };
LITSTR bel ={ 1, { 0x2f      } };
LITSTR disc={ 2, { 0x10, 0x37} };
LITSTR enq ={ 1, { 0x2D      } };
LITSTR eot ={ 1, { 0x37      } };
LITSTR nak ={ 1, { 0x3d      } };
LITSTR rvi ={ 2, { 0x10, 0x7c} };
LITSTR ttd ={ 2, { 0x02, 0x2d} };
LITSTR wack={ 2, { 0x10, 0x6b} };

LITSTR *ctrlptr=NULL;

LITSTR *ackptr[]={ &ack0, &ack1 };

MODEMSTRUCT themodem={ "Unavailable","",0xff};

SND *sndlist=NULL;

