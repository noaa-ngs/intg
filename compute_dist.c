// %P%
// ----- constants ---------------------------------------------------
static const char SCCSID[]="$Id: compute_dist.c 25527 2010-06-07 22:29:43Z Srinivas.Reddy $	20$Date: 2010/06/04 12:17:05 $ NGS";
#include "myconst.h"

// ----- standard library --------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ----- functions ---------------------------------------------------
#include "compute_dist.h"



double compute_dist(double rlat1, double rlon1, double rlat2, double rlon2) {
/*******************************************************************************
* 
*******************************************************************************/
    double glat1;
    double glon1;
    double glat2;
    double glon2;
    double w;
    double vertical_radius;
    double ellip_ht = 0L;
    double x1;
    double y1;
    double z1;
    double x2;
    double y2;
    double z2;
    double xdiff;
    double xdiff2;
    double ydiff;
    double ydiff2;
    double zdiff;
    double zdiff2;
    double sum;
    double dist;

    //-----------------------------------------------------
    // Input Position were read to nearest second only.
    // And are passed to this function as decimal-deg
    // An Ellipsoid Height of 0.0 is used.
    //-----------------------------------------------------
    ellip_ht = 0.0;

    //------------------------------------------------------
    // Compute X, Y, Z for first position
    //------------------------------------------------------
    glat1 = TO_RADIANS(rlat1);
    glon1 = TO_RADIANS(rlon1);

    //------------------------------------------------------
    // E2 should be set for GRS80 ellipsoid in myconst.h
    //------------------------------------------------------
    w = sqrt(1.0-E2 * sin(glat1) * sin(glat1));
    vertical_radius = AX/w;

    x1 = (vertical_radius + ellip_ht) * cos(glat1) * cos(glon1);
    y1 = (vertical_radius + ellip_ht) * cos(glat1) * sin(glon1);
    z1 = (1-E2) * (vertical_radius + ellip_ht) * sin(glat1);

    // printf("%20.10f%20.10f%20.10f\n", x1, y1, z1);

    //------------------------------------------------------
    // Compute X, Y, Z for second position
    //------------------------------------------------------
    glat2 = TO_RADIANS(rlat2);
    glon2 = TO_RADIANS(rlon2);

    w = sqrt(1.0-E2 * sin(glat2) * sin(glat2));
    vertical_radius = AX/w;

    x2 = (vertical_radius + ellip_ht) * cos(glat2) * cos(glon2);
    y2 = (vertical_radius + ellip_ht) * cos(glat2) * sin(glon2);
    z2 = (1-E2) * (vertical_radius + ellip_ht) * sin(glat2);

    // printf("%20.10f%20.10f%20.10f\n", x2, y2, z2);

    if (x2 > x1) {
        xdiff = x2 - x1;
    } else {
        xdiff = x1 - x2;
    }

    if (y2 > y1) {
        ydiff = y2 - y1;
    } else {
        ydiff = y1 - y2;
    }

    if (z2 > z1) {
        zdiff = z2 - z1;
    } else {
        zdiff = z1 - z2;
    }

    xdiff2 = xdiff * xdiff;
    ydiff2 = ydiff * ydiff;
    zdiff2 = zdiff * zdiff;

    sum = xdiff2 + ydiff2 + zdiff2;

    dist = sqrt(sum);

    // printf ("Dist = %20.10f\n", dist);

    return(dist);

}//~compute_dist

