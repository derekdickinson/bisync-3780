#include <dos.h>

#include "include\d3.h"

/*---------------------------------------------------------------------------*/

static void pascal ena8259(int n)
{
  int intmask;

  intmask=inportb(0x21);            /* get interrupts */
  intmask&=~(0x0001 << n);
  disable();
  outportb(0x21,intmask);
  enable();
}

void pascal setup(void)
{
  startmask=inportb(0x21);
  old_break=getvect(BREAKVECT);

  setvect(BREAKVECT,cbreak);
  setvect(CTRLCVECT,ctrlc);
  setvect(CRITVECT,criterr);
}

void pascal cleanup(void)
{
  setvect(BREAKVECT,old_break);
  setvect(intrupt+8,int_prev); /* set previous interrupt vector back. */
  outportb(0x21,startmask);
}

void pascal instint(void)
{
  txstate=0;
  rxstate=0;
  setvect(intrupt+8,int_handler);    /* install interrupt vector */
  ena8259(intrupt);                  /* Enable its interrupt */
}

int pascal inscc(uint reg)
{
  int theval;

  outportb(contsc,reg);
  theval=inportb(contsc);
  return(theval);
}

void pascal initialize(void)
{
  /* outscc(9,0xc0);                force reset*/
  delay(6);
  outscc(4,0x00);               /* sync mode,8 bit sync char*/
  outscc(10,0x00);              /* nrz coding*/
  outscc(2,0x00);               /* INTERRUPT vector*/
  outscc(6,sync);               /* sync char=0x32 or 0x16*/
  outscc(7,sync);               /* sync char=0x32 or 0x16*/
  outscc(3,0xc0);               /* rcv 8 bit characters*/
  outscc(5,0xe4);               /* dtr,rts,tx 8 bit characters*/
  outscc(9,0x06);               /* status low,no vector,vector includes status
                                    master interrupt disabled*/
  outscc(11,0x08);              /* set up clock pins*/
  outscc(12,0x00);              /* time constant - low*/
  outscc(13,0x00);              /* time constant - high*/
  outscc(14,0x00);              /* BR disable, DTR normal*/
  outscc(0,0x80);               /* write register 0, reset tx crc latch*/
  outscc(15,0x40);              /* disable all ext. status interrupts */
  outportb(contsc,0x10);        /* reset any external status ints twice*/
  outportb(contsc,0x10);
  outscc(1,0x00);               /* tx int disable*/
  outscc(9,0x0d);               /* Enable master interrupt */
}
/*---------------------------------------------------------------------------*/
