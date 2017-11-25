#include <conio.h>
#include <dos.h>
#include <string.h>
#include <time.h>

#include "include\d3.h"

void pascal lf(char *filename)
{
  PRAM prs;

  getfilen(filename,&prs);

  strcpy(lfname,(char *) prs.addr);
  
  if (strcmp(lfname,"NUL")==0) fprintf(lfptr,"\r\nList File Suspended\r\n");

  NUCLOS(lfptr);

  if (strcmp(lfname,"NUL")==0) return;

  if ((lfptr=fopen(lfname,"ab"))==NULL) errlist(BADFILE,lfname);

	fprintf(lfptr,"\r\nList File On, Script: %s\r\n",scrnameptr);
}

void pascal td(void)
{
  struct time nowtime; struct date nowdate;
  gettime(&nowtime); getdate(&nowdate);
  disp("** Current Time/Date is %.2d:%.2d:%.2d on %.2d/%.2d\r\n",
      nowtime.ti_hour,nowtime.ti_min,nowtime.ti_sec,nowdate.da_mon,nowdate.da_day);
}
