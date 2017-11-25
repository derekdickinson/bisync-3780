#include <stdio.h>
#include <conio.h>
#include <dir.h>
#include <string.h>
#include <io.h>
#include <stdlib.h>
#include <dos.h>

#define TRUE  1
#define FALSE 0

typedef unsigned char byte;
typedef unsigned char boolean;

void main(void)
{
  char packname[13],strtname[13],scrname[13],hlpname[13],exename[100];
  FILE *thefile;
  char a=0,b=0,c=0,d=0,e=0,f=0,g=0,h=0,length;
  boolean success;
  struct ftime filetime;

  clrscr();

  printf(
	"The package name is the name of the base part of the program.\r\n"
	"It is used to find the program (in the path) under DOS 2.0.\r\n"
	"Input package name:"
	);
  gets(packname);

  printf(
	"The help file contains helps on the different commands.\r\n"
	"Input help file name:"
	);
  gets(hlpname);

  printf(
	"The startup file name is the name of the initial script which executes\r\n"
	"before the modem is initialized. This file is useful for customizing the\r\n"
	"package.\r\n"
	"Input startup (configuration) file name:"
	);
  gets(strtname);

  printf(
	"The default script file name is the initial value given to the script filename.\r\n"
	"If non-interactive mode is specified and no script filename is given then this\r\n"
	"name is used.\r\n"
	"Input default script file name:"
	);
  gets(scrname);

  printf("Input file in which to install [D3-3780.EXE by default]:");
  gets(exename);
  if (strlen(exename)==0) strcpy(exename,"D3-3780.EXE");

  if ( (thefile=fopen(exename,"r+b")) == NULL)
  {
    printf("Unable to open \"%s\"",exename);
    exit(1);
  }

  getftime( fileno(thefile), &filetime);

  printf("\nSearching ....");

  do
  {
    a=b;
    b=c;
    c=d;
    d=e;
    e=f;
    f=g;
    g=h;
    h=getc(thefile);
  } while( ! ( (a=='®') && (b=='®') && (c=='¯') && (d=='¯') &&
               (e=='{') && (f=='{') && (g=='„') && (h=='„')    ) );

  success=TRUE;

  while (TRUE)
  {
	  fseek(thefile,0,SEEK_CUR);
    length=strlen(packname)+1;
    if (fwrite(&packname,1,length,thefile)!=length)
	    {	success=FALSE;	break;	}
			
    fseek(thefile,13-length,SEEK_CUR);
    length=strlen(hlpname)+1;
    if (fwrite(&hlpname,1,length,thefile)!=length) 
			{	success=FALSE; break; }

    fseek(thefile,13-length,SEEK_CUR);
    length=strlen(strtname)+1;
    if (fwrite(&strtname,1,length,thefile)!=length) 
			{	success=FALSE; break; }

    fseek(thefile,13-length,SEEK_CUR);
    length=strlen(scrname)+1;
    if (fwrite(&scrname,1,length,thefile)!=length) 
			{	success=FALSE; break; }
			
    break;
  }

  fclose(thefile);

  /* Now reset the date code so the file time will not be disturbed. */
  thefile=fopen(exename,"r");
  setftime( fileno(thefile), &filetime);
  fclose(thefile);

  if (success) printf("\r\nNames have been recorded.\r\n");
  else printf("\r\nWrite error while attempting to record the names.\r\n");
}
