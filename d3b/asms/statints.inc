
MACRO    ieint_do
; Only external status interrupt allowed in TX underrun.
         mov     dx,[_contsc]
         in      al,dx         ; Get current reg 0 status.
         delay

         mov     al,10h
         mov     dx,[_contsc]
         out     dx,al       ; Reset external status interrupts
         delay

         mov     [_txstate],SENDPADS
         outscc  1,02h
ENDM
