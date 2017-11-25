
GLOBAL   _datasc:WORD       ; The port addresses.
GLOBAL   _contsc:WORD
GLOBAL   _talksc:WORD

GLOBAL   _aportsc:WORD
GLOBAL   _bportsc:WORD

GLOBAL   _inptr:WORD    ; Input and output buffer pointers.
GLOBAL   _outptr:WORD
GLOBAL   _ctrlptr:WORD
GLOBAL   _ctrlbyt:BYTE

GLOBAL   _txstate:WORD   ;global byte variable (unsigned char)
GLOBAL   _rxstate:WORD

GLOBAL   __brklvl:WORD   ;Pointer to the top of the heap

GLOBAL   _error:BYTE
GLOBAL   _sync:BYTE
GLOBAL   _rxtyp:BYTE
GLOBAL   _mdminst:BYTE
GLOBAL   _dispstate:BYTE

GLOBAL   _sendends:BYTE
GLOBAL   _rcvends:BYTE

GLOBAL   _curbuf:WORD

GLOBAL   _sendrsp:BYTE

GLOBAL   _nowtime:DWORD
GLOBAL   _sendtime:DWORD

GLOBAL   _old_break:DWORD
GLOBAL   _int_prev:DWORD

GLOBAL   _video_base:WORD
GLOBAL   _critscrn:BYTE
GLOBAL   _def_attr:BYTE

GLOBAL   _intrupt:BYTE
GLOBAL   _whichint:BYTE

GLOBAL   _birite:PROC

; Procedures in intrupt.asm to be used in C modules.
GLOBAL   _int_handler:PROC
GLOBAL   _outscc:PROC
GLOBAL   _setseg:PROC
GLOBAL   _ctrlc:PROC
GLOBAL   _cbreak:PROC
GLOBAL   _criterr:PROC
GLOBAL   _int2:PROC
GLOBAL   _int3:PROC
GLOBAL   _int4:PROC
GLOBAL   _int5:PROC
GLOBAL   _int_detect:PROC