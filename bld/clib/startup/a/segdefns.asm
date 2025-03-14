;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;* Copyright (c) 2002-2017 The Open Watcom Contributors. All Rights Reserved.
;*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
;*
;*  ========================================================================
;*
;*    This file contains Original Code and/or Modifications of Original
;*    Code as defined in and that are subject to the Sybase Open Watcom
;*    Public License version 1.0 (the 'License'). You may not use this file
;*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
;*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
;*    provided with the Original Code and Modifications, and is also
;*    available at www.sybase.com/developer/opensource.
;*
;*    The Original Code and all software distributed under the License are
;*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
;*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
;*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
;*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
;*    NON-INFRINGEMENT. Please see the License for the specific language
;*    governing rights and limitations under the License.
;*
;*  ========================================================================
;*
;* Description:  Segment definitions for Open Watcom C/C++32.
;*
;*****************************************************************************


include langenv.inc

        name    segdefns
.386p

if COMP_CFG_COFF eq 0

include tinit.inc
include xinit.inc

endif

        assume  nothing

        extrn   __DOSseg__      : byte


if COMP_CFG_COFF
DGROUP group _NULL,_AFTERNULL,CONST,_DATA,DATA,_BSS,STACK
else
ifdef __LINUX__
DGROUP group _NULL,_AFTERNULL,CONST,_DATA,DATA,TIB,TI,TIE,XIB,XI,XIE,YIB,YI,YIE,_BSS
else
ifdef __NT__
DGROUP group _NULL,_AFTERNULL,CONST,_DATA,DATA,_BSS,TIB,TI,TIE,XIB,XI,XIE,YIB,YI,YIE
else
DGROUP group _NULL,_AFTERNULL,CONST,_DATA,DATA,_BSS,STACK,TIB,TI,TIE,XIB,XI,XIE,YIB,YI,YIE
endif
endif
endif

; this guarantees that no function pointer will equal NULL
; (WLINK will keep segment 'BEGTEXT' in front)
; This segment must be at least 4 bytes in size to avoid confusing the
; signal function.

BEGTEXT segment use32 word public 'CODE'
        assume  cs:BEGTEXT
forever label   near
        int     3h
        jmp short forever
        ; NOTE that ___begtext needs to be at offset 3
        ; don't move it.  i.e. don't change any code before here.
        public ___begtext
___begtext label byte
        nop
        nop
        nop
        nop
        assume  cs:nothing
BEGTEXT ends

        assume  ds:DGROUP

_NULL   segment para public 'BEGDATA'
__nullarea label word
        db      01h,01h,01h,00h
        public  __nullarea
_NULL   ends

_AFTERNULL segment word public 'BEGDATA'
_AFTERNULL ends

CONST   segment word public 'DATA'
CONST   ends

ifndef __RDOSDEV__


if COMP_CFG_COFF eq 0

XIB     segment word public 'DATA'
_Start_XI label byte
        public  "C",_Start_XI
XIB     ends

XIE     segment word public 'DATA'
_End_XI label byte
        public  "C",_End_XI
XIE     ends

YIB     segment word public 'DATA'
_Start_YI label byte
        public  "C",_Start_YI
YIB     ends

YIE     segment word public 'DATA'
_End_YI label byte
        public  "C",_End_YI
YIE     ends

endif

endif

_DATA   segment word public 'DATA'
_DATA   ends

DATA    segment word public 'DATA'
DATA    ends

_BSS    segment word public 'BSS'
_BSS    ends


ifndef __LINUX__
ifndef __NT__


ifdef __RDOSDEV__
STACK_SIZE      equ     10h
else
STACK_SIZE      equ     10000h
endif

STACK   segment para stack 'STACK'
        db      (STACK_SIZE) dup(?)
STACK   ends
endif
endif

_TEXT   segment use32 word public 'CODE'
_TEXT   ends

        end
