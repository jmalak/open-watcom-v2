# application makefile test

objs = ap1.obj ap2.obj ap3.obj

.asm:       .;..\asm
.asm.obj:
    optasm $[*

.c:         .;..\c
.c.obj:
    %write $@ wcc386 $[* -zq

app : app.exe .SYMBOLIC
    @%null

app.lnk : makefile
    if 1 == 1 echo NAME app >app.lnk
    for %i in ($(objs)) do echo FILE %i >>app.lnk

app.exe : app.lnk $(objs)
    %write $@ wlink @app.lnk

