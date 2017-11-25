
/* commands.h */
extern COMMAND cmds[];

/* dispvars.h */
extern char *mydat;
extern char *dispforms[];
extern DISPVAR dv[];

/* ragstrs.h */
extern char *errmessages[];
extern char *fileops[];
extern char *fopsnams[];
extern char *noyes[];
extern char *prinpunch[];
extern char *rcvcnv[];
extern char *sndmodename[];
extern char *srs[];
extern char *statmess[];
extern char *sts[];
extern char *escstr[];

/* scrns.h */
extern char *bscrn;
extern char critscrn[];

/* tables.h */
extern byte rcvtable[];
extern byte sentable[];

/* uservars.h */
extern uint version;
extern USERV uv[];

extern boolean as;
extern boolean co;
extern boolean comsend;
extern boolean destsent;
extern boolean dce;
extern boolean dialing;
extern boolean fixedblanks;
extern boolean firstblock;
extern boolean hasp;
extern boolean modata;
extern boolean pb;
extern boolean remcmds;
extern boolean remwaiting;
extern boolean sendmess;
extern boolean sendrsp;
extern boolean skipone;
extern boolean std;
extern boolean tb;
extern boolean vff;
extern boolean vfs;

extern byte buftype;
extern byte cmend;
extern byte ioc;
extern byte lastdest;
extern byte lastrcv;
extern byte naklmt;
extern byte numcmds;
extern byte numdisps;
extern byte prindest;
extern byte rcvtype;
extern byte sync;

extern byte defbytes[MAXBYTES];
extern byte rcvends[2];
extern byte sendends[2];
extern byte strtstate[];

extern char *crlf;
extern char *eschar;
extern char *messptr;
extern char *remptr;

extern char messtring[];
extern char moname[30];
extern char remcmdstring[];

extern uint aportsc;
extern uint base_address;
extern uint bportsc;
extern uint contsc;
extern uint datasc;
extern uint dsrsc;
extern uint idllmt;
extern uint memleft;
extern uint natolmt;
extern uint preclen;
extern uint remver;
extern uint renqcnt;
extern uint rnakcnt;
extern uint rreclen;
extern uint senqcnt;
extern uint snakcnt;
extern uint strtenq;
extern uint strtnak;
extern uint talksc;
extern uint thedel;

extern uint def_addrs[];
extern uint mandsums[];

extern ulng beltimeout;
extern ulng sendtime;
extern ulng strtime;

extern void interrupt (*int_mine[])(void);
extern void interrupt (*int_prevs[4])(void);

extern volatile byte ctrlbyt;
extern volatile byte error;
extern volatile byte rxtyp;

extern BUFPTR ptrbuf;
extern BUFPTR punbuf;
extern BUFPTR rbuf;

extern DEST ptr;
extern DEST pun;

extern DEST *act;

extern DISPVAR *cnvspos[];

extern DISPVAR *namepos[];

extern DISPVAR *updisps[];

extern FILE *sendfile;

extern LITSTR ack0;
extern LITSTR ack1;
extern LITSTR bel;
extern LITSTR disc;
extern LITSTR enq;
extern LITSTR eot;
extern LITSTR nak;
extern LITSTR rvi;
extern LITSTR ttd;
extern LITSTR wack;

extern LITSTR *ctrlptr;

extern LITSTR *ackptr[];

extern MODEMSTRUCT themodem;

extern SND *sndlist;

