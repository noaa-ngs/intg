// %P%
// ----- constants ---------------------------------------------------
static const char SCCSID[]="$Id: spline.c 35344 2010-06-11 13:30:16Z Srinivas.Reddy $	20$Date: 2009/11/05 16:46:22 $ NGS";
static const int  DEBUG = 0;           // diagnostics print if != 0

// ----- standard library --------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// ----- classes, structs, types -------------------------------------
// ----- functions ---------------------------------------------------
#include "spline.h"


double  spline(double X, double* YY, long nn, double* RR) {
/*******************************************************************************
* FAST ONE-DIMENSIONAL EQUIDISTANT SPLINE INTERPOLATION FUNCTION.
* REFERENCE: JOSEF STOER: EINFUHRUNG IN DIE NUMERISCHE MATHEMATIK, I
* SPRINGER 1972, PAGE 81.
*   in - X  : double array, (0,..,nn-1) : INTERPOLATION ARGUMENT
*             Outside the range, linear extrapolation is used.
*
*   in - YY : double array, (0,..,nn-1) : DATA VALUES
*
*   in - RR : double array, (0,..,nn-1) : 
*             DO: SPLINE MOMENTS CALCULATED BY SUBROUTINE 'INITSP'
* --------------------------------------------------------------------
* 1983 Jun    Original by RENE FORSBERG
* 2007 May    Modified at NGS
* 2008 Sep    Modified at NGS - xlate to C
* --------------------------------------------------------------------
*******************************************************************************/
    int     JJ;
    double  XX;
    static double  spline;

    // Lower end extrapolation
    if (X < 0) {
        // printf("Lower end extrapolation\n");
        spline = YY[0] + (X-2) * (YY[1] - YY[0] - RR[1] / 6.0);

    } else if (X >= nn) {
        // printf("Upper end extrapolation\n");
        spline = YY[nn-1] + (X-nn-1.) * (YY[nn-1] - YY[nn-2] + RR[nn-2]/6.);

    // Interpolation
    } else {
        JJ  = (int)X;
        XX = X - JJ;
        spline = YY[JJ-1] + XX * ((YY[JJ]-YY[JJ-1]-RR[JJ-1]/3.-RR[JJ]/6.) 
                          + XX * (RR[JJ-1]/2. 
                          + XX * (RR[JJ]-RR[JJ-1])/6.));
        if (DEBUG > 0) {
            printf( "X       = %lf\n", X);
            printf( "XX      = %lf\n", XX);
            printf( "JJ      = %d\n", JJ);
            printf( "YY[JJ]  = %12.8lf\n", YY[JJ]);
            printf( "YY[JJ-1]= %12.8lf\n", YY[JJ-1]);
            printf( "RR[JJ]  = %12.8lf\n", RR[JJ]);
            printf( "RR[JJ-1]= %12.8lf\n", RR[JJ-1]);
            printf( "spline  = %12.8lf\n\n", spline);
        }
    }

    return( spline );

}//~spline

