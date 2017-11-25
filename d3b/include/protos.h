/* from inthandl.asm */
void interrupt int_handler(void);
void cdecl outscc(byte reg,byte val);
void cdecl setseg(void);
void interrupt ctrlc(void);
void interrupt cbreak(void);
void interrupt criterr(void);
void interrupt int_detect(void);
void interrupt int2(void);
void interrupt int3(void);
void interrupt int4(void);
void interrupt int5(void);

/* File BUFCMDS.C */
void pascal rbufs(char *cmd);

/* File BUFRCV.C: */
void pascal ritercvbuf(byte buf);
void pascal unwrapbuffs(void);

/* File BUFSEND.C: */
void pascal loadsenbuf(byte buf);

/* File CONNCMDS.C */
void pascal getmodfil(void);
void pascal aa(char *timeout);
void pascal c(void);
void pascal dl(char *number);
void pascal ds(void);
void pascal kl(void);
void pascal m(void);
void pascal mo(void);
void pascal mome(char *cmd);
void pascal mv(void);
void pascal so(char *cmd);
void pascal pause(char *time,boolean dodsr);

/* File GETDTA.C: */
boolean pascal chkline(boolean dsr);
uint pascal ahextoi(char *asciihex);
char * pascal convrt(char *str,uint max);
void pascal mseth(char *cmd);
void pascal getdiskdta(void);
void pascal verimodem(boolean checkall);

/* File LISEND.C: */
void pascal listsend(void);
void pascal newsend(byte buf);
void pascal send(void);

/* File MAIN.C: */
int pascal nextname(char lastname[]);

/* File MDMSTART.C: */
void pascal setup(void);
void pascal cleanup(void);
int pascal inscc(uint reg);
void pascal instint(void);
void pascal initialize(void);

/* File RCV.C: */
void pascal desclose(DEST *dest);
boolean pascal rcv(void);
void pascal receive(char *timest);
void pascal defdest(char *line);
void pascal gorcv(boolean isinter);
void cdecl mdmerrlist(int errnum,...);

/* File REMSTUF.C: */
void pascal rcvfilerr(void);
void pascal setdestype(DEST *what);
void pascal setremcmd(void);
void pascal proremcmd(void);

/* File SENDCMDS.C */
void pascal sctrl(char *cmd);
void pascal freesndlist(void);
char * pascal nqstrchr(char *st,char ch);
void pascal sendf(char *what);
void pascal rem(char *cmd);

/* File STATSTUF.C */
void pascal restat(void);
