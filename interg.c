// %P%
// ----- constants ---------------------------------------------------
static const char SCCSID[]="$Id: interg.c 35366 2010-06-11 13:31:48Z Srinivas.Reddy $	20$Date: 2010/03/15 14:52:08 $ NGS";

// ----- standard library --------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// ----- classes, structs, types -------------------------------------
#include "grid_header.h"

typedef struct {
    float ff;
} BUFFER;

// ----- functions ---------------------------------------------------
#include "interg.h"
#include "flip_endian_f.h"
#include "spline6.h"
#include "spline4.h"
#include "bilin.h"
 

double interg(double xlat, double xlon, 
            GRID_HEADER  vec_hdr[50], FILE* vec_ifp[50], int kk) {
/*******************************************************************************
* Interpolates a value  from the kk-th numbered geoid model grid. 
* As the gridded data are all direct-access files,
* only the nearest points (1 thru 9,
* depending on the point's location relative to corners and edges)
* are read into RAM for each point's interpolation.
*
* The size/spacing/etc of the data file's grid
* are defined by the common statement, and the variable kk. 
* Caveats: 
*   1) It is assumed that xlat/xlon fall in the region of the kth grid
*   in - xlat    : latitude  position, decimal degrees, North
*   in - xlon    : longitude position, decimal degrees, West
*   in - vec_hdr : vector of header records, as read from grid files
*   in - vec_ifp : vector of input files, pointing to grid files
*   in - kk      : number (0 offset) of grid file to use
*   out- val     : the interpolated value
*   ret-         : the interpolated value
* 
*******************************************************************************/
    FILE*  infile;
    BUFFER buffer;  // alias for type (*void) for binary grid read

    // Grid header elements for file kk
    double  latMin;
    double  lonMin;
    double  latDelta;
    double  lonDelta;
    long    latRowNum;
    long    lonColNum;
    long    iKind;
    double  latMax;                    // calcd from header data
    double  lonMax;                    // calcd from header data
    double  val;                       // return value

    long   irec;    // binary data file element
    // double xx;
    // double yy;
    double row_counter;
    double col_counter;
    int    irown;
    int    icoln;
    double irown_lat;
    double icoln_lon;
    float  f1;


    // Define some necessary parameters
    // These header data elements are read in the main driver file,
    // are in a common block, and are already checked for endian condition
    latMin    = vec_hdr[kk].lat_min;
    lonMin    = vec_hdr[kk].lon_min;
    latDelta  = vec_hdr[kk].lat_delta;
    lonDelta  = vec_hdr[kk].lon_delta;
    latRowNum = vec_hdr[kk].lat_num;
    lonColNum = vec_hdr[kk].lon_num;
    iKind     = vec_hdr[kk].ikind;
    latMax    = latMin + latDelta * (latRowNum - 1);
    lonMax    = lonMin + lonDelta * (lonColNum - 1);

    infile = vec_ifp[kk];

    // -----------------------------------------------------
    // A little check for safety's sake
    // Verify point is within grid file bounds
    // -----------------------------------------------------
    if (xlat < latMin) {
        fprintf(stderr, "Error: Latitude below minimum bound\n");
        fprintf(stderr, "       Lat = %lf   latMin = %lf\n", xlat, latMin);
        abort();
    }
    if (xlat > (latMin + latDelta * latRowNum) ) {
        fprintf(stderr, "Error: Latitude above maximum bound");
        fprintf(stderr, "       Lat = %lf   latMax = %lf\n", xlat, latMax);
        abort();
    }
    if (xlon < lonMin) {
        fprintf(stderr, "Error: Longitude below minimum bound");
        fprintf(stderr, "       Lon = %lf   lonMin = %lf\n", xlon, lonMin);
        abort();
    }
    if (xlon > (lonMin + lonDelta * lonColNum) ) {
        fprintf(stderr,"Error: Longitude above maximum bound");
        fprintf(stderr, "       Lon = %lf   lonMax = %lf\n", xlon, lonMax);
        abort();
    }


    // --------------------------------------------------------------------
    // Find the row/col of the nearest grid node to the lat/lon point
    // This grid node is southwest from the lat/lon point
    // (row_counter,col_counter) = exact (lat,lon) grid coord location (float)
    // (irown,icoln)             = reference node (to sw) grid coord loc'n (int)
    // --------------------------------------------------------------------
    row_counter = ((xlat-latMin) / latDelta);    // +1 =fortran index corr'n
    col_counter = ((xlon-lonMin) / lonDelta);    // +1 =fortran index corr'n

    irown = (int)floor(row_counter); // reference row, just south from latdd
    icoln = (int)floor(col_counter); // reference col, just west  from londd

    // Find the latitude and longitude of the nearest grid point
    irown_lat = latMin + latDelta*(irown);   // lat just south(up)  from latdd
    icoln_lon = lonMin + lonDelta*(icoln);   // lon just west(left) from londd

    // Find the latitude and longitude of the reference node
    // not needed to find value at a node 
    // yy = (row_counter - row) + 2.0;      //  2 := 4x4 spline window
    // xx = (col_counter - col) + 2.0;


    // -----------------------------------------------------
    // First things first -- 
    // If we're sitting right on or near (0.36 arcsec)
    // a grid node, just assign the value and return
    //   (1.0e-4 * 3600) = 0.36 arcsec
    // 
    //      read(lin(kk),rec=irec)f1
    //      // read big endian values and convert to little endian
    //      if(reverse_bytes(kk)) f1=frev4(f1)
    //      val = f1
    //      return
    // -----------------------------------------------------
    if(fabs(xlat - irown_lat) <= 1.0e-4 
    && fabs(xlon - icoln_lon) <= 1.0e-4) {

        // linear array matrix math: 
        //   44L               gets past the header
        //   irown*lonColNum   gets to the row
        //   icoln             gets the data from the specific column

        irec = 44L + (long)(4*( (irown)*lonColNum + (icoln) ));

        fseek(infile, irec, SEEK_SET);
        fread((char*)&buffer, (sizeof(float)), 1, infile);
        f1 = buffer.ff;

        // Check endian condition
        if (iKind != 1) {
            f1 = flip_endian_f( f1 );
        }
        return( f1 );
    }


    // -----------------------------------------------------
    // Not on a node, so interpolate
    //   1) 6x6 spline grid
    //   2) 4x4 spline grid
    //   3) bilinear
    // -----------------------------------------------------

    if( irown >= 3  &&  irown < (latRowNum - 2) 
    &&  icoln >= 3  &&  icoln < (lonColNum - 2) ) {
        val = spline6(xlat, xlon, vec_ifp, vec_hdr, kk);
    }
    else
    if( irown >= 2  &&  irown < (latRowNum - 1) 
    &&  icoln >= 2  &&  icoln < (lonColNum - 1) ) {
        val = spline4(xlat, xlon, vec_ifp, vec_hdr, kk);
    }
    else {
        val = bilin(  xlat, xlon, vec_ifp, vec_hdr, kk);
    }

    return( val );

}//~interg

