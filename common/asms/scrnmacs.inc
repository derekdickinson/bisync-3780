; Shift register "which" to the left "num" places.
;
MACRO    shlm which,num
  REPT num
         shl   &which,1
  ENDM
ENDM
;--------------------------------------------------------------------------
; Shift register "which" to the right "num" places.
;
MACRO    shrm which,num
  REPT num
         shr   &which,1
  ENDM
ENDM
;--------------------------------------------------------------------------
;--------------------------------------------------------------------------
; Multypush.
;
MACRO    pushr thelist
  IRP thereg, <thelist>
         push   &thereg
  ENDM
ENDM
;--------------------------------------------------------------------------
; Multypop
;
MACRO    popr thelist
  IRP thereg, <thelist>
         pop    &thereg
  ENDM
ENDM
;--------------------------------------------------------------------------

MACRO    dorocol
         LOCAL   rofine,colfine
         cmp     dl,[BYTE _wiright]
         jb      rofine
         call    colover
rofine:
         cmp     dh,[BYTE _wibot]
         jb      colfine
         call    roover
colfine:
ENDM

MACRO    setrocol
         mov      dh,[BYTE _ro]
         add      dh,[BYTE _witop]
         sub      dh,2
         mov      dl,[BYTE _col]
         add      dl,[BYTE _wileft]
         sub      dl,2
         dorocol
         dec      dl
ENDM

; This is performed every output character so is somewhat optimized.
;
; The following code is more descriptive but slower (compares middle only)
;  2      mov     cx,ax    ;  Save ax value
;  3      xor     bx,bx    ;  Zero out bx
;  2      mov     bl,dl    ;  col
;  2      shl     bx,1     ;  bx=2*col
;  4      mov     ax,160
;70-77    mul     dh             ;ro
;  3      add     ax,bx
;  2      mov     di,ax
;  2      mov     ax,cx
; 90+ cycles -- times are for an 8088. Speed is much less critical on others.
;
; Note: ax and dx must be preserved
;
MACRO    set_di
         xor     bx,bx    ;  3 - Zero out bx
         mov     cx,bx    ;  2 - Zero out cx
         mov     bl,dl    ;  2 - col
         shl     bx,1     ;  2 - bx=2*col

         mov     cl,dh    ;  2 - ch (row) in cl
         shlm    cx,5     ; 10 - cx=32*row
         mov     di,cx    ;  2 - di=32*row
         shlm    cx,2     ;  4 - cx=4*32*rows=128*rows
         add     di,bx    ;  3 - di=32*row+2*col
         add     di,cx    ;  3 - di=(128+32)*row+2*col=160*row+2*col
                          ; 33 cycles
ENDM     

MACRO    adjwind
         mov     dl,[BYTE _wileft]
         dec     dl

         inc     dh
         cmp     dh,[BYTE _wibot]
         jb      @@windone

         dec     dh
         push    ax
         mov     bh,ah
         mov     al,1
         call    _scroll
         pop     ax
ENDM


MACRO    checkwind
         LOCAL   set_loc

         inc     dl
         cmp     dl,[BYTE _wiright]
         jb      set_loc

         mov     dl,[BYTE _wileft]
         dec     dl

         inc     dh
         cmp     dh,[BYTE _wibot]
         jb      set_loc

         dec     dh
         push    ax
         mov     bh,ah
         mov     al,1
         call    _scroll
         pop     ax
set_loc:
         set_di
ENDM

MACRO    strtMA
         push    bp
         mov     bp,sp
;         sub     sp,AUTOSIZE
         push    si
         push    es
         push    di
         push    bp

; Set initial values
         mov     al,[_tabinit]
         mov     [tablen],al
         mov     al,[_graphinit]
         mov     [graphlim],al
         mov     ax,[_ditabinit]
         mov     [ditabaddr],ax
         mov     [_scrolls],0
ENDM

MACRO    endMA
         add     dl,3
         sub     dl,[BYTE _wileft]
         mov     [BYTE _col],dl

         add     dh,2
         sub     dh,[BYTE _witop]
         mov     [BYTE _ro],dh

         mov     ax,si
         cld
         pop     bp
         pop     di
         pop     es
         pop     si
         mov     sp,bp
         pop     bp
         ret
ENDM


MACRO    setstos
         mov     es,[_video_base]  ;put base address in es for STOSW
				 mov     si,[stri]         ;strg address for lodsb instruct
         mov     ah,[BYTE _def_attr]
ENDM

MACRO    graphMA
         mov     cx,ax
         mov     ah,[_grph_attr]
         stosw
         mov     ax,cx
ENDM

MACRO    dojmp   table
         xor     bx,bx
         mov     bl,al
         shl     bx,1
         add     bx,[&table]
         jmp     [WORD bx]
ENDM

; Used in repseq and repclr sequences
MACRO    repl
         LOCAL   theloop,inwind
theloop:
         push    bx
         checkwind        ; Sets di, trashes bx,cx
         pop     bx

         xor     cx,cx
         mov     cl,dl
         add     cx,bx
         cmp     cx,[_wiright]
         jbe     inwind

         push    dx
         mov     cx,[_wiright]
         xor     dh,dh
         sub     cx,dx
         sub     bx,cx
         rep     stosw
         pop     dx

         mov     dl,[BYTE _wileft]
         sub     dl,2
         inc     dh
         jmp     theloop
inwind:
ENDM


;---------------------------------------------------------------------------

MACRO    relocMA
         LOCAL   rofine,colfine
         lodsb                   ;gorocol sequence
         add     dh,al
         cmp     dh,[BYTE _wibot]
         jb      colfine
         sub     dh,al
colfine:
         lodsb
         add     dl,al
         inc     dl
         cmp     dl,[BYTE _wiright]
         jb      rofine
         sub     dl,al
rofine:
         dec     dl
ENDM

MACRO    newlocMA
         lodsb                   ;gorocol sequence
         mov     dh,al
         add     dh,[BYTE _witop]
         sub     dh,2
         lodsb
         mov     dl,al
         add     dl,[BYTE _wileft]
         sub     dl,2
         dorocol
         dec     dl
ENDM

MACRO    putMA
         pushr   <ax,bp,si,di>
         mov     bh,0
         mov     ah,2
         int     010h
         popr    <di,si,bp,ax>
ENDM

MACRO    minescMA
         mov     cx,OFFSET ditabmin
         mov     [ditabaddr],cx
         mov     [tablen],0Eh
ENDM

MACRO    anybytMA
         lodsb
         jmp     putbyt
ENDM

MACRO    litstrMA
         mov     [tablen],1
ENDM

MACRO    bellMA
         pushr   <ax,si>
         mov     ax,0e07h
         mov     bh,0
         int     10h
         popr    <si,ax>
ENDM

MACRO    backspcMA
         dorocol
         dec     dl
ENDM

MACRO    tabMA
         LOCAL theloop,notzero
         push  ax
         mov   al,dl
         cbw
         mov   cx,ax
         sub   al,[BYTE _wileft]
         add   al,3
         mov   cl,[_tabwid]
         div   cl
         sub   cl,ah
         jne   notzero
         mov   cl,[_tabwid]
notzero:
         inc   cl
         pop   ax
         mov   al,' '
theloop:
         push  cx
         checkwind
         stosw
         pop   cx
         loop  theloop
ENDM

MACRO    lfMA
         LOCAL   noscro
         inc     dh
         cmp     dh,[BYTE _wibot]
         jb      noscro

         dec     dh
         mov     bh,ah
         mov     al,1
         call    _scroll
noscro:
ENDM

MACRO    scrollMA
         LOCAL   iszero
         lodsb
         cmp     al,0
         je      iszero
         mov     bh,ah
         call    _scroll
iszero:
ENDM

MACRO    clrscrMA
         mov     al,0
         mov     bh,ah
         call    _scroll
ENDM

MACRO    crMA
         mov     dl,[BYTE _wileft]
         sub     dl,2
ENDM

MACRO    flhtogMA
         xor      ah,80h        ; Complement Flash Bit
ENDM

MACRO    hitogMA
         xor      ah,08h        ; Complement Intensity Bit
ENDM

MACRO    grhtogMA
         xor      [graphlim],80h
ENDM

MACRO    newattMA
         lodsb                   ;attribute loaded into al
         mov     ah,al
ENDM

MACRO    defattMA
         mov     ah,[_def_attr]
ENDM

MACRO    chaattMA
         mov     ah,[_cha_attr]
ENDM

MACRO    hiattMA
         mov     ah,[_high_attr]
ENDM

MACRO    grphattMA
         mov     ah,[_grph_attr]
ENDM

MACRO    revMA
         mov     ah,[_rev_attr]
ENDM

MACRO    undMA
         mov     ah,[_und_attr]
ENDM

MACRO    replinMA
         LOCAL   theloop
         push    ax            ;save the video attribute
         lodsb                 ;load number of lines in al
         push    si            ;save si and ds on stack

         xor     cx,cx         
         mov     cl,al         ;Save in lines in cl

         mov     al,[BYTE _wiright]
         cbw
         sub     al,[BYTE _wileft]
         inc     ax            ; Characters per line in ax

         inc     dl            ; Set dl to point to current location
theloop:
         push    cx
         dorocol               ; Trashes cx,sets bh=0, preserves others
         set_di                ; Trashes bx,cx
         mov     si,di
         sub     si,160        ; Set si to point to start of previous line
         mov     cx,ax

         push    ds
         mov     ds,[_video_base]
         rep     movsw         ; trashes si, di
         pop     ds

         inc     dh            ; Advance row
         pop     cx
         loop    theloop
         dorocol
         dec     dl

         pop     si
         pop     ax
ENDM

MACRO    repclrMA
         push    dx

         lodsb
         xor     bx,bx
         mov     bl,al

         mov     al,' '

         repl

         add     dl,bl
         mov     cx,bx
         rep     stosw

         pop     dx
ENDM

MACRO    putcolMA
         LOCAL   nograph,theloop
         push    ax                 ; Save normal attribute

         lodsb                 ;load number of lines in al

         xor     cx,cx         
         mov     cl,al         ;Save in lines in cl

         lodsb                 ; Character in al
         cmp     al,[graphlim]
         jb      nograph
         mov     ah,[_grph_attr]  ;make green att
nograph:

         inc     dl            ; Set dl to point to current location
theloop:
         push    cx
         dorocol               ; Trashes cx,sets bh=0, preserves others
         set_di                ; Trashes bx,cx
         stosw                 ; Write character
         inc     dh            ; Advance row
         pop     cx
         loop    theloop

         dorocol
         dec     dl

         pop     ax
ENDM

MACRO    repMA
         LOCAL   nograph
         push    ax                 ; Save normal attribute

         lodsb
         xor     bx,bx
         mov     bl,al

         lodsb
         cmp     al,[graphlim]
         jb      nograph
         mov     ah,[_grph_attr]    ;make green att
nograph:

         repl

         add     dl,bl
         mov     cx,bx
         rep     stosw
         dec     dl
         pop     ax            ; Restore attribute
ENDM

