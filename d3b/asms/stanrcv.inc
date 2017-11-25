
; Receive macros.
;
;

MACRO    strt_in
         LOCAL   isdle,not_stx
         cmp     al,[_sync]
         jne     $+5
         jmp     isdone

         cmp     al,DLE
         je      isdle

         cmp     al,STX
         jg      not_stx          ; Accept SOH as STX also.

         mov     [rxindex],0
         mov     [_rxtyp],0
         mov     [_rxstate],RCVDATA
         jmp     isdone
not_stx:
         mov     [_ctrlbyt],al
         mov     [padcnt],0

         mov     [rxindex],1
         mov     [_rxtyp],2
         mov     [_rxstate],RCVPATA
         jmp     isdone

isdle:
         mov     [_rxstate],RCVTSTX
         jmp     isdone
ENDM

MACRO    data_in
         LOCAL   data_done, isfil, is_ttd

         cmp     al,[_sync]
         je      isfil

         mov     di,[rxindex]
         mov     bx,[_inptr]
         mov     [(BIGSTR bx+di).bu],al

         inc     [rxindex]

         cmp     al,ETB
         je      data_done

         cmp     al,ETX
         je      data_done

         cmp     al,ENQ
         je      is_ttd
				 
         outscc  3,0c9h               ; Enable CRC checking.
         jmp     isdone

isfil:
         outscc  3,0c1h
         jmp     isdone

is_ttd:
         mov     [_rxtyp],3
data_done:
         mov     bx,[_curbuf]
         mov     [_rcvends+bx],al
         outscc  3,0c9h               ; Enable CRC checking.

         mov     [_rxstate],RCVCRCS
         mov     ax,[rxindex]
         dec     ax
         mov     bx,[_inptr]
         mov     [(BIGSTR bx).len],ax
         mov     [rxindex],0
         jmp     isdone

ENDM

MACRO    crc_in
         LOCAL   wait_more,goodcrc

         inscc   1,[_contsc]

         inc     [rxindex]
         cmp     [rxindex],WHENCRC
         jl      wait_more

         and     al,40h
         or      [_error],al

         mov     [_rxstate],0   ; Through with message.
         mov     [rxindex],0
         outscc  1,00h          ;Disable interrupts
         outscc  3,0d1h         ;reset Sync-Hunt bit

wait_more:

         ENDM
