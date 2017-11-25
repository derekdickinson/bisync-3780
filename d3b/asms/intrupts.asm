        IDEAL

        %TITLE   "Interrupt and other port I/O code."

        DOSSEG
        MODEL   MEDIUM

; Defines and structure definitions.
	include "\d3b\asms\equates.inc"

; External declarations.
	include "\d3b\asms\globals.inc"

; Data Segment stuff.
	include "\d3b\asms\data.inc"

; Standard SCC and interrupt macros.
	include "\d3b\asms\stand.inc"

; Status interrupt macros.
	include "\d3b\asms\statints.inc"

; Standard send macros.
	include "\d3b\asms\stansend.inc"

; Transparent send macros
	include "\d3b\asms\transend.inc"

; Link Control send macros.
	include "\d3b\asms\ctrlsend.inc"

; Standard receive macros.
	include "\d3b\asms\stanrcv.inc"

; Transparent receive macros.
	include "\d3b\asms\transrcv.inc"

; Link Control Receive macros.
	include "\d3b\asms\ctrlrcv.inc"

; Modem Receive macros
	include "\d3b\asms\mdmrcv.inc"

; Error macros.
	include "\d3b\asms\comerrs.inc"

; End of interrupt macros.
	include "\d3b\asms\endints.inc"

; Critical, Ctrl-C and Ctrl-break interrupt macros
	include "\d3b\asms\critmacs.inc"

public	 _thedel

CODESEG

dseg     dw      0

oldstseg dw      0
oldsp    dw      0

PROC     _int_handler
         startint

         mov     [loopcnt],0

theloop:
         inscc   2,[_bportsc]
         and     ax,000Eh     ;input from port B then "and" with 0e
         mov     si,ax
         jmp     [intjmp+si]   ; Jump to appropriate routine

dumy:
badint:
         errint
         jmp     isdone

txempt:
         mov     si,[_txstate]
         jmp     [txjmp+si]

tx_idle:
         mov     [txindex],0
         mov     dx,[_datasc]
         mov     al,PAD
         out     dx,al
         jmp     isdone
;
; External status interrupts.
;
ieint:
         ieint_do
         jmp     isdone

; Standard send mode labels.

tx_syncs:
         syns_out
         jmp     isdone
tx_stx:
         stx_out
         jmp     isdone
tx_data:
         data_out
         jmp     isdone
tx_etx:
         etx_out
         jmp     isdone
tx_crc:
         crc_out
         jmp     isdone
; An underrun interrupt will occur between crc and pads.
tx_pads:
         pads_out
         jmp     isdone

; Transparent mode labels.
;
txt_syncs:
         synt_out
         jmp     isdone
txt_dle1:
         dle1_out
         jmp     isdone
txt_stx:
         stxt_out
         jmp     isdone
txt_data:
         dtat_out
         jmp     isdone
txt_dle:
         dle_out
         jmp     isdone
txt_dle2:
         dle2_out
         jmp     isdone

;
; Sending Control frame.
;
txc_syncs:
         synp_out
         jmp     isdone
txc_data:
         dtap_out
         jmp     isdone

; Done with tx section.  Now rx section.
;
rxchar:
         mov     dx,[_datasc]        ; Get character.
         in      al,dx

         mov     si,[_rxstate]    ; Jump to appropriate process.
         jmp     [rxjmp+si]

rx_idle:
         mov     [rxindex],0
         jmp     isdone

;
; Receives first character of message.  Determines which routine to go to.
;
rx_strt:
         strt_in
         jmp     isdone

;
; Standard mode data input label.
;
rx_data:
         data_in
         jmp     isdone

;
; Used in both transparent and non-transparent modes.
;
rx_crc:
         crc_in
         jmp     isdone

;
; Transparent receive
;
;
rxt_stx:
         stxt_in
         jmp     isdone

rxt_data:
         dtat_in
         jmp     isdone

rxt_dle:
         dle_in
         jmp     isdone

;
; Ctrl frame receiver.
;
rxc_data:
         dtap_in
				 jmp     isdone
;
; Modem frame receiver
;				 
rx_mdm_strt:
         mdm_strt_in
         jmp     isdone         

rx_mdm_data:
         mdm_data_in
				 jmp     isdone

rx_mdm_etx:
         mdm_got_etx
				 jmp     isdone

rx_mdm_err:
         mdm_len_err

isdone:
         rst_ius
         chk_more
         jmp     theloop
int_done:
         endint
         iret
ENDP     _int_handler

PROC     _outscc
         ARG     reg:BYTE,value:BYTE
         push    bp
         mov     bp,sp

         mov     dx,[_contsc]
         mov     al,[reg]

         cli
         out     dx,al
         delay
         mov     al,[value]
         out     dx,al
         delay
         sti

         mov     sp,bp
         pop     bp
         ret
ENDP     _outscc

PROC		 dodelay NEAR
				 REPT 40
				 nop
				 ENDM
lasdelay:
				 ret
ENDP		 dodelay

PROC     _setseg
         mov     [cs:dseg],ds
         ret
ENDP     _setseg

PROC     _ctrlc
         startint        ; Save registers.

         doclear         ; Clear the screen.
         restore         ; Restore interrupt vectors without using DOS.

         endint
         mov    ax,4c64h
         int    21h     ; Leave the sucker.
ENDP     _ctrlc

PROC     _cbreak

         mov    ds,[cs:dseg]

         doclear        ; Clear the screen.
         restore        ; Restore interrupt vectors without using DOS.

         mov    al,20h
         out    20h,al

         mov    ax,4c64h
         int    21h     ; Leave the program.
ENDP     _cbreak

PROC     _criterr
         starcrit
         cmp    [_dispstate],1
         je     notbat
         mov    al,0
         jmp    finished
notbat:

         savescn     ; Save part of screen that will be overwritten.
         putwind     ; Put a window up on the screen.

boguschar:
         mov    ah,0
         int    16h

         and    al,5fh

         cmp    al,'I'
         jne    not_ignore
         mov    al,0
         jmp    endit
not_ignore:

         cmp    al,'R'
         jne    not_retry
         mov    al,1
         jmp    endit
not_retry:

         cmp    al,'A'
         jne    not_abort
         doclear
         restore     ; Put all the old interrupt vectors back.
         mov    al,2
         jmp    finished
not_abort:

         jmp    boguschar

endit:
         restscn     ; Restore the saved screen.
finished:
         endcrit
         iret
ENDP     _criterr

PROC     _int2
         push   ds
         mov    ds,[cs:dseg]
         mov    [_whichint],2
         jmp    endetect
ENDP     _int2

PROC     _int3
         push   ds
         mov    ds,[cs:dseg]
         mov    [_whichint],3
         jmp    endetect
ENDP     _int3

PROC     _int4
         push   ds
         mov    ds,[cs:dseg]
         mov    [_whichint],4
         jmp    endetect
ENDP     _int4

PROC     _int5
         push   ds
         mov    ds,[cs:dseg]
         mov    [_whichint],5
         jmp    endetect
ENDP     _int5

PROC     _int_detect
				 cli
         push   ds
         mov    ds,[cs:dseg]
         mov    [_whichint],1
endetect:
         push   ax
         push   dx
         mov    al,38h
         mov    dx,[_contsc]
         out    dx,al
         delay
         mov    al,30h
         mov    dx,[_contsc]
         out    dx,al
         delay
         mov    al,10h
         mov    dx,[_contsc]
         out    dx,al

         mov    al,20h
         out    20h,al
         pop    dx
         pop    ax
         pop    ds
         iret
ENDP     _int_detect

END
