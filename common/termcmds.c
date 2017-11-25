#include <dos.h>
#include <conio.h>
#include <string.h>

#include "include\d3.h"

void pascal die(char *cmd)
{
  PRAM prs;

  getscal(cmd,&prs,1,0,0xff);

  if (mandind==QI)
  {
    ds();
    DROPDTR();
  }
  doex();
  /* Do not drop DTR but exit anyhow. */
  cleanexit(prs.value);
}

void pascal fq(void)
{
  doex();
  DROPDTR();
  cleanexit(ei);
}

