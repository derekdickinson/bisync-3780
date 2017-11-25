#include <alloc.h>
#include <bios.h>
#include <conio.h>
#include <ctype.h>
#include <string.h>

#include "include\d3.h"

static uint pascal getxt(char **buf,byte rou,byte colu,byte rol,byte coll);
static void pascal putxt(char *buf);
static uint makerun(byte *buf,uint length);
static uint unrun(byte *dst,byte *src,uint length);

/***********************************************************************/
char * pascal setriteops(char *next,PRAM *prs,char *ops)
{
  PRAM oprs;
  char cats[2];
  byte rcount;

  cats[1]=0;

  next=getval(next,prs);

  if (*next==';')
  {
    next=getops(next,&oprs);
    *next++=0;
    strcpy(ops,"%");
    rcount=0;

    while (*( (char *) oprs.addr) != 0 )
    {
      if (toupper(*( (char *) oprs.addr))=='R') rcount++;
      else 
      {
        cats[0]=*( (char *) oprs.addr);
        if (cats[0]!='X') cats[0]=tolower(cats[0]);
        strcat(ops,cats);
      }
      ((char *) oprs.addr)++;
    }
    while (rcount!=0) { strcat(ops,"\r\n"); rcount--; }
  }
  else
  {
    if (prs->evaltype==POINTER) strcpy(ops,"%s");
    else strcpy(ops,"%d");
  }

  return next;
}

/************************************************************************/

void pascal drite(char *cmd)
{
  PRAM prs;
  char ops[100],*next;
  char dummybuf[4000],*dummy;
  static byte drro=1;
  static byte drcol=1;

  if (*cmd==0) errlist(LEASTPRM,1);

  dummy=dummybuf;
  ro=drro; col=drcol;
  for (next=cmd;*next!=0;)
  {
    next=setriteops(next,&prs,ops);

    if (prs.evaltype==POINTER) dummy+=sprintf(dummy,ops,prs.addr);
    else dummy+=sprintf(dummy,ops,prs.value);
  }
  *dummy=0;
  rite(dummybuf);
  drro=ro; drcol=col;
}

void pascal dsup(void)
{
  if (mandind==DSUPI)
  {
		dsrit=(char * cdecl (*)(char *st)) dummy;
		svform=(char * cdecl (*)(char *st,...)) dummy;
  }
  else
  {
    dsrit=rite;
    svform=form;
  }
}

void pascal key(char *cmd)
{
  PRAM prs[2];
  boolean forcechar=FALSE;
	char *next;

#if SYNC
	int olstate;
#endif

	next=getscalasn(cmd,&prs[0]);

  if (*next==';')
  {
    next=getops(next,&prs[1]);
    if ( *((char *)ADDR(1))!='C' ) errlist(OPMUST,'C');

    if (*( (char *) ADDR(1)) =='C') forcechar=TRUE;
    else errlist(OPMUST,'C');
  }
  if (*next!=0) { next++; rits(next); }

#if SYNC
	olstate=keystate;
	keystate=INKEY;
	gorcv(TRUE);
	keystate=olstate;
#endif

#if ASYNC
	while (bioskey(1)==0) updatedisp();
#endif

  switch (ASN(0))
  {
    case INTASN:
      if (forcechar)
      {
        *( (uint *) ADDR(0)) = 0;     /* Old upper byte value of integer. */
        *( (char *) ADDR(0)) = (char) bioskey(0);
      } else *( (uint *) ADDR(0)) = (uint) bioskey(0);
      break;
    case CHARASN: *( (char *) ADDR(0)) = (char) bioskey(0); break;
  }

}

#if ASYNC
static void pascal edstr(char *addr,uint length)
{
  boolean done=FALSE;
	uint *intptr;

	seted(wy()+wintop-1,wx()+winleft-1,addr,length);
	while (!done)
	{
		while (bioskey(1)==0) updatedisp();
		over_char=edbyt();
		for (intptr=over_chars; *intptr!=0; intptr++)
			if (*intptr==over_char) done=TRUE;
	}
  SETLINES(curlines);
}

void pascal edstrcmd(char *cmd)
{
  PRAM prs;
	char *next;

  next=getbuf(cmd,&prs);
  if (*next!=0) { next++; rits(next); }
	edstr(prs.addr,prs.length);
}
#endif

#if SYNC
void pascal edstrcmd(char *cmd)
{
  PRAM prs;
	char *next;
	uint *intptr;
  boolean done=FALSE;
	int olstate;

  next=getbuf(cmd,&prs);
  if (*next!=0) { next++; rits(next); }
	seted(wy(),wx(),prs.addr,prs.length);

	while (!done)
	{
		olstate=keystate;
		keystate=INKEY;
		gorcv(TRUE);
		keystate=olstate;

		over_char=edbyt();
		for (intptr=over_chars; *intptr!=0; intptr++)
			if (*intptr==over_char) done=TRUE;
	}
  SETLINES(curlines);
}
#endif

void pascal redsp(void)
{
  redisplay=TRUE;
  putscrn();
}

void pascal rits(char *cmd)
{
  PRAM prs;
  char ops[100],*next;
  char dummybuf[4000],*dummy;
  byte olsup;

  if (*cmd==0) errlist(LEASTPRM,1);

  dummy=dummybuf;
  for (next=cmd;*next!=0;)
  {
    next=setriteops(next,&prs,ops);

    if (prs.evaltype==POINTER) dummy+=sprintf(dummy,ops,prs.addr);
    else dummy+=sprintf(dummy,ops,prs.value);
  }
  *dummy=0;
  olsup=dosup;
  dosup=FALSE;
  disp(dummybuf);
  dosup=olsup;
}

#define UI(p) *( (uint *) (p) )

void pascal lscrn(char *cmd)
{
  char dummybuf[6000],*nextloc,*lastloc,*datptr,**formptr,**bp,*next;
  ulng wherewe;
  uint uvname,templine;
  PRAM prs;
	PRAM vari;
  USERV *uvptr;
  DISPVAR *disptr;
  FILE *bfile=NULL;

  next=getdest(cmd,&prs);
	if (*next!=0) 
	{
	  getpoint(next,&vari);
		uvptr=NULL;
		bp=vari.paddr;
	}	
  else 
	{
  	if (mandind==LSTVSI) 
    {
      uvname=UI("SV");
      bp=&mydat;
    }
    else 
    {
      uvname=UI("SC");
      bp=&bscrn;
    }
    uvptr=uv;
    while (UI(uvptr->name)!=uvname) uvptr++;
	}	

  switch (prs.evaltype)
  {
    case NONE:    errlist(MUSTPRM,1);
    case POINTER:
    case ISFILE:
      if ((bfile=fopen(prs.addr,"r"))==NULL) errlist(BADFILE,prs.addr);
      break;
    default:
			if (scrbuf!=NULL) errlist(BADFILE,prs.addr);
      wherewe=stell();
			templine=cmdline;
      dogo(&prs);
			cmdline=templine;
      bfile=scrfile;
      break;
  }

  nextloc=dummybuf;
  lastloc=&dummybuf[6000];

  while (fgets(nextloc,lastloc-nextloc,bfile)!=NULL)
  {
    if (toupper(*nextloc)=='E' && toupper(nextloc[1])=='N' && toupper(nextloc[2])=='D') 
    {
      *nextloc=0;
      break;
    }
    nextloc=convrt(nextloc,lastloc-nextloc);
  }

  if (prs.evaltype==ISFILE || prs.evaltype==POINTER) fclose(bfile);
  else fseek(scrfile,wherewe,SEEK_SET);
  
  if (*bp!=NULL) free(*bp);
  if ( (*bp=malloc(nextloc-dummybuf+1))==NULL ) errlist(NOINTMEM);
  memcpy(*bp,dummybuf,nextloc-dummybuf+1);
	if (uvptr==NULL) *(vari.lenptr)=nextloc-dummybuf+1;
	else uvptr->length=nextloc-dummybuf+1;

  if (mandind==LSTVSI)
  {
    datptr=mydat; 
    formptr=dispforms;
    do {
      *formptr++=datptr;
      datptr=strchr(datptr,0);
      datptr++;
    } while (*(datptr-2)!=0);

    disptr=dv; formptr=dispforms;
    while (**formptr!=0) (*disptr++).formstr=*formptr++;
  }

  if (*bp==NULL) errlist(BADSTATE,1000);
  return;
}

void pascal sup(void)
{
  if (dispstate!=CMDBAT) errlist(CANTINT);

  dosup= (mandind==SUPI);
}

void pascal setpr(char *cmd)
{
  PRAM prs;
  if (*cmd==0) { strcpy(prompt,"?"); return; }
  getpoint(cmd,&prs);
  if (strlen(prs.addr)>=sizeof(prompt)) ((char *)prs.addr)[sizeof(prompt)-1]=0;
  strcpy(prompt,prs.addr);
}

void pascal sform(char *cmd)
{
  PRAM prs[2];
  char *next;
  char *fptr;

  next=getscal(cmd,&prs[0],1,0,numdisps );
  getpoint(next,&prs[1]);

  fptr=dv[VALUE(0)].formstr;

  if (fptr!=NULL && fptr!=dispforms[VALUE(0)]) free(fptr);

  if ((dv[VALUE(0)].formstr=malloc(strlen(ADDR(1))+1))==NULL)
  {
    dv[VALUE(0)].formstr=dispforms[VALUE(0)];
    errlist(NOINTMEM);
  }
  strcpy(dv[VALUE(0)].formstr,ADDR(1));
}

void pascal smask(char *cmd)
{
  PRAM prs[2];
  char *next;

  next=getscal(cmd,&prs[0],1,0,numdisps );
  getscal(next,&prs[1],2,0,0xff);

  if (mandind==SMASKI) dv[VALUE(0)].statmask=VALUE(1);
  else dv[VALUE(0)].maxlen=VALUE(1);
}

void pascal stoff(void)
{
	svform=(char * cdecl (*)(char *st,...)) dummy;
}

void pascal ston(void)
{
  svform=form;
}

void pascal vtype(char *cmd)
{
  PRAM prs;

  if (*cmd==0) rite=dirite;
  else
  {
    getscal(cmd,&prs,1,0,0xff);

    switch (toupper(prs.value))
    {
      case 'B': rite=birite; break;
      case 'N': rite=dirite; break;
      default:; /* errlist(?) */
    }
  }
	if (dsrit!=(char * cdecl (*)(char *st)) dummy) dsrit=rite;
}

static uint makerun(byte *buf,uint length)
{
  byte *src,*dst,*end,*p;

  src=dst=buf;
  end=&buf[length];

  while (src<end)
  {
    if (*src!=src[1] || *src!=src[2] || *src!=src[3])
    {
      p=src;
      while (*p==0xfd) p++;
      switch (p-src)
      {
        case 0: *dst++=*src++; break;
        case 1:
          if (dst==src) { memmove(&p[1],p,end-p); end++; src++; }
          *dst++=0xfd; *dst++=0xfd;
          src++;
          break;
        case 2:
          if (dst==src) { memmove(&p[1],p,end-p); end++; src++; }
          *dst++=0xfd;
          *dst++=2;
          *dst++=0xfd;
          src+=2;
          break;
        case 3:
          *dst++=0xfd;
          *dst++=3;
          *dst++=0xfd;
          src+=3;
          break;
      }
    }
    else
    {
      p=src;
      while (*src==*p) p++;
      if ( (p-src) > 0xfc ) p=src+0xfc;
      *dst++=0xFD;
      *dst++=p-src;
      *dst++=src[2]; /* if src==dst, *src and src[1] have been trashed */
      src=p;
    }
  }
  return dst-buf;
}

static uint unrun(byte *dst,byte *src,uint length)
{
  byte *end,*strt;

  strt=dst;
  end=&src[length];
  while (src<end)
  {
    if (*src==0xfd)
    {
      src++;
      if (*src==0xfd)
      {
        *dst++=0xfd;
        continue;
      }
      while ( --src[0]!=0xff ) *dst++=src[1];
      src+=2;
    }
    else *dst++=*src++;
  }
  return strt-dst;
}

void pascal getxtcmd(char *cmd)
{
  PRAM prs[2];
  char *next;
	byte rou,colu,rol,coll;

  next=getpoiasn(cmd,&prs[0]);
  next=getscal(next,&prs[1],1,1,25);
  rou=VALUE(1);
  next=getscal(next,&prs[1],2,1,80);
  colu=VALUE(1);
  next=getscal(next,&prs[1],3,1,25);
  rol=VALUE(1);
  getscal(next,&prs[1],4,1,80);
  coll=VALUE(1);

	LENPV(0)=getxt(&PADDR(0),rou,colu,rol,coll);
}

static uint pascal getxt(char **buf,byte rou,byte colu,byte rol,byte coll)
{
  byte attbuf[3000],chrbuf[3000];
  byte *attptr,*chrptr,*buffer;
  uint attsiz,chrsiz;
  byte far *sc;
  byte lro,lcol;

  attptr=attbuf;
  chrptr=chrbuf;

  if (rite==birite)
  {
    byte olro,olcol;

    olro=wy(); olcol=wx();
    window(1,1,80,25);
    for(lro=rou; lro<=rol; lro++)
    {
      for(lcol=colu; lcol<=coll; lcol++)
      {
        gotoxy(lcol,lro);
        _AH=8; _BH=0; bivid();
        *chrptr++=_AL;
        *attptr++=_AH;
      }
    }
    window(winleft,wintop,winright,winbottom);
    gotoxy(olcol,olro);
  }
  else
  {
	  for(lro=rou; lro<=rol; lro++)
    {
      sc=MK_FP(video_base,lro*160+colu*2-162);
      for(lcol=colu; lcol<=coll; lcol++)
      {
        *chrptr++=*sc++;
        *attptr++=*sc++;
      }
    }
	}
  chrsiz=makerun(chrbuf,chrptr-chrbuf);
  attsiz=makerun(attbuf,attptr-attbuf);
  if ( (*buf=malloc(attsiz+chrsiz+8))==NULL ) errlist(NOINTMEM);
	buffer=(byte *) *buf;
  *buffer++=rou;
  *buffer++=colu;
  *buffer++=rol;
  *buffer++=coll;
  *( (uint *) buffer)++=chrsiz;
  *( (uint *) buffer)++=attsiz;
  memcpy(buffer,chrbuf,chrsiz);
  buffer+=chrsiz;
  memcpy(buffer,attbuf,attsiz);
	return (attsiz+chrsiz+8);
}

void pascal putxtcmd(char *cmd)
{
  PRAM prs;

  getpoint(cmd,&prs);
  if (prs.addr==NULL) errlist(MUSTBUF,prs.pram);

	putxt(prs.addr);
	*(prs.paddr)=NULL;
	*(prs.lenptr)=0;
}

static void pascal putxt(char *buf)
{
  byte attbuf[3000],chrbuf[3000];
  byte *attptr,*chrptr,*buffer;
  uint attsiz,chrsiz;
  char far *sc;
  byte rou,colu,rol,coll,lro,lcol;

	buffer=(byte *) buf;
  if (buffer==NULL) return;

  rou =*buffer++;
  colu=*buffer++;
  rol =*buffer++;
  coll=*buffer++;

  chrsiz=*( (uint *) buffer)++;
  attsiz=*( (uint *) buffer)++;
  chrptr=buffer;
  attptr=&buffer[chrsiz];

  unrun(chrbuf,chrptr,chrsiz);
  unrun(attbuf,attptr,attsiz);

  free(buf);
	buf=NULL;

  chrptr=chrbuf;
  attptr=attbuf;

  if (rite==birite)
  {
    byte olro,olcol;
    olro=wy(); olcol=wx();
    window(1,1,80,25);
    for(lro=rou; lro<=rol; lro++)
    {
      for(lcol=colu; lcol<=coll; lcol++)
      {
        gotoxy(lcol,lro);
        _AL=*chrptr++;
        _BL=*attptr++;
        _AH=9; _BH=0; _CX=1; bivid();
      }
    }
    window(winleft,wintop,winright,winbottom);
    gotoxy(olcol,olro);
    return;
  }

  for(lro=rou; lro<=rol; lro++)
  {
    sc=MK_FP(video_base,lro*160+colu*2-162);
    for(lcol=colu; lcol<=coll; lcol++)
    {
      *sc++=*chrptr++;
      *sc++=*attptr++;
    }
  }

}

void pascal comp(char *cmd)
{
  PRAM prs[2];
  FILE *filein,*fileout;
  char *next;
  byte *buffer;
  uint sln,rln,size;

  next=getfilen(cmd,&prs[0]);
  *next++=0;
  getfilen(next,&prs[1]);
  if ((filein=fopen(ADDR(0),"rb"))==NULL) errlist(BADFILE,ADDR(0));
  if ((fileout=fopen(ADDR(1),"wb"))==NULL) errlist(BADFILE,ADDR(1));

  if (mandind==COMPI)
  {
    for (size=0x7fff; (buffer=malloc(size))==NULL; size-=1000);

    sln=2*size/3;
    while ( (sln=fread(buffer,1,sln,filein))!=0 )
    {
      rln=makerun(buffer,sln);
      fwrite(buffer,1,rln,fileout);
    }

    free(buffer);
  }
  else
  {
    int src,rch,rnum;

    while ( !feof(filein) )
    {
      if ( (src=getc(filein))==EOF && feof(filein)) break;
      if (src==0xfd)
      {
        rnum=getc(filein); /* Should never be EOF */
        if (rnum==0xfd)
        {
          putc(0xfd,fileout);
          continue;
        }
        rch=getc(filein);
        while ( --rnum!=-1 ) putc(rch,fileout);
      }
      else putc(src,fileout);
    }
  }
  fclose(filein);
  fclose(fileout);

}
