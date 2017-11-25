; Register Assumptions
; si points into source string
; bl contains attribute
; bh contains 0
; dh contains active row
; dl contains active column

MACRO    dojmpb  table
         xor     ah,ah
         mov     bp,ax
         shl     bp,1
         add     bp,[table]
         jmp     [WORD bp]
ENDM

MACRO    setcur
         mov     ah,2
         push    si
         int     10h
         pop     si
ENDM

MACRO    getcur
         mov     ah,3
         push    si
         int     10h
         pop     si
ENDM

MACRO    adv_loc
         LOCAL   not_end

         inc     dl
         cmp     dl,[BYTE _wiright]
         jb      not_end

         mov     dl,[BYTE _wileft]
         dec     dl

         inc     dh
         cmp     dh,[BYTE _wibot]
         jb      not_end

         dec     dh
         push    ax
         mov     bh,bl
         mov     al,1
         call    _scroll
         mov     bh,0
         pop     ax

not_end:
ENDM

MACRO    stosMA
         push    si
         adv_loc
         mov     cx,ax
         mov     ah,2
         int     10h
         mov     ax,cx
         mov     ah,9
         mov     cx,1
         int     10h
         pop     si
ENDM


;----------------------------------------------------------------------------

MACRO    strtMAB
         push    bp
         mov     bp,sp
         push    si
         push    es
         push    di
         push    bp

         mov     ah,3   ; Get cursor position and save it to restore when
         mov     bh,0   ;  done.
         int     10h
         push    dx
ENDM

MACRO    endMAB
         add     dl,3
         sub     dl,[BYTE _wileft]
         mov     [BYTE _col],dl

         add     dh,2
         sub     dh,[BYTE _witop]
         mov     [BYTE _ro],dh

         pop     dx        ; Restore initial cursor location
         setcur
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

MACRO    graphMAB
         push    bx
         mov     bl,[_grph_attr]
         stosMA
         pop     bx
ENDM

; Used in repseq and repclr
MACRO    reploop
         LOCAL   theloop,inwind
theloop:
         dorocol               ; Trashes cx; preserves others
         pushr   <ax,si>
         mov     ah,2
         int     10h
         popr    <si,ax>

         xor     cx,cx
         mov     cl,dl
         add     cx,di
         cmp     cx,[_wiright]
         jbe     inwind

         mov     ah,9
         mov     cx,[_wiright]
         pushr   <ax,dx,di,si>
         xor     dh,dh
         sub     cx,dx
         int     10h
         popr    <si,di,dx,ax>

         sub     di,cx
         mov     dl,[BYTE _wileft]
         dec     dl
         inc     dh
         jmp     theloop
inwind:
ENDM

;---------------------------------------------------------------------------

MACRO    putMAB
         mov     di,sp
         mov     [di],dx  ; Move value onto top of the stack
ENDM

MACRO    minescMAB
         mov     cx,OFFSET bitabmin
         mov     [bitabaddr],cx
         mov     [tablen],0Eh
ENDM

MACRO    anybytMAB
         lodsb
         stosMA
ENDM

MACRO    bellMAB
         push  si
         mov   ax,0e07h
         int   10h
         pop   si
ENDM

MACRO    tabMAB
         LOCAL theloop,notzero
         xor   ax,ax
         mov   cx,ax
         mov   al,dl
         sub   al,[BYTE _wileft]
         add   al,3
         mov   cl,[_tabwid]
         div   cl           
         sub   cl,ah
         jne   notzero
         mov   cl,[_tabwid]
notzero:
         inc   cl
         mov   al,' '
theloop:
         push  cx
         stosMA
         pop   cx
         loop  theloop
ENDM

MACRO    lfMAB
         LOCAL   noscro
         inc     dh
         cmp     dh,[BYTE _wibot]
         jb      noscro

         dec     dh
         mov     bh,bl
         mov     al,1
         call    _scroll
         mov     bh,0
noscro:
ENDM

MACRO    scrollMAB
         LOCAL   iszero
         lodsb
         cmp     al,0
         je      iszero
         mov     bh,bl
         call    _scroll
         mov     bh,0
iszero:
ENDM

MACRO    clrscrMAB
         push   bx
         mov    al,00
         mov    bh,bl
         call   _scroll
         mov    bh,0
         pop    bx
ENDM

MACRO    crMAB
         mov     dl,[BYTE _wileft]
         sub     dl,2
ENDM

MACRO    flhtogMAB
         xor      bl,80h        ; Complement flashing bit of attribute
ENDM

MACRO    hitogMAB
         xor      bl,08h        ; Complement Intensity bit of attribute
ENDM

MACRO    grhtogMAB
         xor      [graphlim],80h
ENDM

MACRO    newattMAB
         lodsb
         mov     bl,al
ENDM

MACRO    defattMAB
         mov     bl,[_def_attr]
ENDM

MACRO    chaattMAB
         mov     bl,[_cha_attr]
ENDM

MACRO    hiattMAB
         mov     bl,[_high_attr]
ENDM

MACRO    grphattMAB
         mov     bl,[_grph_attr]
ENDM

MACRO    revMAB
         mov     bl,[_rev_attr]
ENDM

MACRO    undMAB
         mov     bl,[_und_attr]
ENDM

MACRO    replinMAB
         LOCAL   linloop
         lodsb                 ;load number of lines in al
         push    si            ;save si on stack
         push    bx

         mov     cl,[BYTE _wiright]
         sub     cl,[BYTE _wileft]
         inc     cl
         mul     cl
         mov     cx,ax         ;put count in cx
  linloop:
         push    cx            ; Save cx value
         adv_loc               ; Advance Location

         dec     dh            ; Decrement Row
         mov     ah,2
         int     10h           ; Set Cursor position

         mov     ah,8
         int     10h           ; Read at cursor
         push    ax            ; Save char and attribute

         inc     dh            ; Increment row
         mov     ah,2
         int     10h           ; Set Cursor

         pop     ax            ; Restore char and attribute
         mov     bl,ah
         mov     ah,9
         mov     cx,1
         int     10h           ; Write at cursor

         pop     cx            ; Restore cx
         loop    linloop

         pop     bx
         pop     si
ENDM

MACRO    repclrMAB
         LOCAL   nograph,doit,theloop
         push    dx                 ; Save start location

         xor     ax,ax
         lodsb
         mov     di,ax

         mov     al,' '

         inc     dl

         reploop

         mov     cx,di
         mov     ah,9
         push    si
         int     10h
         pop     si

         pop     dx            ; Restore location
ENDM

MACRO    putcolMAB
         LOCAL   nograph,theloop,noscro
         push    bx                 ; Save normal attribute
         adv_loc

         lodsb                 ;load number of lines in al

         xor     cx,cx
         mov     cl,al         ;Save in lines in cl

         lodsb                 ; Character in al
         cmp     al,[graphlim]
         jb      nograph
         mov     bl,[_grph_attr]    ;make green att
nograph:

theloop:
         push    cx

         push    si

         mov     cx,ax
         mov     ah,2
         int     10h         ; Set cursor
         mov     ax,cx

         mov     ah,9
         mov     cx,1
         int     10h          ; Write char

         pop     si

         inc     dh            ; Advance row

         cmp     dh,[BYTE _wibot] ; Check for overflow
         jb      noscro
         dec     dh
         mov     cx,ax
         mov     bh,bl
         mov     al,1
         call    _scroll
         mov     bh,0
         mov     ax,cx
noscro:

         pop     cx
         loop    theloop

         dec     dl
         pop     bx
ENDM

MACRO    repMAB
         LOCAL   nograph
         push    bx                 ; Save normal attribute

         xor     ax,ax
         lodsb
         mov     di,ax
         lodsb
         cmp     al,[graphlim]
         jb      nograph
         mov     bl,[_grph_attr]    ;make green att
nograph:

         inc     dl

         reploop

         mov     cx,di
         mov     ah,9
         push    si
         int     10h
         pop     si

         add     dl,cl
         dec     dl
         pop     bx            ; Restore attribute
ENDM

