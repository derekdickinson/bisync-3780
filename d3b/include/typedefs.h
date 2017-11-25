
typedef unsigned char byte;
typedef unsigned char boolean;
typedef unsigned int  uint;
typedef unsigned long ulng;

typedef struct
{
  uint len;
  char st[BUFFSIZE+3];
  byte strtstate;
} BIGSTR;

typedef struct
{
  uint len;
  char st[3];
} LITSTR;

typedef struct
{
  char filename[FILENAMESIZE];
  byte destyp; /* CONTYP=screen, NORMTYP=disk, SPCTYP=printer, serial port? */
  byte stdcnv;  /* Convert standard receives EBC, EBCNORS, BINARY, HEXPAIR */
  byte trancnv; /* convert transparent receives */
  boolean autoname; /* Autonaming? TRUE/FALSE */
	uint index;		/* Autonaming index */
  FILE *fileptr;
} DEST;

typedef struct thesnd
{
  char filename[FILENAMESIZE];
  byte sensrc;   /* MESBUF, SENFILE */
  char *mesbuf;
  byte prindest; /* PRINTER, PUNCH, DESTFILE */
  byte buftype;  /* Bit Mapped: Bits 01 EBC, EBCNORS, BINARY, HEXPAIR, Bit 2 STAN TRAN */
  byte catype;   /* ETB, ETX */
  struct thesnd *nextsnd; /* Pointer to next value to be sent. */
} SND;

typedef struct
{
  char convr;   /* '%', '&', '*', '#', 0 */
  int value;    /* value of parm for numeric, value pointed to for pointer */
  char **paddr; /* Pointer to the pointer in addr field if POITYP */
  void *addr;   /* address of value for numeric, value of pointer */
  uint *lenptr; /* A pointer to length of the value which addr points to. */
  uint length;  /* Length of pointer arguments. */
  uint evaltype;/* SCALAR, POINTER, FILE */
  uint assign;  /* NOASN, CHARASN, POIASN, INTASN, IARASN,... */
  char *pram;		/* Pointer to the parameter */
} PRAM;

typedef struct
{
  boolean avail;  /* Can the buffer be written too? */
  char *bufbeg;   /* Location of buffer */
  uint buflen;    /* Space allocated for buffer. */
  boolean wrap;   /* Wrap around and destroy old data? */
  char *strt;     /* Start of valid data in buffer Note:if (!wrap) bufbeg==strt) */
  uint len;       /* Before buffer is full, the length of valid data. */
  char *end;      /* Before buffer is full, ptr to first bad data */
  uint left;      /* Before buffer is full, the remaining space in buff */
} BUFPTR;

typedef struct callret
{
  ulng seekto;
  uint line;
  struct callret *nextret;
} CALLRET;

typedef struct
{
  ulng seekval;
  uint line;
  /* Array size is irrelevant. Structure is used as a template, only. */
  char label[100];
} TBLENT;

typedef union
{
  byte abyte;
  uint auint;
  ulng aulng;
  char astr;
} VALTYP;

typedef struct
{
  byte varstat;   /* Status data byte */
  char *formstr;  /* The format string to use when displaying value */
  byte maxlen;    /* The maximum length of the value */
  byte statmask;  /* Mask Checks certain status bits to decide if displayable*/
  char **dispptr; /* Pointer to array of pointers necessary for proper display*/
  VALTYP *dispval;/* Pointer to scalar variable or name to be displayed */
  VALTYP lastval; /* Last value of dispval */
} DISPVAR;

typedef int pascal VSTF(const char *st);
typedef int cdecl VVF(const char *st,...);
typedef void pascal CMDF(char *st);

typedef struct
{
  char *name;
  CMDF *func;
} COMMAND;

typedef struct
{
  char name[2];
  void *addr;  /* address of value for numeric, value of pointer */
  uint length; /* The length of the variable value which is pointed too. */
  uint assign; /* NOASN, CHARASN, POIASN, INTASN, ... */
  uint evaltype; /* SCALAR, POINTER, FILE */
  byte vtype; /* */
} USERV;

typedef struct parstack
{
	uint value;
	void *addr;
	uint length;
	struct parstack *next;
} PARSTACK;

/***************************************************************************/


/* SU */
#define NAMELEN           22
#define NUMLEN            37
#define STSIZE           300
#define MAXVALS            8
#define MODNAMLEN         20
#define MAXSCRENS         10
#define MAXBYTES          10
#define MAXOPS            17
#define MAXOPNAMLEN       24
#define MAXVALNAMLEN       9
#define MAXST             45
#define MAXN              35

#define MAXNAME           35
#define PROGLEN           13
#define PROMPLEN         200
#define MAXEMUNAME        25
#define MAXEMUDIR         60
#define MAXCOMLIN         60
#define NUMENS             5
#define DEFMENS            8

#define MAXEMU             2

/* modopts types */
typedef struct d1
{
  char valname[MAXVALNAMLEN];  /* The name for the value. */
  byte valmask;       /* Mask which when ored with option byte sets the
                         necessary bits in the field to obtain the value. */
  byte screenindexes[MAXSCRENS];  /* Index which indicates which screen
                         that the software should goto or stay in after
                         this value is obtained. */
} THEVALS;

typedef struct d2
{
  char let;  /* The letter which should be pushed to change the option.*/
  char opname[MAXOPNAMLEN];  /* The name of the option. */
  byte numvals;    /* The number of possible values for the options. */
  byte thebyte;    /* The byte which the option affects. */
  byte helpindex;  /* Index into the array of long pointers pointing to help.*/
  byte field_mask; /* Mask which when anded with option byte zeroes out the
                      option's field. */
  uint prompseek;  /* The seek value for prompt corresponding to this option. */
  THEVALS opvals[MAXVALS];  /* The array of possible values for the optoin. */
} OPTION;

typedef struct d3
{
  byte numops;          /* Number of options relevant to this screen. */
  byte defbytes[MAXBYTES]; /* Default option bytes for this screen. */
  uint opseeks[MAXOPS];   /* The options in this screen. */
} OPSCREN;

typedef struct d4
{
  char name[MODNAMLEN]; /* The modem's name. */
  char selval[MAXVALNAMLEN]; /* The shortened name for the Selected column. */
  byte version;         /* First byte of version message. */
  byte numbytes;        /* Number of option bytes. */
  byte AAbyte;          /* The option byte with the Auto-Answer bit. */
  byte AAbit;           /* The Auto-Answer bit. */
  byte privmode;        /* The mode for enabling private line. */
                        /* mode 0 = disable LCD, toggle talk/data */
                        /* mode 1 = set byte "x" equal to zero */
                        /* mode 2 = status setting */
  byte privbyte;        /* Status byte to be effected. */
  byte privmaskand;     /* The "and" mask for enabling private line. */
  byte privmaskor;      /* The "or" mask for enabling private line. */
  uint seekscrn[MAXSCRENS]; /* Index in seek array of screens. */
} MODEMSTRUCT;

typedef struct
{
  char     name[22],
           phone_num[37],
           modstring[MODNAMLEN];

  uint     timeout,
           base;

  byte     link,
           intrupt,
           screen,
           modnum,
           version,
           opbytes[MAXBYTES];
  boolean  ispriv;
  byte     emutype,
           emuind;
} PHENT;

