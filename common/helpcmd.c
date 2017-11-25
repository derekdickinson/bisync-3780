#include <dir.h>
#include <ctype.h>
#include <string.h>
#include <io.h>

#include "include\d3.h"

#define HELPBUF 2000

void pascal help(char *st)
{
#if SCOPE
	disp("The help command is not supportedin this revision [%s]",st);
#else
	struct ffblk ffblk;
  long int arrptr[200];
  FILE *helpfil;
  char helpbuff[HELPBUF],helpfilname[FILENAMESIZE];

  if (*helpstrt==0) sprintf(helpfilname,"%s.HLP",packname);
	else strcpy(helpfilname,helpstrt);

	if (findfirst(helpfilname,&ffblk,0)!=0)
	{
		strcpy(helpbuff,helpfilname);
		sprintf(helpfilname,"%s\\%s",emudir,helpbuff);
	}

  if ( (helpfil=fopen(helpfilname,"rb"))==NULL ) errlist(NOHELP,helpfilname);
  else fread((char *) arrptr, sizeof(long), numcmds, helpfil);

	nocr(st);
  st++;
  if (mandind==HELPQI) st++;

  getind(st); /* Sets mandind to appropriate value. */

  fseek(helpfil, arrptr[mandind], SEEK_SET);
  fread(helpbuff,1,HELPBUF,helpfil);
  disp(helpbuff);
  fclose(helpfil);
#endif	
}