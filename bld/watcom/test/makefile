!ifndef v
!  define v
!endif

.c.obj:
    wcc$(v)p $[* -oalit -d1 -zq -w4

.obj.exe:
    wlink form dos DEBUG line OPTION quiet NAME $@ FILE $[@

all : .SYMBOLIC testtiny.exe testdir.exe
    @%null

global : .SYMBOLIC
    rm -f *.?bj *.ex?
