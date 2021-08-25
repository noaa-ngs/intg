// %P%
// ----- constants ---------------------------------------------------
static const char SCCSID[]="$Id: flip_endian_f.c 35332 2010-06-11 13:29:27Z Srinivas.Reddy $	20$Date: 2009/05/26 18:21:41 $ NGS";

// ----- standard library --------------------------------------------
#include <stdio.h>
#include <string.h>

// ----- structures --------------------------------------------------
typedef union {
    float flt;
    char  char_flt[sizeof(float)];
} unionFloat;

// ----- local functions ---------------------------------------------
#include "flip_endian_f.h"


float flip_endian_f( float ff ) {
/*******************************************************************************
* Reverse the byte order of a float (four byte real)
* Changes from big endian to little endian (and vice-versa)
*   in - ff :  number of type float, passed in by value
*   out-    : nothing - NOT returned by pointer
*   ret- number of type float, corrected for endian condition
*******************************************************************************/
    unionFloat  ft1;
    unionFloat  ft2;
    int         len = sizeof( float );
    int   ii = 0;

    ft1.flt = ff;
    for (ii = 0; ii < len; ++ii) {
        ft2.char_flt[ii] = ft1.char_flt[(len-1)-ii];
    }
    ff = ft2.flt;

    return( ff );

}//~flip_endian_f

