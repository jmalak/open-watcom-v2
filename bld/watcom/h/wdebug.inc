;****************************************************************************
;*
;*                            Open Watcom Project
;*
;* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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
;* Description:  Watcom wdebug VxD for Win16 interface.
;*
;****************************************************************************


ERR_SERVER_EXISTS               equ     0ffffh          ; -1
ERR_NO_MORE_CONVERSATIONS       equ     0fffeh          ; -2
ERR_NO_SUCH_SERVER              equ     0fffdh          ; -3
ERR_ALREADY_ACCESSED_SERVER     equ     0fffch          ; -4
ERR_HAVE_NOT_ACCESSED_SERVER    equ     0fffbh          ; -5
ERR_NOT_THAT_SERVER             equ     0fffah          ; -6
ERR_NO_SUCH_CONVERSATION        equ     0fff9h          ; -7
ERR_CONVERSATION_NOT_STARTED    equ     0fff8h          ; -8
ERR_NOT_A_SERVER                equ     0fff7h          ; -9
ERR_NOT_CONNECTED               equ     0fff6h          ; -10
ERR_NO_SUCH_ID                  equ     0fff5h          ; -11
ERR_COULD_NOT_BLOCK             equ     0fff4h          ; -12
ERR_COULD_NOT_RESUME            equ     0fff3h          ; -13
ERR_TIMED_OUT                   equ     0fff2h          ; -14
ERR_HAS_CLIENTS                 equ     0fff1h          ; -15
NO_BLOCK                        equ     0
BLOCK                           equ     1
WGOD_VERSION                    equ     0666h

Declare_Watcom_Debug_Virtual_Device macro Major_Ver, Minor_Ver, Ctrl_Proc, Device_Num, Init_Order, V86_Proc, PM_Proc, Reference_Data
  VxD_LOCKED_DATA_SEG
  PUBLIC WDEBUG_DDB
  WDEBUG_DDB VxD_Desc_Block <,,Device_Num,Major_Ver,Minor_Ver,,"WDEBUG  ",Init_Order, OFFSET Ctrl_Proc, 0, 0,,, 0, 0, 0>
  VxD_LOCKED_DATA_ENDS
endm
