
MACRO    savescn
         mov  cx,40

         mov  si,1478
         mov  di,OFFSET critbuf

         push ds
         mov  ax,ds
         mov  es,ax

         mov  ax,[_video_base]
         mov  ds,ax

         rep  movsw

         mov  si,1638
         mov  cx,40
         rep  movsw

         mov  si,1798
         mov  cx,40
         rep  movsw

         pop  ds
ENDM

MACRO    restscn
         mov  cx,40

         mov  di,1478
         mov  si,OFFSET critbuf

         mov  es,[_video_base]

         rep  movsw

         mov  di,1638
         mov  cx,40
         rep  movsw

         mov  di,1798
         mov  cx,40
         rep  movsw
ENDM

MACRO    putwind
         lea    ax,[_critscrn]
         push   ax
         call   _birite
         inc    sp
         inc    sp
ENDM

MACRO    doclear
         mov    ah,2
         mov    bh,0
         mov    dx,0
         int    10h

         mov    ax,0920
         mov    bh,0
         mov    bl,[_def_attr]
         mov    cx,2000
         int    10h
ENDM

MACRO    restore
         LOCAL  nomdm
         mov    ax,0
         mov    es,ax
         cmp    [_mdminst],1
         jne    nomdm

         outscc 9,0c0h
         delay
nomdm:

         cli
         mov    di,4*BREAKVECT
         mov    ax,[WORD LOW _old_break]
         stosw
         mov    ax,[WORD HIGH _old_break]
         stosw

         mov    al,[_intrupt]
         and    ax,0ffh
         add    ax,8
         shlm   ax,2
         mov    di,ax

         mov    ax,[WORD LOW _int_prev]
         stosw
         mov    ax,[WORD HIGH _int_prev]
         stosw
         sti
ENDM

MACRO    starcrit
         sti
         mov     [cs:oldstseg],ss
         mov     [cs:oldsp],sp
         push    bx
         push    cx
         push    dx
         push    di
         push    si
         push    ds
         push    es
         mov     ds,[cs:dseg]
         mov     ss,[cs:dseg]
         mov     sp,[__brklvl]
         add     sp,400
ENDM

MACRO    endcrit
         pop     es
         pop     ds
         pop     si
         pop     di
         pop     dx
         pop     cx
         pop     bx
         mov     ss,[cs:oldstseg]
         mov     sp,[cs:oldsp]
ENDM
