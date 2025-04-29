/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  WGML implementation of .bd, .bi, and .us script control
*                    words
*                   interactions are only partially understood/implemented
*
****************************************************************************/


#include    "wgml.h"


/**************************************************************************/
/* BOLD boldfaces the specified number of input text lines.               */
/*                                                                        */
/*      +-------+--------------------------------------------------+      */
/*      |       |                                                  |      */
/*      |  .BD  |    <1|n|ON|OFF|line>                             |      */
/*      |       |                                                  |      */
/*      +-------+--------------------------------------------------+      */
/*                                                                        */
/*                                                                        */
/* n:   The next "n" input text lines are to be made Bold by overstriking */
/*      each character with itself.  If "n" is omitted,  a value  of 1 is */
/*      assumed.                                                          */
/* ON:  All following input text lines are to be made Bold.               */
/* OFF: Terminates  the boldface  effect,  whether  initiated by  "n" or  */
/*      "ON".                                                             */
/* line: The  boldface effect will be  applied to the  resulting "line".  */
/*    If "line"  starts with a control  word indicator,  the  Bold action */
/*    will start one blank after the control word at the start of "line", */
/*    and the result of overstriking must  fit within the maximum allowed */
/*    input line  length.   This restriction does  not apply to  a "line" */
/*    operand of text or text following a numeric or "ON" operand.        */
/*                                                                        */
/* This control word does not cause a break.  BOLD operates independently */
/* of other control words that modify text.    When more than one of .BD, */
/* .BI,  .UC,  .UL,  .UP,  or .US are  in effect,  the result is the best */
/* equivalent of the  sum of the effects.    They may be disabled  in any */
/* order;  the result  will be the best  equivalent of the ones  still in */
/* effect.   The .UD "INCLUDE/IGNORE/SET" characters  may be used to turn */
/* the function on and  off within a "line" or in  subsequent input text. */
/* See .UD for defaults and details.                                      */
/*                                                                        */
/* NOTES                                                                  */
/* (1) For output devices  that support multiple fonts,   the output from */
/*     this  control word  is  not necessarily  bold  but  may merely  be */
/*     selected from a different font.                                    */
/* (2) Text may also be emphasized with the &'bold( function.             */
/* (3) Do not  attempt to use a  numeric expression as a  "line" operand, */
/*     because  it  will be  taken  as  an  "n" operand.    For  example, */
/*     ".BD (1988)" will boldface  the next 1988 input lines  of text and */
/*     not the string  "(1988)".   Use ".BD 1;(1988)" or  ".BD;(1988)" or */
/*     &'bold(1988) instead.                                              */
/*                                                                        */
/**************************************************************************/

/**************************************************************************/
/* BOLD ITALIC overstrikes an input line with itself and underscores it   */
/* for a boldface italic effect.                                          */
/*                                                                        */
/*      +-------+--------------------------------------------------+      */
/*      |       |                                                  |      */
/*      |  .BI  |    <1|n|ON|OFF|line>                             |      */
/*      |       |                                                  |      */
/*      +-------+--------------------------------------------------+      */
/*                                                                        */
/* This control word does not cause a break. The operands are identical   */
/* to those for the .BD control word.                                     */
/*                                                                        */
/* NOTES                                                                  */
/* (1) For output devices  that support multiple fonts,   the output from */
/*     this control word is not necessarily underscored.   It often will  */
/*     display in a bold italic font, for example.                        */
/* (2) Text may also be emphasized with the &'bitalic( function.          */
/*                                                                        */
/**************************************************************************/

/**************************************************************************/
/* UNDERSCORE underscores text in the specified number of input lines.    */
/*                                                                        */
/*      +-------+--------------------------------------------------+      */
/*      |       |                                                  |      */
/*      |  .US  |    <1|n|ON|OFF|line>                             |      */
/*      |       |                                                  |      */
/*      +-------+--------------------------------------------------+      */
/*                                                                        */
/* This control  word does not  cause a  break.   The operands  and other */
/* considerations are identical to those for the other control words that */
/* emphasize text.  See .BD for details.                                  */
/*                                                                        */
/* NOTES                                                                  */
/* (1) For output devices  that support multiple fonts,   the output from */
/*     this control word is not  necessarily underscored.   It often will */
/*     display in an italic font, for example.                            */
/* (2) Text may also be emphasized with the &'italic( function.           */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/* The common function does all the work; the sc_xx functions specify     */
/* which control word is being processed                                  */
/* NOTE: the treatment of BI in wgml 4.0 is quite different from what     */
/*       might be expected from the way BI is documented; this attempts   */
/*       to follow a reasonable interpretation of the documentation       */
/*       by treating BI as a quick way to set or clear both BD and US     */
/**************************************************************************/

static void scr_style_common( style_cw_type type, style_cw_info * cw_info )
{
    char            *   p;
    char            *   pa;
    char            *   pb;
    condcode            cc;
    getnum_block        gn;
    int                 len;

    if( script_style.style == SCT_none ) {  // save current font if no script style
        script_style.font = layout_work.defaults.font;
    }

    p = scan_start;     // for use later
    pb = scan_start;    // for use if outputting text
    cc = getarg();      // sets tok_start, scan_start and scan_end

    if( cc == omit ) {                  // same as BD 1
        script_style.style |= type;
        if( type != SCT_bi ) {
            cw_info->count = 1;
            cw_info->scope = SCS_count;
        }
        pa = scan_start;                // nothing following
    } else {                            // "pos", "quotes", "quote0"
        SkipSpaces( p );                // first token start
        pa = p;
        SkipNonSpaces( p );             // end of token
        len = p - pa;
        SkipSpaces( p );                // second token?
        if( *p == '\0' ) {              // no second token
            if( (len == 3)
              && (strnicmp( pa, "OFF", len ) == 0) ) { // OFF
                if( type == SCT_bi ) {
                    script_style.style &= ~SCT_bd;
                    script_style.cw_bd.count = 0;
                    script_style.cw_bd.scope = SCS_none;
                    script_style.style &= ~SCT_us;
                    script_style.cw_us.count = 0;
                    script_style.cw_us.scope = SCS_none;
                } else {
                    script_style.style &= ~type;
                    cw_info->count = 0;
                    cw_info->scope = SCS_none;
                }
                pa = p;                         // no following text
            } else if( (len == 2)
              && (strnicmp( pa, "ON", len ) == 0) ) {  // ON
                if( type == SCT_bi ) {
                    script_style.style |= SCT_bd;
                    script_style.cw_bd.count = 0;
                    script_style.cw_bd.scope = SCS_on;
                    script_style.style |= SCT_us;
                    script_style.cw_us.count = 0;
                    script_style.cw_us.scope = SCS_on;
                } else {
                    script_style.style |= type;
                    cw_info->count = 0;
                    cw_info->scope = SCS_on;
                }
                pa = p;
            } else {                            // no following text
                gn.arg.s = g_tok_start;
                gn.arg.e = scan_stop;
                gn.ignore_blanks = false;
                cc = getnum( &gn );             // try numeric expression evaluation
                if( (cc == notnum) || (cc == neg) ) {
                    if( ProcFlags.concat ) {
                        p = pa;                 // output as text
                    } else {
                        pb++;                   // over blank after control word
                        pa = pb;                 // keep spaces at start of text when CO is OFF
                    }
                } else if( gn.result == 0 ) {
                    pa = p;                     // nothing happens at all
                } else {                        // number > 0
                    if( type == SCT_bi ) {
                        script_style.style |= SCT_bd;
                        script_style.cw_bd.count = gn.result;
                        script_style.cw_bd.scope = SCS_count;
                        script_style.style |= SCT_us;
                        script_style.cw_us.count = gn.result;
                        script_style.cw_us.scope = SCS_count;
                    } else {
                        script_style.style |= type;
                        cw_info->count = gn.result;
                        cw_info->scope = SCS_count;
                    }
                    pa = p;                     // no following text
                }
            }
        }
    }

    if( *pa != '\0' ) {
        if( type == SCT_bi ) {
            script_style.style |= SCT_bd;
            script_style.cw_bd.count = 0;
            script_style.cw_bd.scope = SCS_line;
            script_style.style |= SCT_us;
            script_style.cw_us.count = 0;
            script_style.cw_us.scope = SCS_line;
        } else {
            script_style.style |= type;
            cw_info->count = 0;
            cw_info->scope = SCS_line;
        }
        process_text( pa, g_curr_font );         // submit text on the same line
        if( type == SCT_bi ) {
            script_style.style &= ~SCT_bd;
            script_style.cw_bd.scope = SCS_none ;
            script_style.style &= ~SCT_us;
            script_style.cw_us.scope = SCS_none;
        } else {
            script_style.style &= ~type;
            cw_info->scope = SCS_none;
        }
        g_curr_font = script_style.font;
    }

    scan_restart = scan_stop;
    return;

}

void scr_bd( void )
{
    scr_style_common( SCT_bd, &script_style.cw_bd );
    return;
}

void scr_bi( void )
{
    scr_style_common( SCT_bi, NULL );   // BI is handled in a special manner
    return;
}

void scr_us( void )
{
    scr_style_common( SCT_us, &script_style.cw_us );
    return;
}

/************************************************************************/
/* scr_style_end() terminates all processing controlled by the control  */
/*   words defined in this file                                         */
/* for example, the next tag will cause this function to be called      */
/************************************************************************/

void scr_style_end( void )
{
    script_style.style = SCT_none;
    script_style.cw_bd.count = 0;
    script_style.cw_bd.scope = SCS_none;
    script_style.cw_us.count = 0;
    script_style.cw_us.scope = SCS_none;

    g_curr_font = script_style.font;

    return;
}

/************************************************************************/
/* scr_style_font() adjusts script_style for a new input line and       */
/*   returns the resulting font                                         */
/************************************************************************/

font_number scr_style_font( font_number in_font )
{
    font_number     font;

    if( !input_cbs->fm_hh ) {                           // only adjust for SOL
        if( script_style.cw_bd.scope == SCS_count ) {   // adjust/remove BD if appropriate
            if( script_style.cw_bd.count > 0 ) {
                script_style.cw_bd.count --;
            } else {
                script_style.cw_bd.scope = SCS_none;
                script_style.style &= ~SCT_bd;
            }
        }

        if( script_style.cw_us.scope == SCS_count ) {   // adjust/remove US if appropriate
            if( script_style.cw_us.count > 0 ) {
                script_style.cw_us.count --;
            } else {
                script_style.cw_us.scope = SCS_none;
                script_style.style &= ~SCT_us;
            }
        }
    }

    if( (script_style.style & SCT_bd) && (script_style.style & SCT_us) ) {
        font = 3;
    } else if( script_style.style & SCT_bd ) {
        font = 2;
    } else if( script_style.style & SCT_us ) {
        font = 1;
    } else {                                // no script_style
        font = in_font;                     // TBD
        g_curr_font = script_style.font;    // restore current font
    }

    return( font );
}
