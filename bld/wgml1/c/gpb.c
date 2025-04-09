/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  WGML tag  :PB trial implementation
*                         PB is not documented, so this is guessing
****************************************************************************/


#include    "wgml.h"


/***************************************************************************/
/*  :PB.                                                                   */
/*                                                                         */
/*  not documented in Reference Manual                                     */
/*  Probable full name: Paragraph Break                                    */
/*  when used after P, PB, PC, or LP: passess g_post_skip on to next       */
/*    document element                                                     */
/*  otherwise, uses the g_post_skip as a pre_skip                          */
/*                                                                         */
/*  NOTE: this works when concatenation is ON, at least at present         */
/*        it also works when PB follows P and conatenation is OFF          */
/*        but fails otherwise when concatenation is OFF                    */
/***************************************************************************/

extern  void    gml_pb( const gmltag * entry )
{
    bool        has_text;   // will be true if following P, PB, PC, or LP
    char    *   p;

    (void)entry;

    scan_err = false;
    p = scan_start;

    start_doc_sect();                   // if not already done
    if( ProcFlags.concat ) {
        ProcFlags.para_starting = false;// avoid set_skip_vars in script_process_break()
    }
    has_text = ProcFlags.para_has_text; // save value
    script_process_break();                // clears ProcFlags.para_has_text

    if( ProcFlags.concat ) {
        if( has_text ) {
            ProcFlags.skips_valid = true;       // keep existing skips inside paragraph
        }
        ProcFlags.para_has_text = has_text; // reset flag
    }

    SkipDot( p );                       // over '.'
    post_space = 0;
    if( *p != '\0' ) {
        process_text( p, g_curr_font );
    }

    scan_start = scan_stop + 1;
    return;
}

