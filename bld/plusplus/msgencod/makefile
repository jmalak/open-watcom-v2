host_cpu = i86
host_os  = dos

!include cproj.mif

objs = wrdencod.obj msgencio.obj

.c.obj:
    $(plusplus_dir)\bin\ppsp.exe $[@ $[.
    $(cc) $[. -oailt -s -d1 -zq -w3 -ml
    rm -f $[.

msgencod.exe : $(objs) setbin.obj dosencod.exe msgencod.lnk $(__MAKEFILES__)
    $(linker) @msgencod.lnk

msgencod.lnk : makefile
    %create $@
    @%append $@ NAME $^*
    @%append $@ SYSTEM os2 pmc
    @%append $@ OPTION quiet, heap=8k, stub=dosencod.exe
    @%append $@ DEBUG line
    @for %i in ( $(objs) ) do @%append $@ FILE %i
    @%append $@ FILE setbin
    @%append $@ LIB siliblw

dosencod.exe : $(objs) dosencod.lnk $(__MAKEFILES__)
    $(linker) @dosencod.lnk

dosencod.lnk : makefile
    %create $@
    @%append $@ NAME $^*
    @%append $@ SYSTEM dos
    @%append $@ OPTION quiet
    @%append $@ DEBUG line
    @for %i in ( $(objs) ) do @%append $@ FILE %i
    @%append $@ LIB siliblw

global : .SYMBOLIC
    rm -f *.obj *.exe *.gh *.lnk

reference : .SYMBOLIC msgencod.exe
    cp msgencod.exe ../bin
