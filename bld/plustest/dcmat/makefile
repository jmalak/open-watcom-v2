plustest_name = dcmat

!ifdef test1
test_flags = -d2
alt_error=01
!else
test_flags = -ox-d1
alt_error=00
!endif

test_flags += -e10 -w0 -D__ATT_complex__ -I.

test_flags_386 =
test_flags_i86 = -mm -zm
test_flags_axp =

!include testenv.mif

objs = &
    compmat.obj &
    cosft.obj &
    doubmat.obj &
    four1.obj &
    realft.obj &
    svdcmp.obj

stack_386=opt stack=16k
stack_i86=opt stack=8k
stack_axp=opt stack=16k

opt_link = LIBRARY matrix.lib

exes = test.$(exe) tmatrix.$(exe)

test : .SYMBOLIC start matrix.lib test.lnk $(exes)
    $(noecho)%make common_set_result

matrix.lib : $(objs) matrix.lbc
    @%make set_path_wlib
    @%make echo_lib
    $(noecho)$(librarian) $@ @matrix.lbc
    @%make set_old_path

matrix.lbc : makefile
    $(noecho)%create $@
    @for %i in ( $(objs) ) do @%append $@ -+%i

clean : .procedure
    @%make echo_clean
    $(noecho)%make common_clean
