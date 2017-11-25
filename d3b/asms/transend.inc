
; Transparent transmit mode macros
;
;

MACRO    synt_out
         mov     dx,[_datasc]
         mov     al,[_sync]
         out     dx,al
         mov     [txindex],0
         mov     [_txstate],SENTDLE1
ENDM

MACRO    dle1_out
	    mov     dx,[_datasc]
         mov     al,DLE
         out     dx,al
         mov     [_txstate],SENTSTX
ENDM

MACRO    stxt_out
         mov     dx,[_datasc]
         mov     al,STX
         out     dx,al
         mov     [_txstate],SENTATA
ENDM

MACRO    dtat_out
         LOCAL   more_to_send

         outscc  5,0efh          ; Enable CRC check.

         mov     bx,[_outptr]
         mov     si,[txindex]
         inc     [txindex]
         mov     al,[(BIGSTR bx+si).bu]        ;txchar=txbuf[txindex];

         mov     dx,[_datasc]
         out     dx,al           ;outp(datasc,txchar);

         cmp     al,DLE
         jne     not_dle
         mov     [_txstate],SENTDLE
         jmp     more_to_send
not_dle:

         inc     si
         cmp     si,[(BIGSTR bx).len]  ;if (txindex!=mout)
         jl      more_to_send

         mov     [txindex],0
         mov     [_txstate],SENTDLE2

more_to_send:
ENDM

MACRO    dle_out
         LOCAL   more_to_send

         outscc  5,0eeh          ; Disable CRC check.

         mov     al,DLE
         mov     dx,[_datasc]
         out     dx,al           ;outp(datasc,txchar);

         mov     bx,[_outptr]
         mov     ax,[txindex]
         cmp     ax,[(BIGSTR bx).len]  ;if (txindex!=mout)
         jl      more_to_send

         mov     [txindex],0
         mov     [_txstate],SENTDLE2
         jmp     isdone

more_to_send:
         mov     [_txstate],SENTATA
ENDM

MACRO    dle2_out
         outscc  5,0eeh          ; Disable CRC check.
         mov     dx,[_datasc]
         mov     al,DLE
         out     dx,al
         mov     [_txstate],SENDETX
ENDM
