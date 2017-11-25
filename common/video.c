#include <bios.h>
#include <conio.h>
#include <dir.h>
#include <dos.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include "include\d3.h"

#define DISPBYTE thevar->dispval->abyte
#define DISPUINT thevar->dispval->auint
#define DISPULNG thevar->dispval->aulng
#define DISPASTR ((char *) thevar->dispval)
#define MAXLEN   thevar->maxlen

#define LASTBYTE thevar->lastval.abyte
#define LASTUINT thevar->lastval.auint
#define LASTULNG thevar->lastval.aulng
#define LASTASTR thevar->lastval.auint

#define FORMSTR thevar->formstr

#define STRS thevar->dispptr

static void pascal shovar(DISPVAR *thevar)
{
  if (thevar==NULL) return;
  if ( (thevar->statmask & themask) == 0) return;

  switch (thevar->varstat)
  {
    case 0: return;
    case BINDV:
      if (LASTBYTE==DISPBYTE) return;
      svform(FORMSTR,STRS[DISPBYTE]);
      LASTBYTE=DISPBYTE;
      return;
    case INTV:
      if (LASTUINT==DISPUINT) return;
      svform(FORMSTR,DISPUINT);
      LASTUINT=DISPUINT;
      return;
    case LONGV:
      if (LASTULNG==DISPULNG) return;
      svform(FORMSTR,DISPULNG);
      LASTULNG=DISPULNG;
			return;

#if SYNC
    case SDEST:
      {
        byte ind;
        ind= (sndlist==NULL) ? prindest : sndlist->prindest ;
        if (LASTBYTE==ind) return;
        svform(FORMSTR,STRS[ind]);
        LASTBYTE=ind;
      }
      return;
    case SMODE:
      {
        byte ind;
        ind=(sndlist==NULL) ? buftype : sndlist->buftype;
        if (LASTBYTE==ind) return;
        svform(FORMSTR,STRS[ind]);
        LASTBYTE=ind;
      }
			return;
#endif

    case STRV:
      {
        char *st;
        st=DISPASTR;
        if (chkcrc(st)==LASTASTR) return;
        LASTASTR=chkcrc(st);
        if (MAXLEN!=0 && strlen(st)>MAXLEN) st=&st[strlen(st)-MAXLEN];
        svform(FORMSTR,st);
      }
  }
}

#define STRCHANG(str) (str->lastval.auint!=chkcrc((char *)str->dispval))
#define BYTECHANG(str) (str->lastval.abyte!=str->dispval->abyte)

void pascal shoact(void)
{
#if SYNC
  byte i;
  DISPVAR *actname;
  static DISPVAR *actcnv;

  actname=NULL;

  if ( f_state>13 && f_state<21 )
  {
    themask|=0x04;
    if (act==&ptr)
    {
      actname=&dv[PTRNAMEDV];
      switch (rxtyp)
      {
        case STANTYPE: actcnv=&dv[PTRSTDDV];  break;
        case TRANTYPE: actcnv=&dv[PTRTRANDV]; break;
      }
    }
    else
    {
      actname=&dv[PUNNAMEDV];
      switch (rxtyp)
      {
        case STANTYPE: actcnv=&dv[PUNSTDDV];  break;
        case TRANTYPE: actcnv=&dv[PUNTRANDV]; break;
      }
    }
  }
  else actcnv=NULL;

  if (f_state<9)
  {
    themask|=0x02;
    actname=&dv[SNDNAMEDV];
  }

  for (i=0; i<3; i++)
  {
    if (*(namepos[i]->formstr)==0) continue;
    if (namepos[i]==actname)
    {
      if (actname->formstr[0]!=actat || STRCHANG(actname) )
      {
        actname->lastval.auint=0xff;
        actname->formstr[0]=actat; shovar(actname);
      }
    }
    else
    {
      if (namepos[i]->formstr[0]!=dimat || STRCHANG(namepos[i]))
      {
        namepos[i]->lastval.auint=0xff;
        namepos[i]->formstr[0]=dimat; shovar(namepos[i]);
      }
    }
  }

  for (i=0; i<4; i++)
  {
    if (*(namepos[i]->formstr)==0) continue;
    if (cnvspos[i]==actcnv)
    {
      if ( actcnv->formstr[0]!=actat || BYTECHANG(actcnv) )
      {
        actcnv->lastval.abyte=0xff;
        actcnv->formstr[0]=actat; shovar(actcnv);
      }
    }
    else
    {
      if (cnvspos[i]->formstr[0]!=dimat || BYTECHANG(cnvspos[i]) )
      {
        cnvspos[i]->lastval.abyte=0xff;
        cnvspos[i]->formstr[0]=dimat; shovar(cnvspos[i]);
      }
    }
	}
#endif
}

void pascal updatedisp(void)
{
  static int oldsecond;
  static byte old_day;
  static byte oltop=1;
  static byte olbot=25;
  static byte oleft=1;
  static byte olrit=80;
  DISPVAR **curdisp;
  char dummybuf[20];

/*	form("\2\1\1rx %.2d, tx %.2d, f %.2d ",rxstate,txstate,f_state); */

	if (winbottom!=olbot || wintop!=oltop || winleft!=oleft || winright!=olrit)
  {
    WINDOW(wintop,winleft,winbottom,winright);
		if (!dosup)
		{
	    myputs("\2\1\1\f\3"); /* Clear Screen place cursor. */
		  promp();
		}
    olbot=winbottom; oltop=wintop; oleft=winleft; olrit=winright;
  }

  _AX=0x2c00; geninterrupt(0x21); hour=_CH; minute=_CL; second=_DH;
  if (second != oldsecond)
  {
    oldsecond=second;
    svform(dv[TIMEDV].formstr,( (hour+11) % 12 ) + 1,minute,second);
  }

  getdate(&thedate);
  if (redisplay || thedate.da_day!=old_day)
  {
    svform(dv[DATEDV].formstr,thedate.da_mon,
                   thedate.da_day,
                  (itoa(thedate.da_year,dummybuf,10)+2) );
    old_day=thedate.da_day;
  }

	restat();

  for (curdisp=updisps; *curdisp!=NULL; curdisp++)
  {
    if (redisplay) (*curdisp)->lastval.aulng=-1L;
    shovar(*curdisp);
  }
  redisplay=FALSE;

  shoact();
}

void pascal putscrn(void)
{
  dsrit(bscrn);

  redisplay=TRUE;
  updatedisp();
}

int pascal wx(void)
{
	if (hidecur) return 0x00FF & vx;
	return wherex();
}

int pascal wy(void)
{
	if (hidecur) return (0x00FF & vy);
	return wherey();
}

void pascal gorocol(byte ro,byte col)
{
  if (col > winright-winleft+1) /* Check for row out of window */
  {
    ro+=( (col-1)/(winright-winleft+1) );
    col=(col-1)%(winright-winleft+1)+1;
  }
  if (ro > winbottom-wintop+1) ro=winbottom-wintop+1;
	vx=col;	vy=ro;
  if (hidecur) GOROCOL(26,1)
	else gotoxy(col,ro);
}

void pascal getrocol(byte *ro,byte *col)
{
	if (hidecur) { *ro=vy; *col=vx; return; }
  GETROCOL(*ro,*col);
}

/* Use video mode to determine how and where to write direct video writes. */
void pascal getvidmode(void)
{
	_AH=0x0f; geninterrupt(0x10); vidmode=_AL;

  video_base=0xb800;
  switch (vidmode)
  {
    case 7: video_base=0xb000;
    case 2:
      def_attr=grph_attr=cha_attr=key_attr=0x07;
      high_attr=0x0f;
      rev_attr=0x70;
      und_attr=(vidmode==2) ? 0x07 : 0x01;
      break;
    default:
      vidmode=0x03;
			_AX=0x0003;
			geninterrupt(0x10);
    case 3:
      key_attr=def_attr=0x0e;
      high_attr=0x0b;
      cha_attr=0x0f;
      grph_attr=0x0a;
      rev_attr=0x20;
      und_attr=0x0c;
      break;
  }
}

/* This is used for writing formatted output directly to the screen.
The "dirite" function is a direct video write routine in scrnlib.asm. */
char * cdecl form(char *st, ...)
{
  char dummyst[2500];

  vsprintf(dummyst,st,...);

  rite(dummyst); /* Write resulting string to screen or buffer */

  return &st[strlen(st)+1];
}

void pascal promp(void)
{
  if (wx()!=1) dputs("\r\n");
  if (lfptr!=NULL) fprintf(lfptr,"%.3d ",cmdline);
  dputs(prompt);
  promplen=col;
}

void pascal cleanexit(int errorval)
{
  byte i;

	if (lfptr!=NULL)
	{
	  fprintf(lfptr, "\r\n%s ",packname);
	  for(i=1;i<argcg;i++) fprintf(lfptr,"%s ",argvg[i]);
	  fprintf(lfptr,"-- returned errorlevel %d",errorval);
	  fclose(lfptr);
	}
  CLRBUF();
  exit(errorval);
};

void pascal myputs(char *st)
{
  byte olatt;
  witop=wintop; wileft=winleft; wibot=winbottom; wiright=winright;

  olatt=def_attr; def_attr=win_attr; ro=wy(); col=wx();
  rite(st);
  def_attr=olatt; gorocol(ro,col);

  witop=wileft=1; wibot=25; wiright=80;
}

void cdecl myprintf(char *st,...)
{
  char dummyst[2500];

  vsprintf(dummyst,st,...);

  myputs(dummyst); /* Write resulting string to screen or buffer */
}

void cdecl dputs(char *st, ...)
{
  va_list argptr;       /* Argument list type. */
  char dummyst[2500];

  va_start(argptr,st);          /* Initialize list. */
  vsprintf(dummyst,st,argptr);  /* Use as argument for vsprintf. */
  va_end(argptr);               /* Clean up result. */

  if (dispstate!=CMDINT && lfptr!=NULL) 
	{ 
		if (dummyst[0]==4) fputs(&dummyst[1],lfptr); /* Get rid of video control char */
		else fputs(dummyst,lfptr);
	}
  if (dosup) return;
  put(dummyst);
  return;
}

void cdecl disp(char *st, ...)
{
  va_list argptr;       /* Argument list type. */
  char dummyst[2500];

  va_start(argptr,st);          /* Initialize list. */
  vsprintf(dummyst,st,argptr);  /* Use as argument for vsprintf. */
  va_end(argptr);               /* Clean up result. */

  if (dispstate==CMDINT)
  {
    switch (keystate)
    {
      case INCMD:
        prin("\r\n%s",dummyst);
        if (wx()==1) put(prompt); else put("\r\n");
        edro=wy();
        { byte dummy; dummy=edcur; edcur=0; edrite(); edcur=dummy; }
        gorocol(edro,edcur+edcol);
        return;
      case ATPROMPT: put("\r\n");
      case DISPING:
				put(dummyst);
#if SYNC
				if ( (f_state==START_R) && (!remwaiting) )
#endif
#if ASYNC
				if (f_state==START_R)
#endif
        {
          if (wx()==1) put(prompt); else prin("\r\n%s",prompt);
          keystate=ATPROMPT;
        }
        else keystate=DISPING;
        return;
      case INREMCMD:
        put(dummyst);
        return;
    }
  }
  if (dispstate!=CMDINT && lfptr!=NULL) fputs(dummyst,lfptr);
  if (dosup) return;
  put(dummyst);
  return;
}

void pascal cls(void)
{
  myputs("\2\1\1\f\3");
}