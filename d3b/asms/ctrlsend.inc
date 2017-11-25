;
; Non-Data frame tx macros.
;
MACRO    synp_out
         mov     dx,[_datasc]
         mov     al,[_sync]
         out     dx,al
         mov     [txindex],0
         mov     [_txstate],SENPATA
ENDM

MACRO    dtap_out
         LOCAL   more_to_send

         mov     si,[txindex]
         inc     [txindex]
         mov     bx,[_ctrlptr]
         mov     al,[(BIGSTR bx+si).bu]

         mov     dx,[_datasc]
         out     dx,al

         inc     si
         cmp     si,[(BIGSTR bx).len]
         jl      more_to_send

         mov     [txindex],0
         mov     [_txstate],SENDPADS

more_to_send:
ENDM
