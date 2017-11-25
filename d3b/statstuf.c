#include <alloc.h>
#include <string.h>
#include <stdlib.h>

#include "include\d3.h"

void pascal restat(void)
{
  themask=1;
  themask|= ( (f_state<9)                << 1);
  themask|= ( (f_state>13 && f_state<21) << 2);
  themask|= ( (act==&ptr)                << 3);
  themask|= ( (act==&pun)                << 4);

  /* Booleans used for status display. */
  srd = (f_state<9) + ((f_state>13 && f_state<21) << 1);
  switch (srd)
  {
    case 0: std=0;                         break;
    case 1: std=((buftype & TRAN)!=0) + 1; break;
    case 2: std=(rxtyp==STANTYPE) ? 1 : (rxtyp==TRANTYPE) << 1 ;
  }

	memleft=coreleft();
  pun.index=atoi(strchr(pun.filename,'.')+1);
  ptr.index=atoi(strchr(ptr.filename,'.')+1);
	rcvtype=(lastdest) ? (lastdest + 2*(lastrcv==TRANTYPE)) : 0;

  switch (statval)
  {
    case OFFLIN: DROPDTR(); getmodfil();
    case DIALING: case GOTONE: case RINGING: case DSRWAIT: break;
    case CONNECTING: case ANSWERING: 
      if (!NODSR()) statval=ONLIN; sync=0x32; 
			outscc(6,sync); outscc(7,sync);
			break;
    case ONLIN: case SNDING: case RCVING:
      switch (themask & 0x06)
      {
        case 0: statval=ONLIN;  break;
        case 2: statval=SNDING; break;
        case 4: statval=RCVING; break;
      }
      if (NODSR()) { statval=OFFLIN; DROPDTR(); sync=0x16; } 
      break;
  }
}
