/****************************************************************************
*
*                           Open Watcom Project
*
* Copyright (c) 2004-2025 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  WGML internal subroutine getnum  for expression evaluation
*
****************************************************************************/


#include <errno.h>
#include "wgml.h"


#define NULC    '\0'
#define not_ok  (-1)
#define ok      0

typedef struct operator {
        int     priority;
        char    operc;
} operator;

static  operator opers[] = {
    {  4,    '+'  },
    {  8,    '-'  },
    { 64,    '*'  },
    {128,    '/'  },
    {256,    ')'  },
    {  0,    '('  },
    {  0,    NULC }                     // terminating entry
};

#define MAXOPER 128                     // operator maximum
#define MAXTERM 128                     // terms maximum

static  char    tokbuf[256];            // workarea

static  int     coper;                  // current operator stack ptr
static  int     cvalue;                 // current argument stack ptr
static  int     nparens;                // nesting level

static  char    oper_stack[MAXOPER];    // operator stack
static  int     value_stack[MAXTERM];   // argument stack
static  bool    ignore_blanks;


/*
 *   get priority of operator
 */

static  int get_prio( char token )
{
    operator *op;

    for( op = opers; op->operc; ++op ) {
        if( token == op->operc ) {
            break;
        }
    }
    return( op->priority );
}

/*
 *  get priority of the top of stack operator
 */

static  int get_prio_m1( void )
{
    if( !coper ) {
        return( 0 );
    }
    return( get_prio( oper_stack[coper - 1] ) );
}


/*
 * stack functions
 */

static  int pop_val( int *arg )
{
    if( --cvalue < 0 ) {
        return( not_ok );
    }
    *arg = value_stack[cvalue];
    return( ok );
}

static  void push_val( int arg )
{
    value_stack[cvalue++] = arg;
}

static  int pop_op( int *op )
{
    if( --coper < 0 ) {
        return( not_ok );
    }
    *op = oper_stack[coper];
    return( ok );
}

static  void push_op( char op )
{
    if( !get_prio( op ) ) {
        nparens++;
    }
    oper_stack[coper++] = op;
}


/*
 *  evaluate expression
 */

static  int do_expr( void )
{
    int arg1;
    int arg2;
    int op;

    if( not_ok == pop_op( &op ) ) {
        return( not_ok );
    }

    if( not_ok == pop_val( &arg1 ) ) {
        return( not_ok );
    }

    pop_val( &arg2 );

    switch( op ) {
    case '+':
        push_val( arg2 + arg1 );
        break;

    case '-':
        push_val( arg2 - arg1 );
        break;

    case '*':
        push_val( arg2 * arg1 );
        break;

    case '/':
        if( 0 == arg1 ) {
            return( not_ok );
        }
        push_val( arg2 / arg1 );
        break;

    case '(':
        cvalue += 2;
        break;

    default:
        return( not_ok );
    }

    if( 1 > cvalue ) {
        return( not_ok );
    } else {
        return( op );
    }
}

/*
 *  Evaluate one level
 */

static int do_paren( void )
{
    int op;

    if( 1 > nparens-- ) {
        return( not_ok );
    }

    do {
        op = do_expr();
        if( op < ok ) {
            break;
        }
    } while( get_prio( (char)op ) );

    return( op );
}


/*
 *  Get an operator
 */

static  operator *get_op( const char *str )
{
    operator *op;

    for( op = opers; op->operc; ++op ) {
        if( *str == op->operc ) {
            return( op );
        }
    }
    return( NULL );
}


/*
 *  Get an expression
 */

static char *get_exp( const char *str )
{
    const char *ptr  = str;
    char *tptr = tokbuf;
    struct operator *op;

    while( *ptr != '\0' ) {
        if( *ptr == ' ' ) {
            if( ignore_blanks ) {
                ptr++;
                continue;
            } else {
                break;
            }
        }
        op = get_op( ptr );
        if (NULL != op ) {
            if( ('-' == ptr[0]) || ('+' == ptr[0]) ) {
                if( ('-' == ptr[1]) || ('+' == ptr[1]) ) {
                    return( NULL );
                }
                if( str != ptr )
                    break;
                if( !my_isdigit( ptr[1] ) && '.' != ptr[1] ) {
                    push_val( 0 );
                    *tptr++ = *ptr++;
                    break;
                }
            } else if( str == ptr ) {
                *tptr++ = *ptr++;
                break;
            } else {
                break;
            }
        }

        *tptr++ = *ptr++;
    }
    *tptr = NULC;

    return tokbuf;
}

static  int evaluate( char **line, int *val )
{
    int         arg;
    char    *   ptr;
    char    *   str;
    char    *   endptr;
    int         ercode;
    operator *  op;
    int         expr_oper;              // looking for term or operator

    expr_oper = 0;
    coper     = 0;
    cvalue    = 0;
    nparens   = 0;
    ptr       = *line;

    while( *ptr != '\0' ) {
        if( *ptr == ' ' ) {
            if( ignore_blanks ) {
                ptr++;
                continue;
            } else {
                break;
            }
        }
        switch( expr_oper ) {
        case 0:                         // look for term
            str = get_exp( ptr );

            if( str == NULL ) {         // nothing is error
                return( not_ok );
            }

            op = get_op( str );
            if( *(str +1) == NULC ) {
                if( NULL != op ) {
                    push_op( op->operc );
                    ptr++;
                    break;
                }

                if( (*str == '-' ) || (*str == '+' ) ) {
                    push_op(*str);
                    ++ptr;
                    break;
                }
            }

            {
                long num = strtol( str, &endptr, 10 );
                if( (((num <= INT_MIN) || (num >= INT_MAX)) && errno == ERANGE)
                 || (str == endptr) ) {
                    return( not_ok );
                }
                arg = (int)num;
            }
            push_val( arg );

            ptr += endptr - str;        // to the next unprocessed char

            expr_oper = 1;              // look for operator next
            break;

        case 1:                         // look for operator
            op = get_op( ptr );
            if( NULL == op ) {
                if( !coper ) {
                    *line = ptr;                    // next scan position

                    /********************************************************/
                    /* This little bit of confusion is brought to you by    */
                    /* the tendency of the calling code to replace the byte */
                    /* following the expression with '\0' before invoking   */
                    /* this function and then restoring it afterwards.      */
                    /* Thus, this byte /should/ be '\0' here but can be     */
                    /* expected to become something else after this         */
                    /* function returns                                     */
                    /********************************************************/

                    if( *line ) {                   // should be '\0' here
                        return( not_ok );
                    } else {
                        return( pop_val( val ) );   // no operations left return result
                    }
                } else {
                    return( not_ok );
                }
            }
            if( ')' == *ptr ) {
                ercode = do_paren();
                if( ok > ercode ) {
                    return( ercode );
                }
            } else {
                while( coper && op->priority <= get_prio_m1() ) {
                    do_expr();
                }
                push_op( op->operc );
                expr_oper = 0;      // look for term next
            }
            ptr++;
            break;
        }
    }

    while( 1 < cvalue ) {
        ercode = do_expr();
        if( ok > ercode )
             return ercode;
    }
    if( !coper ) {
        *line = ptr;                    // next scan position
        return( pop_val( val ) );       // no operations left return result
    } else {
        return( not_ok );
    }
}

/***************************************************************************/
/*  ideas from cbt282.122                                                  */
/*  getnum  evaluate a numeric result                                      */
/***************************************************************************/

condcode getnum( getnum_block *gn )
{
    char        *p;
    char        *pend;
    char        c;
    int         rc;

    p = gn->arg.s;
    pend = gn->arg.e;
    while( p < pend && *p == ' ' ) {
        p++;                    // skip leading blanks
    }
    gn->errstart = p;
    gn->first    = p;
    if( p == pend ) {
        gn->cc = omit;
        return( omit );         // nothing there
    }
    c = *p;
    if( c == '+' || c == '-' ) {
        gn->num_sign = c;       // unary sign
    } else {
        gn->num_sign = ' ';     // no unary sign
    }
    ignore_blanks = gn->ignore_blanks;
    c = *pend;
    *pend = '\0';               // make null terminated string
    rc = evaluate( &p, &gn->result );
    *pend = c;
    if( rc != 0 ) {
        gn->cc = notnum;
    } else {
        gn->arg.s = p;          // start for next scan
        gn->length = sprintf( gn->resultstr, "%d", gn->result );
        if( gn->result >= 0 ) {
            gn->cc = pos;
        } else {
            gn->cc = neg;
        }
    }
    return( gn->cc );
}

