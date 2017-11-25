

; Transparent receive macros.
;
MACRO    stxt_in
         LOCAL   not_stx, iseot
         cmp     al,STX
         jg      not_stx

         mov     [rxindex],0
         mov     [_rxtyp],1
         mov     [_rxstate],RCVTATA
         jmp     isdone

not_stx:
         mov     [_ctrlbyt],al
         mov     [padcnt],0
         mov     [rxindex],1
         mov     [_rxstate],RCVPATA

         cmp     al,EOT
         je      iseot

         mov     [_rxtyp],2
         jmp     isdone

iseot:
         mov     [_rxtyp],4
ENDM

MACRO    dtat_in
         LOCAL   more_data,no_overflow

         mov     di,[rxindex]
         mov     bx,[_inptr]
         mov     [(BIGSTR bx+di).bu],al

         cmp     di,BUFFSIZE
         jbe     no_overflow

         or      [_error],4
         mov     [_rxstate],0
         mov     [rxindex],0
         outscc  1,00h
         outscc  3,0d1h
         jmp     isdone
no_overflow:

         inc     [rxindex]

         cmp     al,DLE
         jne     more_data

         mov     [_rxstate],RCVTDLE
         outscc  3,0c1h               ; Disable CRC checking.
         jmp     isdone

more_data:

         outscc  3,0c9h               ; Enable CRC checking.

ENDM


MACRO    dle_in
         LOCAL   is_dle, is_ttd, is_etb, is_etx, is_sync, a_ttd

         cmp     al,[_sync]
         je      is_sync

         cmp     al,ENQ
         je      is_ttd

         cmp     al,ETB
         je      is_etb

         cmp     al,ETX
         je      is_etx

         jmp     is_dle

is_ttd:
         mov     [_rxtyp],3
is_etb:
is_etx:
         mov     bx,[_curbuf]
         mov     [_rcvends+bx],al

         mov     [_rxstate],RCVCRCS
         mov     di,[rxindex]
         dec     di

         mov     bx,[_inptr]
         mov     [(BIGSTR bx).len],di
         mov     [rxindex],0
         jmp     is_end
is_sync:
         dec     [rxindex]
         mov     [_rxstate],RCVTATA
         outscc  3,0c1h               ; Disable CRC checking.
         jmp     isdone

; Put whatever character follows DLE in buf
is_dle:                          
         mov     [_rxstate],RCVTATA
         mov     di,[rxindex]
         dec     di
         mov     bx,[_inptr]
         mov     [(BIGSTR bx+di).bu],al
is_end:
         outscc  3,0c9h               ; Enable CRC checking.
ENDM
