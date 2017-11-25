ACK0      equ    70h
ACK1      equ    61h
ENQ       equ    2dh
EOT       equ    37h
DLE       equ    10h
STX       equ    2
ETX       equ    3
ETB       equ    26h

BREAKVECT equ    1Bh
CTRLCVECT equ    23h
CRITVECT  equ    24h

FALSE     equ    0
TRUE      equ    1

PAD       equ    0FFH

CTRLSEND  equ    26

PADMAX    equ    3
NUMPADS   equ    4
WHENCRC   equ    4

; The below states apply to the "_txstate" variable.
;
; Standard Send states.
SENDSYNS  equ    2
SENDSTX   equ    4
SENDATA   equ    6
SENDETX   equ    8
SENDCRCS  equ   10
SENDPADS  equ   12
;
; Transparent Send states.
SENTSYNS  equ   14
SENTDLE1  equ   16
SENTSTX   equ   18
SENTATA   equ   20
SENTDLE   equ   22
SENTDLE2  equ   24
;
; Control message send states.
SENPSYN   equ   26
SENPATA   equ   28

; These apply to the the "_rxstate" variable.
;
; Standard Receive states.
RCVSTRT   equ    2
RCVDATA   equ    4
RCVCRCS   equ    6
;
; Transparent Receive states.
RCVTSTX   equ    8
RCVTATA   equ   10
RCVTDLE   equ   12
;
; Control Receive state.
RCVPATA   equ   14
;
; Modem Receive states
MDMSTRT   equ   16
MRCVDATA  equ   18
MGOTETX   equ   20

; These states apply to the "_fl_states"
;
; States for sending a file.
ENQ1SND   equ    2
ENQSND    equ    4
BLKSND    equ    6
EOTSND    equ    8
FRSTRCV   equ   10
RESPRCV   equ   12
LASTRCV   equ   14
;
; States for receiving a file.
ENQ1_RCV  equ   16
BLK_RCV   equ   18
EOT_RCV   equ   20
ACK_SND   equ   22
NAK_SND   equ   24
ACK_LAST  equ   26

STRUC   BIGSTR
len     dw   ?
bu      db   500 dup(?)
ENDS    BIGSTR

BUFFSIZE  equ   10240