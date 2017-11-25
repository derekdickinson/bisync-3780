
MACRO    rst_ius
         delay
         mov     al,38h
         mov     dx,[_contsc]
         out     dx,al
ENDM

MACRO    chk_more
         LOCAL   no_error

         cmp     [loopcnt],8
         jl      no_error

         mov     [txindex],0     ; Watch for big trouble interrupt.
         mov     [rxindex],0
         mov     [_txstate],0
         mov     [_rxstate],0
         or      [_error],80h
         errint
				 outscc	 1,00h           ; Disable interrupts
         jmp     int_done

no_error:
         inscc   3,[_aportsc]
         cmp     al,0
         je      int_done    ;SCC reg3=0

         or      [_error],2
         inc     [loopcnt]
ENDM
