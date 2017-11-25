#include <string.h>
#include <ctype.h>

#include "include\d3.h"

static void pascal binsend(byte buf);
static void pascal commandsend(byte buf);
static char * pascal ebcrec(char *ebcptr);
static char * pascal ebcrecfixed(char *ebcptr);
static void pascal fixedsend(byte buf);
static void pascal hexsend(byte buf);
static boolean pascal speof(void);
static boolean pascal speoftext(void);
static void pascal varysend(byte buf);
static boolean cdecl mygets(char *s, int n, FILE *fp);
static boolean cdecl myspgets(char *s,int n);

static uint thesize;

static boolean pascal speof(void)
{
	int c;

  if (sendmess) return (*messptr==0);
  if (sendfile==NULL) return TRUE;
	c=getc(sendfile);
	if (c==EOF) return TRUE;
	fseek(sendfile,-1,SEEK_CUR);
	return FALSE;
}

static boolean pascal speoftext(void)
{
	int c;
	
  if (sendmess) return (*messptr==0);
  if (sendfile==NULL) return TRUE;
	c=getc(sendfile);
	if (c==EOF) return TRUE;
	if (c==0x1A) { while (!feof(sendfile)) fgetc(sendfile); return TRUE; }
	fseek(sendfile,-1,SEEK_CUR);
	return FALSE;
}


void pascal loadsenbuf(byte buf)
{
  if (speof()) { newsend(buf); bufs[buf].len=0; return; }

  if (sndlist!=NULL) bufs[buf].strtstate=strtstate[sndlist->buftype >> 2];
  else bufs[buf].strtstate=strtstate[buftype >> 2];

	thesize=0;

  if (comsend) { commandsend(buf); return; }
	
	if (!destsent && sndlist->prindest==PUNCH)
  {
	  bufs[buf].st[0]=DC2;
		bufs[buf].len=1;
    sendends[buf]=ETB;
	  destsent=TRUE;
    return;
	}
	
  switch (buftype & TRANCLR)
	{
		case VARY:    varysend(buf);    break;
    case FIXED:   fixedsend(buf);   break;
	  case BINARY:  binsend(buf);     break;
		case HEXPAIR: hexsend(buf);     break;
  }
	
	bufs[buf].len=thesize; dataout+=thesize;
  if (thesize!=0) sblkcnt++;
}

static void pascal commandsend(byte buf)
{
  byte i;

	sprintf(bufs[buf].st,"AaAa0002%sZzZz",messptr);
  bufs[buf].len=thesize=strlen(bufs[buf].st);
	messptr+=thesize;

  for(i=0; i<thesize; i++) bufs[buf].st[i]+=0x40;

  newsend(buf);
}

#define BACKPTR() {                             \
  if (sendmess) messptr-=(ebcptr-endptr);       \
  else fseek(sendfile,ebcptr-endptr,SEEK_CUR);  \
                                                }

static void pascal varysend(byte buf)
{
  char *ebcptr,*endptr;
	boolean cdecl (*thegets)(char *s, int n, FILE *fp);

	if (sendmess) thegets=(boolean cdecl (*)(char *s, int n, FILE *fp))myspgets;
  else thegets=mygets;

  endptr=&bufs[buf].st[blocksiz];
  ebcptr=&bufs[buf].st[0];
  *endptr=0;

  while (TRUE)
  {
    if ((*thegets)(ebcptr,endptr-ebcptr,sendfile))
    {
      if (speoftext())
      {
        newsend(buf);
        if (ebcptr[0]!=0) 
        {
					if (ebcptr==&bufs[buf].st[0])
					{
	          ebcptr=ebcrec(ebcptr);
		        if (!as) *(--ebcptr)=0;
					}
					else ebcptr=ebcrec(ebcptr);
        }
        break;
      }
      sendends[buf]=ETB;
      if (ebcptr==&bufs[buf].st[0]) 
      {
        ebcptr=ebcrec(ebcptr);
        if (!as) *(--ebcptr)=0;
      }
      else BACKPTR();
      break;
    }
    ebcptr=ebcrec(ebcptr);
  }
  thesize=ebcptr-&bufs[buf].st[0];
}

static void pascal fixedsend(byte buf)
{
  char *ebcptr,*endptr;
	boolean cdecl (*thegets)(char *s, int n, FILE *fp);

	if (sendmess) thegets=(boolean cdecl (*)(char *s, int n, FILE *fp))myspgets;
  else thegets=mygets;

  endptr=&bufs[buf].st[blocksiz];
  ebcptr=&bufs[buf].st[0];
  *endptr=0;

  while (ebcptr<(endptr-rreclen))
  {
    if ((*thegets)(ebcptr,endptr-ebcptr,sendfile))
    {
      if (speoftext())
      {
        if (ebcptr[0]!=0) ebcptr=ebcrecfixed(ebcptr);
        break;
      }
      if (ebcptr==&bufs[buf].st[0]) ebcptr=ebcrecfixed(ebcptr);
      else BACKPTR();
      break;
    }
    ebcptr=ebcrecfixed(ebcptr);
  }
  if (*messptr==LF) messptr++;
  if (speoftext()) newsend(buf); else sendends[buf]=ETB;
  thesize=ebcptr-&bufs[buf].st[0];
}

static void pascal binsend(byte buf)
{
  uint nufbuf[5];

  if (sendmess)
  {
    bufs[buf].st[blocksiz]=0;
    strncpy(bufs[buf].st,messptr,blocksiz);
    thesize=strlen(bufs[buf].st); messptr+=thesize;
    if (*messptr==0) newsend(buf); else sendends[buf]=ETB;
  }
  else
  {
    thesize=fread(bufs[buf].st,1,blocksiz,sendfile);
    if (fread(nufbuf,1,1,sendfile)==0) newsend(buf);
    else { fseek(sendfile,-1L,SEEK_CUR); sendends[buf]=ETB; }
  }
}

static char pascal nextchar(void)
{
	if (sendmess)
	{
    while (!isxdigit(*messptr)) { if (*messptr==0) break; messptr++; }
		if (*messptr!=0) messptr++;
		return (*messptr);
	}
	else
	{
		char hexbuf;
    do 
		{
      if (fread(&hexbuf,1,1,sendfile)==0) break;
		} while (!isxdigit(hexbuf));
		return (hexbuf);
	}
}

static void pascal hexsend(byte buf)
{
  char *hexptr,*endptr,hexbuf[3];

  endptr=&bufs[buf].st[blocksiz];
  hexptr=&bufs[buf].st[0];
  *endptr=0;
  hexbuf[2]=0;

  while (hexptr<endptr)
  {
		hexbuf[0]=nextchar();
		hexbuf[1]=nextchar();
		if (speoftext()) break;
    *hexptr++=ahextoi(hexbuf);
  }
	if (speoftext()) newsend(buf); else sendends[buf]=ETB;
  thesize=hexptr-&bufs[buf].st[0];
}

/* General conversion function. */
static char * pascal ebcrec(char *ebcptr)
{
  byte numblanks;
  boolean done;
  char *ascptr;

  ascptr=ebcptr;

  for (numblanks=0,done=FALSE; !done; ascptr++)
  {
    switch (0x7f & *ascptr)
    {
      case 0:
        if (tb)
        {
          switch (numblanks)
          {
            case 0: break;
            case 1: *ebcptr++=0x40; break;
            default: *ebcptr++=IGS; *ebcptr++=0x40+numblanks;
          }
        }
        *ebcptr++=IRS; *ebcptr=0; done=TRUE; 
        break;
      case 0x20:
        if (co)
        {
          if (++numblanks==63)
          {
            *ebcptr++=IGS; *ebcptr++=0x7f;
            numblanks=0;
          }
          break;
        }
      default:
        switch (numblanks)
        {
          case 0: break;
          case 1: *ebcptr++=0x40; break;
          default: *ebcptr++=IGS; *ebcptr++=0x40+numblanks;
        }
        numblanks=0;
        *ebcptr++=sentable[0x7f & *ascptr];
    }
  }

  return ebcptr;
}

static char * pascal ebcrecfixed(char *ebcptr)
{
  uint i;
  boolean hitCR=FALSE;
  char *ascptr;

  ascptr=ebcptr;

  for (i=0; i<rreclen; i++)
  {
    if (hitCR) *ebcptr++=0x40;
    else
    {
      switch (0x7f & *ascptr)
      {
        case 0:
        case CR: hitCR=TRUE; *ebcptr++=0x40; break;
        default: *ebcptr++=sentable[0x7f & *ascptr++];
      }
    }
  }
  return ebcptr;
}

static boolean cdecl mygets(char *s, int n, FILE *fp)
{
  register int c = 0;
  register char *P;

  P = s;
  while (TRUE)
  {
    if (--n<0)   break;
    c=getc(fp);
    if (c==0)    c=0x20;
    if (c==EOF)  { fp->flags|=_F_EOF; /* Set feof flag to true. */ break; }
		if (c==0x1a) { while (!feof(fp)) fgetc(fp); break; }
    if (c==CR)   { *P=0; return FALSE; }
    if (c==LF)   { n++; continue; }
    *P++=c;
  }
  *P = 0;
  return TRUE;
}

static boolean cdecl myspgets(char *s,int n)
{
  register int c = 0;
  register char *P;

  P = s;
  while (TRUE)
  {
    if (--n<0)   break;
    c=*messptr;  messptr++;
    if (c==EOF)  break;
    if (c==0)    { messptr--; break; }
    if (c==0x1a) { while (*messptr!=0) messptr++; break;}
    if (c==CR)   { *P=0; return FALSE; }
    if (c==LF)   { n++; continue; }
    *P++=c;
  }
  *P = 0;
  return TRUE;
}
