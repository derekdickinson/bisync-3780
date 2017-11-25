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

MACRO    putcon where,what
         push  es
         push  si
         mov   si,0b000h
         mov   es,si
         mov   si,where
         mov   [BYTE es:si],what
         pop   si
         pop   es
ENDM

;MACRO    delay
;  REPT 8
;         nop
;  ENDM
;ENDM

;MACRO    delay
;	pushf
; REPT 5
;        or			 ax,ax
;  ENDM
;	nop
;	popf
;ENDM

MACRO		 delay
				 call		 [_thedel]
ENDM

MACRO    outscc  reg,val
         mov     al,&reg
         mov     dx,[_contsc]
         out     dx,al
         delay
         mov     al,&val
         out     dx,al
         delay
ENDM

MACRO    inscc   reg,port
         mov     al,&reg
         mov     dx,&port
         out     dx,al
         delay
         in      al,dx         ;inp(ad?)
         delay
ENDM

MACRO    startint
         push    ax
         push    bx
         push    dx
         push    di
         push    si
         push    ds
         push    es
         mov     ds,[cs:dseg]
ENDM

MACRO    endint
         pop     es
         pop     ds
         pop     si
         pop     di
         pop     dx
         pop     bx
         mov     al,20h
         out     20h,al
         pop     ax
ENDM

MACRO    rite ach
         push  di
         push  es

				 mov   di,0b000h
				 mov   es,di
				 mov   di,80
				 mov	 [BYTE es:di],&ach

         pop   es
         pop   di
ENDM