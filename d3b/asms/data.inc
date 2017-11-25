
DATASEG
rxindex dw  0   ; Receive state variables
txindex dw  0   ; Transmit state variables
padcnt  dw  0   ; Count of pads received
loopcnt db  0   ; count of primary loops in the interrupt handler

intjmp  dw  txempt,  ieint,   rxchar,  badint
        dw  txempt,  ieint,   rxchar,  badint

txjmp   dw  tx_idle
        dw  tx_syncs,  tx_stx,   tx_data
        dw  tx_etx,    tx_crc,   tx_pads
        dw  txt_syncs, txt_dle1, txt_stx, txt_data, txt_dle,  txt_dle2
        dw  txc_syncs, txc_data

rxjmp   dw  rx_idle
        dw  rx_strt, rx_data,  rx_crc
        dw  rxt_stx, rxt_data, rxt_dle
        dw  rxc_data
				dw  rx_mdm_strt, rx_mdm_data, rx_mdm_etx, rx_mdm_err

critbuf dw 40*3 DUP (?)  ; Will contain part of screen contents
                         ;   if a critical error occurs.

_thedel	dw	lasdelay