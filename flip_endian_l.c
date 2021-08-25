// %P%
// ----- constants ---------------------------------------------------
static const char SCCSID[]="$Id: flip_endian_l.c 35333 2010-06-11 13:29:31Z Srinivas.Reddy $	20$Date: 2009/05/26 18:21:48 $ NGS";

// ----- standard library --------------------------------------------
#include <stdio.h>
#include <string.h>

// ----- structures --------------------------------------------------
typedef  union {
    long    lng;
    char    char_lng[sizeof(long)];
} unionLong;

// ----- local functions ---------------------------------------------
#include "flip_endian_l.h"


long flip_endian_l( long ll ) {
/*******************************************************************************
* Reverse the byte order of a long  (four byte int)
* Changes from big endian to little endian (and vice-versa)
*   in - ll : number of type long, passed in by value
*   out-    : nothing - NOT returned by pointer
*   ret- number of type long, corrected for endian condition
*******************************************************************************/
    unionLong lu1;
    unionLong lu2;
    int       len = sizeof( long );
    int  ii = 0;

    lu1.lng = ll;

    for (ii = 0; ii < len; ++ii) {
        lu2.char_lng[ii] = lu1.char_lng[(len-1)-ii];
    }
    ll = lu2.lng;

    return( ll );

}//~flip_endian_l

