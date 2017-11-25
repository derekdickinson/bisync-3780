#include <bios.h>
#include <conio.h>
#include <ctype.h>
#include <dir.h>
#include <dos.h>
#include <io.h>
#include <stdlib.h>
#include <string.h>

#include "include\d3.h"

static void pascal newbatrcv(void);
static void pascal nextrcvstate(void);

void pascal desclose(DEST *dest)
{
	if (dest->destyp!=CONTYP && dest->fileptr!=NULL)
	{
		fflush(dest->fileptr);
		if (filelength(fileno(dest->fileptr))==0L)
		{
			fclose(dest->fileptr);
			unlink(dest->filename);
		}
		else
		{
			fclose(dest->fileptr);
			if (dest->autoname) nextname(dest->filename);
		}
		dest->fileptr=NULL;
	}
}

boolean pascal rcv(void)
{
	ulng timeout;

	if (!mdminst) return FALSE;
	timeout=*nowtime+546;
	while (txstate!=0) if (*nowtime>timeout || ABRT()) return FALSE;

	/* inptr must be set prior to the call rcv. */
	rxstate=2;		/* Look for message. */
	error=0;

	outscc(1,0x10);		/* Enable RX ints */
	outscc(3,0xD1);		/* RX enable and hunt*/

	return TRUE;
};

void pascal receive(char *timest)
{
	PRAM prs;

	if (NODSR()) errlist(CANTOFF);

	getscal(timest,&prs,1,LOWVAL,HIGHVAL);

	natolmt=prs.value*18+prs.value/5; strtime=*nowtime;
	datain=0; f_state=START_R; curbuf=1; firstblock=TRUE;
	setdestype(act);
	txstate=0; rxstate=0;
	gorcv(FALSE);
	desclose(act);
	if (!act->autoname && act->destyp==NORMTYP)
		disp("°±²Û Received file \"%s\" Û²±°\r\n",act->filename);
}

void pascal defdest(char *line)
{
	DEST *dest;
	PRAM prs[2];
	char *opptr;
	byte gotype;

	if (mandind==DPI) dest=&ptr; else dest=&pun;

	gotype=0;
	
	desclose(dest);

	opptr=getfilen(line,&prs[0]);
	if (*opptr!=0) *opptr++=0;
	getops(opptr,&prs[1]);

	strcpy(dest->filename,ADDR(0));
	setdestype(dest);

	if (EVAL(1)!=NONE)
	{

		for (opptr=ADDR(1); *opptr!=0; opptr++)
		{
			switch (toupper(*opptr))
			{
				case 'S': dest->stdcnv=VARY;	 gotype='S'; break;
				case 'T': dest->trancnv=FIXED; gotype='T'; break;
				case 'V':
					switch (gotype)
					{
						case 0:	 dest->stdcnv=dest->trancnv=VARY; break;
						case 'S': dest->stdcnv=VARY;							 break;
						case 'T': dest->trancnv=VARY;							break;
					} break;
				case 'F':
					switch (gotype)
					{
						case 0:	 dest->stdcnv=dest->trancnv=FIXED; break;
						case 'S': dest->stdcnv=FIXED;							 break;
						case 'T': dest->trancnv=FIXED;							break;
					} break;
				case 'B':
					switch (gotype)
					{
						case 0:	 dest->trancnv=BINARY; break;
						case 'S': dest->stdcnv=BINARY;	break;
						case 'T': dest->trancnv=BINARY; break;
					} break;
				case 'H':
					switch (gotype)
					{
						case 0:	 dest->stdcnv=dest->trancnv=HEXPAIR; break;
						case 'S': dest->stdcnv=HEXPAIR;							 break;
						case 'T': dest->trancnv=HEXPAIR;							break;
					} break;
				case 'N': dest->autoname=TRUE;	 break;
				case 'C': dest->autoname=FALSE;	break;
				case '/': break;
				default: errlist(OPRBAD,*opptr);
			}
		}
	}
	setdestype(dest);
	if (dest->autoname) 
	{
		struct ffblk ffblk;
		if (findfirst(dest->filename,&ffblk,0)==0) nextname(dest->filename);
	}
}

static void pascal newbatrcv(void)
{
	firstblock=TRUE;

	unwrapbuffs(); /* Unwrap any monitor buffers. */

	if (act->destyp<NORMTYP) fwrite("\r\n",1,2,act->fileptr);
	if (!act->autoname || act->destyp!=NORMTYP) return;

	disp("°±²Û Received file \"%s\" Û²±°\r\n",act->filename);

	desclose(act);
	setdestype(act);
}

static void pascal nextrcvstate(void)
{
	switch (rxtyp)
	{
		case CTRLTYPE:
			switch (ctrlbyt)
			{
				case ENQ: f_state=ENQIN; renqcnt++;	 return;
				case BEL: f_state=RESPBELL;					 return;
				case RVI: /* Additional stuff to recover and restart should go here! */
				case EOT: RCVEOT();									 return;
				default: f_state=GARBAGE;						 return;
			};
		case STANTYPE:
		case TRANTYPE:
			if ((error & 0x4f)==0)
			{
				f_state=GOODBLK;
				if (cmend==ETX && STRLCMP("\x81\xA1\x81\xA1",bufs[curbuf].st))
				{
					setremcmd();
					bufs[curbuf].len=0;
					return;	 
				}
				if (rcvends[curbuf]==ETX) f_state=RCVDONE;
				strtnak=rnakcnt;
				return;
			}
			f_state=BADBLK;
			if (RNAKOVR()) f_state=RNAKOUT;
			return;
		case TTD:
			if (!STRLCMP("\x81\xA1\x81\xA1",bufs[curbuf].st))	
				{ f_state=BADBLK; return; }
			if ((error & 0x4f)==0)
			{
				f_state=GOODBLK;
				setremcmd();
				bufs[curbuf].len=0; /* Stop the command from being put on disk. */
				return;
			}
			f_state=BADBLK;
			return;
		case DISCSEQ:
			f_state=LINEDOWN;
			DROPDTR();
	}
}

void cdecl mdmerrlist(int errnum,...)
{
	ulng timeout;
	/* This buffer determines whether an EOT should be sent to the remote when an error occurs. */
	static byte doeot[]=
/*00,02,04,06,08,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52,54,56,58,60 */
{	 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0,};

	if (NODSR())
	{
		txstate=rxstate=0;
		DROPDTR();
		statval=OFFLIN;
		sync=0x16;
	}
	else
	{
		timeout=*nowtime+182;
		while (txstate!=0) if (ABRTE() || *nowtime>timeout) break;
		while (rxstate!=0 && rxstate!=2) if (ABRTE() || *nowtime>timeout) break;

		if (doeot[f_state/2])
		{
			timeout=*nowtime+182;
			SENDEOT();
			while (txstate!=0 && *nowtime<timeout) if (ABRTE()) break;
		}
		statval=ONLIN;
	}
	initialize();
	txstate=rxstate=0;
  f_state=BEGINRCV;	sendrsp=FALSE;
	curbuf=1;

	if (sndlist!=NULL)
	{
		freesndlist();
		if (!sendmess) NUCLOS(sendfile);
		sendmess=FALSE;
	}
	if (!firstblock) newbatrcv();

	errlist(errnum,...);
}

static boolean outmid=FALSE;
static boolean nobatch=FALSE;

static boolean pascal keyexit(void)
{
	updatedisp();
	if (nobatch) return (KBHIT() && cmdproc());
	if (ABRTE()) mdmerrlist(USRABRT);
	return FALSE;
}

void pascal gorcv(boolean isinter)
{
	ulng abrtime;
	/* Boolean indicator is set if exited in the middle of a reception. */
	nobatch=isinter;
	if (nobatch) natolmt=0;
	else
	{
		if (natolmt==0xFFFF) { natolmt=364; outmid=TRUE; }
		else outmid=FALSE;
	}

	while (!mdminst) if (keyexit()) return;

	while (TRUE)
	{
		if (keyexit()) return;
		if (!outmid) switch (f_state)
		{
	/* Receiving a file states. */
			case BEGINRCV: f_state=START_R;
			case GARBAGE:
			case START_R:	STRTRCV();														break;
			case ENQIN:		RESEND(CTRLSEND);											break;
			case GOODBLK:	SENDACK(); ritercvbuf(curbuf^0x0001);	break;
			case BADBLK:	SENDNAK();														break;
			case RCVDONE:	SENDACK(); BATDONE();									break;
	/* Error condition states. */
			case PREMEOT:	mdmerrlist(RREMABRT);
			case RNAKOUT: mdmerrlist(RNAKERR);
			case ACTYOUT:	mdmerrlist(NOACTIM);
			case DISKERR: mdmerrlist(DISKRCV);
			case LINEDOWN: mdmerrlist(ABDISC);
			case BEGINSND: break;
			default: mdmerrlist(BADSTATE,f_state);
		}

		abrtime=*nowtime+60;
		while (txstate != 0)
		{
			if (keyexit()) { if (f_state!=START_R) outmid=TRUE; return; }
			if (*nowtime>abrtime) { txstate=0; break; }
		}

		if (f_state==BEGINRCV) { f_state=START_R; continue; }

		while (rxstate != 0)
		{
			if (keyexit()) { if (f_state!=START_R) outmid=TRUE; return; }
			if (!nobatch)
			{
				if (NOACTY()) { f_state=ACTYOUT; break; }
				if (NODSR()) { f_state=LINEDOWN; break; }
			}
			if (IDLETIMEOUT()) { f_state=IDLEOUT; break; }
		}
		outmid=FALSE;
		switch (f_state)
		{
			case START_R:
				switch (rxtyp)
				{
				case DISCSEQ: f_state=LINEDOWN;	DROPDTR(); continue;
				case CTRLTYPE: if (ctrlbyt==ENQ) break;
				default: f_state=BEGINRCV; continue;
				}
				RESETRCV();
			case ENQIN: case GOODBLK: case BADBLK: case RCVDONE: case GARBAGE:
				nextrcvstate();									break;
			case IDLEOUT: mdmerrlist(IDLELMT);
			case ACTYOUT: mdmerrlist(NOACTIM);
			case LINEDOWN:
			case BEGINRCV: break;
			default: mdmerrlist(BADSTATE,f_state);
		}
		if (nobatch)
		{
			if (f_state==START_R)
			{
				natolmt=0; curbuf=1;

				if ( !act->autoname && act->destyp!=CONTYP)
					disp("°±²Û Receive complete \"%s\" Û²±°\r\n",act->filename);

				if (keystate==DISPING) { promp();	keystate=ATPROMPT; }
				proremcmd();
			}
		}
		else if (f_state==START_R || f_state==BEGINRCV) { proremcmd(); break; }
	} /* while (TRUE) */
}
