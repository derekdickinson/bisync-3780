#include <dos.h>
#include <conio.h>
#include <ctype.h>
#include <bios.h>
#include <string.h>
#include <stdlib.h>

#include "include\d3.h"

static byte pascal mrcv(char *st,uint ticks);
static void pascal msend(char *st,byte len);
static boolean pascal getent(char *pram,char *buffer);

/**************************************************************************/

void pascal getmodfil(void)
{
  FILE *modemfile;
  char mvers[50];
	register byte len; 
	register byte olsup;
	MODEMSTRUCT modembuf;
	OPSCREN dummyscreen;
	jmp_buf locenv;

	if (!mdminst || ioc || (modata!=INITVAL && modata!=LINEUP) ) return;

  if (!NODSR()) { if (modata==INITVAL) modata=LINEUP; return; }

  olsup=dosup;
  dosup=TRUE;
  memcpy(locenv,env,sizeof(jmp_buf));
  if (setjmp(env)!=0)
  {
    memcpy(env,locenv,sizeof(jmp_buf));
		initialize();
		f_state=BEGINRCV;
    modata=INITVAL;
    dosup=olsup;
		return;
  }
	if (rxstate==4) rxstate=0;
	msend("V",1);
  len=mrcv(mvers,1000);
	f_state=BEGINRCV;
  memcpy(env,locenv,sizeof(jmp_buf));
  dosup=olsup;

	themodem.version=mvers[0];

  initialize(); /* Restore the 8530 back to interrupt mode. */

  if (len!=4) { modata=CANTTALK; return; }

  /* Read Data in on all the modems */
  if ( (modemfile=fopen("MODEMS.DTA","rb")) == NULL)
  {
		char dir2[FILENAMESIZE];
		sprintf(dir2,"%s\\MODEMS.DTA",emudir);
    if ( (modemfile=fopen(dir2,"rb")) == NULL)
			{ modata=NOMODATA; return; }
	}

  while (TRUE) /* while relevant modems are being read in */
  {
    fread(&modembuf,sizeof(MODEMSTRUCT),1,modemfile);
    if ( strcmp(modembuf.name,"ððNO MORE MODEMSðð") == 0 )
      { modata=BADVER; break; }

    if (modembuf.version==mvers[0])
    {
      themodem=modembuf;
      modata=GOODSTUF;
      sprintf(moname,"Sync-Up %s",themodem.selval);
      break;
    }
  }

  fseek(modemfile,themodem.seekscrn[0],SEEK_SET);
  fread( (char *) &dummyscreen, 1, (uint) sizeof(OPSCREN), modemfile);
  memmove(defbytes,dummyscreen.defbytes,MAXBYTES);

  fclose(modemfile);
}  /* end getmodfile */

static void pascal msend(char *st,byte len)
{
  sync=0x16;
	outscc(6,sync);
	outscc(7,sync);

  /* Reset Modem */
  outscc(5,0x65); /* Drop DTR, wait 8ms, Raise DTR */
  delay(8);
  outscc(5,0xe5);

	memcpy(bufs[0].st,st,len);
  bufs[0].len=len;

  sendrsp=FALSE;
  curbuf=0;
  outptr=&bufs[0];
  sendends[0]=sendends[1]=ETX;
  txstate=STANSEND;
  send();
}

static byte pascal mrcv(char *st,uint ticks)
{
  byte len;  /* String index. */
	ulng abrtime;

  inptr=&bufs[1];
  error=0;

	abrtime=*nowtime+100L; /* No more than five seconds to send. */
  while ( txstate!=0 ) 
	{
	  if (ABRTE()) errlist(USRABRT);
    if (*nowtime>abrtime) errlist(NORESPNS);
  }
	rxstate=16;

  outscc(1,0x10);    /* Enable RX ints */
  outscc(3,0xD1);    /* RX enable and hunt*/

  abrtime=*nowtime+ticks;
  while (rxstate==16)
  {
    if (ABRTE()) errlist(USRABRT);
		if (*nowtime>abrtime) break; 
  }
	
  abrtime=*nowtime+100;  /* Message itself should take no more than 5 sec. */
  while (rxstate!=0)
  {
    if (ABRTE()) errlist(USRABRT);
    if (*nowtime>abrtime) break; 
  }
	
  if (error) { outscc(3,0xC0); rxstate=0; return 0; }
	
	len=bufs[1].len;
  memcpy(st,bufs[1].st,len);

  while (len>0 && st[len]!=ETX) len--;
  if ((byte)st[len+1]==0xff && st[len-2]==ETX) len-=2; /* if second byte of CRC==ETX. */
  if ((byte)st[len+2]==0xff && st[len-1]==ETX) len-=1; /* if first byte of CRC==ETX. */

  outscc(3,0xC0);  /* Disable RX. */

	rxstate=0;
  return len;
} 

/****************************************************************************/

void pascal atest(void)
{
	outscc(9,0xc0);               /* force reset*/
	delay(6);
	outscc(4,0x0C);               /* async mode*/
	outscc(3,0xc0);               /* rcv 8 bit characters*/
	outscc(5,0x60);               /* tx 8 bit characters*/
	outscc(9,0x00);               /* master interrupt disabled*/
	outscc(10,0x00);              /* nrz coding*/
	outscc(11,0x05);              /* set up clock pins*/
	outscc(12,0x01);              /* time constant - low*/
	outscc(13,0x00);              /* time constant - high*/
	outscc(14,0x00);              /* BR disable, DTR normal*/

}

void pascal aa(char *timeout)
{
  if (!NODSR()) errlist(CANTON);

  disp("The modem is now ready to answer\r\n");
  statval=ANSWERING;
  outscc(0x05,0xe5); /* Raise Dtr */
  curbuf=1;

  if (dispstate==CMDBAT)
  {
    if (*timeout!=0) { pause(timeout,TRUE); return; }
    while (TRUE)
    {
      if (!NODSR()) { disp("The modem is online\r\n"); break; }
      if (ABRTE()) errlist(USRABRT);
      updatedisp();
    }
  }
}

void pascal c(void)
{
  char sendbytes[MAXBYTES];
  ulng dsrtime;

  if (themodem.numbytes==0) errlist(MOOPTS);
  if (!NODSR()) errlist(CANTON);

  memcpy((byte *)sendbytes,defbytes,themodem.numbytes);

  sendbytes[themodem.privbyte]&=themodem.privmaskand;
  sendbytes[themodem.privbyte]|=themodem.privmaskor;

  bufs[0].st[0]='S';
  bufs[0].st[1]=0x30+themodem.numbytes;
  bufs[0].len=themodem.numbytes+2;
  memcpy(&bufs[0].st[2],sendbytes,themodem.numbytes);

  outscc(5,0x65);
  delay(8);

  outscc(5,0xe5);
  curbuf=0;
  outptr=&bufs[0];
  sendends[0]=sendends[1]=ETX;
  inptr=&bufs[1];
  txstate=STANSEND;
  send();

  disp("The modem is now trying to connect (press Esc to abort)\r\n");
  statval=CONNECTING;

  dsrtime=*nowtime+1092;
  while (NODSR())
  {
    if (ABRTE()) 
    { 
      disp("Unable to connect\r\n"); 
      errlist(USRABRT); 
    }
    if (*nowtime>dsrtime) 
    {
      disp("Unable to connect\r\n");
      errlist(DSROUT);
    }
    updatedisp();
  }

  disp("Connected\r\n");
  statval=ONLIN;
  curbuf=1;
  sync=0x32;
  outscc(6,0x32);
  outscc(7,0x32);
}

#define ST(p)  ( (char *) (p))

static boolean pascal getent(char *pram,char *buffer)
{
  PRAM prs;
  FILE *numbfile;
  PHENT entry;

  memset(buffer,0,99);

  if (!ioc && !NODSR()) errlist(CANTON);

  *pramset(pram,&prs)=0;

  switch (prs.evaltype)
  {
    case NONE:    errlist(MUSTPRM,1);
    case LABEL:   errlist(NOLABEL,prs.pram);
    case POINTER: strcpy(buffer,prs.addr); return TRUE;
    case SCALAR:
      if ((numbfile=fopen("numbers.cfg","rb"))==NULL)
        errlist(BADFILE,"NUMBERS.CFG");
      fseek(numbfile,(prs.value-1)*sizeof(PHENT),SEEK_SET);
      fread(&entry,sizeof(PHENT),1,numbfile);
      fclose(numbfile);

      if (mandind==DLI) strcpy(buffer,entry.phone_num);
      else memcpy(buffer,entry.opbytes,MAXBYTES);
      return FALSE;

    default: /* ISFILE */
      if ( ST(prs.addr)[0]=='|')
      {
        if ((numbfile=fopen("numbers.cfg","rb"))==NULL)
          errlist(BADFILE,"NUMBERS.CFG");
        while (fread(&entry,sizeof(PHENT),1,numbfile)!=0)
        {
          if (STRLCMP(&ST(prs.addr)[1],entry.name))
          {
            if (mandind==DLI) strcpy(buffer,entry.phone_num);
            else memcpy(buffer,entry.opbytes,MAXBYTES);
            break;
          }
        }
        fclose(numbfile);
        if (!STRLCMP(&ST(prs.addr)[1],entry.name))
          errlist(ENTNO,&ST(prs.addr)[1]);
        return FALSE;
      }
      else
      {
        if (strpbrk(prs.addr,"*?")!=NULL ) errlist(NOSPECS);
        if ((numbfile=fopen(prs.addr,"r"))==NULL) errlist(BADFILE,prs.addr);
        fgets(buffer,99,numbfile);
        fclose(numbfile);
				nocr(buffer);
				if (buffer[0]=='"') 
				{
					memcpy(buffer,&buffer[0],98);
					*strrchr(buffer,'"')=0;
				}
        return TRUE;
      }
  }
}

void pascal dl(char *number)
{
  boolean done=FALSE;
  unsigned long dsrtime;
  char buffer[100],ch,*cp;
	byte olbtype;
	DEST temp;
	uint error=0;

  getent(number,buffer);
	sprintf(bufs[0].st,"D%s",buffer);

	memcpy(&temp,&ptr,sizeof(DEST)); /* Only used if ioc!=0 */
	outscc(5,0x65); delay(8); outscc(5,0xe5);
	statval=DIALING;

  /* Dial, display responses and go back to command when needed. */
	if (ioc)
	{
		olbtype=buftype;
		strcpy(buffer,bufs[0].st); 
		for (cp=buffer; *cp!=0; cp++) *cp=sentable[*cp];
		if ((sndlist=malloc(sizeof(SND)))==NULL) errlist(NOINTMEM);
		sndlist->sensrc=MESBUF;
		sndlist->mesbuf=buffer;
		sndlist->prindest=PRINTER;
		sndlist->buftype=STAN|BINARY;
		sndlist->catype=ETX;
		sndlist->nextsnd=NULL;
		dialing=TRUE;
		listsend();
		freesndlist();
		mandind=DPI;
		defdest(" CON,SV");
		mandind=DLI;
		buftype=olbtype;
	}
	else
	{
		bufs[0].len=strlen(bufs[0].st); sendrsp=TRUE; curbuf=0;
		outptr=&bufs[0]; inptr=&bufs[1]; sendends[0]=sendends[1]=ETX;
	  txstate=STANSEND;
	  send();
	}

  while (!done)
  {
    while ( txstate!=0 || rxstate!=0 )
    {
      updatedisp();
      if (ABRTE()) errlist(USRABRT);
    }
		if (ioc) { receive(" 70"); disp("-"); ch=rcvtable[(byte) bufs[1].st[0]]; }
		else { rcv(); ch=bufs[1].st[0]; }
    switch (ch)
    {
      case 'A': disp("Received answer back tone\r\n"); done=TRUE; break;
      case 'D': statval=GOTONE; disp("Received Dial tone\r\n");   break;
      case 'R': statval=RINGING; disp("Ringing\r\n");             break;
			case 'E': error=BADNUM; done=TRUE;													break;
			case 'T':	
			case 'B':	error=BUSYSIG; done=TRUE;													break;
			case 'I':	case 'Q': 
			case 'N': error=NOTONE; done=TRUE;													break;
    }
  }
	memcpy(&ptr,&temp,sizeof(DEST));
	dialing=FALSE;
	if (error) errlist(error);

	dsrtime=*nowtime+1092; /* Allow 1 minute to train. */

  statval=DSRWAIT;

  while (NODSR())
  {
    if (ABRTE()) errlist(USRABRT);
    if (*nowtime>dsrtime) errlist(DSROUT);
    updatedisp();
  }

  sync=0x32;
  outscc(6,0x32);
  outscc(7,0x32);
  disp("Online\r\n");
  statval=ONLIN;
  curbuf=1;
}                        

void pascal ds(void)
{
  if (NODSR()) 
  {
    if (cmdbuf[0]!='D') return;
    errlist(CANTOFF);
  }
  /* Send disconnect sequence, Drop DTR */
  delay(100);
  ctrlptr=&disc; txstate=CTRLSEND; send(); delay(150);
  DROPDTR();
  disp("Line Disconnected\r\n");
}

void pascal kl(void)
{
  /* Terminate file transmissions.  Decide whether some processing will
  go on in background loop of gobatch (instead of the keyboard processing).
  This code should somehow cause an eot to be sent to stop tx or rx.*/

  switch (statval)
  {
    case CONNECTING: case ANSWERING:
      DROPDTR(); statval=OFFLIN; disp("Connect Killed\r\n");break;
    case SNDING: 
    case RCVING: mdmerrlist(USRABRT); break;
    default: errlist(CANTPROG);
  }
}

void pascal m(void)
{
  unsigned long dsrtime;

  if (!NODSR()) errlist(CANTON);

  outscc(5,0x65); /* Drop DTR and hold it down for 5ms to reset modem. */
  delay(15);

  outportb(talksc,0);
  delay(15);

  outportb(talksc,5);
  delay(1);
  outportb(talksc,0xe5); /* Raise DTR */

  disp("Establish a connection with the handset and then press Enter ");
  while ( !(KBHIT() && (bioskey(0)==RET) ) );

  outportb(contsc,0);

  disp("\r\nThe modem is now trying to connect (press Esc to abort)\r\n");
  statval=CONNECTING;

  dsrtime=*nowtime+1092; /* Allow 1 minute to train. */

  while (NODSR()) 
  {
    if (ABRTE()) errlist(USRABRT);
    if (*nowtime>dsrtime) errlist(DSROUT);
    updatedisp();
  }

  curbuf=1; 
  sync=0x32;
  outscc(6,0x32);
  outscc(7,0x32);
  disp("Connected\r\n");
  statval=ONLIN;
}

void pascal mo(void)
{
  char mvers[50];
  byte len,i;

  if (!NODSR()) errlist(CANTON);

  msend("S1\1",3);
  len=mrcv(mvers,100);

  initialize(); /* Restore the 8530 back to interrupt mode. */

  disp("Modem options currently are:");
  for (i=0; i<len; i++) disp("%02x ",(uint) (0xff & mvers[i]));
}

void pascal mome(char *cmd)
{
  PRAM prs[2];
  char mmess[100],*next,hexbuf[3],*hexptr,*messptr;
  byte len,i;

  if (!NODSR()) errlist(CANTON);

  hexbuf[2]=0;

  next=getpoint(cmd,&prs[0]);
  getops(next,&prs[1]);
  switch (*((char *)ADDR(1)))
  {
    case 'N': case 'M': case '1': case 0: break;
    default: errlist(OPMUST,'N');
  }

  messptr=ADDR(0);
  hexptr=mmess;

  while (*messptr!=0)
  {
    while (!isxdigit(*messptr)) { if (*messptr==0) break; messptr++; }
    if (*messptr==0) break;
    hexbuf[0]=*messptr++; 

    while (!isxdigit(*messptr)) { if (*messptr==0) break; messptr++; }
    if (*messptr==0) break;
    hexbuf[1]=*messptr++;

    *hexptr++=ahextoi(hexbuf);
  }

  msend(mmess,hexptr-mmess);

  if ( EVAL(1)!=SCALAR && *((char *) ADDR(1))!='N')
  {
	  do {
      len=mrcv(mmess,1000);

      disp("Message back is:");
      for (i=0; i<len; i++) disp("%02x ",(uint) (0xff & mmess[i]));
      mmess[len]=0;
      disp("\r\nASCII:%s\r\n",mmess);
		}	while (*((char *) ADDR(1))=='M');
  } else { disp("°±²Û Message Sent to Modem Û²±°"); delay(20); }
}

void pascal mv(void)
{
  char mvers[50];
  byte len,i;

  if (!NODSR()) errlist(CANTON);

  msend("V",1);
  len=mrcv(mvers,100);
  if (len!=4) errlist(MDMERR,len);

  initialize(); /* Restore the 8530 back to interrupt mode. */

  disp("The modem is a :%s\r\nModem Version Number is:",moname);
  for (i=0; i<len; i++) disp("%02x ",(uint) (0xff & mvers[i]));
}

void pascal so(char *cmd)
{
  char mops[102],hexbuf[3],*hexptr,*messptr,len;

  hexbuf[2]=0;
  messptr=hexptr=&mops[2];
  mops[0]='S';

  if ( getent(cmd,&mops[2]) )
  {
    while (*messptr!=0)
    {
      while (!isxdigit(*messptr)) { if (*messptr==0) break; messptr++; }
      if (*messptr==0) break;
      hexbuf[0]=*messptr++;
      while (!isxdigit(*messptr)) { if (*messptr==0) break; messptr++; }
      if (*messptr==0) break;
      hexbuf[1]=*messptr++;
      *hexptr++=ahextoi(hexbuf);
    }
    len=hexptr-mops-2;
  }
  else len=themodem.numbytes;

  mops[1]=0x30+len; /* Construct ASCII numeral indicating number of ops. */
  len+=2;
  memcpy(defbytes,&mops[2],len);
  msend(mops,len);
	while (txstate!=0) if (ABRTE()) errlist(USRABRT);
  delay(8);
	DROPDTR();
}

void pascal pause(char *time,boolean dodsr)
{
  PRAM prs;
  boolean dodce;
  char *minutestr;
  uint j,error=0;
  int thehour,theminute;
  unsigned long connectime;
  struct time t;

  dodce=(dce && !dodsr);

  if ( *time==',' || *time==' ' ) time++;
  if (time[0]=='+')
  {
    getscal(&time[1],&prs,1,0,0x7fff);
    j=prs.value;
    if (dodsr) connectime=*nowtime+j*182;
    else connectime=*nowtime+j+(4*j)/5+j/50;
    if (dodce)
    {
      outscc(1,0x00);
      contsc&=0xFFF7;
    }
    while (TRUE)
    {
      if (dodce) outportb(contsc,0);
      if (dodsr && !NODSR()) { disp("The modem is online"); break; }
      if (ABRTE()) { error=USRABRT; break;}
      if (*nowtime>connectime)
      {
        if (dodsr) { error=TIMEOUT; break; }
        disp("Delay Complete\r\n");
        break;
      }
			updatedisp();
    }
    if (dodce) contsc|=0x0008;
    outportb(contsc,0);
		if (error) errlist(error);
		return;
	}
	
  minutestr=time;
  while (*minutestr!=':' && *minutestr!=0) minutestr++;
  if (*minutestr==0) errlist(NOCOLON);
  *minutestr=0; minutestr++;
  getscal(time,&prs,1,0,23); thehour=prs.value;
  getscal(minutestr,&prs,1,0,59); theminute=prs.value;

  if (dodsr)
    disp("Waiting until \"%02d:%02d\" (24 hour time) for phone to ring.  Press Esc to abort.\r\n",thehour,theminute);
  else disp("Waiting until \"%02d:%02d\" (24 hour time).  Press Esc to abort.\r\n",thehour,theminute);

  if (dodce)
  {
    outscc(1,0x00);
    contsc&=0xFFF7;
  }
  do
  {
    if (dodce) outportb(contsc,0);
    gettime(&t);
		updatedisp();
    if (dodsr && !NODSR()) { disp("The modem is online\r\n"); break; }
		if (ABRTE()) { error=USRABRT; break; }
  } while (!((t.ti_hour==thehour) && (t.ti_min==theminute)));

  if (dodce) contsc|=0x0008;
  outportb(contsc,0);
	if (error) errlist(error);

  if (dodsr)
  {
    if (NODSR()) errlist(TIMEDOUT);
    disp("The modem is online");
    return;
  }
  disp("Delay Complete\r\n");
}

