mov %i0,0
mov %i1,0
:tryagain

* A number should be in the file called number.
dl number.txt
on %ei,0="ex",5=:wasbusy,15=:notone
q,1

:wasbusy
inc %i0
ifi %i0=3,:busyout
* Wait 20 seconds between redials when busy
p,+20
go,:tryagain

:notone
inc %i1
ifi %i1=3,:toneout
* Wait 2 minutes between redials if no dial or answer tone detected
p,+20
go,:tryagain

:busyout
sup
rite "Detected a busy signal "+%i0+" times.";rs
go,:keystuf

:toneout
sup
rite "Received no dial or answer back tone "+%i1+" times.";rs

:keystuf
key %i3;c,"Do you want to continue ('Y'es or 'N'o) ?";rs
unsup
upr %i3
ifi %i3='Y',,"q,1"
mov %i0,0
mov %i1,0
go,:tryagain
