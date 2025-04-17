/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  WGML GML tags definition header.
*               only tags with a routinename other than gml_dummy
*               are processed so far (not always complete)
*
*    layout tags are defined in gtagslay.h
*
*    :cmt :imbed :include   are defined here and in gtagslay.h identically
*    other tags for example :abstract are defined differently
*
*    If an eXXX tag exists for a XXX tag, it must follow immediately
*
****************************************************************************/


/*
 *       tagname  tagname
 *    upper case  length  gml routine gmlflags        gmllocflags                 clsflags
 *
 *                        lay routine layflags        laylocflags
 */
pickc( ABSTRACT,    8,  gml_abstract,   0,              0,                          0,
                        lay_abspref,    0,              0 )
pickc( ADDRESS,     7,  gml_address,    0,              titlep_tag,                 0,
                        lay_address,    0,              0 )
pickg( EADDRESS,    8,  gml_eaddress,   0,              titlep_tag | address_tag,   0 )
pickc( ALINE,       5,  gml_aline,      tag_out_txt,    address_tag,                0,
                        lay_aline,      0,              0 )
pickc( APPENDIX,    8,  gml_appendix,   0,              0,                          0,
                        lay_appendix,   0,              0 )
pickl( ATTN,        4,  lay_dummy,      0,              0 ) // letter not supported
pickc( AUTHOR,      6,  gml_author,     tag_out_txt,    titlep_tag,                 0,
                        lay_author,     0,              0 )
pickc( BACKM,       5,  gml_backm,      0,              0,                          0,
                        lay_backbod,    0,              0 )
pickl( BANNER,      6,  lay_banner,     0,              0 )
pickl( EBANNER,     7,  lay_ebanner,    0,              banner_tag )
pickl( BANREGION,   9,  lay_banregion,  0,              banner_tag )
pickl( EBANREGION,  10, lay_ebanregion, 0,              banreg_tag )
pickg( BINCLUDE,    8,  gml_binclude,   0,              titlep_tag | address_tag,   0 )
pickc( BODY,        4,  gml_body,       0,              0,                          0,
                        lay_backbod,    0,              0 )
pickc( CIT,         3,  gml_cit,        tag_out_txt,    0,                          ip_start_tag,
                        lay_xx,         0,              0 )
pickg( ECIT,        4,  gml_ecit,       tag_out_txt,    0,                          ip_end_tag )
pickl( CLOSE,       5,  lay_dummy,      0,              0 ) // letter not supported
pickl( ECLOSE,      6,  lay_dummy,      0,              0 ) // letter not supported
pickc( CMT,         3,  gml_cmt,        tag_only | tag_is_general, 0,               0,
                        gml_cmt,        0,              0 )
pickl( CONVERT,     7,  lay_convert,    0,              0 )
pickc( DATE,        4,  gml_date,       tag_out_txt,    titlep_tag,                 0,
                        lay_date,       0,              0 )
pickc( DD,          2,  gml_dd,         tag_out_txt,    0,                          def_tag | list_tag,
                        lay_dd,         0,              0 )
pickc( DDHD,        4,  gml_ddhd,       tag_out_txt,    0,                          def_tag | list_tag,
                        lay_xx,         0,              0 )
pickl( DEFAULT,     7,  lay_default,    0,              0 )
pickc( DL,          2,  gml_dl,         tag_out_txt,    0,                          0,
                        lay_dl,         0,              0 )
pickg( EDL,         3,  gml_edl,        tag_out_txt,    0,                          0 )
pickl( DISTRIB,     7,  lay_dummy,      0,              0 ) // letter not supported
pickc( DOCNUM,      6,  gml_docnum,     tag_out_txt,    titlep_tag,                 0,
                        lay_docnum,     0,              0 )
pickc( DT,          2,  gml_dt,         tag_out_txt,    0,                          list_tag,
                        lay_xx,         0,              0 )
pickc( DTHD,        4,  gml_dthd,       tag_out_txt,    0,                          list_tag,
                        lay_xx,         0,              0 )
pickc( FIG,         3,  gml_fig,        tag_out_txt,    0,                          0,
                        lay_fig,        0,              0 )
pickg( EFIG,        4,  gml_efig,       tag_out_txt,    figcap_tag,                 0 )
pickc( FIGCAP,      6,  gml_figcap,     tag_out_txt,    0,                          0,
                        lay_figcap,     0,              0 )
pickc( FIGDESC,     7,  gml_figdesc,    tag_out_txt,    figcap_tag,                 0,
                        lay_figdesc,    0,              0 )
pickc( FIGLIST,     7,  gml_figlist,    tag_out_txt,    0,                          0,
                        lay_figlist,    0,              0 )
pickg( FIGREF,      6,  gml_figref,     tag_out_txt,    0,                          0 )
pickl( FLPGNUM,     7,  lay_flpgnum,    0,              0 )
pickc( FN,          2,  gml_fn,         tag_out_txt,    0,                          0,
                        lay_fn,         0,              0 )
pickg( EFN,         3,  gml_efn,        tag_out_txt,    0,                          0 )
pickc( FNREF,       5,  gml_fnref,      tag_out_txt,    0,                          0,
                        lay_fnref,      0,              0 )
pickl( FROM,        4,  lay_dummy,      0,              0 ) // letter not supported
pickg( FRONTM,      6,  gml_frontm,     0,              0,                          0 )
pickg( GDOC,        4,  gml_gdoc,       0,              0,                          0 )
pickg( EGDOC,       5,  gml_egdoc,      tag_out_txt,    0,                          0 )
pickg( GL,          2,  gml_gl,         tag_out_txt,    0,                          0 )
pickg( EGL,         3,  gml_egl,        tag_out_txt,    0,                          0 )
pickc( GD,          2,  gml_gd,         tag_out_txt,    0,                          def_tag | list_tag,
                        lay_xx,         0,              0 )
pickl( GL,          2,  lay_gl,         0,              0 )
pickg( GRAPHIC,     7,  gml_graphic,    tag_out_txt,    titlep_tag | address_tag,   0 )
pickc( GT,          2,  gml_gt,         tag_out_txt,    0,                          list_tag,
                        lay_xx,         0,              0 )
pickc( H0,          2,  gml_h0,         0,              0,                          0,
                        lay_hx,         0,              0 )
pickc( H1,          2,  gml_h1,         0,              0,                          0,
                        lay_hx,         0,              0 )
pickc( H2,          2,  gml_h2,         0,              0,                          0,
                        lay_hx,         0,              0 )
pickc( H3,          2,  gml_h3,         0,              0,                          0,
                        lay_hx,         0,              0 )
pickc( H4,          2,  gml_h4,         0,              0,                          0,
                        lay_hx,         0,              0 )
pickc( H5,          2,  gml_h5,         0,              0,                          0,
                        lay_hx,         0,              0 )
pickc( H6,          2,  gml_h6,         0,              0,                          0,
                        lay_hx,         0,              0 )
pickg( HDREF,       5,  gml_hdref,      tag_out_txt,    0,                          0 )
pickl( HEADING,     7,  lay_heading,    0,              0 )
pickg( HP0,         3,  gml_hp0,        tag_out_txt,    0,                          ip_start_tag )
pickg( EHP0,        4,  gml_ehp0,       tag_out_txt,    0,                          ip_end_tag )
pickg( HP1,         3,  gml_hp1,        tag_out_txt,    0,                          ip_start_tag )
pickg( EHP1,        4,  gml_ehp1,       tag_out_txt,    0,                          ip_end_tag )
pickg( HP2,         3,  gml_hp2,        tag_out_txt,    0,                          ip_start_tag )
pickg( EHP2,        4,  gml_ehp2,       tag_out_txt,    0,                          ip_end_tag )
pickg( HP3,         3,  gml_hp3,        tag_out_txt,    0,                          ip_start_tag )
pickg( EHP3,        4,  gml_ehp3,       tag_out_txt,    0,                          ip_end_tag )
pickc( I1,          2,  gml_i1,         tag_out_txt,    0,                          index_tag,
                        lay_ix,         0,              0 )
pickc( I2,          2,  gml_i2,         tag_out_txt,    0,                          index_tag,
                        lay_ix,         0,              0 )
pickc( I3,          2,  gml_i3,         tag_out_txt,    0,                          index_tag,
                        lay_ix,         0,              0 )
pickg( IH1,         3,  gml_ih1,        tag_out_txt,    0,                          index_tag )
pickg( IH2,         3,  gml_ih2,        tag_out_txt,    0,                          index_tag )
pickg( IH3,         3,  gml_ih3,        tag_out_txt,    0,                          index_tag )
pickc( IMBED,       5,  gml_include,    tag_is_general, 0,                          0,
                        gml_include,    0,              0 )
pickc( INCLUDE,     7,  gml_include,    tag_is_general, 0,                          0,
                        gml_include,    0,              0 )
pickc( INDEX,       5,  gml_index,      tag_out_txt,    0,                          0,
                        lay_index,      0,              0 )
pickg( IREF,        4,  gml_iref,       tag_out_txt,    0,                          0 )
pickl( IXHEAD,      6,  lay_ixhead,     0,              0 )
pickl( IXMAJOR,     7,  lay_xx,         0,              0 )
pickl( IXPGNUM,     7,  lay_xx,         0,              0 )
pickg( LAYOUT,      6,  gml_layout,     0,              0,                          0 )
pickl( ELAYOUT,     7,  lay_elayout,    0,              0 )
pickl( LETDATE,     7,  lay_dummy,      0,              0 ) // letter not supported
pickg( LI,          2,  gml_li,         tag_out_txt,    0,                          li_lp_tag | list_tag )
pickg( LIREF,       5,  gml_dummy,      tag_out_txt,    0,                          0 )
pickc( LP,          2,  gml_lp,         tag_out_txt,    0,                          li_lp_tag | list_tag,
                        lay_lp,         0,              0 )
pickc( LQ,          2,  gml_lq,         tag_out_txt,    0,                          0,
                        lay_lq,         0,              0 )
pickg( ELQ,         3,  gml_elq,        tag_out_txt,    0,                          0 )
pickc( NOTE,        4,  gml_note,       tag_out_txt,    0,                          0,
                        lay_note,       0,              0 )
pickc( OL,          2,  gml_ol,         tag_out_txt,    0,                          0,
                        lay_ol,         0,              0 )
pickg( EOL,         3,  gml_eol,        tag_out_txt,    0,                          0 )
pickl( OPEN,        4,  lay_dummy,      0,              0 ) // letter not supported
pickc( P,           1,  gml_p,          tag_out_txt,    0,                          0,
                        lay_p,          0,              0 )
pickl( PAGE,        4,  lay_page,       0,              0 )
pickg( PB,          2,  gml_pb,         tag_out_txt,    0,                          0 )
pickc( PC,          2,  gml_pc,         tag_out_txt,    0,                          0,
                        lay_pc,         0,              0 )
pickc( PREFACE,     7,  gml_preface,    tag_out_txt,    0,                          0,
                        lay_abspref,    0,              0 )
pickc( PSC,         3,  gml_dummy,      tag_out_txt,    0,                          0,
                        lay_dummy,      0,              0 )
pickg( EPSC,        4,  gml_dummy,      tag_out_txt,    0,                          0 )
pickg( Q,           1,  gml_q,          tag_out_txt,    0,                          ip_start_tag )
pickg( EQ,          2,  gml_eq,         tag_out_txt,    0,                          ip_end_tag )
pickl( SAVE,        4,  lay_dummy,      0,              0 )
pickg( SET,         3,  gml_set,        tag_is_general, 0,                          li_lp_tag )
pickg( SF,          2,  gml_sf,         tag_out_txt,    0,                          ip_start_tag )
pickg( ESF,         3,  gml_esf,        tag_out_txt,    0,                          ip_end_tag )
pickc( SL,          2,  gml_sl,         tag_out_txt,    0,                          0,
                        lay_sl,         0,              0 )
pickg( ESL,         3,  gml_esl,        tag_out_txt,    0,                          0 )
pickl( SUBJECT,     7,  lay_dummy,      0,              0 ) // letter not supported
pickc( TITLE,       5,  gml_title,      tag_out_txt,    titlep_tag,                 0,
                        lay_title,      0,              0 )
pickc( TITLEP,      6,  gml_titlep,     0,              0,                          0,
                        lay_titlep,     0,              0 )
pickg( ETITLEP,     7,  gml_etitlep,    tag_out_txt,    titlep_tag,                 0 )
pickl( TO,          2,  lay_dummy,      0,              0 ) // letter not supported
pickc( TOC,         3,  gml_toc,        tag_out_txt,    0,                          0,
                        lay_toc,        0,              0 )
pickl( TOCH0,       5,  lay_tochx,      0,              0 )
pickl( TOCH1,       5,  lay_tochx,      0,              0 )
pickl( TOCH2,       5,  lay_tochx,      0,              0 )
pickl( TOCH3,       5,  lay_tochx,      0,              0 )
pickl( TOCH4,       5,  lay_tochx,      0,              0 )
pickl( TOCH5,       5,  lay_tochx,      0,              0 )
pickl( TOCH6,       5,  lay_tochx,      0,              0 )
pickl( TOCPGNUM,    8,  lay_tocpgnum,   0,              0 )
pickc( UL,          2,  gml_ul,         tag_out_txt,    0,                          0,
                        lay_ul,         0,              0 )
pickg( EUL,         3,  gml_eul,        tag_out_txt,    0,                          0 )
pickl( WIDOW,       5,  lay_widow,      0,              0 )
pickc( XMP,         3,  gml_xmp,        tag_out_txt,    0,                          0,
                        lay_xmp,        0,              0 )
pickg( EXMP,        4,  gml_exmp,       tag_out_txt,    0,                          0 )
