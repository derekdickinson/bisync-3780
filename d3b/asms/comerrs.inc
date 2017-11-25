
MACRO    errint
         or      [_error],1   ; Set error flag.

         mov     al,30h
         mov     dx,[_contsc]
         out     dx,al      ;outscc(0,0x30); /* Error Reset */

         delay

         mov     al,10h
         mov     dx,[_contsc]
         out     dx,al      ;outscc(0,0x10); /* Reset External Status ints */

         mov     dx,[_datasc] ; Get character. /* Getting char resets rx errs */
         in      al,dx
ENDM
