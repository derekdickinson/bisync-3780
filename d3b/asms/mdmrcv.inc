; Modem Receive macros.
;

MACRO    mdm_strt_in
         LOCAL   done
         cmp     al,STX
         jne     short done          

         mov     [rxindex],0
         mov     [_rxstate],MRCVDATA
done:
ENDM

MACRO    mdm_data_in
         LOCAL   got_etx,skip

         mov     di,[rxindex]
         mov     bx,[_inptr]
         mov     [(BIGSTR bx+di).bu],al

         inc     [rxindex]
				 cmp     [rxindex],70
				 jna     skip
				 jmp     rx_mdm_err
skip:				 

         cmp     al,ETX
         je      short got_etx
         jmp     isdone
				 
got_etx:
         mov     [_rxstate],MGOTETX
ENDM

MACRO    mdm_got_etx
         LOCAL   got_pad,done,skip

         mov     di,[rxindex]
         mov     bx,[_inptr]
         mov     [(BIGSTR bx+di).bu],al

         inc     [rxindex]
				 cmp     [rxindex],70
				 jna     skip
				 jmp     rx_mdm_err
skip:				 

         cmp     al,PAD
         je      short got_pad

				 mov     [padcnt],0
         jmp     isdone

got_pad:
         inc     [padcnt]
				 cmp		 [padcnt],5
				 jl      done

         mov     ax,[rxindex]
         mov     bx,[_inptr]
         mov     [(BIGSTR bx).len],ax

         mov     [_rxstate],0   ; Through with message.
         mov     [rxindex],0
         outscc  1,00h          ;Disable interrupts
         outscc  3,0d1h         ;reset Sync-Hunt bit
done:				 
ENDM

MACRO    mdm_len_err 
				 mov     [_error],1
         mov     [_rxstate],0   ; Through with message.
         mov     [rxindex],0
         outscc  1,00h          ;Disable interrupts
         outscc  3,0d1h         ;reset Sync-Hunt bit
ENDM   