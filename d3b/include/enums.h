
/* DISPVAR types */
enum { BINDV=1,INTV,LONGV,SDEST,SMODE,STRV,DATEV,TIMEV };

/* For sensrc of SND structure. */
enum { MESBUF, SENFILE, REMCMD };

enum { FALSE, TRUE };

enum { PRINTER, PUNCH, DESTFILE };

/* Used for initial value of txstate when sending. */
enum { STANSEND=2, TRANSEND=14, CTRLSEND=26 };

/* for status messages (the statval variable). */
enum { 
OFFLIN,  ANSWERING, CONNECTING, DIALING, GOTONE, 
RINGING, DSRWAIT,   ONLIN,      SNDING,  RCVING };

/* Buffer Types (Bit Mapped to bits 0 and 1 of buftype)
Also the values for stdcnv and trancnv states. */
enum { VARY, FIXED, BINARY, HEXPAIR };

/* dispstate screen variable. */
enum { STARTING, CMDINT, CMDBAT, ENDING };

/* rcvtyp values */
enum { LPT1TYP=0, LPT2TYP=1, NORMTYP, CONTYP, SPCTYP };

/* EI values, Recoverable */
enum {
NOERROR=0,     ABDISC=1,       BADFILE=-2,     BADFILE2=-3,    BUSYSIG=4,
CANTDEL=-5,    CANTOFF=6,      CANTON=7,       CANTPROG=8,     CURSEND=9,
DISKRCV=10,    DSROUT=11,      IDLELMT=12,     LOOPOUT=13,     NOACTIM=14,
NOCTS=15,      NONEFOUND=-16,  NOTFOUND=-17,   NOTXBUF=18,     NOTONE=19,
NOTOPEN=20,    READEOF=21,     RNAKERR=22,     RREMABRT=23,    SBADACKLMT=24,
SENQLMT=25,    SNAKERR=26,     SREMABRT=27,    TOOBIG=28,      TIMEDOUT=29,

/* ei values, Unrecoverable */
ARGOVER=50,    ALEXISTS=-51,   BADAMP=-52,     BADBRAK=-53,    BADCARAT=-54,
BADCMD=55,     BADEND=-56,     BADIND=-57,     BADLBL=-58,     BADLINE=-59,
BADMATCH=60,   BADNUM=61,      BADOPR=62,      BADOPRP=63,     BADSTATE=64,
BADTYP=-65,    BADVAR=-66,     CANTINT=67,     CMDFILRERR=68,  OUTRANGE=-69,
ERRHIGH=70,    ERRLOW=71,      FNAMESIZE=-72,  INDBIG=-73,     LEASTPRM=74,
MDMERR=75,     MUSTASN=-76,    MUSTBUF=-77,    MUSTDEST=-78,   MUSTDEST2=-79,
MUSTFIL=-80,   MUSTINTASN=-81, MUSTLEN=-82,    MUSTOPR=-83,    MUSTPASN=-84,
MUSTPOINT=-85, MUSTPRM=86,     MUSTSCAL=-87,   MUSTSCASN=-88,  MUSTSPEC=-89,
MUSTVAL=-90,   NOCOLON=91,     NOHELP=-92,     NOINTMEM=93,    NOLABEL=-94,
NOMATCH=95,    NOSTACK=96,     NORET=-97,      NOSPECS=98,     NOTHDEV=99,
OPMUST=100,    OPRBAD=101,     TYPMISS=-102,   RETOVER=103,    STRFIL=-104,
STRSPEC=-105,  MOOPTS=106,     NORESPNS=107,   ENTNO=-108,     NOMODEM=109,
NOINTRUPT=110, USRABRT=111
};

/* Lastdest values */
enum { NONEYET, PTR, PUN };

/* modata values */
enum { INITVAL, LINEUP, NOMODATA, CANTTALK, BADVER, GOODSTUF };

/* mandind */
enum { 
GENI,   P0I,    P1I,    P2I,    P3I,    P4I,    P5I,    P6I,    P7I,    P8I,
LABELI, CMENTI, DOS0I,  DOS1I,  DOS2I,  AAI,    ALLOCI, ALLFBI, ALLSTI, APPI,
ASTATI, BDNI,   BDPI,   BUFI,   CI,     CALLI,  CATI,   CLALLI, CLOSEI, CLRCA,
CLSI,   COMPI,  COPYI,	CPYI,   DECI,   DELI,   DIRI,   DLI,    DNI,    DPI,    
DRITEI,	DSI,    DSUPI,  EDSTRI, ENDI,   ERI,    EXI,    FREEI,  FQI,    FRITEI,	
FSTATI, GBUFI,  GOI,    GETXTI, HELPI,  HELPQI, IFI,    IFEI,   IFEFI,  IFGI,		
IFII,   IFOI,   IFSI,   IFTI,   ILI,    INCI,   INTRPI, INTERI,	KEYI,		KLI,    
LDI,    LFI,    LOADI,  LSCRNI, LSTVSI, LWRI,   MI,     MOI,    MOMEI,	MOVI,   
MSETHI, MVI,    ONI,    OPENI,  OPRI,   PI,     PBUFI,  POPI,		PUSHI,	PUTXTI, 
QI,			RDLNI,  READI,  REALLI, REDIRI, REDSPI, REMI,   RENI,   RETI,   RITEI,	
SI,     SAVEI,  SCTRLI, SEEKI,  SETBFI, SETMPI, SETPRI, SFORMI, SIZEI,  SLENI,	
SMASKI, SMAXI,  SOI,    SPTRI,  SRITEI, STCI,   STOFFI,	STONI,  STSTI,  SUPI,   
TDI,    TELLI,  UNCMPI, UNDSBI, UNSUPI, UPRI,		VTYPEI, WI,     XBI };

/* dispvars INDEXS */
enum {
BUFDV,         DIRDV,          INDV,           OUTDV,          PTRAUTODV,
PTRNAMEDV,     PTRSTDDV,       PTRTRANDV,      PUNAUTODV,      PUNNAMEDV,
PUNSTDDV,      PUNTRANDV,      RBLKDV,         RENQDV,         RNAKDV,
SBLKDV,        ACTPRINDV,      SNDNAMEDV,      SENQDV,         SNAKDV,
SRVARDV,       STATMDV,        STDV,           DATEDV,         TIMEDV
};

enum {
STRTYP,  CHARTYP,  UINTYP,   DSTYP,    NXTYP,    PXTYP,    POITYP,   RTTYP,
IARTYP,  PARTYP
};

enum {
FSTRT, FEXIT, FCON, FCALL, FMAIN
};