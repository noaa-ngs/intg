// %P%
// ----- constants ---------------------------------------------------
static const char SCCSID[]="$Id: dd_dms.c 35335 2010-06-11 13:29:39Z Srinivas.Reddy $	20$Date: 2009/06/29 14:20:51 $ NGS";

// ----- standard library --------------------------------------------
#include <stdio.h>         // sprintf()
#include <string.h>        // memset()
#include <math.h>          // modf()

// ----- functions ---------------------------------------------------
#include "dd_dms.h"


char* dd_dms(double dd, char* answer) {
/*******************************************************************************
* Decimal degrees to char deg-min-sec string.
* Combine with function shift(answer, 0.0) to inverse function dms_dd.
* Returns char[14] string DDDMMSS.ddddd'\0' WITHOUT hemisphere
*  in - dd     : decdeg                      ie, 123.456789
*  in - answer : char string with minimum length = [14]
*  out- answer : dms string  by reference    ie, 1232724.4404
*  ret- dms string                           ie, 1232724.4404
*       output string is padded with zero's - no blanks
*******************************************************************************/
    int deg, min;
    double temp, dm, sec;
    double sign = 1;
    static double TOL = 0.00001;

    if (dd < 0) {     // Test for South latitude
        dd  *= -1.0;
        sign = -1;
    }

    strncpy( answer, "\0", sizeof(answer) );

    dm = 60.0 * modf(dd, &temp);

    deg = (int)(temp * sign);

    sec = 60.0 * modf(dm, &temp);

    min = (int)temp;

    // Rounding corrections
    if((sec - 60.0) > TOL) {
        sec -= 60.0;
        ++min;
    }
    if((min - 60) > TOL) {
        min -= 60;
        ++deg;
    }

    sprintf(answer,"%03d%02d%08.5lf", deg,min,sec);

    return(answer);

}//~dd_dms

