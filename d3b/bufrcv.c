#include <alloc.h>
#include <bios.h>
#include <conio.h>
#include <io.h>
#include <string.h>

#include "include\d3.h"

static void pascal binrite(byte buf);
static void pascal cpybuff(byte which,byte *buff,uint size);
static void pascal fixedrite(byte buf);
static void pascal hexrite(byte buf);
static void pascal putout(byte *buff,uint size);
static void pascal resetbuffs(void);
static void pascal setdest(byte buf);
static void pascal varyrite(byte buf);

void pascal ritercvbuf(byte buf)
{
  char curcnv;

  if (firstblock) setdest(buf);
  switch (rxtyp)
  {
    case STANTYPE: lastrcv=STANTYPE; curcnv=act->stdcnv;  break;
    case TRANTYPE: lastrcv=TRANTYPE; curcnv=act->trancnv; break;
    default: break;
  }
  rblkcnt++;
  switch (curcnv)
  {
    case VARY:    varyrite(buf);  break;
    case FIXED:   fixedrite(buf); break;
    case BINARY:  binrite(buf);   break;
    case HEXPAIR: hexrite(buf);   break;
  }
	updatedisp();
}

static void pascal resetbuffs(void)
{
  BUFPTR *b;
  byte i;

  for (i=0; i<3; i++)
  {
    switch (i) 
    {
      case 0:  b=&rbuf;   break;
      case 1:  b=&ptrbuf; break;
      default: b=&punbuf; break;
    }
    if (b->bufbeg==NULL) { b->avail=FALSE; continue; }
    b->avail=TRUE;
    b->strt=b->end=b->bufbeg;
    b->len=0;
    b->left=b->buflen;
  }
}

void pascal unwrapbuffs(void)
{
  BUFPTR *b;
  byte *copyarea,i;

  for (i=0; i<3; i++)
  {
    switch (i)
    {
      case 0:  b=&rbuf;   break;
      case 1:  b=&ptrbuf; break;
      default: b=&punbuf; break;
    }
    if (b->bufbeg==NULL) continue;
    if (b->strt==b->bufbeg) continue;
    if (!b->wrap) continue;

    if ((copyarea=malloc(b->end-b->bufbeg))==NULL) errlist(NOINTMEM);

    memcpy(copyarea,b->strt,b->end-b->strt);
    memcpy(&copyarea[b->end-b->strt],b->bufbeg,b->strt-b->bufbeg);
    memcpy(b->bufbeg,copyarea,b->buflen);

    free(copyarea);

    b->strt=b->bufbeg;
  }

}

#define RBUF   0
#define PTRBUF 1
#define PUNBUF 2

#define REMAIN   (size-b->left)
#define TILLWRAP (b->end-b->strt)

static void pascal cpybuff(byte which,byte *buff,uint size)
{
  BUFPTR *b;

  switch (which)
  {
    case RBUF:   b=&rbuf;   break;
    case PTRBUF: b=&ptrbuf; break;
    default:     b=&punbuf;
  }

  if (b->bufbeg==NULL) return;

  if (b->left > size)
  {
    memmove(b->end,buff,size);
    b->len+=size;
    b->left-=size;
    b->end+=size;
    return;
  }

  if (b->left > 0)
  {
    memmove(b->end,buff,b->left);
    if (b->wrap)
    {
      if (REMAIN>b->buflen)
      {
        memmove(b->bufbeg,&buff[size-b->buflen],b->buflen);
        b->strt=b->bufbeg;
      }
      else
      {
        memmove(b->bufbeg,&buff[b->left],REMAIN);
        b->strt=&b->bufbeg[REMAIN];
      }
    }
    else b->avail=FALSE;
    b->len=b->buflen;
    b->left=0;
    b->end=&b->bufbeg[b->buflen];
    return;
  }

  if (!b->wrap) return;

  if (size>b->buflen)
  {
    memmove(b->bufbeg,&buff[size-b->buflen],b->buflen);
    b->strt=b->bufbeg;
    return;
  }

  if (size>TILLWRAP)
  {
    memmove(b->strt,buff,TILLWRAP);
    memmove(b->bufbeg,&buff[TILLWRAP],size-TILLWRAP );
    b->strt=&b->bufbeg[size-TILLWRAP];
    return;
  }

  memmove(b->strt,buff,size);
  b->strt+=size;
}

static void pascal putout(byte *buff,uint size)
{
  if (size==0) return;

  if (rbuf.avail) cpybuff(RBUF,buff,size);
  if (act==&ptr) { if (ptrbuf.avail) cpybuff(PTRBUF,buff,size);	}
  else { if (punbuf.avail) cpybuff(PUNBUF,buff,size);	}

  datain+=size;

  if (act->destyp==CONTYP)
	{
		int thex,they;
		thex=wx(); they=wy();
		buff[size]=0;
		disp( (char *) buff);
		if ( thex==wx() && they==wy() ) disp("\r\n");
		return;
	}
	
  if (act->fileptr==NULL)
  {
    if (act->autoname) act->fileptr=fopen(act->filename,"wb");
    else act->fileptr=fopen(act->filename,"ab");
    if (act->fileptr==NULL) { f_state=DISKERR; return; }
  }
	
	if (act->destyp>=NORMTYP || pb==1)
	{ 
		if (fwrite(buff,1,size,act->fileptr)<size) f_state=DISKERR;	
		return;
	}

	/* Must be printer I/O */
	{
		uint i,olbyt=0,olrx,pstat=0x90;
		byte noerr=TRUE; /* Stop Redundant error messages */
		ulng wacktime=0;
		LITSTR *olptr;

		olptr=ctrlptr;

		for(i=0; i<size; i++)
		{
			pstat=0xB8 & biosprint(2,buff[i],act->destyp);
			if (pstat==0x90) { noerr=TRUE; biosprint(0,buff[i],act->destyp); }
			else
			{
				if (noerr && pstat!=0x10)
				{
					noerr=FALSE;
					if (0x20 & pstat) disp("\7Out of paper\r\n");
					else disp("\7Printer Not Selected, Status [%02X]\r\n",pstat);
				}
				i--;
			}
			if (txstate==0 && rxstate==0)
			{
				if (olbyt==0)
				{
					wacktime=*nowtime+14L;
					olrx=rxtyp;
					olbyt=ctrlbyt;
				}
				else if (*nowtime>wacktime)
				{
					wacktime=*nowtime+16L;
				  ctrlptr=&wack; txstate=CTRLSEND; send();
				}
			}
		}
			
		if (olbyt!=0)
		{
			if (olbyt==EOT) rxstate=0;
			while (txstate!=0 || rxstate!=0) if (*nowtime>wacktime) break;
			rxtyp=olrx;
			ctrlbyt=olbyt;
		}
		ctrlptr=olptr;
	}
}

static void pascal varyrite(byte buf)
{
  uint thesize=0;
  char escbuf[300],*escptr,linebuf[1024];
  byte *bufptr,*endptr,i,j;
  char *lineptr;

  escptr=escbuf;
  bufptr=(byte *) &bufs[buf].st[0];
  endptr=(byte *) &bufs[buf].st[bufs[buf].len];
  lineptr=&linebuf[0];

  if (skipone) { skipone=FALSE; bufptr++; }

  while (bufptr<endptr)
  {
    if (thesize>(sizeof(linebuf)-70))
    {
      putout((byte *) linebuf,thesize);
      lineptr=&linebuf[0]; thesize=0;
    }
    switch (*bufptr)
    {
      case 0x27:  /* EBCDIC Escape */
        bufptr++;
				for (j=0; eschar[j]!=0; j++) 
					if (*bufptr==eschar[j]) 
						for (i=0; escstr[j][i]!=0; i++) *escptr++=escstr[j][i];
				bufptr++;
        break;
      case IRS:
				for (i=0; crlf[i]!=0; i++) *lineptr++=crlf[i];
        for (i=0;i<(escptr-escbuf);i++) *lineptr++=escbuf[i];
        escptr=escbuf;
        bufptr++;
        break;
      case IGS:
        *(++bufptr)-=0x40;
        while ((*bufptr)--) *lineptr++=' ';
        bufptr++;
        break;
      case 0x8D:
				if (hasp)
				{
	        *(++bufptr)-=0x40;
		      while ((*bufptr)--) *lineptr++='0';
			    bufptr++;
				  break;
				}
      default:
        *lineptr++=rcvtable[*bufptr++];
    }
    thesize=(uint) (lineptr-linebuf);
  }

  putout((byte *) linebuf,thesize);
}

static void pascal fixedrite(byte buf)
{
  uint thesize=0,i;
  char escbuf[100],*escptr,linebuf[1024];
  byte *bufptr,*endptr;
  char *lineptr;

  escptr=escbuf;
  bufptr=(byte *) &bufs[buf].st[0];
  endptr=(byte *) &bufs[buf].st[bufs[buf].len];
  lineptr=&linebuf[0];

  if (skipone) { skipone=FALSE; bufptr++; }

  while (bufptr<endptr)
  {
    for (i=0; i<preclen; i++)
    {
      switch (*bufptr)
      {
        case 0x27:  /* EBCDIC Escape */
          bufptr++;
          switch (*bufptr++)
          {
            case 0x61: *escptr++=' ';  break; /* EBCDIC '/', Single space */
            case 0xC1: *escptr++=0x0C; break; /* EBCDIC 'A', Form Feed */
            case 0xE2: /* EBCDIC 'S', Double Space. */
              *escptr++=' '; *escptr++=' '; break;
            case 0xE3: /* EBCDIC 'T', Triple Space */
              *escptr++=' '; *escptr++=' '; *escptr++=' '; break;
            default:; /* Ignore other sequences. */
          }
          break;
        default:*lineptr++=rcvtable[*bufptr++];
      }
    }

		if (!fixedblanks)
		{
	    lineptr--;
		  while (*lineptr==' ') lineptr--;
			lineptr++;
		}
    *lineptr++=CR; *lineptr++=LF;
    for (i=0;i<(escptr-escbuf);i++) *lineptr++=escbuf[i];
    escptr=escbuf;
    thesize=(uint) (lineptr-linebuf);
    putout((byte *) linebuf,thesize);
    lineptr=&linebuf[0]; thesize=0;
  }
}

static void pascal binrite(byte buf)
{
  if (skipone) 
  {
    putout((byte *)&bufs[buf].st[1],bufs[buf].len-1);
    skipone=FALSE;
    return;
  }
  putout((byte *)bufs[buf].st,bufs[buf].len);
}

#define LBUFSIZ() (lineptr-(byte *) linebuf)

static void pascal hexrite(byte buf)
{
  char linebuf[1024];
  byte *hexptr,*endptr,*endline,*lineptr;
  byte chrcnt=0;

  memset(linebuf,0,sizeof(linebuf));

  hexptr=(byte *) &bufs[buf].st[0];
  if (skipone) { hexptr++; skipone=FALSE; }

  endptr=(byte *) &bufs[buf].st[bufs[buf].len];

  lineptr=(byte *) &linebuf[0];
  endline=(byte *) &linebuf[1020];

  while (hexptr<endptr)
  {
    if (lineptr>=endline)
    {
      putout((byte *) linebuf,LBUFSIZ());
      memset(linebuf,0,sizeof(linebuf));
      lineptr=(byte *) &linebuf[0];
    }
    if (chrcnt++<25) sprintf( (char *) lineptr,"%.2X ",*hexptr++);
    else { sprintf( (char *) lineptr,"%.2X\r\n",*hexptr++); lineptr++; chrcnt=0; }
    lineptr+=3;
  }
  if (chrcnt!=0) { sprintf( (char *) lineptr,"\r\n"); lineptr+=2; }

  putout((byte *) linebuf,LBUFSIZ());
}

/* Sets destination if punch/printer change is requested. */
static void pascal setdest(byte buf)
{
  skipone=TRUE;

  resetbuffs(); /* Set copy buffs for use. */

  switch (bufs[buf].st[0])
  {
    case DC2:
    case DC3: if (act==&ptr) { desclose(&ptr); act=&pun; } break;
    default: skipone=FALSE;
    case DC1: if (act==&pun) { desclose(&pun); act=&ptr; }
  }
  firstblock=FALSE;
  if (act==&ptr) lastdest=PTR; else lastdest=PUN;
}
