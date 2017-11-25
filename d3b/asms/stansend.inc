
MACRO    syns_out
         mov     dx,[_datasc]
         mov     al,[_sync]
         out     dx,al
         mov     [txindex],0
         mov     [_txstate],SENDSTX
ENDM

MACRO    stx_out
         mov     dx,[_datasc]
         mov     al,STX
         out     dx,al
         mov     [_txstate],SENDATA
ENDM

MACRO    data_out
         LOCAL   more_to_send

         outscc  5,0efh          ; Enable CRC check.

         mov     bx,[_outptr]
         mov     si,[txindex]
         inc     [txindex]
         mov     al,[(BIGSTR bx+si).bu]        ;txchar=txbuf[txindex];

         mov     dx,[_datasc]
         out     dx,al           ;outp(datasc,txchar);

         inc     si
         cmp     si,[(BIGSTR bx).len]  ;if (txindex!=mout)
         jl      more_to_send

         mov     [txindex],0
         mov     [_txstate],SENDETX

more_to_send:
ENDM

MACRO    etx_out
         LOCAL is_etb
         outscc  6,PAD   ; Set Sync char regs to pad since they are dumped.
         outscc  7,PAD   ;  after CRC.

         outscc  5,0efh          ; Enable CRC check.
         mov     dx,[_datasc]

         mov     bx,[_curbuf]
         mov     al,[_sendends+bx]

         out     dx,al
         mov     [_txstate],SENDCRCS
ENDM

MACRO    crc_out

         outscc  1,01h    ; Enable Underrun int, disable tx int.

         mov     al,28h
         mov     dx,[_contsc]
         out     dx,al

         delay

ENDM

MACRO    pads_out
         LOCAL no_more_pads,done

         inc     [txindex]
         cmp     [txindex],NUMPADS
         jge     no_more_pads

         outscc  6,[_sync]   ; Set Sync char regs to pad since they are dumped.
         outscc  7,[_sync]   ;  after CRC.

         mov     dx,[_datasc]
         mov     al,PAD
         out     dx,al
         jmp     isdone

no_more_pads:

         mov     al,28h
         mov     dx,[_contsc]
         out     dx,al       ;outscc(0,0x28); /* scc register 0 */
         delay
         mov     [txindex],0
         mov     [_txstate],0
         outscc  5,0e5h   ; Turn off TX,RTS

         cmp     [_sendrsp],FALSE
         je      done

         les     bx,[_nowtime]
         mov     dx,[es:bx+2]
         mov     ax,[es:bx]
         mov     [WORD HIGH _sendtime],dx
         mov     [WORD LOW _sendtime],ax

         mov     [_rxstate],2
         mov     [_error],0
         outscc  1,10h
         outscc  3,0d1h
         jmp     isdone

done:
         outscc  1,00h    ; Turn off all ints.
ENDM
