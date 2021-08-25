// /ngslib/source/Clib_billw/SCCS/s.initsp.c
// ----- constants ---------------------------------------------------
static const char SCCSID[]="$Id: initsp.c 50355 2010-10-19 15:55:47Z Srinivas.Reddy $";
static const int  DEBUG = 0;           // diagnostics print if != 0

// ----- standard library --------------------------------------------
#include <stdio.h>
// #include <string.h>

// ----- Sybase library ----------------------------------------------
// ----- functions ---------------------------------------------------
#include "initsp.h"


void initsp(double* YY, long nn, double* RR, double* QQ) {
/*******************************************************************************
* INITIALIZATION PROCEDURE FOR FAST 1-DIMENSIONAL EQUIDISTANT
* SPLINE INTERPOLATION, WITH FREE BOUNDARY END CONDITIONS
* REFERENCE: JOSEF STOER: EINFUHRUNG IN DIE NUMERISCHE MATHEMATIK I, 
* SPRINGER 1972, PAGE 82 AND 86.
*    in - YY : GIVEN VALUES, Y(0), ..., Y(nn-1)
*              was: in - YY : GIVEN VALUES, Y(1), ..., Y(nn)
* 
*    out- RR : SPLINE MOMENTS (0 ... nn-1), TO BE USED BY FUNCTION 'SPLINE'
*      was: out- RR : SPLINE MOMENTS (1 ... nn), TO BE USED BY FUNCTION 'SPLINE'
* 
*    in - QQ : WORK-ARRAY, DECLARED AT LEAST 0:nn-1
* 
* --------------------------------------------------------------------
*  1983 Jul    Original by RENE FORSBERG
*  2007 May    Modified at NGS
*  2009 May    Modified at NGS -xlate to c
* --------------------------------------------------------------------
*******************************************************************************/
    double  pp;
    long    kk;

    QQ[0] = 0.0;   //was: QQ(1) = 0.0
    RR[0] = 0.0;   //was: RR(1) = 0.0

    for (kk = 1; kk < nn-1; ++kk) {
        pp     = QQ[kk-1]/2 + 2;
        QQ[kk] = -0.5/pp;
        RR[kk] = (3.0*(YY[kk+1] - 2.0*YY[kk] + YY[kk-1]) - RR[kk-1]/2.0)/pp;
        if (DEBUG > 0) {
            printf("In initsp 1: RR[%ld] = %12.9lf\n", kk, RR[kk]);
        }
    }

    RR[nn-1] = 0.0;
    for ( kk = nn-2; kk > 0; --kk) {       //was: DO kk = nn-1, 2, -1
        RR[kk] = QQ[kk] * RR[kk+1] + RR[kk];
        if (DEBUG > 0) {
            printf("In initsp 2: RR[%ld] = %12.9lf\n", kk, RR[kk]);
        }
    }

}//~initsp

