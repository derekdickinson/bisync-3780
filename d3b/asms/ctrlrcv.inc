
;
; Non-Data frame in.
;
;
MACRO    dtap_in
         LOCAL   more_data,more_pads

         inc     [rxindex]

         cmp     al,PAD
         jne     more_data

         inc     [padcnt]
         cmp     [padcnt],PADMAX
         jl      more_pads

         mov     [_rxstate],0   ; Through with message.
         mov     [rxindex],0
         outscc  1,00h        ; Turn off interrupts.
         outscc  3,0d1h       ; reset Sync-Hunt bit

         jmp     isdone

more_data:
         mov     [padcnt],0
more_pads:
ENDM
