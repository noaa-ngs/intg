// %P%
// ----- constants ---------------------------------------------------
static const char SCCSID[]="$Id: flip_endian_d.c 35331 2010-06-11 13:29:23Z Srinivas.Reddy $	20$Date: 2009/05/26 18:21:33 $ NGS";

// ----- standard library --------------------------------------------
#include <stdio.h>
#include <string.h>

// ----- structures --------------------------------------------------
typedef union {
    double  dbl;
    char    char_dbl[sizeof(double)];
} unionDouble;

// ----- local functions ---------------------------------------------
#include "flip_endian_d.h"


double flip_endian_d( double dd ) {
/*******************************************************************************
* Reverse the byte order of a double (eight byte float)
* Changes from big endian to little endian (and vice-versa)
*   in - dd : number of type double, passed in by value
*   out- dd : nothing - NOT returned by pointer
*   ret- number of type double, corrected for endian condition
*******************************************************************************/
    unionDouble  du1;
    unionDouble  du2;
    int          len = sizeof( double );
    int  ii = 0;

    du1.dbl = dd;
    for (ii = 0; ii < len; ++ii) {
        du2.char_dbl[ii] = du1.char_dbl[(len-1)-ii];
    }
    dd = du2.dbl;

    return( dd );

}//~flip_endian_d

