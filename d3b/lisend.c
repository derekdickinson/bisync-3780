#include <bios.h>
#include <dos.h>
#include <stdlib.h>
#include <string.h>

#include "include\d3.h"

static void pascal dosend(void);
static void pascal pollout(char ch);
static void pascal nextsenstate(void);
static void pascal sbatcomp(void);

static void pascal pollout(char ch)
{
  ulng abrtime;

  rxstate=0;
	outscc(1,0x00);  /* Disable Interrupts */
  outscc(3,0xc0);  /* Turn off the Receiver. */
  outscc(5,0xEE);  /* Turn on RTS, DTR and TXer. */

	abrtime=*nowtime+2;
	while ( (inportb(contsc) & 0x0004) != 0x0004) /* Wait for a TX buffer empty. */
  {
	  if (ABRTE()) mdmerrlist(USRABRT);
		if (*nowtime>abrtime) break;
  }
	outportb(datasc,ch); /* Output sync char to TX buffer */
}

static void setsend(void)
{
  buftype=sndlist->buftype;

  sendmess=TRUE;
  comsend=FALSE;
  messptr=sndlist->mesbuf;

	switch (sndlist->sensrc)
	{
	case REMCMD:	
		strcpy(senfilname,remcmdstring);	
		comsend=TRUE; 
		break;
	case MESBUF:	
		strcpy(senfilname,messtring);										
		break;
	default:			
		strcpy(senfilname,sndlist->filename); 
		sendmess=FALSE;
		if ((sendfile=fopen(senfilname,"rb"))==NULL) errlist(BADFILE,senfilname);
  }
}

void pascal listsend(void)
{
  sendends[0]=sendends[1]=0; sblkcnt=snakcnt=senqcnt=0; dataout=0;
  sendrsp=TRUE; f_state=START_S; curbuf=1;

	setsend();
  destsent=FALSE;

  loadsenbuf(0);
  dosend();
  if (ABRTE()) mdmerrlist(USRABRT);
}

void pascal newsend(byte buf)
{
  SND *olsnd;

  if (sndlist==NULL)
  {
    if (comsend) { sendends[buf]=cmend; return; }
    sendends[buf]=ETX;
    return;
  }

  if (sndlist->sensrc==SENFILE) NUCLOS(sendfile);

  if (sndlist->nextsnd==NULL)
  {
    if (comsend) sendends[buf]=cmend; else sendends[buf]=ETX;
    free(sndlist);
    sndlist=NULL;
    return;
  }

  sendends[buf]=sndlist->catype;

  olsnd=sndlist;
  sndlist=olsnd->nextsnd;
  free(olsnd);

  if (sendends[buf]==ETX) destsent=FALSE;
	setsend();
}

static void pascal sbatcomp(void)
{
  if (sndlist->sensrc==SENFILE) NUCLOS(sendfile);

  f_state=BEGINRCV; sendrsp=FALSE;
  SENDEOT();
  curbuf=1; 
  free(sndlist);
  sndlist=NULL;
  return;
}

static void pascal dosend(void)
{
	ulng abrtime;
  txstate=0; rxstate=0;

  while (TRUE)
  {
    if (ABRT()) mdmerrlist(USRABRT);
    switch (f_state)
    {
  /* Sending a file states. */
      case START_S: strtenq=senqcnt;
      case TIMEOUT: SENDENQ();                                           break;
      case GOODACK: SENDBUF(); loadsenbuf(curbuf^0x0001);                break;
      case BADACK:  RESEND(bufs[curbuf].strtstate);                      break;
      case SENDONE: sbatcomp();                                          break;
  /* Receiving a file states. */
      case START_R: STRTRCV();                                           break;
  /* Error condition states. */
      case BAKOUT:   mdmerrlist(SBADACKLMT);
      case SNAKOUT:  mdmerrlist(SNAKERR);
      case REMABRT:  mdmerrlist(SREMABRT);
      case LINEDOWN: mdmerrlist(ABDISC);
      case DISKERR:  mdmerrlist(DISKRCV);
      default: mdmerrlist(BADSTATE,f_state);

    }

    if (f_state==TIMEOUT) f_state=GOODACK;

		abrtime=*nowtime+(ulng) blocksiz*91L/750L;
    while (txstate != 0)
    {
			if (ABRTE()) mdmerrlist(USRABRT);
      if (NODSR()) mdmerrlist(ABDISC);
			if (*nowtime>abrtime) 
			{ 
				pollout(ENQ); txstate=0;
				sendtime=*nowtime;
				rcv();
				break;
			}
      updatedisp();
    }

    while (rxstate != 0)
    {
			if (ABRTE()) mdmerrlist(USRABRT);
      if (ENQTIMEOUT()) { f_state=TIMEOUT; break; }
      if (NODSR()) mdmerrlist(ABDISC);
      updatedisp();
    }

    switch (f_state)
    {
      case TIMEOUT: if (ENQOVR()) mdmerrlist(SENQLMT);
        continue;
      case START_S: case GOODACK: case BADACK: case SENDONE:
        strtenq=senqcnt; nextsenstate(); break;
			case LINEDOWN: mdmerrlist(ABDISC);
      case BEGINRCV: break;
      default: mdmerrlist(BADSTATE,f_state);
    }

    updatedisp();

    if (f_state==START_R) break;
    if (f_state==BEGINRCV) break;

  } /* while (TRUE) */
  
  rxstate=0; txstate=0; f_state=12;
  initialize();
}

void pascal send(void)
{
	ulng abrtime,maxrcv;
	uint strtstate;

	if (!mdminst) errlist(NOMODEM);
  error=0;

  if (NODSR()) { sync=0x16;	maxrcv=20; }
	else { sync=0x32; maxrcv=450;	}
  outscc(6,sync);
  outscc(7,sync);

	abrtime=*nowtime+maxrcv;
  /* Make sure that no receive is in progress. */
  while ( !((rxstate==0) || (rxstate==2)) )
	{
    if (ABRTE()) mdmerrlist(USRABRT);
		if (*nowtime>abrtime) mdmerrlist(NOCTS);
	}

  rxstate=0;

  outscc(3,0xc0);  /* Turn off the Receiver. */

  outscc(5,0xEE);       /* Turn on RTS, DTR and TXer. */

  abrtime=*nowtime+maxrcv;
  while ( (inportb(contsc) & 0x0020) != 0x0020)   /* Wait for CTS. */
  {
    if (ABRTE()) mdmerrlist(USRABRT);
    if (*nowtime>abrtime) mdmerrlist(NOCTS);
  }

	for (strtstate=txstate; txstate==strtstate; )
	{
	  outportb(contsc,0x80);               /*Reset TX CRC Generator*/

		outscc(1,0x02);  /* Enable TX interrupts */

		abrtime=*nowtime+maxrcv;
		while ( (inportb(contsc) & 0x0004) != 0x0004) /* Wait for a TX buffer empty. */
	  {
		  if (ABRTE()) mdmerrlist(USRABRT);
			if (*nowtime>abrtime) mdmerrlist(NOTXBUF);
	  }
		outportb(datasc,sync); /* Output sync char to TX buffer */
	  outportb(contsc,0xC0); /* Reset TX Underrun/EOM latch */

		abrtime=*nowtime+40;
		while (txstate==strtstate)
			if (*nowtime>abrtime)
			{
				if (f_state>13 && f_state<21) txstate=strtstate=0;
				break;
			}
	}

}

static void pascal nextsenstate(void)
{
  /* The ack sent is for the buffer expected which the receiver expects next.
  Thus the 0 position if ACK1 and the 1 position is for ACK0. */
  static byte goodacks[]={ 0x61, 0x70 };

  if (rxtyp!=CTRLTYPE)
  {
    f_state=TIMEOUT;
    return;
  }

  if (goodacks[curbuf]==ctrlbyt)
  {
    f_state=GOODACK; strtnak=snakcnt;
    /* The "sendends[curbuf]==cmend" is to send remote commands. */
    if ( (sendends[curbuf]==ETX || sendends[curbuf]==cmend) &&
         sndlist==NULL &&
         bufs[curbuf^0x0001].len==0 ) f_state=SENDONE;
    return;
  }

  switch (ctrlbyt)
  {
    case ACK0:
    case ACK1:
    case NAK:
      snakcnt++;
      if (f_state!=START_S) f_state=BADACK;
      if (SNAKOVR()) f_state=SNAKOUT;
      break;
    case RVI: /* Put stuff here to take up where I left off. */
    case EOT: f_state=REMABRT; break;
    case WACK: strtenq=senqcnt;
    default: f_state=TIMEOUT; break;
  }

}

