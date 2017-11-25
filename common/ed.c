#include <bios.h>
#include <conio.h>
#include <dos.h>
#include <mem.h>
#include <stdlib.h>
#include <string.h>

#include "include\d3.h"

static void pascal dochar(uint ch);
static void pascal insert(char ch,char *st,uint i);

#define DELETE(st,i,n) strcpy(&st[i-1],&st[i+n-1])
#define DELETEM(st,i,n) strcpy(&st[i],&st[i+n])

#define BAKSPC() if (--edcur>edln) edcur++;                 gorocol(edro,edcur+edcol)
#define CURLEF() dorite=FALSE; if (--edcur>edln) edcur++;   gorocol(edro,edcur+edcol)
#define CURIGT() dorite=FALSE; if (++edcur>lenstr) edcur--; gorocol(edro,edcur+edcol)
#define CURHOM() dorite=FALSE; edcur=0;                     gorocol(edro,edcur+edcol)
#define CUREND() dorite=FALSE; edcur=lenstr;                gorocol(edro,edcur+edcol)
#define DOINS()  {                   \
  dorite=FALSE; insmode=!insmode;    \
  if (insmode)                       \
    { SETLINES(curlines); }          \
  else                               \
    { SETLINES(curlines & 0x00ff); } }
#define DOCLR()                                     \
  myprintf("\2%c%c\x1d%c",edro,edcol,edcol+lenstr); \
  setmem(edst,edln,0); edcur=0; gorocol(edro,edcol)

static void pascal insert(char ch,char *st,uint i)
{
  memmove(&st[i+1],&st[i],strlen(&st[i])+1);
  st[i]=ch;
}

static void pascal dochar(uint ch)
{
  if ((ch & 0x00ff)==0) return;
  if (insmode)
  {
    if (edcur<edln)
    {
      if (lenstr>=edln) edst[lenstr-1]=0;
      insert((byte) ch,edst,edcur);
    }
    edst[edln]=0;
  }
  else edst[edcur]=(byte) ch;
  edrite();
  dorite=FALSE;
  if (++edcur>edln) edcur--; gorocol(edro,edcur+edcol);
}

void pascal edrite(void)
{
  byte oldef;
  witop=wintop; wileft=winleft; wibot=winbottom; wiright=winright; 

  oldef=def_attr;
  def_attr=key_attr;

  ro=edro; col=edcol+edcur;
  form("\x10\6%s ",&edst[edcur]);
  edro-=scrolls;

  def_attr=oldef; witop=wileft=1; wibot=25; wiright=80;
}

void pascal seted(byte rov,byte colv,char *st,byte len)
{
  _AH=3; _BH=0; geninterrupt(0x10); curlines=_CX;
  edst=st; edro=rov; edcol=colv; edln=len-1;
  gorocol(rov,colv);
  insmode=TRUE; edcur=0;
  edrite();
}

uint pascal edbyt(void)
{
  uint ch;

  dorite=TRUE;
  lenstr=strlen(edst);
  ch=bioskey(0); 
  switch (ch)
  {
    case INS : DOINS();               break;
    case HOME: CURHOM();              break;
    case END : CUREND();              break;
    case LARR: CURLEF();              break;
    case RARR: CURIGT();              break;
    case BACK: BAKSPC();
    case DEL : DELETEM(edst,edcur,1); break;
    case F10 : DOCLR();               break;
    case ESC :
    case RET : dorite=FALSE;          break;
    default: dochar(ch);
  }
  if (dorite) edrite();
  return ch;
}

