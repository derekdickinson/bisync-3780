/* Maximum delay for CTS or TX buffer ready from modem in ticks. */
#define MAXDEL       1000
#define MDMTIME      900

/* Minimum and maximum possible integers. */
#define LOWVAL       0x8000
#define HIGHVAL      32767

/* Buffer sizes. */
#define ABUFSIZE		 200
#define BUFFSIZE     5120
#define CMDBUFSIZ    200
#define FILENAMESIZE 65

/* Send Types (Bit Mapped to bit 2 of buftype) */
#define STAN         0
#define TRAN         4

/* Masks for setting and clearing send type bit of buftype. */
#define TRANSET      0x04
#define TRANCLR      0xFB

/* Keyboard Scan codes. */
#define INS          0x5200
#define HOME         0x4700
#define DEL          0x5300
#define END          0x4f00
#define LARR         0x4b00
#define RARR         0x4d00
#define UARR         0x4800
#define DARR         0x5000
#define BACK         0x0e08
#define TAB          0x0f09
#define ESC          0x011b
#define RET          0x1c0d
#define F1           0x3b00
#define F2           0x3c00
#define F3           0x3d00
#define F4           0x3e00
#define F5           0x3f00
#define F6           0x4000
#define F7           0x4100
#define F8           0x4200
#define F9           0x4300
#define F10          0x4400
#define CTLB         0x3002
#define CTLS         0x1F13
#define CTLT         0x1414

/* ASCII */
#define LF           0x0A
#define CR           0x0D
#define MYEOF        0x1A

/* EBCDIC */
#define STX          0x02
#define ETX          0x03
#define DLE          0x10
#define DC1          0x11
#define DC2          0x12
#define DC3          0x13
#define IGS          0x1D
#define IRS          0x1E
#define ETB          0x26
#define ENQ          0x2D
#define BEL          0x2F
#define EOT          0x37
#define NAK          0x3D
#define ACK1         0x61
#define WACK         0x6B
#define ACK0         0x70
#define RVI          0x7C

/* Possible types for receive (for rxtyp variable). */
#define STANTYPE     0
#define TRANTYPE     1
#define CTRLTYPE     2
#define TTD          3
#define DISCSEQ      4

/* Sending file states (for f_state variable). */
#define START_S      0
#define GOODACK      2
#define TIMEOUT      4
#define BADACK       6
#define SENDONE      8

/* Receiving file states (for f_state variable). */
#define START_R  12
#define ENQIN    14
#define GOODBLK  16
#define BADBLK   18
#define RCVDONE  20

/* Bell related states. */
#define SENDBELL 22
#define RESPBELL 24
#define ISENTBEL 26
#define IGOTBELL 28
#define BELLOUT  30

/* Other states of f_state. */
#define IDLE     10
#define GARBAGE	 32
#define BEGINRCV 34
#define BEGINSND 36

/* Error states. */
#define IDLEOUT  38
#define BAKOUT   40
#define SNAKOUT  42
#define RNAKOUT  44
#define REMABRT  46
#define ACTYOUT  48
#define KILLSEND 50
#define KILLRCV  52
#define DISKERR  54
#define PREMEOT  56
#define LINEDOWN 58
#define RVIKILL  60

/* Errorlevel return values. */
#define QUIT      0
#define ONLINE    1
#define FORCED    2
#define NOPEN     3

/* interrupt vectors. */
#define BREAKVECT    0x1B
#define CTRLCVECT    0x23
#define CRITVECT     0x24

/* keystate values. */
#define NORM      0
#define INRCV     2
#define INSEN     4
#define INMES     6
#define INHLP     8
#define INCMD    10
#define ATPROMPT 12
#define DISPING  14
#define INREMCMD 16
#define INKEY		 18

/* Values for PRAM evaltype, assign members */
#define NOASN        0x0000
#define STRASN       0x0001
#define CHARASN      0x0002
#define INTASN       0x0004
#define POIASN       0x0008
#define PARASN       0x0010

#define SCALAR       0x0010
#define POINTER      0x0020

#define ISFILE       0x0040
#define LABEL        0x0080
#define COMMA        0x0100
#define NONE         0x0200
