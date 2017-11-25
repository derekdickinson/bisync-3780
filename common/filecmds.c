#include <io.h>
#include <dir.h>
#include <conio.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys\stat.h>

#include "include\d3.h"

#define RETX     fileops[0]
#define WRTX     fileops[1]
#define APTX     fileops[2]

#define REPLTX   fileops[4]
#define WRPLTX   fileops[5]
#define APPLTX   fileops[6]

#define REBI     fileops[8]
#define WRBI     fileops[9]
#define APBI     fileops[10]

#define REPLBI   fileops[12]
#define WRPLBI   fileops[13]
#define APPLBI   fileops[14]

void pascal filebuf(char *cmd)
{
  PRAM prs[3];
  char *next;
  FILE *thefile;
  char *oprams;

  next=getval(cmd,&prs[0]);

  next=getfilen(next,&prs[1]);
  *next++=0;
  if (EVAL(0)==NONE || EVAL(1)==NONE) errlist(LEASTPRM,2);

  getlen(next,&prs[2],&prs[0]);

  switch (mandind)
  {
    case APPI:  oprams=APBI; break;
    case LOADI: oprams=REBI; break;
    case SAVEI: oprams=WRBI; break;
  }

  if ((thefile=fopen(ADDR(1),oprams))==NULL )
  {
    char thech;

    if (mandind==LOADI) errlist(BADFILE,ADDR(1));

    thech=oprams[0];
    oprams[0]='w';
    if ( (thefile=fopen(ADDR(1),oprams))==NULL )
    {
      oprams[0]=thech;
      errlist(BADFILE,ADDR(1));
    }
    oprams[0]=thech;
  }

  if (mandind==LOADI)
  {
    if (prs[0].length==0) { fclose(thefile); errlist(MUSTBUF,prs[0].pram); }
    switch (ASN(0))
    {
      case POIASN|STRASN:
      case STRASN:  fread(ADDR(0),VALUE(2),1,thefile);         break;
      case INTASN:  fscanf(thefile,"%d",ADDR(0));              break;
      case CHARASN: *((char *) ADDR(0))=(char) fgetc(thefile); break;
      case NOASN:   errlist(MUSTASN,prs[0].pram);
    }
    fclose(thefile);
    return;
  }

  if (EVAL(0)==SCALAR)
  {
    fprintf(thefile,"%d ",VALUE(0));
    fclose(thefile);
    return;
  }

  switch (EVAL(2))
  {
    case POINTER:
    case SCALAR:  fwrite(ADDR(0),VALUE(2),1,thefile);        break;
    case NONE:    fputs(ADDR(0),thefile);                    break;
  }
  fclose(thefile);
}

/*
astat
clall
close han
fstat han
frite han,val[;ops][+val[;ops]]...
read han,val[;ops][+val[;ops]]...

gbuf han,buf[,val]
rdln han,ptr[,val]
pbuf han,buf[,val]

open han,file[;ops]

ifef han,eofdest,noteofdest
seek han,wherenum[,refnum]
tell han,varnum
*/
void pascal filehands(char *cmd)
{
  static FILE *fptrs[10]={ NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
  PRAM prs[3];
  char *next;
  int origin;

  next=getscal(cmd,&prs[0],1,0,9);
  if ( EVAL(0)==NONE || *next==0 )
  {
    switch (mandind)
    {
      case ASTATI: case CLALLI: break;
      case CLOSEI: case FSTATI: if (EVAL(0)==NONE) errlist(MUSTPRM,1); break;
      case TELLI: errlist(MUSTPRM,2);  break;
      default:    errlist(LEASTPRM,2); break;
    }
  }
  if (EVAL(0)!=NONE &&
      mandind!=OPENI  &&
      mandind!=IFEFI   &&
      fptrs[VALUE(0)]==NULL) errlist(NOTOPEN,VALUE(0));

  switch (mandind)
  {
    case ASTATI:
      {
        byte num;
        struct stat buffer;
        for (num=0; num<10; num++)
        {
          if (fstat(fileno(fptrs[num]),&buffer)==0)
          disp("Handle %d, Size is %ld bytes, Time stamp is %s\r",
            num,buffer.st_size, ctime(&buffer.st_atime) );
        }
      }
      break;
		case CLALLI: { byte i; for (i=0; i<10; i++) NUCLOS(fptrs[i]); } break;
		case CLOSEI: NUCLOS(fptrs[VALUE(0)]); break;
    case FRITEI:
      {
        char ops[100];

        while (*next!=0)
        {
          next=setriteops(next,&prs[1],ops);
          if (EVAL(1)==POINTER) fprintf(fptrs[VALUE(0)],ops,ADDR(1));
          else fprintf(fptrs[VALUE(0)],ops,VALUE(1));
        }
      } break;
    case FSTATI:
      {
        struct stat buffer;
        if (fstat(fileno(fptrs[VALUE(0)]),&buffer)==0)
        {
          disp("Size is %ld bytes, Time stamp is %s",
            buffer.st_size,ctime(&buffer.st_atime) );
        }
      }
      break;
    case GBUFI:
      next=getval(next,&prs[1]);
      getlen(next,&prs[2],&prs[1]);
      if (prs[1].length==0) errlist(MUSTBUF,prs[1].pram);
      if (feof(fptrs[VALUE(0)])) errlist(READEOF);
      switch (ASN(1))
      {
        case STRASN|POIASN:
        case STRASN:  fread(ADDR(1),1,VALUE(2),fptrs[VALUE(0)]); break;
        case INTASN:  fread(ADDR(1),1,2,fptrs[VALUE(0)]);        break;
        case CHARASN: fread(ADDR(1),1,1,fptrs[VALUE(0)]);        break;
        default: errlist(MUSTASN,prs[1].pram);
      }
      break;
    case IFEFI:
      *next++=0;
      next=getdest(next,&prs[1]);
      getdest(next,&prs[2]);
      if (fptrs[VALUE(0)]==NULL) { dogo(&prs[1]); break; }
      if (feof(fptrs[VALUE(0)])) { dogo(&prs[1]); break; }
      else dogo(&prs[2]);
      break;
    case OPENI:
      {
        byte opind;
        next=getfilen(next,&prs[1]);
        getops(next,&prs[2]);

        *next=0;
        opind=0;
        if ( EVAL(2)==POINTER || EVAL(2)==ISFILE )
        {
          while (*( (char *) ADDR(2)) != 0 )
          {
            switch (*( (char *) ADDR(2)))
            {
              case 'R': opind&=0xfc;              break;
              case 'W': opind&=0xfc; opind|=0x01; break;
              case 'A': opind&=0xfc; opind|=0x02; break;
              case 'B': opind|=0x08;              break;
              case 'P': opind|=0x04;              break;
            }
            ((char *) ADDR(2))++;
          }
        }
        if ( (fptrs[VALUE(0)]=fopen(ADDR(1),fileops[opind]))==NULL)
        {
          if ( (opind & 0x03)==0 && EVAL(2)==NONE )
          {
            opind++;
            if ( (fptrs[VALUE(0)]=fopen(ADDR(1),fileops[opind]))==NULL)
              errlist(BADFILE2,ADDR(1),fopsnams[opind]);
          }
          else errlist(BADFILE2,ADDR(1),fopsnams[opind]);
        }
        disp("File \"%s\" opened in \"%s\" mode.",ADDR(1),fopsnams[opind]);
      }
      break;
    case PBUFI:
      next=getval(next,&prs[1]);
      getlen(next,&prs[2],&prs[1]);
      switch (EVAL(1))
      {
        case POINTER: fwrite(ADDR(1),1,VALUE(2),fptrs[VALUE(0)]); break;
        case SCALAR:  fwrite(&VALUE(1),1,2,fptrs[VALUE(0)]);      break;
      }
      break;
    case RDLNI:
      next=getbuf(next,&prs[1]);
      getlen(next,&prs[2],&prs[1]);
      if (feof(fptrs[VALUE(0)])) errlist(READEOF);
      fgets(ADDR(1),VALUE(2),fptrs[VALUE(0)]);
      break;
    case READI:
      {
        char ops[100];
        if (feof(fptrs[VALUE(0)])) errlist(READEOF);
        while (*next!=0)
        {
          next=setriteops(next,&prs[1],ops);
          if ((ASN(1) & (STRASN|INTASN|CHARASN)) == 0) errlist(MUSTASN,prs[1].pram);
          strlwr(ops);
          fscanf(fptrs[VALUE(0)],ops,ADDR(1));
        }
      } break;
    case SEEKI:
      next=getscal(next,&prs[1],2,LOWVAL,HIGHVAL);
      getscal(next,&prs[2],3,0,2); /* If no parameter, value set to zero.*/
      switch (VALUE(2))
      {
        default: origin=SEEK_SET; break; /* case 0: */
        case 1: origin=SEEK_CUR; break;
        case 2: origin=SEEK_END; break;
      }
      fseek(fptrs[VALUE(0)],(int) VALUE(1),origin);
      break;
    case TELLI:
      getscalasn(next,&prs[1]);
      *((uint *) ADDR(1))=(uint) ftell(fptrs[VALUE(0)]);
      break;
  }
}

void pascal del(char *filename)
{
  struct ffblk dirinfo;
	char drive[3],ddir[66],name[9],ext[5];
  byte numb=0;
  PRAM prs;

  *getspec(filename,&prs)=0;
  if (prs.evaltype==NONE) errlist(MUSTPRM,1);

	fnsplit(prs.addr,drive,ddir,name,ext);

  if (findfirst(prs.addr,&dirinfo,0)==0)
  {
    do
    {
			fnsplit(dirinfo.ff_name,NULL,NULL,name,ext);
			fnmerge(prs.addr,drive,ddir,name,ext);
      if (unlink(prs.addr)==-1) errlist(CANTDEL,prs.addr);
      numb++;
    } while (findnext(&dirinfo)==0);

    disp("%d File(s) deleted\r\n",numb);
  }
  else disp("No files found (using \"%s\")\r\n",prs.addr);

}

void pascal dir(char *drive)
{
  PRAM prs;
  struct ffblk dirinfo;
	char dirpath[100],name[9],ext[5];
	static char *crtab[]={ "\r\n","\t" };

  *getspec(drive,&prs)=0;

  if (prs.evaltype==NONE) strcpy(dirpath,"*.*");
  else strcpy(dirpath,(char *) prs.addr);

  /* Request and display a directory in wide format. */
  switch (dirpath[strlen(dirpath)-1])
  {
    case ':':
    case '\\':strcat(dirpath,"*.*"); break;
  }

  if (findfirst(dirpath,&dirinfo,0)==0)
  {
    byte olwid;

    olwid=tabwid;
		tabwid=16;
		fnsplit(dirinfo.ff_name,NULL,NULL,name,ext);
		disp("%-9s%-3s",name,&ext[1]);
    while (findnext(&dirinfo)==0)
    {
			fnsplit(dirinfo.ff_name,NULL,NULL,name,ext);
			disp("%s%-9s%-3s",crtab[(winright-winleft-wx()>16)],name,&ext[1]);
			ext[1]=0; /* Stop bleed through from prior dir, (fnsplit only clears firs byte if name has no extension) */
    }
    tabwid=olwid;
  }
  else disp("No files found (using \"%s\")",dirpath);
  disp("\r\n");
}

static void pascal dowild(char *ud,char *d,char *us)
{
	*ud=0;
	while (*d!=0)
	{
		switch (*d)
		{
			case '*': strcat(ud,us);						return;
			case '?': *ud=(*us!=0) ? *us : ' ';	break;
			default:  *ud=*d;
		}
		if (*us) us++;
		*(++ud)=0;
		d++;
	}
}

void pascal ren(char *st)
{
	FILE *sfile,*dfile;
  PRAM prs[2];
	byte numb=0;
	char sourcename[FILENAMESIZE],destname[FILENAMESIZE],buffer[1000];
	char sdrive[3],sdir[66],usname[9],usext[5];
	char ddrive[3],ddir[66],dname[9],dext[5],udname[9],udext[5];
	char *next,*saddr,*daddr;
  struct ffblk dirinfo;

  next=getspec(st,&prs[0]);
  if (*next==0) errlist(MUSTPRM,2);
  *next++=0;
  getspec(next,&prs[1]);

	saddr=ADDR(0); daddr=ADDR(1);

	fnsplit(daddr,ddrive,ddir,dname,dext);
	fnsplit(saddr,sdrive,sdir,NULL,NULL);

	if (findfirst(saddr,&dirinfo,0)==0)
	{
		do
		{
			fnsplit(dirinfo.ff_name,NULL,NULL,usname,usext);

			dowild(udname,dname,usname);
			dowild(udext,dext,usext);
			
			if (udext[1]==0) udext[0]=0; /* in effect: if udext=="." then udext="" */

			fnmerge(sourcename,sdrive,sdir,usname,usext);
			fnmerge(destname,ddrive,ddir,udname,udext);
			if (strcmp(sourcename,destname)==0) continue;
			if (mandind==RENI) 
			{
				disp("Renaming \"%s\" to \"%s\"\r\n",sourcename,destname);
				
				if (rename(sourcename,destname)==0) numb++;
				else 
				{
					switch (errno)
					{
						case EACCES: errlist(ALEXISTS,destname);
						case ENOENT: errlist(NOTFOUND,sourcename);
						case EXDEV:  errlist(NOTHDEV);
					}
				}
			}
			else 
			{
				if (numb==0) unlink(destname);
				disp("Copying \"%s\" to \"%s\"\r\n",sourcename,destname);
				if ((sfile=fopen(sourcename,"rb"))==NULL) errlist(BADFILE,sourcename);
				if ((dfile=fopen(destname,"ab"))==NULL) errlist(BADFILE,destname);
				while (!eof(fileno(sfile))) 
					write(fileno(dfile),buffer,read(fileno(sfile),buffer,1000));
				fclose(sfile); fclose(dfile);
				numb++;
			}
			
		} while (findnext(&dirinfo)==0);
		
		if (mandind==RENI) disp("%d File(s) renamed\r\n",numb);
		else disp("%d File(s) copied\r\n",numb);
	}	else errlist(NOTFOUND,saddr);
}

void pascal size(char *cmd)
{
  struct stat buffer;
  char *next;
  PRAM prs[3];

  next=pramset(cmd,&prs[0]);
  if (ASN(0) != INTASN) errlist(MUSTINTASN,1,prs[0].pram);

  if (*next==';')
  {
    next=pramset(next,&prs[1]);
    if (ASN(0) != INTASN) errlist(MUSTINTASN,2,prs[1].pram);
  }
  else ASN(1)=NONE;

  next=getfilen(next,&prs[2]);

  if (stat((char *) ADDR(2),&buffer)!=0) errlist(BADFILE,ADDR(2));

  *( (uint *) ADDR(0) )=(uint) buffer.st_size;

  if (ASN(1)==INTASN) *( (uint *) ADDR(1) )= (uint) (buffer.st_size >> 16);
  else
  {
    if ((uint) (buffer.st_size >> 16) > 0) errlist(TOOBIG);
  }
}

