/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description: WGML implement :CONVERT LAYOUT tag
*                              and file output for all subtags
*
****************************************************************************/


#include "wgml.h"


/***************************************************************************/
/* Layout attributes as character strings                                  */
/***************************************************************************/

const   char    att_names[e_dummy_max + 1][18] = {
//   18 is enough for longest attribute name  ( extract_threshold )
    { "DUMMY" },                       // enum zero not used
    #define pick( name, funci, funco, result ) { #name },
    #include "glayutil.h"
    #undef pick
};


/***************************************************************************/
/*                                                                         */
/*  Format: :CONVERT file='file name'.                                     */
/*                                                                         */
/*  alternate, undocumented but used format:                               */
/*                                                                         */
/*          :CONVERT.FILE.EXT                                              */
/*                                                                         */
/* Convert the current layout into the specified file name. The resulting  */
/* file will contain the entire layout in a readable form.                 */
/***************************************************************************/

/***************************************************************************/
/*   output for :ABSTRACT or :PREFACE values                               */
/***************************************************************************/
static  void    put_lay_abspref( FILE * layfile, abspref_lay_tag * ap,
                                 hx_sect_lay_tag * apsect, char * name )
{
    int                 k;
    lay_att             curr;

    fprintf_s( layfile, ":%s\n", name );

    for( k = 0, curr = abspref_att[k]; curr > 0; k++, curr = abspref_att[k] ) {

        switch( curr ) {
        case   e_post_skip:
            o_space_unit( layfile, curr, &apsect->post_skip );
            break;
        case   e_pre_top_skip:
            o_space_unit( layfile, curr, &apsect->pre_top_skip );
            break;
        case   e_font:
            o_font_number( layfile, curr, &apsect->text_font );
            break;
        case   e_spacing:
            o_spacing( layfile, curr, &apsect->spacing );
            break;
        case   e_header:
            o_yes_no( layfile, curr, &apsect->header );
            break;
        case   e_abstract_string:
            if( *name == 'A' ) {        // :Abstract output
                o_xx_string( layfile, curr, ap->string );
            }
            break;
        case   e_preface_string:
            if( *name == 'P' ) {        // :Preface output
                o_xx_string( layfile, curr, ap->string );
            }
            break;
        case   e_page_eject:
            o_page_eject( layfile, curr, &ap->page_eject );
            break;
        case   e_page_reset:
            o_yes_no( layfile, curr, &ap->page_reset );
            break;
        case   e_columns:
            o_int8( layfile, curr, &ap->columns );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}

static  void    put_lay_abstract( FILE * layfile, layout_data * lay )
{
    put_lay_abspref( layfile, &(lay->abstract), &(lay->hx.hx_sect[hds_abstract]), "ABSTRACT" );
}

static  void    put_lay_preface( FILE * layfile, layout_data * lay )
{
    put_lay_abspref( layfile, &(lay->preface), &(lay->hx.hx_sect[hds_preface]), "PREFACE" );
}


/***************************************************************************/
/*   :ADDRESS   output address attribute values                            */
/***************************************************************************/
static  void    put_lay_address( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             curr;

    fprintf_s( layfile, ":ADDRESS\n" );

    for( k = 0, curr = address_att[k]; curr > 0; k++, curr = address_att[k] ) {

        switch( curr ) {
        case   e_left_adjust:
            o_space_unit( layfile, curr, &lay->address.left_adjust );
            break;
        case   e_right_adjust:
            o_space_unit( layfile, curr, &lay->address.right_adjust );
            break;
        case   e_page_position:
            o_page_position( layfile, curr, &lay->address.page_position );
            break;
        case   e_font:
            o_font_number( layfile, curr, &lay->address.font );
            break;
        case   e_pre_skip:
            o_space_unit( layfile, curr, &lay->address.pre_skip );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :ALINE     output aline attribute values                              */
/***************************************************************************/
static  void    put_lay_aline( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             curr;

    fprintf_s( layfile, ":ALINE\n" );

    for( k = 0, curr = aline_att[k]; curr > 0; k++, curr = aline_att[k] ) {

        switch( curr ) {
        case   e_skip:
            o_space_unit( layfile, curr, &lay->aline.skip );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :APPENDIX  output attribute values                                    */
/***************************************************************************/
static  void    put_lay_appendix( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             curr;

    fprintf_s( layfile, ":APPENDIX\n" );

    for( k = 0, curr = appendix_att[k]; curr > 0; k++, curr = appendix_att[k] ) {

        switch( curr ) {
        case   e_indent:
            o_space_unit( layfile, curr, &lay->hx.hx_head[hds_appendix].indent );
            break;
        case   e_pre_top_skip:
            o_space_unit( layfile, curr, &lay->hx.hx_sect[hds_appendix].pre_top_skip );
            break;
        case   e_pre_skip:
            o_space_unit( layfile, curr, &lay->hx.hx_head[hds_appendix].pre_skip );
            break;
        case   e_post_skip:
            o_space_unit( layfile, curr, &lay->hx.hx_sect[hds_appendix].post_skip );
            break;
        case   e_spacing:
            o_spacing( layfile, curr, &lay->hx.hx_sect[hds_appendix].spacing );
            break;
        case   e_font:
            o_font_number( layfile, curr, &lay->hx.hx_sect[hds_appendix].text_font );
            break;
        case   e_number_font:
            o_font_number( layfile, curr, &lay->hx.hx_head[hds_appendix].number_font );
            break;
        case   e_number_form:
            o_number_form( layfile, curr, &lay->hx.hx_head[hds_appendix].number_form );
            break;
        case   e_page_position:
            o_page_position( layfile, curr, &lay->hx.hx_head[hds_appendix].line_position );
            break;
        case   e_number_style:
            o_number_style( layfile, curr, &lay->hx.hx_head[hds_appendix].number_style );
            break;
        case   e_page_eject:
            o_page_eject( layfile, curr, &lay->hx.hx_head[hds_appendix].page_eject );
            break;
        case   e_line_break:
            o_yes_no( layfile, curr, &lay->hx.hx_head[hds_appendix].line_break );
            break;
        case   e_display_heading:
            o_yes_no( layfile, curr, &lay->hx.hx_head[hds_appendix].display_heading );
            break;
        case   e_number_reset:
            o_yes_no( layfile, curr, &lay->hx.hx_head[hds_appendix].number_reset );
            break;
        case   e_case:
            o_case( layfile, curr, &lay->hx.hx_head[hds_appendix].hd_case );
            break;
        case   e_align:
            o_space_unit( layfile, curr, &lay->hx.hx_head[hds_appendix].align );
            break;
        case   e_header:
            o_yes_no( layfile, curr, &lay->hx.hx_sect[hds_appendix].header );
            break;
        case   e_appendix_string:
            o_xx_string( layfile, curr, lay->appendix.string );
            break;
        case   e_page_reset:
            o_yes_no( layfile, curr, &lay->appendix.page_reset );
            break;
        case   e_section_eject:
            o_page_eject( layfile, curr, &lay->appendix.section_eject );
            break;
        case   e_columns:
            o_int8( layfile, curr, &lay->appendix.columns );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :AUTHOR   output author attribute values                              */
/***************************************************************************/
static  void    put_lay_author( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             curr;

    fprintf_s( layfile, ":AUTHOR\n" );

    for( k = 0, curr = author_att[k]; curr > 0; k++, curr = author_att[k] ) {

        switch( curr ) {
        case   e_left_adjust:
            o_space_unit( layfile, curr, &lay->author.left_adjust );
            break;
        case   e_right_adjust:
            o_space_unit( layfile, curr, &lay->author.right_adjust );
            break;
        case   e_page_position:
            o_page_position( layfile, curr, &lay->author.page_position );
            break;
        case   e_font:
            o_font_number( layfile, curr, &lay->author.font );
            break;
        case   e_pre_skip:
            o_space_unit( layfile, curr, &lay->author.pre_skip );
            break;
        case   e_skip:
            o_space_unit( layfile, curr, &lay->author.skip );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   output for :BACKM or :BODY values                                     */
/***************************************************************************/
static  void    put_lay_backbod( FILE * layfile, backbod_lay_tag * bb,
                                 hx_sect_lay_tag * bbsect, char * name )
{
    int                 k;
    lay_att             curr;

    fprintf_s( layfile, ":%s\n", name );

    for( k = 0, curr = backbod_att[k]; curr > 0; k++, curr = backbod_att[k] ) {

        switch( curr ) {
        case   e_post_skip:
            o_space_unit( layfile, curr, &bbsect->post_skip );
            break;
        case   e_pre_top_skip:
            o_space_unit( layfile, curr, &bbsect->pre_top_skip );
            break;
        case   e_header:
            o_yes_no( layfile, curr, &bbsect->header );
            break;
        case   e_body_string:
            if( *(name + 1) == 'O') {   // BODY tag
                o_xx_string( layfile, curr, bb->string );
            }
            break;
        case   e_backm_string:
            if( *(name + 1) == 'A') {   // BACKM tag
                o_xx_string( layfile, curr, bb->string );
            }
            break;
        case   e_page_eject:
            o_page_eject( layfile, curr, &bb->page_eject );
            break;
        case   e_page_reset:
            o_yes_no( layfile, curr, &bb->page_reset );
            break;
        case   e_columns:
            if( *(name + 1) == 'A') {   // BACKM tag
                o_int8( layfile, curr, &bb->columns );
            }
            break;
        case   e_font:
            o_font_number( layfile, curr, &bbsect->text_font );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}

static  void    put_lay_backm( FILE * layfile, layout_data * lay )
{
    put_lay_backbod( layfile, &(lay->backm), &(lay->hx.hx_sect[hds_backm]), "BACKM" );
}

static  void    put_lay_body( FILE * layfile, layout_data * lay )
{
    put_lay_backbod( layfile, &(lay->body), &(lay->hx.hx_sect[hds_body]), "BODY" );
}


/***************************************************************************/
/*  output a banner region                                                 */
/***************************************************************************/
static  void    put_lay_region( FILE * layfile, region_lay_tag * reg )
{
    int                 k;
    lay_att             curr;

    fprintf_s( layfile, ":BANREGION\n" );

    for( k = 0, curr = banregion_att[k]; curr > 0;
         k++, curr = banregion_att[k] ) {

        switch( curr ) {
        case   e_indent:
            o_space_unit( layfile, curr, &reg->indent );
            break;
        case   e_hoffset:
            o_space_unit( layfile, curr, &reg->hoffset );
            break;
        case   e_width:
            o_space_unit( layfile, curr, &reg->width );
            break;
        case   e_voffset:
            o_space_unit( layfile, curr, &reg->voffset );
            break;
        case   e_depth:
            o_space_unit( layfile, curr, &reg->depth );
            break;
        case   e_font:
            o_font_number( layfile, curr, &reg->font );
            break;
        case   e_refnum:
            o_int8( layfile, curr, &reg->refnum );
            break;
        case   e_region_position:
            o_page_position( layfile, curr, &reg->region_position );
            break;
        case   e_pouring:
            o_pouring( layfile, curr, &reg->pouring );
            break;
        case   e_script_format:
            o_yes_no( layfile, curr, &reg->script_format );
            break;
        case   e_contents:
            o_content( layfile, curr, &reg->contents );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
    fprintf_s( layfile, ":eBANREGION\n" );
}


/***************************************************************************/
/*  output a single banner with regions                                    */
/***************************************************************************/
static  void    put_lay_single_ban( FILE * layfile, banner_lay_tag * ban )
{
    int                 k;
    lay_att             curr;
    region_lay_tag  *   reg;

    fprintf_s( layfile, ":BANNER\n" );

    for( k = 0, curr = banner_att[k]; curr > 0; k++, curr = banner_att[k] ) {

        switch( curr ) {
        case   e_left_adjust:
            o_space_unit( layfile, curr, &ban->left_adjust );
            break;
        case   e_right_adjust:
            o_space_unit( layfile, curr, &ban->right_adjust );
            break;
        case   e_depth:
            o_space_unit( layfile, curr, &ban->depth );
            break;
        case   e_place:
            o_place( layfile, curr, &ban->place );
            break;
        case   e_docsect:
            o_docsect( layfile, curr, &ban->docsect );
            break;
        case   e_refplace:
        case   e_refdoc:
            /* no action these are only used for input */
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
    reg = ban->region;
    while( reg != NULL ) {
        put_lay_region( layfile, reg );
        reg = reg->next;
    }

    fprintf_s( layfile, ":eBANNER\n" );
}


/***************************************************************************/
/*   :BANNER   output all banners                                          */
/***************************************************************************/
static  void    put_lay_banner( FILE * layfile, layout_data * lay )
{
    banner_lay_tag      *   ban;

    ban = lay->banner;
    while( ban != NULL ) {
        put_lay_single_ban( layfile, ban );
        ban = ban->next;
    }
}


/***************************************************************************/
/*   :DATE     output date attribute values                                */
/***************************************************************************/
static  void    put_lay_date( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             curr;

    fprintf_s( layfile, ":DATE\n" );

    for( k = 0, curr = date_att[k]; curr > 0; k++, curr = date_att[k] ) {

        switch( curr ) {
        case   e_date_form:
            o_date_form( layfile, curr, lay->date.date_form );
            break;
        case   e_left_adjust:
            o_space_unit( layfile, curr, &lay->date.left_adjust );
            break;
        case   e_right_adjust:
            o_space_unit( layfile, curr, &lay->date.right_adjust );
            break;
        case   e_page_position:
            o_page_position( layfile, curr, &lay->date.page_position );
            break;
        case   e_font:
            o_font_number( layfile, curr, &lay->date.font );
            break;
        case   e_pre_skip:
            o_space_unit( layfile, curr, &lay->date.pre_skip );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :DD        output definition data attribute values                    */
/***************************************************************************/
static  void    put_lay_dd( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             curr;

    fprintf_s( layfile, ":DD\n" );

    for( k = 0, curr = dd_att[k]; curr > 0; k++, curr = dd_att[k] ) {

        switch( curr ) {
        case   e_line_left:
            o_space_unit( layfile, curr, &lay->dd.line_left );
            break;
        case   e_font:
            o_font_number( layfile, curr, &lay->dd.font );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :DEFAULT   output default attribute values                            */
/***************************************************************************/
static  void    put_lay_default( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             curr;

    fprintf_s( layfile, ":DEFAULT\n" );

    for( k = 0, curr = default_att[k]; curr > 0; k++, curr = default_att[k] ) {

        switch( curr ) {
        case   e_spacing:
            o_spacing( layfile, curr, &lay->defaults.spacing );
            break;
        case   e_columns:
            o_int8( layfile, curr, &lay->defaults.columns );
            break;
        case   e_font:
            o_font_number( layfile, curr, &lay->defaults.font );
            break;
        case   e_justify:
            o_yes_no( layfile, curr, &lay->defaults.justify );
            break;
        case   e_input_esc:
            o_char( layfile, curr, &lay->defaults.input_esc );
            break;
        case   e_gutter:
            o_space_unit( layfile, curr, &lay->defaults.gutter );
            break;
        case   e_binding:
            o_space_unit( layfile, curr, &lay->defaults.binding );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :DL        output attribute values                                    */
/***************************************************************************/
static  void    put_lay_dl( FILE * layfile, layout_data * lay )
{
    dl_lay_level    *   dl_layout;
    int                 k;
    lay_att             curr;

    dl_layout = lay->dl.first;

    while( dl_layout != NULL ) {
        fprintf_s( layfile, ":DL\n" );

        for( k = 0, curr = dl_att[k]; curr > 0; k++, curr = dl_att[k] ) {

            switch( curr ) {
            case   e_level:
                o_int8( layfile, curr, &dl_layout->level );
                break;
            case   e_left_indent:
                o_space_unit( layfile, curr, &dl_layout->left_indent );
                break;
            case   e_right_indent:
                o_space_unit( layfile, curr, &dl_layout->right_indent );
                break;
            case   e_pre_skip:
                o_space_unit( layfile, curr, &dl_layout->pre_skip );
                break;
            case   e_skip:
                o_space_unit( layfile, curr, &dl_layout->skip );
                break;
            case   e_spacing:
                o_spacing( layfile, curr, &dl_layout->spacing );
                break;
            case   e_post_skip:
                o_space_unit( layfile, curr, &dl_layout->post_skip );
                break;
            case   e_align:
                o_space_unit( layfile, curr, &dl_layout->align );
                break;
            case   e_line_break:
                o_yes_no( layfile, curr, &dl_layout->line_break );
                break;
            default:
                internal_err( __FILE__, __LINE__ );
                break;
            }
        }
        dl_layout = dl_layout->next;
    }
}


/***************************************************************************/
/*   :DOCNUM    output documentnumber values                               */
/***************************************************************************/
static  void    put_lay_docnum( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             curr;

    fprintf_s( layfile, ":DOCNUM\n" );

    for( k = 0, curr = docnum_att[k]; curr > 0; k++, curr = docnum_att[k] ) {

        switch( curr ) {
        case   e_left_adjust:
            o_space_unit( layfile, curr, &lay->docnum.left_adjust );
            break;
        case   e_right_adjust:
            o_space_unit( layfile, curr, &lay->docnum.right_adjust );
            break;
        case   e_page_position:
            o_page_position( layfile, curr, &lay->docnum.page_position );
            break;
        case   e_font:
            o_font_number( layfile, curr, &lay->docnum.font );
            break;
        case   e_pre_skip:
            o_space_unit( layfile, curr, &lay->docnum.pre_skip );
            break;
        case   e_docnum_string:
            o_xx_string( layfile, curr, lay->docnum.string );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :FIG       output figur attribute values                              */
/***************************************************************************/
static  void    put_lay_fig( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             curr;

    fprintf_s( layfile, ":FIG\n" );

    for( k = 0, curr = fig_att[k]; curr > 0; k++, curr = fig_att[k] ) {

        switch( curr ) {
        case   e_left_adjust:
            o_space_unit( layfile, curr, &lay->fig.left_adjust );
            break;
        case   e_right_adjust:
            o_space_unit( layfile, curr, &lay->fig.right_adjust );
            break;
        case   e_pre_skip:
            o_space_unit( layfile, curr, &lay->fig.pre_skip );
            break;
        case   e_post_skip:
            o_space_unit( layfile, curr, &lay->fig.post_skip );
            break;
        case   e_spacing:
            o_spacing( layfile, curr, &lay->fig.spacing );
            break;
        case   e_font:
            o_font_number( layfile, curr, &lay->fig.font );
            break;
        case   e_default_place:
            o_place( layfile, curr, &lay->fig.default_place );
            break;
        case   e_default_frame:
            o_default_frame( layfile, curr, &lay->fig.default_frame );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :FIGCAP    output figure caption attribute values                     */
/***************************************************************************/
static  void    put_lay_figcap( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             curr;

    fprintf_s( layfile, ":FIGCAP\n" );

    for( k = 0, curr = figcap_att[k]; curr > 0; k++, curr = figcap_att[k] ) {

        switch( curr ) {
        case   e_pre_lines:
            o_space_unit( layfile, curr, &lay->figcap.pre_lines );
            break;
        case   e_font:
            o_font_number( layfile, curr, &lay->figcap.font );
            break;
        case   e_figcap_string:
            o_xx_string( layfile, curr, lay->figcap.string );
            break;
        case   e_string_font:
            o_font_number( layfile, curr, &lay->figcap.string_font );
            break;
        case   e_delim:
            o_char( layfile, curr, &lay->figcap.delim );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :FIGDESC   output figure description attribute values                 */
/***************************************************************************/
static  void    put_lay_figdesc( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             curr;

    fprintf_s( layfile, ":FIGDESC\n" );

    for( k = 0, curr = figdesc_att[k]; curr > 0; k++, curr = figdesc_att[k] ) {

        switch( curr ) {
        case   e_pre_lines:
            o_space_unit( layfile, curr, &lay->figdesc.pre_lines );
            break;
        case   e_font:
            o_font_number( layfile, curr, &lay->figdesc.font );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :FIGLIST   output figure list attribute values                        */
/***************************************************************************/
static  void    put_lay_figlist( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             curr;

    fprintf_s( layfile, ":FIGLIST\n" );

    for( k = 0, curr = figlist_att[k]; curr > 0; k++, curr = figlist_att[k] ) {

        switch( curr ) {
        case   e_left_adjust:
            o_space_unit( layfile, curr, &lay->figlist.left_adjust );
            break;
        case   e_right_adjust:
            o_space_unit( layfile, curr, &lay->figlist.right_adjust );
            break;
        case   e_skip:
            o_space_unit( layfile, curr, &lay->figlist.skip );
            break;
        case   e_spacing:
            o_spacing( layfile, curr, &lay->figlist.spacing );
            break;
        case   e_columns:
            o_int8( layfile, curr, &lay->figlist.columns );
            break;
        case   e_fill_string:
            o_xx_string( layfile, curr, lay->figlist.fill_string );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :FLPGNUM  output figlist number attribute values                      */
/***************************************************************************/
static  void    put_lay_flpgnum( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             curr;

    fprintf_s( layfile, ":FLPGNUM\n" );

    for( k = 0, curr = flpgnum_att[k]; curr > 0; k++, curr = flpgnum_att[k] ) {

        switch( curr ) {
        case   e_size:
            o_space_unit( layfile, curr, &lay->flpgnum.size );
            break;
        case   e_font:
            o_font_number( layfile, curr, &lay->flpgnum.font );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :FN        output footnote attribute values                            */
/***************************************************************************/
static  void    put_lay_fn( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             curr;

    fprintf_s( layfile, ":FN\n" );

    for( k = 0, curr = fn_att[k]; curr > 0; k++, curr = fn_att[k] ) {

        switch( curr ) {
        case   e_line_indent:
            o_space_unit( layfile, curr, &lay->fn.line_indent );
            break;
        case   e_align:
            o_space_unit( layfile, curr, &lay->fn.align );
            break;
        case   e_pre_lines:
            o_space_unit( layfile, curr, &lay->fn.pre_lines );
            break;
        case   e_skip:
            o_space_unit( layfile, curr, &lay->fn.skip );
            break;
        case   e_spacing:
            o_spacing( layfile, curr, &lay->fn.spacing );
            break;
        case   e_font:
            o_font_number( layfile, curr, &lay->fn.font );
            break;
        case   e_number_font:
            o_font_number( layfile, curr, &lay->fn.number_font );
            break;
        case   e_number_style:
            o_number_style( layfile, curr, &lay->fn.number_style );
            break;
        case   e_frame:
            o_frame( layfile, curr, &lay->fn.frame );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :FNREF     output footnote attribute values                            */
/***************************************************************************/
static  void    put_lay_fnref( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             curr;

    fprintf_s( layfile, ":FNREF\n" );

    for( k = 0, curr = fnref_att[k]; curr > 0; k++, curr = fnref_att[k] ) {

        switch( curr ) {
        case   e_font:
            o_font_number( layfile, curr, &lay->fnref.font );
            break;
        case   e_number_style:
            o_number_style( layfile, curr, &lay->fnref.number_style );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :GL        output attribute values                                    */
/***************************************************************************/
static  void    put_lay_gl( FILE * layfile, layout_data * lay )
{
    gl_lay_level    *   gl_layout;
    int                 k;
    lay_att             curr;

    gl_layout = lay->gl.first;

    while( gl_layout != NULL ) {
        fprintf_s( layfile, ":GL\n" );

        for( k = 0, curr = gl_att[k]; curr > 0; k++, curr = gl_att[k] ) {

            switch( curr ) {
            case   e_level:
                o_int8( layfile, curr, &gl_layout->level );
                break;
            case   e_left_indent:
                o_space_unit( layfile, curr, &gl_layout->left_indent );
                break;
            case   e_right_indent:
                o_space_unit( layfile, curr, &gl_layout->right_indent );
                break;
            case   e_pre_skip:
                o_space_unit( layfile, curr, &gl_layout->pre_skip );
                break;
            case   e_skip:
                o_space_unit( layfile, curr, &gl_layout->skip );
                break;
            case   e_spacing:
                o_spacing( layfile, curr, &gl_layout->spacing );
                break;
            case   e_post_skip:
                o_space_unit( layfile, curr, &gl_layout->post_skip );
                break;
            case   e_align:
                o_space_unit( layfile, curr, &gl_layout->align );
                break;
            case   e_delim:
                o_char( layfile, curr, &gl_layout->delim );
                break;
            default:
                internal_err( __FILE__, __LINE__ );
                break;
            }
        }
        gl_layout = gl_layout->next;
    }
}


/***************************************************************************/
/*   :Hx        output header attribute values for :H0 - :H6               */
/***************************************************************************/
static  void    put_lay_hx( FILE * layfile, layout_data * lay )
{
    int                 k;
    int                 lvl;
    lay_att             curr;

    for( lvl = 0; lvl < 7; ++lvl ) {

        fprintf_s( layfile, ":H%c\n", '0' + lvl );

        for( k = 0, curr = hx_att[k]; curr > 0; k++, curr = hx_att[k] ) {

            switch( curr ) {
            case   e_group:
                o_int8( layfile, curr, &lay->hx.group );
                break;
            case   e_indent:
                o_space_unit( layfile, curr, &lay->hx.hx_head[lvl].indent );
                break;
            case   e_pre_top_skip:
                o_space_unit( layfile, curr, &lay->hx.hx_sect[lvl].pre_top_skip );
                break;
            case   e_pre_skip:
                o_space_unit( layfile, curr, &lay->hx.hx_head[lvl].pre_skip );
                break;
            case   e_post_skip:
                o_space_unit( layfile, curr, &lay->hx.hx_sect[lvl].post_skip );
                break;
            case   e_spacing:
                o_spacing( layfile, curr, &lay->hx.hx_sect[lvl].spacing );
                break;
            case   e_font:
                o_font_number( layfile, curr, &lay->hx.hx_sect[lvl].text_font );
                break;
            case   e_number_font:
                o_font_number( layfile, curr, &lay->hx.hx_head[lvl].number_font );
                break;
            case   e_number_form:
                o_number_form( layfile, curr, &lay->hx.hx_head[lvl].number_form );
                break;
            case   e_page_position:
                o_page_position( layfile, curr, &lay->hx.hx_head[lvl].line_position );
                break;
            case   e_number_style:
                o_number_style( layfile, curr, &lay->hx.hx_head[lvl].number_style );
                break;
            case   e_page_eject:
                o_page_eject( layfile, curr, &lay->hx.hx_head[lvl].page_eject );
                break;
            case   e_line_break:
                o_yes_no( layfile, curr, &lay->hx.hx_head[lvl].line_break );
                break;
            case   e_display_heading:
                o_yes_no( layfile, curr, &lay->hx.hx_head[lvl].display_heading );
                break;
            case   e_number_reset:
                o_yes_no( layfile, curr, &lay->hx.hx_head[lvl].number_reset );
                break;
            case   e_case:
                o_case( layfile, curr, &lay->hx.hx_head[lvl].hd_case );
                break;
            case   e_align:
                o_space_unit( layfile, curr, &lay->hx.hx_head[lvl].align );
                break;
            default:
                internal_err( __FILE__, __LINE__ );
                break;
            }
        }
    }
}


/***************************************************************************/
/*   :HEADING   output header attribute values                             */
/***************************************************************************/
static  void    put_lay_heading( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             curr;

    fprintf_s( layfile, ":HEADING\n" );

    for( k = 0, curr = heading_att[k]; curr > 0; k++, curr = heading_att[k] ) {
        switch( curr ) {
        case   e_delim:
            o_char( layfile, curr, &lay->heading.delim );
            break;
        case   e_stop_eject:
            o_yes_no( layfile, curr, &lay->heading.stop_eject );
            break;
        case   e_para_indent:
            o_yes_no( layfile, curr, &lay->heading.para_indent );
            break;
        case   e_threshold:
            o_threshold( layfile, curr, &lay->heading.threshold );
            break;
        case   e_max_group:
            o_int8( layfile, curr, &lay->heading.max_group );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :Ix        output index  attribute values for :I1 - :I3               */
/***************************************************************************/
static  void    put_lay_ix( FILE * layfile, layout_data * lay )
{
    int                 k;
    int                 lvl;
    lay_att             curr;

    for( lvl = 0; lvl < 3; ++lvl ) {

        fprintf_s( layfile, ":I%c\n", '1' + lvl );

        for( k = 0, curr = ix_att[k]; curr > 0; k++, curr = ix_att[k] ) {
            switch( curr ) {
            case   e_pre_skip:
                o_space_unit( layfile, curr, &lay->ix[lvl].pre_skip );
                break;
            case   e_post_skip:
                o_space_unit( layfile, curr, &lay->ix[lvl].post_skip );
                break;
            case   e_skip:
                o_space_unit( layfile, curr, &lay->ix[lvl].skip );
                break;
            case   e_font:
                o_font_number( layfile, curr, &lay->ix[lvl].font );
                break;
            case   e_indent:
                o_space_unit( layfile, curr, &lay->ix[lvl].indent );
                break;
            case   e_wrap_indent:
                o_space_unit( layfile, curr, &lay->ix[lvl].wrap_indent );
                break;
            case   e_index_delim:
                o_xx_string( layfile, curr, lay->ix[lvl].index_delim );
                break;
            case   e_string_font:
                if( lvl < 2 ) {         // :I3 has no string font
                    o_font_number( layfile, curr, &lay->ix[lvl].string_font );
                }
                break;
            default:
                internal_err( __FILE__, __LINE__ );
                break;
            }
        }
    }
}


/***************************************************************************/
/*   :INDEX     output index attribute values                              */
/***************************************************************************/
static  void    put_lay_index( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             curr;

    fprintf_s( layfile, ":INDEX\n" );

    for( k = 0, curr = index_att[k]; curr > 0; k++, curr = index_att[k] ) {

        switch( curr ) {
        case   e_post_skip:
            o_space_unit( layfile, curr, &lay->hx.hx_sect[hds_index].post_skip );
            break;
        case   e_pre_top_skip:
            o_space_unit( layfile, curr, &lay->hx.hx_sect[hds_index].pre_top_skip );
            break;
        case   e_left_adjust:
            o_space_unit( layfile, curr, &lay->index.left_adjust );
            break;
        case   e_right_adjust:
            o_space_unit( layfile, curr, &lay->index.right_adjust );
            break;
        case   e_spacing:
            o_spacing( layfile, curr, &lay->hx.hx_sect[hds_index].spacing );
            break;
        case   e_columns:
            o_int8( layfile, curr, &lay->index.columns );
            break;
        case   e_see_string:
            o_xx_string( layfile, curr, lay->index.see_string );
            break;
        case   e_see_also_string:
            o_xx_string( layfile, curr, lay->index.see_also_string );
            break;
        case   e_header:
            o_yes_no( layfile, curr, &lay->hx.hx_sect[hds_index].header );
            break;
        case   e_index_string:
            o_xx_string( layfile, curr, lay->index.index_string );
            break;
        case   e_page_eject:
            o_page_eject( layfile, curr, &lay->index.page_eject );
            break;
        case   e_page_reset:
            o_yes_no( layfile, curr, &lay->index.page_reset );
            break;
        case   e_font:
            o_font_number( layfile, curr, &lay->hx.hx_sect[hds_index].text_font );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :IXHEAD    output index header attribute values                       */
/***************************************************************************/
static  void    put_lay_ixhead( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             curr;

    fprintf_s( layfile, ":IXHEAD\n" );

    for( k = 0, curr = ixhead_att[k]; curr > 0; k++, curr = ixhead_att[k] ) {

        switch( curr ) {
        case   e_pre_skip:
            o_space_unit( layfile, curr, &lay->ixhead.pre_skip );
            break;
        case   e_post_skip:
            o_space_unit( layfile, curr, &lay->ixhead.post_skip );
            break;
        case   e_font:
            o_font_number( layfile, curr, &lay->ixhead.font );
            break;
        case   e_indent:
            o_space_unit( layfile, curr, &lay->ixhead.indent );
            break;
        case   e_ixhead_frame:
            curr = e_frame;             // frame = instead of ixhead_frame =
            o_default_frame( layfile, curr, &lay->ixhead.frame );
            break;
        case   e_header:
            o_yes_no( layfile, curr, &lay->ixhead.header );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :LP        output list part attribute values                          */
/***************************************************************************/
static  void    put_lay_lp( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             curr;

    fprintf_s( layfile, ":LP\n" );

    for( k = 0, curr = lp_att[k]; curr > 0; k++, curr = lp_att[k] ) {

        switch( curr ) {
        case   e_left_indent:
            o_space_unit( layfile, curr, &lay->lp.left_indent );
            break;
        case   e_right_indent:
            o_space_unit( layfile, curr, &lay->lp.right_indent );
            break;
        case   e_line_indent:
            o_space_unit( layfile, curr, &lay->lp.line_indent );
            break;
        case   e_pre_skip:
            o_space_unit( layfile, curr, &lay->lp.pre_skip );
            break;
        case   e_post_skip:
            o_space_unit( layfile, curr, &lay->lp.post_skip );
            break;
        case   e_spacing:
            o_spacing( layfile, curr, &lay->lp.spacing );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :LQ        output long quotation attribute values                     */
/***************************************************************************/
static  void    put_lay_lq( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             curr;

    fprintf_s( layfile, ":LQ\n" );

    for( k = 0, curr = lq_att[k]; curr > 0; k++, curr = lq_att[k] ) {

        switch( curr ) {
        case   e_left_indent:
            o_space_unit( layfile, curr, &lay->lq.left_indent );
            break;
        case   e_right_indent:
            o_space_unit( layfile, curr, &lay->lq.right_indent );
            break;
        case   e_pre_skip:
            o_space_unit( layfile, curr, &lay->lq.pre_skip );
            break;
        case   e_post_skip:
            o_space_unit( layfile, curr, &lay->lq.post_skip );
            break;
        case   e_spacing:
            o_spacing( layfile, curr, &lay->lq.spacing );
            break;
        case   e_font:
            o_font_number( layfile, curr, &lay->lq.font );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :NOTE      output note attribute values                               */
/***************************************************************************/
static  void    put_lay_note( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             curr;

    fprintf_s( layfile, ":NOTE\n" );

    for( k = 0, curr = note_att[k]; curr > 0; k++, curr = note_att[k] ) {

        switch( curr ) {
        case   e_left_indent:
            o_space_unit( layfile, curr, &lay->note.left_indent );
            break;
        case   e_right_indent:
            o_space_unit( layfile, curr, &lay->note.right_indent );
            break;
        case   e_pre_skip:
            o_space_unit( layfile, curr, &lay->note.pre_skip );
            break;
        case   e_post_skip:
            o_space_unit( layfile, curr, &lay->note.post_skip );
            break;
        case   e_spacing:
            o_spacing( layfile, curr, &lay->note.spacing );
            break;
        case   e_font:
            o_font_number( layfile, curr, &lay->note.font );
            break;
        case   e_note_string:
            o_xx_string( layfile, curr, lay->note.string );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :OL        output attribute values                                    */
/***************************************************************************/
static  void    put_lay_ol( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             curr;
    ol_lay_level    *   ol_layout;

    ol_layout = lay->ol.first;

    while( ol_layout != NULL ) {
        fprintf_s( layfile, ":OL\n" );

        for( k = 0, curr = ol_att[k]; curr > 0; k++, curr = ol_att[k] ) {

            switch( curr ) {
            case   e_level:
                o_int8( layfile, curr, &ol_layout->level );
                break;
            case   e_left_indent:
                o_space_unit( layfile, curr, &ol_layout->left_indent );
                break;
            case   e_right_indent:
                o_space_unit( layfile, curr, &ol_layout->right_indent );
                break;
            case   e_pre_skip:
                o_space_unit( layfile, curr, &ol_layout->pre_skip );
                break;
            case   e_skip:
                o_space_unit( layfile, curr, &ol_layout->skip );
                break;
            case   e_spacing:
                o_spacing( layfile, curr, &ol_layout->spacing );
                break;
            case   e_post_skip:
                o_space_unit( layfile, curr, &ol_layout->post_skip );
                break;
            case   e_font:
                o_font_number(layfile, curr, &ol_layout->font );
                break;
            case   e_align:
                o_space_unit( layfile, curr, &ol_layout->align );
                break;
            case   e_number_style:
                o_number_style( layfile, curr, &ol_layout->number_style );
                break;
            case   e_number_font:
                o_font_number( layfile, curr, &ol_layout->number_font );
                break;
            default:
                internal_err( __FILE__, __LINE__ );
                break;
            }
        }
        ol_layout = ol_layout->next;
    }
}


/***************************************************************************/
/*   :PAGE   output  page attribute values                                 */
/***************************************************************************/
static  void    put_lay_page( FILE * layfile, layout_data * lay )
{
    int             k;
    lay_att         curr;
    su          *   units;

    fprintf_s( layfile, ":PAGE\n" );

    for( k = 0, curr = page_att[k]; curr > 0; k++, curr = page_att[k] ) {

        switch( curr ) {
        case   e_top_margin:
            units = &(lay->page.top_margin);
            break;
        case   e_left_margin:
            units = &(lay->page.left_margin);
            break;
        case   e_right_margin:
            units = &(lay->page.right_margin);
            break;
        case   e_depth:
            units = &(lay->page.depth);
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
        o_space_unit( layfile, curr, units );
    }
}


/***************************************************************************/
/*   :P         output paragraph attribute values                          */
/*   :PC        output paragraph continue attribute values                 */
/***************************************************************************/
static  void    put_lay_p_pc( FILE * layfile, p_lay_tag * ap, char * name )
{
    int                 k;
    lay_att             curr;

    fprintf_s( layfile, ":%s\n", name );

    for( k = 0, curr = p_att[k]; curr > 0; k++, curr = p_att[k] ) {

        switch( curr ) {
        case   e_line_indent:
            o_space_unit( layfile, curr, &ap->line_indent );
            break;
        case   e_pre_skip:
            o_space_unit( layfile, curr, &ap->pre_skip );
            break;
        case   e_post_skip:
            o_space_unit( layfile, curr, &ap->post_skip );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}

static  void    put_lay_p( FILE * layfile, layout_data * lay )
{
    put_lay_p_pc( layfile, &(lay->p), "P" );
}

static  void    put_lay_pc( FILE * layfile, layout_data * lay )
{
    put_lay_p_pc( layfile, &(lay->pc), "PC" );
}

/***************************************************************************/
/*   :SL        output attribute values                                    */
/***************************************************************************/
static  void    put_lay_sl( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             curr;
    sl_lay_level    *   sl_layout;

    sl_layout = lay->sl.first;

    while( sl_layout != NULL ) {
        fprintf_s( layfile, ":SL\n" );

        for( k = 0, curr = sl_att[k]; curr > 0; k++, curr = sl_att[k] ) {

            switch( curr ) {
            case   e_level:
                o_int8( layfile, curr, &sl_layout->level );
                break;
            case   e_left_indent:
                o_space_unit( layfile, curr, &sl_layout->left_indent );
                break;
            case   e_right_indent:
                o_space_unit( layfile, curr, &sl_layout->right_indent );
                break;
            case   e_pre_skip:
                o_space_unit( layfile, curr, &sl_layout->pre_skip );
                break;
            case   e_skip:
                o_space_unit( layfile, curr, &sl_layout->skip );
                break;
            case   e_spacing:
                o_spacing( layfile, curr, &sl_layout->spacing );
                break;
            case   e_post_skip:
                o_space_unit( layfile, curr, &sl_layout->post_skip );
                break;
            case   e_font:
                o_font_number(layfile, curr, &sl_layout->font );
                break;
            default:
                internal_err( __FILE__, __LINE__ );
                break;
            }
        }
        sl_layout = sl_layout->next;
    }
}


/***************************************************************************/
/*   :TITLE     output title attribute values                              */
/***************************************************************************/
static  void    put_lay_title( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             curr;

    fprintf_s( layfile, ":TITLE\n" );

    for( k = 0, curr = title_att[k]; curr > 0; k++, curr = title_att[k] ) {

        switch( curr ) {
        case   e_left_adjust:
            o_space_unit( layfile, curr, &lay->title.left_adjust );
            break;
        case   e_right_adjust:
            o_space_unit( layfile, curr, &lay->title.right_adjust );
            break;
        case   e_page_position:
            o_page_position( layfile, curr, &lay->title.page_position );
            break;
        case   e_font:
            o_font_number( layfile, curr, &lay->title.font );
            break;
        case   e_pre_top_skip:
            o_space_unit( layfile, curr, &lay->title.pre_top_skip );
            break;
        case   e_skip:
            o_space_unit( layfile, curr, &lay->title.skip );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :TITLEP    output title page attribute values                         */
/***************************************************************************/
static  void    put_lay_titlep( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             curr;

    fprintf_s( layfile, ":TITLEP\n" );

    for( k = 0, curr = titlep_att[k]; curr > 0; k++, curr = titlep_att[k] ) {

        switch( curr ) {
        case   e_spacing:
            o_spacing( layfile, curr, &lay->titlep.spacing );
            break;
        case   e_columns:
            o_int8( layfile, curr, &lay->titlep.columns );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :TOC       output table of contents attribute values                  */
/***************************************************************************/
static  void    put_lay_toc( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             curr;

    fprintf_s( layfile, ":TOC\n" );

    for( k = 0, curr = toc_att[k]; curr > 0; k++, curr = toc_att[k] ) {

        switch( curr ) {
        case   e_left_adjust:
            o_space_unit( layfile, curr, &lay->toc.left_adjust );
            break;
        case   e_right_adjust:
            o_space_unit( layfile, curr, &lay->toc.right_adjust );
            break;
        case   e_spacing:
            o_spacing( layfile, curr, &lay->toc.spacing );
            break;
        case   e_columns:
            o_int8( layfile, curr, &lay->toc.columns );
            break;
        case   e_toc_levels:
            o_int8( layfile, curr, &lay->toc.toc_levels );
            break;
        case   e_fill_string:
            o_xx_string( layfile, curr, lay->toc.fill_string );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :TOCPGNUM  output table of contents number attribute values           */
/***************************************************************************/
static  void    put_lay_tocpgnum( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             curr;

    fprintf_s( layfile, ":TOCPGNUM\n" );

    for( k = 0, curr = tocpgnum_att[k]; curr > 0; k++, curr = tocpgnum_att[k] ) {

        switch( curr ) {
        case   e_size:
            o_space_unit( layfile, curr, &lay->tocpgnum.size );
            break;
        case   e_font:
            o_font_number( layfile, curr, &lay->tocpgnum.font );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :TOCHx     output TOC header attribute values for :TOCH0 - :TOCH6     */
/***************************************************************************/
static  void    put_lay_tochx( FILE * layfile, layout_data * lay )
{
    int                 k;
    int                 lvl;
    lay_att             curr;

    for( lvl = 0; lvl < 7; ++lvl ) {


        fprintf_s( layfile, ":TOCH%c\n", '0' + lvl );

        for( k = 0, curr = tochx_att[k]; curr > 0; k++, curr = tochx_att[k] ) {

            switch( curr ) {
            case   e_group:
                o_int8( layfile, curr, &lay->tochx[lvl].group );
                break;
            case   e_indent:
                o_space_unit( layfile, curr, &lay->tochx[lvl].indent );
                break;
            case   e_skip:
                o_space_unit( layfile, curr, &lay->tochx[lvl].skip );
                break;
            case   e_pre_skip:
                o_space_unit( layfile, curr, &lay->tochx[lvl].pre_skip );
                break;
            case   e_post_skip:
                o_space_unit( layfile, curr, &lay->tochx[lvl].post_skip );
                break;
            case   e_font:
                o_font_number( layfile, curr, &lay->tochx[lvl].font );
                break;
            case   e_align:
                o_space_unit( layfile, curr, &lay->tochx[lvl].align );
                break;
            case   e_display_in_toc:
                o_yes_no( layfile, curr, &lay->tochx[lvl].display_in_toc );
                break;
                break;
            default:
                internal_err( __FILE__, __LINE__ );internal_err( __FILE__, __LINE__ );
                break;
            }
        }
    }
}


/***************************************************************************/
/*   :UL        output attribute values                                    */
/***************************************************************************/
static  void    put_lay_ul( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             curr;
    ul_lay_level    *   ul_layout;

    ul_layout = lay->ul.first;

    while( ul_layout != NULL ) {
        fprintf_s( layfile, ":UL\n" );

        for( k = 0, curr = ul_att[k]; curr > 0; k++, curr = ul_att[k] ) {

            switch( curr ) {
            case   e_level:
                o_int8( layfile, curr, &ul_layout->level );
                break;
            case   e_left_indent:
                o_space_unit( layfile, curr, &ul_layout->left_indent );
                break;
            case   e_right_indent:
                o_space_unit( layfile, curr, &ul_layout->right_indent );
                break;
            case   e_pre_skip:
                o_space_unit( layfile, curr, &ul_layout->pre_skip );
                break;
            case   e_skip:
                o_space_unit( layfile, curr, &ul_layout->skip );
                break;
            case   e_spacing:
                o_spacing( layfile, curr, &ul_layout->spacing );
                break;
            case   e_post_skip:
                o_space_unit( layfile, curr, &ul_layout->post_skip );
                break;
            case   e_font:
                o_font_number(layfile, curr, &ul_layout->font );
                break;
            case   e_align:
                o_space_unit( layfile, curr, &ul_layout->align );
                break;
            case   e_bullet:
                o_char( layfile, curr, &ul_layout->bullet );
                break;
            case   e_bullet_translate:
                o_yes_no( layfile, curr, &ul_layout->bullet_translate );
                break;
            case   e_bullet_font:
                o_font_number( layfile, curr, &ul_layout->bullet_font );
                break;
            default:
                internal_err( __FILE__, __LINE__ );
                break;
            }
        }
        ul_layout = ul_layout->next;
    }
}


/***************************************************************************/
/*   :WIDOW    output widow attribute value                                */
/***************************************************************************/
static  void    put_lay_widow( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             curr;

    fprintf_s( layfile, ":WIDOW\n" );

    for( k = 0, curr = widow_att[k]; curr > 0; k++, curr = widow_att[k] ) {

        switch( curr ) {
        case   e_threshold:
            o_threshold( layfile, curr, &lay->widow.threshold );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :XMP       output example attribute values                            */
/***************************************************************************/
static  void    put_lay_xmp( FILE * layfile, layout_data * lay )
{
    int                 k;
    lay_att             curr;

    fprintf_s( layfile, ":XMP\n" );

    for( k = 0, curr = xmp_att[k]; curr > 0; k++, curr = xmp_att[k] ) {

        switch( curr ) {
        case   e_left_indent:
            o_space_unit( layfile, curr, &lay->xmp.left_indent );
            break;
        case   e_right_indent:
            o_space_unit( layfile, curr, &lay->xmp.right_indent );
            break;
        case   e_pre_skip:
            o_space_unit( layfile, curr, &lay->xmp.pre_skip );
            break;
        case   e_post_skip:
            o_space_unit( layfile, curr, &lay->xmp.post_skip );
            break;
        case   e_spacing:
            o_spacing( layfile, curr, &lay->xmp.spacing );
            break;
        case   e_font:
            o_font_number( layfile, curr, &lay->xmp.font );
            break;
        default:
            internal_err( __FILE__, __LINE__ );
            break;
        }
    }
}


/***************************************************************************/
/*   :xx        output for font only value                                 */
/***************************************************************************/
static  void    put_lay_xx( FILE * layfile, font_number *font, char * name )
{
    int                 k;
    lay_att             curr;

    fprintf_s( layfile, ":%s\n", name );

    for( k = 0, curr = xx_att[k]; curr > 0; k++, curr = xx_att[k] ) {

        switch( curr ) {
        case   e_font:
            o_font_number( layfile, curr, font );
            break;
        default:
            out_msg( "WGML logic error glconvrt.c.\n");
            err_count++;
            break;
        }
    }
}

static  void    put_lay_dt( FILE * layfile, layout_data * lay )
{
    put_lay_xx( layfile, &(lay->dt.font), "DT" );
}

static  void    put_lay_gt( FILE * layfile, layout_data * lay )
{
    put_lay_xx( layfile, &(lay->gt.font), "GT" );
}

static  void    put_lay_dthd( FILE * layfile, layout_data * lay )
{
    put_lay_xx( layfile, &(lay->dthd.font), "DTHD" );
}

static  void    put_lay_cit( FILE * layfile, layout_data * lay )
{
    put_lay_xx( layfile, &(lay->cit.font), "CIT" );
}

static  void    put_lay_gd( FILE * layfile, layout_data * lay )
{
    put_lay_xx( layfile, &(lay->gd.font), "GD" );
}

static  void    put_lay_ddhd( FILE * layfile, layout_data * lay )
{
    put_lay_xx( layfile, &(lay->ddhd.font), "DDHD" );
}

static  void    put_lay_ixpgnum( FILE * layfile, layout_data * lay )
{
    put_lay_xx( layfile, &(lay->ixpgnum.font), "IXPGNUM" );
}

static  void    put_lay_ixmajor( FILE * layfile, layout_data * lay )
{
    put_lay_xx( layfile, &(lay->ixmajor.font), "IXMAJOR" );
}


/***************************************************************************/
/*   :XXXXX     output for unsupported LETTER tags                         */
/***************************************************************************/
static  void    put_lay_letter_unsupported( FILE * layfile )
{

    fprintf_s( layfile, ":FROM\n");
    fprintf_s( layfile, "        left_adjust = 0\n");
    fprintf_s( layfile, "        page_position = right\n");
    fprintf_s( layfile, "        pre_top_skip = 6\n");
    fprintf_s( layfile, "        font = 0\n");

    fprintf_s( layfile, ":TO\n");
    fprintf_s( layfile, "        left_adjust = 0\n");
    fprintf_s( layfile, "        page_position = left\n");
    fprintf_s( layfile, "        pre_top_skip = 1\n");
    fprintf_s( layfile, "        font = 0\n");

    fprintf_s( layfile, ":ATTN\n");
    fprintf_s( layfile, "        left_adjust = 0\n");
    fprintf_s( layfile, "        page_position = left\n");
    fprintf_s( layfile, "        pre_top_skip = 1\n");
    fprintf_s( layfile, "        font = 1\n");
    fprintf_s( layfile, "        attn_string = \"Attention: \"\n");
    fprintf_s( layfile, "        string_font = 1\n");

    fprintf_s( layfile, ":SUBJECT\n");
    fprintf_s( layfile, "        left_adjust = 0\n");
    fprintf_s( layfile, "        page_position = centre\n");
    fprintf_s( layfile, "        pre_top_skip = 2\n");
    fprintf_s( layfile, "        font = 1\n");

    fprintf_s( layfile, ":LETDATE\n");
    fprintf_s( layfile, "        date_form = \"$ml $dsn, $yl\"\n");
    fprintf_s( layfile, "        depth = 15\n");
    fprintf_s( layfile, "        font = 0\n");
    fprintf_s( layfile, "        page_position = right\n");

    fprintf_s( layfile, ":OPEN\n");
    fprintf_s( layfile, "        pre_top_skip = 2\n");
    fprintf_s( layfile, "        font = 0\n");
    fprintf_s( layfile, "        delim = ':'\n");

    fprintf_s( layfile, ":CLOSE\n");
    fprintf_s( layfile, "        pre_skip = 2\n");
    fprintf_s( layfile, "        depth = 6\n");
    fprintf_s( layfile, "        font = 0\n");
    fprintf_s( layfile, "        page_position = centre\n");
    fprintf_s( layfile, "        delim = ','\n");
    fprintf_s( layfile, "        extract_threshold = 2\n");

    fprintf_s( layfile, ":ECLOSE\n");
    fprintf_s( layfile, "        pre_skip = 1\n");
    fprintf_s( layfile, "        font = 0\n");

    fprintf_s( layfile, ":DISTRIB\n");
    fprintf_s( layfile, "        pre_top_skip = 3\n");
    fprintf_s( layfile, "        skip = 1\n");
    fprintf_s( layfile, "        font = 0\n");
    fprintf_s( layfile, "        indent = '0.5i'\n");
    fprintf_s( layfile, "        page_eject = no\n");
}


/***************************************************************************/
/*   output layout data to file                                            */
/***************************************************************************/
static  void    put_layout( char * name, layout_data * lay )
{
    static  FILE    *   layfile;
    symsub          *   sversion;

    layfile = fopen( name, "wt" );
    if( layfile == NULL ) {
        out_msg( "open error %s\n", name );
        return;
    }

    fprintf_s( layfile, ":LAYOUT\n" );
//  if( WgmlFlags.research ) {
        find_symvar( global_dict, "$version", no_subscript, &sversion );
        fprintf_s( layfile, ":cmt. Created with %s\n", sversion->value );
//  }

    put_lay_page( layfile, lay );
    put_lay_default( layfile, lay );
    put_lay_widow( layfile, lay );
    put_lay_fn( layfile, lay );
    put_lay_fnref( layfile, lay );
    put_lay_p( layfile, lay );
    put_lay_pc( layfile, lay );
    put_lay_fig( layfile, lay );
    put_lay_xmp( layfile, lay );
    put_lay_note( layfile, lay );
    put_lay_hx( layfile, lay );
    put_lay_heading( layfile, lay );
    put_lay_lq( layfile, lay );
    put_lay_dt( layfile, lay );
    put_lay_gt( layfile, lay );
    put_lay_dthd( layfile, lay );
    put_lay_cit( layfile, lay );
    put_lay_figcap( layfile, lay );
    put_lay_figdesc( layfile, lay );
    put_lay_dd( layfile, lay );
    put_lay_gd( layfile, lay );
    put_lay_ddhd( layfile, lay );
    put_lay_abstract( layfile, lay );
    put_lay_preface( layfile, lay );
    put_lay_body( layfile, lay );
    put_lay_backm( layfile, lay );
    put_lay_lp( layfile, lay );
    put_lay_index( layfile, lay );
    put_lay_ixpgnum( layfile, lay );
    put_lay_ixmajor( layfile, lay );
    put_lay_ixhead( layfile, lay );
    put_lay_ix( layfile, lay );
    put_lay_toc( layfile, lay );
    put_lay_tocpgnum( layfile, lay );
    put_lay_tochx( layfile, lay );
    put_lay_figlist( layfile, lay );
    put_lay_flpgnum( layfile, lay );
    put_lay_titlep( layfile, lay );
    put_lay_title( layfile, lay );
    put_lay_docnum( layfile, lay );
    put_lay_date( layfile, lay );
    put_lay_author( layfile, lay );
    put_lay_address( layfile, lay );
    put_lay_aline( layfile, lay );

    put_lay_letter_unsupported( layfile );  // dummy output

    put_lay_appendix( layfile, lay );
    put_lay_sl( layfile, lay );
    put_lay_ol( layfile, lay );
    put_lay_ul( layfile, lay );
    put_lay_dl( layfile, lay );
    put_lay_gl( layfile, lay );
    put_lay_banner( layfile, lay );

    fprintf_s( layfile, ":eLAYOUT\n" );

    fclose( layfile );
}


/***************************************************************************/
/*  lay_convert   process :CONVERT tag                                     */
/***************************************************************************/

void    lay_convert( const gmltag * entry )
{
    char        *   p;

    (void)entry;

    p = scan_start;
    scan_start = scan_stop + 1;

    if( !WgmlFlags.firstpass ) {
        ProcFlags.layout = true;
        return;                         // process during first pass only
    }
    SkipSpaces( p );
    *token_buf = '\0';
    if( !strnicmp( "file=", p, 5 ) ) {  // file attribute?
        char    quote;
        char    *fnstart;

        p += 5;
        if( *p == '"' || *p == '\'' ) {
            quote = *p;
            ++p;
        } else {
            quote = '.';                // error?? filename without quotes
        }
        fnstart = p;
        while( *p != '\0' && *p != quote ) {
            ++p;
        }
        *p = '\0';
        strcpy_s( token_buf, buf_size, fnstart );
    } else {                            // try undocumented format
        if( *p != '\0' && *p == '.' ) {
            strcpy_s( token_buf, buf_size, p + 1 );
        }
    }
    if( *token_buf == '\0' ) {           // file name missing
        xx_err( err_att_missing );
    } else {
        put_layout( token_buf, &layout_work );
    }
    return;
}
