// ----- constants ---------------------------------------------------
static const char SCCSID[]="$Id: idw_interp.c 50355 2010-10-19 15:55:47Z Srinivas.Reddy $";
static const int  DEBUG = 0;           // diagnostics print if != 0

// ----- standard library --------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <math.h>    // pow

// ----- classes, structs, types -------------------------------------
#include "idw_point.h"

// ----- functions ---------------------------------------------------
#include "compute_dist.h"


double idw_interp(double latdd, double londd, IDW_POINT *points, int num) {
/*******************************************************************************
* This routine impements the simplest form of the Inverse Distance 
* Weighted interpolation algorithm sometimes known as Shepherd's Method.
* 
* The interpolated value is a weighted average of sample points, where 
* the weights are based on the inverse of the distance to the sample point.
* 
*   in - latdd   : North lat of point in question  units = decdeg
*   in - londd   : East  lon of point in question  units = decdeg
*   in - points  : Vector of structs with data point values and locations
*   in - num     : number of data values to use in points
*   out- interp  : interpolated data value
*   ret- interp  : interpolated data value
*******************************************************************************/

    //const double p = 2.0;        // weighting exponent
    const double p = 1.0;        // weighting exponent
    const double eps = 0.001;        // minimum distance

    double   weight;         // list of weights for each data point
    double   sum_weights;
    double   dist;
    double   interp;

    sum_weights = 0.0;
    interp = 0.0;

    // ---------------------------------------------------------------
    // For each sample data point:
    //     - compute distance from data point to requested lat/lon
    //     - calculate the interp weight to use with this data
    //       point (inverse of the distance raised to some power p)
    //     - keep accumulating sum of the weights 
    //     - keep accumulating sum of the weights * data value
    // ---------------------------------------------------------------

    for ( int i=0; i < num; i++ ) {

        dist = compute_dist(latdd, londd, 
                            points[i].latitude, points[i].longitude);
        if ( dist < eps ) dist = eps;

        weight = 1.0 / pow(dist,p); 
        sum_weights += weight;
        interp += points[i].value * weight;

        if (DEBUG > 0)  
              printf("  point %d: value = %12.8f   dist = %12.8lf   weight = %12.8f\n", i, points[i].value, dist, weight );

    }

    interp = interp / sum_weights;
    if (DEBUG > 0)  
          printf("IDW interpolated value = %12.8f\n", interp);

    return( interp );

}//~idw_interp
