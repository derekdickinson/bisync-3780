
extern char instdata[];

/* scrnlib.asm */
extern int video_base;
extern int vidmode;
extern byte def_attr;
extern byte grph_attr;
extern byte high_attr;
extern byte cha_attr;
extern byte rev_attr;
extern byte und_attr;
extern byte scrolls;
extern byte graphinit;
extern byte tabwid;
extern uint bitabinit;
extern uint ditabinit;
extern uint ditabs[];
extern uint bitabs[];
extern byte tabinit;
extern byte tablens[];
extern uint wibot;
extern uint wileft;
extern uint wiright;
extern uint witop;

extern boolean dorite;
extern boolean hidecur;
extern boolean insmode;
extern boolean mb;
extern boolean mdminst;
extern boolean redisplay;
extern boolean srd;
extern boolean step;

extern byte actat;
extern byte baklmt;
extern byte dimat;
extern byte dispstate;
extern byte dosup;
extern byte curcol;
extern byte curro;
extern byte edcol;
extern byte edcur;
extern byte edln;
extern byte edro;
extern byte enqlmt;
extern byte hour;
extern byte intrupt;
extern byte key_attr;
extern byte lenstr;
extern byte mandind;
extern byte mdmerr;
extern byte minute;
extern byte promplen;
extern byte second;
extern byte startmask;
extern byte statval;
extern byte themask;
extern byte win_attr;
extern byte winbottom;
extern byte winleft;
extern byte winright;
extern byte wintop;
extern byte whichint;
extern byte vx;
extern byte vy;

extern char *db;
extern char *dosgo;
extern char *dosreturn;
extern char *edst;
extern char *exfile;
extern char *helpstrt;
extern char *labtable;
extern char *llens;
extern char *packname;
extern char *scrbuf;
extern char *scrbufptr;
extern char *scrnameptr;
extern char *stepline;
extern char *strtname;
extern char *strtscr;

extern char abuf[ABUFSIZE];
extern char cfgfilname[FILENAMESIZE];
extern char scrname[FILENAMESIZE];
extern char cmdbuf[CMDBUFSIZ];
extern char copyright[];
extern char curdir[FILENAMESIZE];
extern char defstring[];
extern char dostype;
extern char emudir[FILENAMESIZE];
extern char errmessbuf[100];
extern char lascmd[CMDBUFSIZ];
extern char lfname[FILENAMESIZE];
extern char prompt[30];
extern char senfilname[FILENAMESIZE];

extern char *argvg[11];
extern char *p[10];
extern char *s[10];

extern char **messptrs[30];

extern char * cdecl (*rite)(char *st);
extern char * cdecl (*dsrit)(char *st);
extern char * cdecl (*svform)(char *st,...);

extern int ei;
extern int el;
extern int ep;

extern int g[10];
extern int i[10];

extern int *gptr;
extern int *iptr;

extern jmp_buf env;

extern uint argcg;
extern uint blocksiz;
extern uint bportsc;
extern uint cmdline;
extern uint col;
extern uint curbuf;
extern uint curlines;
extern uint f_state;
extern uint keystate;
extern uint loopvar;
extern uint over_char;
extern uint rblkcnt;
extern uint ro;
extern uint sblkcnt;
extern uint usrmode;

extern uint def_overs[];
extern uint *over_chars;

extern uint *n[20];

extern ulng datain;
extern ulng dataout;
extern ulng timeout;

extern ulng far *nowtime;

extern void interrupt (*old_break)(void);
extern void interrupt (*int_prev)(void);

extern volatile uint rxstate;
extern volatile uint txstate;

extern BIGSTR *inptr;
extern BIGSTR *outptr;

extern BIGSTR bufs[2];

extern CALLRET *calls;

extern FILE *scrfile;
extern FILE *lfptr;

extern VSTF *put;
extern VVF *prin;

extern struct date thedate;
