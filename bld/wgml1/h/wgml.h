/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  WGML global include header.
*
****************************************************************************/

#ifndef WGML_H_INCLUDED
#define WGML_H_INCLUDED

#define __STDC_WANT_LIB_EXT1__  1   /* use safer C library             */

#include <stddef.h>
#include <stdio.h>
#include <setjmp.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <limits.h>
#if defined( __WATCOMC__ ) || !defined( __UNIX__ )
#include <process.h>
#endif
#include "wio.h"

#include "copfiles.h"       // mostly for access to bin_device & wgml_fonts
#include "gvars.h"
#include "findfile.h"


#ifdef  __cplusplus
extern "C" {    /* Use "C" linkage when in C++ mode */
#endif


//================= Function Prototypes ========================

/* copfiles.c                           */
extern char         cop_in_trans( char in_char, font_number font );
extern void         cop_setup( void );
extern void         cop_teardown( void );
extern uint32_t     cop_text_width( const char *text, size_t count, font_number font );
extern void         cop_ti_table( const char *p );
extern void         fb_dbox( dbox_element *in_dbox );
extern void         fb_document( void );
extern void         fb_document_page( void );
extern void         fb_finish( void );
extern void         fb_hline( hline_element *in_hline );
extern void         fb_output_textline( text_line *out_line );
extern void         fb_start( void );
extern void         fb_vline( vline_element *in_vline );


/* devfuncs.c                           */
extern void         fb_absoluteaddress( void );
extern void         fb_new_section( uint32_t v_start );
extern void         fb_position( uint32_t h_start, uint32_t v_start );
extern void         set_oc_pos( void );


/* gargutil.c                           */
extern  condcode    getarg( void );
extern  condcode    getqst( void );
extern  bool        is_quote_char( char c );
extern  bool        is_function_char( char c );
extern  bool        is_att_char( char c );
extern  bool        is_id_char( char c );
extern  bool        is_macro_char( char c );
extern  bool        is_space_tab_char( char c );
extern  bool        is_stop_char( char c );
extern  bool        is_symbol_char( char c );
extern  char        parse_char( const char *pa, size_t len );
extern  void        unquote_if_quoted( tok_type *tok );

/* gbanner.c                            */
extern  void        out_ban_bot( void );
extern  void        out_ban_top( void );
extern  void        set_banners( void );
extern  void        set_headx_banners( int hx_lvl );
extern  void        set_pgnum_style( void );


/* gdata.c                              */
extern  void        init_global_vars( void );
extern  void        init_pass_data( void );


/* gdeflay.c                            */
extern  void        init_def_lay( void );


/* gdocpage.c                           */
extern  void        do_page_out( void );
extern  void        full_col_out( void );
extern  void        insert_col_bot( doc_el_group * a_group );
extern  void        insert_col_fn( doc_el_group * a_group );
extern  void        insert_col_main( doc_element * a_element );
extern  void        insert_col_width( doc_el_group * a_group );
extern  void        insert_page_width( doc_el_group * a_group );
extern  void        last_col_out( void );
extern  void        last_page_out( void );
extern  void        next_column( void );
extern  void        reset_bot_ban( void );
extern  void        reset_t_page( void );
extern  void        reset_top_ban( void );
extern  void        set_skip_vars( su * pre_skip, su * pre_top_skip, su * post_skip, text_space text_spacing, font_number font );
extern  bool        split_element( doc_element * a_element, uint32_t req_depth );
extern  void        text_col_out( void );


/* gdocsect.c                           */
extern  void        set_section_banners( doc_section ds );
extern  void        start_doc_sect( void );


/* getnum.c                             */
extern  condcode    getnum( getnum_block * gn );


/* ghx.c                                */
extern  void        gen_heading( char *h_text, const char *hdrefid, hdsrc hn_lvl, hdsrc hds_lvl );
extern  void        out_head_page( ffh_entry * in_entry, ref_entry * in_ref, uint32_t in_pageno );


/* gindexut.c                           */
extern  void        eol_index_page( eol_ix * eol_index, uint32_t page_nr );
extern  void        find_create_ix_e_entry( ix_h_blk * ixhwork, char * ref, size_t len, ix_h_blk * seeidwork, ereftyp type );
extern  ix_h_blk    *find_create_ix_h_entry( ix_h_blk * ixhwork, ix_h_blk * ixhbase, char * printtxt, size_t printtxtlen, char * txt, size_t txtlen, uint32_t lvl );
extern  void        free_index_dict( ix_h_blk ** dict );
extern  void        init_entry_list( ix_h_blk * term );


/* glayutil.c                           */
extern  void        eat_lay_sub_tag( void );
extern  void        free_layout( void );
extern  condcode    lay_attr_and_value( lay_att_val *lay_attr );
extern  bool        i_case( char *p, lay_attr_i lay_attr, case_t * tm );
extern  void        o_case( FILE *fp, lay_attr_o lay_attr, const case_t * tm );
extern  bool        i_char( char *p, lay_attr_i lay_attr, char * tm );
extern  void        o_char( FILE *fp, lay_attr_o lay_attr, const char * tm );
extern  bool        i_content( char *p, lay_attr_i lay_attr, content * tm );
extern  void        o_content( FILE *fp, lay_attr_o lay_attr, const content * tm );
extern  bool        i_default_frame( char *p, lay_attr_i lay_attr, def_frame * tm );
extern  void        o_default_frame( FILE *fp, lay_attr_o lay_attr, const def_frame * tm );
extern  bool        i_docsect( char *p, lay_attr_i lay_attr, ban_docsect * tm );
extern  void        o_docsect( FILE *fp, lay_attr_o lay_attr, const ban_docsect * tm );
extern  bool        i_frame( char *p, lay_attr_i lay_attr, bool * tm );
extern  void        o_frame( FILE *fp, lay_attr_o lay_attr, const bool * tm );
extern  bool        i_int32( char *p, lay_attr_i lay_attr, int32_t * tm );
extern  void        o_int32( FILE *fp, lay_attr_o lay_attr, const int32_t * tm );
extern  bool        i_int8( char *p, lay_attr_i lay_attr, int8_t * tm );
extern  void        o_int8( FILE *fp, lay_attr_o lay_attr, const int8_t * tm );
extern  bool        i_spacing( char *p, lay_attr_i lay_attr, text_space *tm );
extern  void        o_spacing( FILE *fp, lay_attr_o lay_attr, const text_space *tm );
extern  bool        i_font_number( char *p, lay_attr_i lay_attr, font_number *tm );
extern  void        o_font_number( FILE *fp, lay_attr_o lay_attr, const font_number *tm );
extern  bool        i_number_form( char *p, lay_attr_i lay_attr, num_form * tm );
extern  void        o_number_form( FILE *fp, lay_attr_o lay_attr, const num_form * tm );
extern  bool        i_number_style( char *p, lay_attr_i lay_attr, num_style * tm );
extern  void        o_number_style( FILE *fp, lay_attr_o lay_attr, const num_style * tm );
extern  bool        i_page_eject( char *p, lay_attr_i lay_attr, page_ej * tm );
extern  void        o_page_eject( FILE *fp, lay_attr_o lay_attr, const page_ej * tm );
extern  bool        i_page_position( char *p, lay_attr_i lay_attr, page_pos * tm );
extern  void        o_page_position( FILE *fp, lay_attr_o lay_attr, const page_pos * tm );
extern  bool        i_place( char *p, lay_attr_i lay_attr, bf_place * tm );
extern  void        o_place( FILE *fp, lay_attr_o lay_attr, const bf_place * tm );
extern  bool        i_pouring( char *p, lay_attr_i lay_attr, reg_pour * tm );
extern  void        o_pouring( FILE *fp, lay_attr_o lay_attr, const reg_pour * tm );
extern  bool        i_space_unit( char *p, lay_attr_i lay_attr, su * tm );
extern  void        o_space_unit( FILE *fp, lay_attr_o lay_attr, const su * tm );
extern  bool        i_threshold( char *p, lay_attr_i lay_attr, uint16_t * tm );
extern  void        o_threshold( FILE *fp, lay_attr_o lay_attr, const uint16_t * tm );
extern  bool        i_xx_string( char *p, lay_attr_i lay_attr, xx_str * tm );
extern  void        o_xx_string( FILE *fp, lay_attr_o lay_attr, const xx_str * tm );
extern  bool        i_date_form( char *p, lay_attr_i lay_attr, xx_str * tm );
extern  void        o_date_form( FILE *fp, lay_attr_o lay_attr, const xx_str * tm );
extern  bool        i_yes_no( char *p, lay_attr_i lay_attr, bool * tm );
extern  void        o_yes_no( FILE *fp, lay_attr_o lay_attr, const bool * tm );


/* glbandef.c                           */
extern void         banner_defaults( void );


/* gmacdict.c                           */
extern  void            add_macro_entry( mac_dict_hdl dict, mac_entry *me );
extern  mac_dict_hdl    init_macro_dict( void );
extern  void            free_macro_dict( mac_dict_hdl dict );
extern  void            free_macro_entry( mac_dict_hdl dict, mac_entry *me );
extern  void            print_macro_dict( mac_dict_hdl dict, bool with_mac_lines );
extern  mac_entry       *find_macro( mac_dict_hdl dict, const char *macname );


/* gmemory.c                            */
extern void             *mem_alloc( size_t size );
extern void             mem_banner( void );
extern void             mem_free( void * p );
extern void             mem_init( void );
extern void             mem_fini( void );
extern unsigned long    mem_get_peak_usage( void );
extern void             mem_prt_curr_usage( void );
extern void             *mem_realloc( void * p, size_t size );
extern int              mem_validate( void );
extern void             mem_prt_list( void );
extern char             *mem_strdup( const char *str );

/* gnestut.c                            */
extern  void            init_nest_cb( void );
extern  nest_stack  *   copy_to_nest_stack( void );


/* goptions.c                           */
extern  int     proc_options( char * cmdline );
extern  void    split_attr_file( char * filename, char * attr, size_t attrlen );


/* gpagegeo.c                           */
extern  void    do_layout_end_processing( void );
extern  void    init_def_margins( void );
extern  void    init_page_geometry( void );
extern  void    set_page_position( doc_section ds );


/* gppcnote.c                           */
extern  void    do_force_pc( char * p );


/* gprocess.c                           */
extern  void        classify_record( char firstchar );
extern  void        finalize_subscript( char * * result, bool splittable );
extern  void        process_late_subst( char * buf );
extern  void        process_line( void );
extern  bool        resolve_symvar_functions( char * buf, bool splittable );
extern  void        split_input( char * buf, char * split_pos, i_flags fmflags );


/* gproctxt.c                           */
extern  void            do_justify( uint32_t left_m, uint32_t right_m, text_line *line );
extern  void            insert_hard_spaces( const char * spaces, size_t len, font_number font );
extern  size_t          intrans( char *text, size_t count, font_number font );
extern  void            process_line_full( text_line *a_line, bool justify );
extern  void            process_text( char *text, font_number font );
extern  text_chars  *   process_word( const char *text, size_t count, font_number font, bool hard_spaces );
extern  void            set_h_start( void );


/* grefdict.c                           */
extern  void        add_ref_entry( ref_dict *dict, ref_entry *me );
extern  void        init_ref_dict( ref_dict *dict );
extern  void        free_ref_dict( ref_dict *dict );
extern  void        print_ref_dict( ref_dict dict, const char *type );
extern  ref_entry   *find_refid( ref_dict dict, const char *refid );
extern  void        init_ref_entry( ref_entry *re, const char *refid, ffh_entry *ffh );
extern  char        *get_refid_value( char *p, att_val_type *attr_val, char *refid );
extern  ref_entry   *add_new_refid( ref_dict *dict, const char *refid, ffh_entry *ffh );

/* gresrch.c                            */
extern  void    add_GML_tag_research( char * tag );
extern  void    free_GML_tags_research( void );
extern  void    print_GML_tags_research( void );
extern  void    add_SCR_tag_research( char * tag );
extern  void    free_SCR_tags_research( void );
extern  void    print_SCR_tags_research( void );
extern  void    add_single_func_research( char * in );
extern  void    free_single_funcs_research( void );
extern  void    print_single_funcs_research( void );
extern  void    add_multi_func_research( char * in );
extern  void    free_multi_funcs_research( void );
extern  void    print_multi_funcs_research( void );
extern  void    printf_research( char * msg, ... );
extern  void    test_out_t_line( text_line  * a_line );


/* gsbdbius.c                           */
extern  void        scr_style_end( void );
extern  font_number scr_style_font( font_number in_font );


/* gsbr.c                               */
extern  void    script_process_break( void );


/* gsbx.c                               */
extern  void    eoc_bx_box( void );


/* gscan.c                              */
extern  const   gmltag  *   find_lay_tag( char * token, size_t toklen );
extern  const   gmltag  *   find_sys_tag( char * token, size_t toklen );
extern          char    *   get_text_line( char * p );
extern          bool        is_ip_tag( e_tags offset );
extern          void        set_overload( struct gtentry *in_gt );
extern          void        scan_line( void );
extern          void        set_if_then_do( ifcb * cb );
extern          condcode    test_process( ifcb * cb );


/* gsfbfk.c                             */
extern  void                fb_blocks_out( void );


/* gsfuncs.c                            */
extern  char    *   scr_multi_funcs( char * in, char * pstart, char * * ppval, int32_t valsize );


/* gsfunelu.c                           */
extern  char    *   scr_single_funcs( char * in, char * end, char * * result );


/* gsgoto.c                             */
extern  void    print_labels( labelcb * lb, const char * name );
extern  bool    gotarget_reached( void );
extern  void    gotarget_err( void );


/* gsgt.c                               */
extern  void    init_tag_att( void );
extern  bool    get_tag_name( const char *p, char *tagname );


/* gsifdoel.c                           */
extern  void    show_ifcb( char * txt, ifcb * cb );


/* gsetvar.c                            */
extern char *   scan_sym( char * p, symvar * sym, sub_index * subscript, char * * result, bool splittable );


/* gsmacro.c                            */
extern  void    add_macro_cb_entry( mac_entry * me, struct gtentry *ge );
extern  void    add_macro_parms( char * p );
extern  void    free_lines( inp_line * line );
extern  bool    get_macro_name( const char *p, char *macname );


/* gspe.c                               */
extern  void    reset_pe_cb( void );


/* gspu.c                               */
extern  void    close_pu_file( int numb );
extern  void    close_all_pu_files( void );


/* gsymvar.c                            */
extern sym_dict_hdl init_dict( bool global );
extern void         free_dict( sym_dict_hdl dict );
extern int          find_symvar( sym_dict_hdl dict, char * name, sub_index subscript, symsub * * symsubval );
extern int          find_symvar_l( sym_dict_hdl dict, char * name, sub_index subscript, symsub * * symsubval );
extern int          add_symvar( sym_dict_hdl dict, char * name, char * val, sub_index subscript, symbol_flags f );
extern int          add_symvar_addr( sym_dict_hdl dict, char * name, char * val, sub_index subscript, symbol_flags f, symsub * * sub );
extern void         print_sym_dict( sym_dict_hdl dict );
extern void         reset_auto_inc_dict( sym_dict_hdl dict );


/* gsyssym.c                            */
extern void         add_to_sysdir( char * var_name, char char_val );
extern sym_dict_hdl init_sys_dict( void );
extern void         init_sysparm( char * cmdline, char * banner );
extern void         link_sym( sym_dict_hdl dict, symvar * sym );


/* gtagdict.c                           */
extern  struct gtentry  *add_tag( tag_dict_hdl *dict, const char *tagname, const char *macname, const int flags );
extern  struct gtentry  *change_tag( tag_dict_hdl dict, const char *tagname, const char *macname );
extern  tag_dict_hdl    init_tag_dict( void );
extern  void            free_tag_dict( tag_dict_hdl dict );
extern  struct gtentry  *free_tag( tag_dict_hdl *dict, struct gtentry *ge );
extern  void            print_tag_dict( tag_dict_hdl dict );
extern  void            print_tag_entry( struct gtentry *entry );
extern  struct gtentry  *find_tag( tag_dict_hdl dict, const char *tagname );

/* gtxtpool.c                           */
extern  void                add_single_text_chars_to_pool( text_chars * a_chars );
extern  void                add_text_chars_to_pool( text_line * a_line );
extern  text_chars      *   alloc_text_chars( const char *text, size_t cnt, font_number font );
extern  void                add_text_line_to_pool( text_line * a_line );
extern  text_line       *   alloc_text_line( void );
extern  void                add_box_col_set_to_pool( box_col_set * a_set );
extern  box_col_set     *   alloc_box_col_set( void );
extern  void                add_box_col_stack_to_pool( box_col_stack * a_stack );
extern  box_col_stack   *   alloc_box_col_stack( void );
extern  void                add_doc_el_to_pool( doc_element * a_element );
extern  doc_element     *   alloc_doc_el( element_type type );
extern  void                add_doc_el_group_to_pool( doc_el_group * a_group );
extern  doc_el_group    *   alloc_doc_el_group( group_type type );
extern  void                add_eol_ix_to_pool( eol_ix * an_eol_ix );
extern  eol_ix          *   alloc_eol_ix( ix_h_blk * in_ixh, ereftyp in_type );
extern  void                add_sym_list_entry_to_pool( sym_list_entry * cb );
extern  sym_list_entry  *   alloc_sym_list_entry( void );
extern  void                add_tag_cb_to_pool( tag_cb * cb );
extern  tag_cb          *   alloc_tag_cb( void );
extern  void                clear_doc_element( doc_element * a_element );
extern  void                free_pool_storage( void );
extern  doc_element     *   init_doc_el( element_type type, uint32_t depth );


/* gtitlepo.c                           */
extern  void        titlep_output( void );


/* gusertag.c                           */
extern  bool        process_tag( struct gtentry *ge, mac_entry *me );


/* gutil.c                              */
extern  void        add_dt_space( void );
extern  bool        att_val_to_su( su *spaceunit, bool pos, att_val_type *attr_val, bool specval );
extern  bool        cw_val_to_su( char **scaninput, su *spaceunit );
extern  int32_t     conv_hor_unit( su *spaceunit, font_number font );
extern  int32_t     conv_vert_unit( su *spaceunit, text_space text_spacing, font_number font );
extern  num_style   find_pgnum_style( void );
extern  char        *format_num( uint32_t n, char *r, size_t rsize, num_style ns );
extern  void        free_ffh_list( ffh_entry *ffh_list );
extern  void        free_fwd_refs( fwd_ref *fwd_refs );
extern  void        g_keep_nest( const char *cw_tag );
extern  char        *get_att_name( char *p, char **pa, char *attname );
extern  char        *get_att_value( char *p, att_val_type *attr_val );
extern  font_number get_font_number( char *p, size_t len );
extern  char        *get_tag_value( char *p, att_val_type *attr_val );
extern  ffh_entry   *init_ffh_entry( ffh_entry *ffh_list );
extern  fwd_ref     *init_fwd_ref( fwd_ref *fr_dict, const char *fr_id );
extern  char        *int_to_roman( uint32_t n, char *r, size_t rsize );
extern  bool        lay_init_su( const char *p, su *spaceunit );
extern  size_t      len_to_trail_space( const char *p , size_t len );
extern  char        *skip_to_quote( char *p, char quote );
extern  int         uinttohex( unsigned value, char *buffer );
extern  int         uinttodec( unsigned value, char *buffer );
extern  int         sinttodec( int value, char *buffer );


/* messages.c                           */
extern  void    out_msg( const char * fmt, ... );
extern  void    out_msg_research( const char *msg, ... );
extern  void    g_info( const msg_ids err, ... );
extern  void    g_info_lm( const msg_ids err, ... );
extern  void    g_info_research( const msg_ids num, ... );
extern  void    file_mac_info( void );
extern  void    file_mac_info_nest( void );
extern  void    att_req_err( const char *tagname, const char *attname );
extern  void    ban_reg_err( msg_ids num, banner_lay_tag * in_ban1, banner_lay_tag * in_ban2, region_lay_tag * in_reg1, region_lay_tag * in_reg2 );
extern  void    internal_err( const char * file, int line );
extern  void    list_level_err( const char * xl_tag, uint8_t xl_level );
extern  void    main_file_err( const char * filename );
extern  void    numb_err( void );
extern  void    symbol_name_length_err( const char * symname );
extern  void    val_parse_err( const char * pa, bool tag );
extern  void    dup_id_err( const char * id, const char * context );
extern  void    g_err_if_int( void );
extern  void    g_err_tag( const char *tagname );
extern  void    g_err_tag_mac( struct gtentry * ge );
extern  void    g_err_tag_nest( const char *tagname );
extern  void    g_err_tag_no( const char *tagname );
extern  void    g_err_tag_prec( const char *tagname );
extern  void    g_err_tag_rsloc( locflags inloc, const char * pa );
extern  void    g_wng_hlevel( hdsrc hd_found, hdsrc hd_expected );
extern  void    keep_nest_err( const char * arg1, const char * arg2 );
extern  void    xx_err( const msg_ids errid );
extern  void    xx_err_c( const msg_ids errid, const char * arg );
extern  void    xx_err_cc( const msg_ids errid, const char * arg1, const char * arg2 );
extern  void    xx_line_err_c( const msg_ids errid, const char * pa );
extern  void    xx_line_err_ci( const msg_ids errid, const char * pa, size_t len );
extern  void    xx_line_err_cc( const msg_ids errid, const char * cw, const char * pa );
extern  void    xx_line_err_cci( const msg_ids errid, const char * cw, const char * pa, size_t len );
extern  void    xx_nest_err( const msg_ids errid );
extern  void    xx_nest_err_cc( const msg_ids errid, const char * arg1, const char * arg2 );
extern  void    xx_simple_err( const msg_ids errid );
extern  void    xx_simple_err_c( const msg_ids errid, const char * arg );
extern  void    xx_simple_err_i( const msg_ids errid, int arg );
extern  void    xx_simple_err_cc( const msg_ids errid, const char * arg1, const char * arg2 );
extern  void    xx_source_err( const msg_ids errid );
extern  void    xx_source_err_c( const msg_ids errid, const char * arg );
extern  void    xx_warn( const msg_ids errid );
extern  void    xx_warn_c( const msg_ids errid, const char * arg );
extern  void    xx_warn_c_info( const msg_ids errid, const char * arg, const msg_ids warnid );
extern  void    xx_warn_c_info_c( const msg_ids errid, const char * arg1, const msg_ids warnid, const char * arg2 );
extern  void    xx_warn_cc( const msg_ids errid, const char * arg1, const char * arg2 );
extern  void    xx_warn_info_cc( const msg_ids errid, const msg_ids warnid, const char * arg1, const char * arg2 );
extern  void    xx_line_warn_c( const msg_ids errid, const char * pa );
extern  void    xx_line_warn_cc( const msg_ids errid, const char * cw, const char * pa );
extern  void    xx_simple_warn( const msg_ids errid );
extern  void    xx_simple_warn_info_cc( const msg_ids errid, const char * arg1, const msg_ids warnid, const char * arg2 );


/* outbuff.c                            */
extern void     cop_tr_table( const char *p );
extern void     ob_binclude( binclude_element * in_el );
extern void     ob_graphic( graphic_element * in_el );
extern void     ob_oc( const char *p );


/* wgmlmsg.c                            */
extern  bool    init_msgs( void );
extern  void    fini_msgs( void );
extern  bool    get_msg( msg_ids resourceid, char *buffer, size_t buflen );
//extern  void Msg_Do_Put_Args( char rc_buff[], MSG_ARG_LIST *arg_info, char *types, ... );
//extern  void Msg_Put_Args( char message[], MSG_ARG_LIST *arg_info, char *types, va_list *args );


/* wgmlsupp.c                           */
extern  void    free_some_mem( void );
extern  void    g_banner( void );
// extern  char  * get_filename_full_path( char * buff, const char * name, size_t max );
extern  bool    check_realloc_line_buff( size_t len );
extern  bool    get_line( bool researchoutput );
extern  void    inc_inc_level( void );
extern FILE     *fopen_rb( const char *fname );
#if defined( __WATCOMC__ )
#pragma aux     my_exit __aborts;
#endif
extern  void    my_exit( int );
extern  void    show_include_stack( void );


/*
 * prototypes for the gml + layout tag processing routines
 */
#define pickc( name, length, gmlfunc, gmlflags, gmllocflags, clsflags, layfunc, layflags, laylocflags ) \
    extern void gmlfunc( const gmltag *entry ); \
    extern void layfunc( const gmltag *entry );
#define pickg( name, length, gmlfunc, gmlflags, gmllocflags, clsflags ) \
    extern void gmlfunc( const gmltag *entry );
#define pickl( name, length, layfunc, layflags, laylocflags ) \
    extern void layfunc( const gmltag *entry );
#include "gtags.h"
#undef pickl
#undef pickg
#undef pickc

/*
 * prototypes for the layout tag attribute processing routines
 */

/*          for input scanning              */
#define pick( name, funci, funco, restype )     extern  bool    funci( char *buf, lay_attr_i attr, restype *result );
#include "glayutil.h"
#undef pick

/*          for output via :convert tag     */
#define pick( name, funci, funco, restype )     extern  void    funco( FILE *fp, lay_attr_o attr, const restype *in );
#include "glayutil.h"
#undef pick

/*
 * prototypes for the script control word processing routines
 */

#define picklab( name, routine, flags )  extern void routine( void );
#define picks( name, routine, flags )  extern void routine( void );
#include "gscrcws.h"
#undef picks
#undef picklab

/*
 * prototypes for the script string function routines , ie. &'substr( ,..
 */

#define pick( name, length, mand_parms, opt_parms, routine ) \
    extern condcode routine( parm parms[MAX_FUN_PARMS], int parm_count, char **ppval, int32_t valsize );
#include "gsfuncs.h"
#undef pick

#ifdef  __cplusplus
}   /* End of "C" linkage for C++ */
#endif

#endif  /* WGML_H_INCLUDED */
