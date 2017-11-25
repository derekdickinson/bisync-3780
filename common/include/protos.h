/* from scrnlib.asm */
void cdecl bivid(void);
uint cdecl chkcrc(char *st);
void cdecl scroll(void);
char * cdecl dirite(char *st);
char * cdecl birite(char *st);

/* File BRANCMDS.C */
boolean pascal sgets(char *buf);
long pascal stell(void);
void pascal sclose(void);
boolean pascal sfeof(void);
void pascal setllens(void);
void pascal dogo(PRAM *prs);
void pascal call(char *cmd);
void pascal clrca(void);
void pascal end(void);
void pascal go(char *line);
void pascal ifcond(char *st);
void pascal ife(char *cmd);
void pascal ififs(char *cmd);
void pascal ifo(char *st);
void pascal ift(char *cmd);
void pascal il(char *value);
void pascal inter(void);
void pascal on(char *st);
void pascal ret(void);
void pascal xb(char *filename);

/* File DIAGCMDS.C */
void pascal lf(char *filename);
void pascal td(void);

/* File DYNACMDS.C */
void pascal allstuf(char *cmd);
void pascal dofree(char *cmd);
void pascal setmp(char *cmd);

/* File ED.C: */
void pascal edrite(void);
void pascal seted(byte ro,byte col,char *st,byte len);
uint pascal edbyt(void);

/* File ERRSTUF.C */
void cdecl errlist(int errnum,...);
char * pascal getbuf(char *pram, PRAM *prs);
char * pascal getdest(char *pram, PRAM *prs);
char * pascal getfilen(char *pram, PRAM *prs);
char * pascal getlen(char *pram, PRAM *prs, PRAM *prsbuf);
char * pascal getops(char *pram, PRAM *prs);
char * pascal getpoiasn(char *pram, PRAM *prs);
char * pascal getpoint(char *pram, PRAM *prs);
char * pascal getscal(char *pram, PRAM *prs, byte pnum, int lowval, int highval);
char * pascal getscalasn(char *pram, PRAM *prs);
char * pascal getspec(char *pram, PRAM *prs);
char * pascal getstorfil(char *pram, PRAM *prs);
char * pascal getval(char *pram, PRAM *prs);

/* File FILECMDS.C */
void pascal filebuf(char *cmd);
void pascal filehands(char *cmd);
void pascal del(char *filename);
void pascal dir(char *drive);
void pascal ren(char *st);
void pascal size(char *cmd);

/* File GENCMDS.C */
void pascal dosit(char *dosbuf);
void pascal intrp(char *cmd);
void pascal pa(char *cmd);
void pascal redir(char *cmd);

/* File HELPCMD.C */
void pascal help(char *st);

/* File NUMBCMDS.C */
void pascal mathfuncs(char *cmd);
void pascal pushpop(char *cmd);

/* File PARPRAMS.C */
char * pascal pramset(char *pram,PRAM *vals);

/* File PARSER.C: */
void pascal nocr(char *buffer);
boolean pascal cmdproc(void);
byte pascal getind(char *buf);
void pascal parcmd(void);
void pascal concmd(void);
void pascal doex(void);
void pascal filecmd(byte ctype);
void pascal reperror(void);
void pascal stripblanks(char *st);
void pascal dummy(void);

/* File SCRNCMDS.C */
char * pascal setriteops(char *next,PRAM *prs,char *ops);
void pascal drite(char *cmd);
void pascal dsup(void);
void pascal getxtcmd(char *cmd);
void pascal key(char *cmd);
void pascal edstrcmd(char *cmd);
void pascal redsp(void);
void pascal rits(char *cmd);
void pascal lscrn(char *cmd);
void pascal putxtcmd(char *cmd);
void pascal sup(void);
void pascal setpr(char *cmd);
void pascal sform(char *cmd);
void pascal smask(char *cmd);
void pascal smax(char *cmd);
void pascal stoff(void);
void pascal ston(void);
void pascal undsp(void);
void pascal vtype(char *cmd);
void pascal comp(char *cmd);

/* File STRCMDS.C */
void pascal strfuncs(char *cmd);
void pascal lwr(char *cmd);
void pascal setbf(char *cmd);
void pascal slen(char *cmd);
void pascal sptr(char *cmd);
void pascal srite(char *cmd);
void pascal upr(char *cmd);

/* File TERMCMDS.C */
void pascal die(char *cmd);
void pascal fq(void);

/* File VIDEO.C: */
void pascal shoact(void);
void pascal updatedisp(void);
void pascal putscrn(void);
int pascal wx(void);
int pascal wy(void);
void pascal gorocol(byte ro,byte col);
void pascal getrocol(byte *ro,byte *col);
void pascal getvidmode(void);
char * cdecl form(char *st, ...);
void pascal promp(void);
void pascal cleanexit(int errorval);
void pascal myputs(char *st);
void cdecl myprintf(char *st,...);
void cdecl dputs(char *st,...);
void cdecl disp(char *st,...);
void pascal cls(void);