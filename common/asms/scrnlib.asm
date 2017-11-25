        IDEAL
        %TITLE   "Screen handling routines."

        DOSSEG
        MODEL MEDIUM

include "\common\asms\scrnmacs.inc"
include "\common\asms\scrnmacb.inc"

GLOBAL  _video_base:WORD
GLOBAL  _vidmode:WORD
GLOBAL  _ro:WORD
GLOBAL  _col:WORD
GLOBAL  _def_attr:BYTE
GLOBAL  _high_attr:BYTE
GLOBAL  _cha_attr:BYTE
GLOBAL  _grph_attr:BYTE
GLOBAL  _rev_attr:BYTE
GLOBAL  _und_attr:BYTE
GLOBAL  _graphinit:BYTE
GLOBAL  _wibot:WORD
GLOBAL  _wileft:WORD
GLOBAL  _wiright:WORD
GLOBAL  _witop:WORD
GLOBAL  _tabwid:BYTE
GLOBAL  _scrolls:BYTE


GLOBAL  _chkcrc:PROC
GLOBAL  _bivid:PROC
GLOBAL  _scroll:PROC
GLOBAL  _dirite:PROC
GLOBAL  _birite:PROC

DATASEG
_video_base   DW    0B800h
_vidmode      DW    03h

_def_attr     DB    0Eh
_high_attr    DB    0Fh
_cha_attr     DB    0Bh
_grph_attr    DB    0ah
_rev_attr     DB    2eh
_und_attr     DB    01h
_tabwid       DB    10
_scrolls      DB    0

_wibot        DW    25
_witop        DW    1
_wileft       DW    1
_wiright      DW    80

graphlim      DB    7Fh
_graphinit    DB    7Fh

LABEL         ditabn WORD
 DW  thend,    reloc,   newloc,  putcur,  minesc,  anybyt,   litstr,  bell
 DW  backspc,  tab,     lf,      scroll,  clrscr,  cr,       flhtog,  hitog
 DW  grhtog,   newatt,  defatt,  chaatt,  hiatt,   grphatt,  revatt,  undatt
 DW  putbyt,   putbyt,  putbyt,  putbyt,  replin,  repclr,   putcol,  repseq 

LABEL         bitabn WORD
 DW  thendb,   relocb,  newlocb, putcurb, minescb, anybytb,  litstrb, bellb
 DW  backspcb, tabb,    lfb,     scrollb, clrscrb, crb,      flhtogb, hitogb
 DW  grhtogb,  newattb, defattb, chaattb, hiattb,  grphattb, revattb, undattb
 DW  putbytb,  putbytb, putbytb, putbytb, replinb, repclrb,  putcolb, repseqb

LABEL         ditabmin WORD
 DW  thend,    putbyt,  putbyt,  putbyt,  putbyt,  putbyt,   putbyt,  bell
 DW  backspc,  tab,     lf,      putbyt,  putbyt,  cr

LABEL         bitabmin WORD
 DW  thendb,   putbytb, putbytb, putbytb, putbytb, putbytb,  putbytb, bellb
 DW  backspcb, tabb,    lfb,     putbytb, putbytb, crb

; Address of jump table used for dirite and birite
ditabaddr     DW    ditabn
bitabaddr     DW    bitabn

; Value loaded into ditabaddr and bitabaddr at the start of a write
_ditabinit    DW    ditabn
_bitabinit    DW    bitabn

; The table addresses so that C can assign values to the two inits above
_ditabs       DW    ditabn,ditabmin
_bitabs       DW    bitabn,bitabmin

; The lengths of the jump tables above
tablen        DB    ' '
_tabinit      DB    ' '
_tablens      DB    ' ',0Eh

CODESEG

; ax=crc
; si=stri
; cx
; bx
; dx=0A001h

PROC            _chkcrc
								ARG stri:WORD
                push   bp
                mov    bp,sp
                push   si
                push   di

                xor    ax,ax
                xor    bx,bx
                mov    dx,0A001h
								mov    si,[stri]
@@sloop:
                mov    bl,[BYTE si]   ; 8+5
                cmp    bx,0           ; 4
                je     @@done         ; 4
                inc    si             ; 2
                mov    cx,7           ; 4          17

@@nloop:
                mov    di,bx          ; 2                  2
                xor    di,ax          ; 3                  3
                and    di,1           ; 4                  4
                jz     @@iszero       ; 4 or 16            4   12
                shr    ax,1           ; 2                            2
                xor    ax,dx          ; 3                            3
                jmp    SHORT @@nzero  ; 15                          15
@@iszero:
                shr    ax,1           ; 2                       2
@@nzero:
                shr    bx,1           ; 2                  2   14   20
                loop   @@nloop        ; 15 or 5
                                      ;                  32*7 + 15*6 + 5
                mov    di,bx          ; 2
                xor    di,ax          ; 3
                and    di,1           ; 4
                jz     @@iszero1      ; 4 or 16
                shr    ax,1           ; 2
                xor    ax,dx          ; 3
                jmp    @@sloop        ; 15                 ~ 29
@@iszero1:
                shr    ax,1           ; 2
                jmp    @@sloop        ; 15
@@done:
                pop    di
                pop    si
                mov    sp,bp
                pop    bp
                ret
ENDP            _chkcrc

PROC            _bivid
                pushr   <bp,si,di>
                int     10h
                popr    <di,si,bp>
                ret
ENDP            _bivid

PROC            _scroll
                pushr   <ax,cx,dx,bp,si,di>
                add     [_scrolls],al ; Allow other functions to check for
                mov     ch,[BYTE _witop]   ;  a scroll in the video writes.
                dec     ch
                mov     cl,[BYTE _wileft]
                dec     cl
                mov     dh,[BYTE _wibot]
                dec     dh
                mov     dl,[BYTE _wiright]
                dec     dl
                mov     ah,06h
                or      al,al   ; Set flags without changing value
                jns     @@pos   ; Negative number means scroll down
                mov     ah,07
                neg     al
@@pos:
                int     10h
                popr    <di,si,bp,dx,cx,ax>
                ret
ENDP            _scroll

PROC            colover
                push    ax
                xor     ax,ax
                mov     al,dl
                inc     ax
                sub     al,[BYTE _wileft]
                mov     cl,[BYTE _wiright]
                inc     cl
                sub     cl,[BYTE _wileft]
                div     cl
                add     dh,al
                mov     dl,ah
                add     dl,[BYTE _wileft]
                dec     dl
                pop     ax
                ret
ENDP            colover

PROC            roover
                mov     cx,ax     ; Save ax value in cx register
                mov     al,dh
                inc     al
                sub     al,[BYTE _wibot]
                mov     bh,[BYTE _def_attr]
                call    _scroll
                mov     dh,[BYTE _wibot]
                dec     dh
                mov     bh,0
                mov     ax,cx     ; Restore original ax value
                ret
ENDP            roover

PROC            _dirite
								ARG     stri:WORD
; si is pointer into string, di is pointer into screen, ah contains attribute
;
                strtMA
                setrocol
                setstos

@@getbyt:
                lodsb                 ;load byte from ds:di string and auto inc di

                cmp     al,[tablen]
                jb      esc_seq

putbyt:
                inc     dl
                cmp     dl,[BYTE _wiright]
                jae     @@adjwind
@@windone:
                set_di

                cmp     al,[graphlim] ;Check for graphics character
                ja      graph
                stosw
                jmp     @@getbyt

@@adjwind:      adjwind 
                jmp     @@windone

esc_seq:                              ; Jump table to following labels.
                dojmp   ditabaddr     ; Never drops through

graph:          graphMA
                jmp     @@getbyt

reloc:          relocMA
                jmp     @@getbyt

newloc:         newlocMA
                jmp     @@getbyt

putcur:         putMA
                jmp     @@getbyt

minesc:         minescMA
                jmp     @@getbyt

anybyt:         anybytMA
                jmp     @@getbyt

litstr:         litstrMA
                jmp     @@getbyt

bell:           bellMA
                jmp     @@getbyt

backspc:        backspcMA
                jmp     @@getbyt

tab:            tabMA
                jmp     @@getbyt

lf:             lfMA
                jmp     @@getbyt

scroll:         scrollMA
                jmp     @@getbyt

clrscr:         clrscrMA
                jmp     @@getbyt

cr:             crMA
                jmp     @@getbyt

flhtog:         flhtogMA
                jmp     @@getbyt

hitog:          hitogMA
                jmp     @@getbyt

grhtog:         grhtogMA
                jmp     @@getbyt

newatt:         newattMA
                jmp     @@getbyt

defatt:         defattMA
                jmp     @@getbyt

chaatt:         chaattMA
                jmp     @@getbyt

hiatt:          hiattMA
                jmp     @@getbyt

grphatt:        grphattMA
                jmp     @@getbyt

revatt:         revMA
                jmp     @@getbyt

undatt:         undMA
                jmp     @@getbyt

replin:         replinMA
                jmp     @@getbyt

repclr:         repclrMA
                jmp     @@getbyt

putcol:         putcolMA
                jmp     @@getbyt

repseq:         repMA
                jmp     @@getbyt

thend:
                endMA
ENDP            _dirite

;               bh = page = 0
;               bl = attribute = [BYTE LOW _def_attr]
;
PROC            _birite
								ARG     stri:WORD
; si is pointer into string, di is pointer into screen, ah contains attribute
;
                strtMAB
                mov     al,[_tabinit]
                mov     [tablen],al
                mov     al,[_graphinit]
                mov     [graphlim],al
                mov     ax,[_bitabinit]
                mov     [bitabaddr],ax
                mov     [_scrolls],0
                setrocol
								mov     si,[stri]         ;strg address for lodsb instruct
                mov     bl,[BYTE _def_attr]
@@getbytb:                            
                lodsb                 ;load byte from ds:si string and auto inc si

                cmp     al,[tablen]
                jb      esc_seqb

                cmp     al,[graphlim] ;Check for graphics character
                ja      graphb
putbytb:
                stosMA
                jmp     @@getbytb
graphb:
                graphMAB
                jmp     @@getbytb

esc_seqb:                             ; Jump table to following labels.
                dojmpb  bitabaddr

relocb:         relocMA
                jmp     @@getbytb

newlocb:        newlocMA           ;Same as that of rite
                jmp     @@getbytb

putcurb:        putMAB
                jmp     @@getbytb

minescb:        minescMAB
                jmp     @@getbytb

anybytb:        anybytMAB
                jmp     @@getbytb

litstrb:        litstrMA
                jmp     @@getbytb

bellb:          bellMAB
                jmp     @@getbytb

backspcb:       backspcMA
                jmp     @@getbytb

tabb:           tabMAB
                jmp     @@getbytb

lfb:            lfMAB
                jmp     @@getbytb

scrollb:        scrollMAB
                jmp     @@getbytb

clrscrb:        clrscrMAB
                jmp     @@getbytb

crb:            crMAB
                jmp     @@getbytb

flhtogb:        flhtogMAB
                jmp     @@getbytb

hitogb:         hitogMAB
                jmp     @@getbytb

grhtogb:        grhtogMAB
                jmp     @@getbytb

newattb:        newattMAB
                jmp     @@getbytb

defattb:        defattMAB
                jmp     @@getbytb

chaattb:        chaattMAB
                jmp     @@getbytb

hiattb:         hiattMAB
                jmp     @@getbytb

grphattb:       grphattMAB
                jmp     @@getbytb

revattb:        revMAB
                jmp     @@getbytb

undattb:        undMAB
                jmp     @@getbytb

replinb:        replinMAB
                jmp     @@getbytb

repclrb:        repclrMAB
                jmp     @@getbytb

putcolb:        putcolMAB
                jmp     @@getbytb

repseqb:        repMAB
                jmp     @@getbytb
thendb:
                endMAB
ENDP            _birite


END
