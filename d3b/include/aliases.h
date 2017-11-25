/* Modify the cursor's appearance. */
#define SETLINES(l) { _CX=l; _AH=1; geninterrupt(0x10); }

#define NUCLOS(i) { if (i!=NULL) { fclose(i); i=NULL; }; }

#define DROPDTR() outscc(5,0x65);

#define B(value)  ((int) (0x00ff & value))

#define GOROCOL(ro,col) {                                \
  _DH=ro-1; _DL=col-1; _AH=2; _BH=0; geninterrupt(0x10); }

#define GETROCOL(ro,col) {                               \
  _AH=3; _BH=0; geninterrupt(0x10); ro=_DH+1; col=_DL+1; }

#define KBHIT() (bioskey(1))

#define ABRT()  (KBHIT() && (bioskey(1)==ESC))

#define ABRTE() (KBHIT() && (bioskey(0)==ESC))

#define ENQTIMEOUT() ( (f_state<IDLE) && ((*nowtime-sendtime)>timeout) )

#define KILTIMEOUT() (((*nowtime-sendtime)>timeout))

#define BELLTIMEOUT() ( (f_state==ISENTBEL) && ((*nowtime-sendtime)>beltimeout) )

#define WINDOW(top,left,bottom,right) { 											\
	winleft=left; wintop=top;	winright=right; winbottom=bottom; \
	window(left,top,right,bottom);															}

#define STRLCMP(st1,st2) (strnicmp(st1,st2,strlen(st1))==0)

#define CLRBUF() while (bioskey(1)) bioskey(0);

#define IDLETIMEOUT() ( (idllmt!=0)                  &&\
                        (rxstate==2)                 &&\
                        (f_state>START_R)            &&\
                        (f_state<SENDBELL)           &&\
                        ((*nowtime-sendtime)>idllmt) )

#define NOACTY() ( (natolmt!=0)                 &&\
                   (rxstate==2)                 &&\
                   (f_state==START_R)           &&\
                   ((*nowtime-strtime)>natolmt) )

#define STRTRCV() {                                                 \
  sendrsp=TRUE; ctrlptr=ackptr[curbuf^0x0001]; inptr=&bufs[curbuf]; \
  rcv();                                                            }

#define SENDACK() {                                            \
  ctrlptr=ackptr[curbuf]; curbuf^=0x0001; inptr=&bufs[curbuf]; \
  txstate=CTRLSEND;                                            \
  send(); }

#define SENDBUF() { curbuf^=0x0001; outptr=&bufs[curbuf];         \
  txstate=bufs[curbuf].strtstate;                                 \
  send();                                                         }

#define SENDNAK() { \
	rnakcnt++;				\
  ctrlptr=&nak;			\
	txstate=CTRLSEND; \
	send(); }

#define SENDEOT() { \
  ctrlptr=&eot;			\
	txstate=CTRLSEND; \
	send(); }

#define SENDENQ() { \
  senqcnt++;				\
	ctrlptr=&enq;			\
	txstate=CTRLSEND; \
	send(); }

#define RESEND(type) { txstate=type; send(); }

#define ENQOVR()  (enqlmt!=0 && ((senqcnt-strtenq)>enqlmt))
#define SNAKOVR() (naklmt!=0 && ((snakcnt-strtnak)>naklmt))
#define RNAKOVR() (naklmt!=0 && ((rnakcnt-strtnak)>naklmt))

#define DSR()     (chkline(TRUE))
#define NODSR()   (chkline(FALSE))


#define RESETRCV() {                                                       \
  redisplay=TRUE; shoact();                                                \
  datain=0; rblkcnt=rnakcnt=renqcnt=0; firstblock=TRUE;                    }

#define RCVEOT() {                                                   \
  f_state=START_R; if (rcvends[curbuf^0x0001]==ETB) f_state=PREMEOT; \
  curbuf=1;                                                          }

#define BATDONE() { ritercvbuf(curbuf^0x0001); newbatrcv(); }

/* Aliases for accessing values in PRAM structure. */
#define VALUE(i) prs[i].value
#define ADDR(i)  prs[i].addr
#define EVAL(i)  prs[i].evaltype
#define ASN(i)   prs[i].assign
#define PADDR(i) (*(prs[i].paddr))
#define LENPV(i) (*(prs[i].lenptr))
#define LENVA(i) (prs[i].length)