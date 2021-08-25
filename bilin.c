// /ngslib/source/Clib_billw/SCCS/s.bilin.c
// ----- constants ---------------------------------------------------
static const char SCCSID[]="@(#)bilin.c	1.4	2010/04/23 NGS";
static const int  DEBUG = 0;           // diagnostics print if != 0

// ----- standard library --------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ----- classes, structs, types -------------------------------------
#include "grid_header.h"

typedef struct {
    float ff;
} BUFFER;

// ----- functions ---------------------------------------------------
#include "bilin.h"
#include "initsp.h"
#include "spline.h"
#include "flip_endian_f.h"


double bilin(double latdd, double londd, 
             FILE* vec_ifp[50], GRID_HEADER vec_hdr[50], int kk) {
/*******************************************************************************
* Bilinear Interpolation  to find a value (val) from the kk-th geoid grid. 
* As the gridded data are all direct-access files,
* only the nearest points (1 thru 9,
* depending on the point's location relative to corners and edges)
* are read into RAM for each point's interpolation.
* 
* The size/spacing/etc of the data file's grid
* are defined by the common statement, and the variable "kk"
* Caveats:
*   1) It is assumed that latdd/londd fall in the region of the kth grid
*   in - latdd   : North latitude  position, units = decdeg
*   in - londd   : East  longitude position, units = decdeg
*   in - vec_ifp : vector of file pointers, pointing to grid files
*   in - vec_ifp : vector of structs, with structs containing grid header data
*   in - kk      : number of grid file to be used
*   out- val     : interpolated geoid height, units as per data file (meters)
*******************************************************************************/
    FILE*   infile;
    BUFFER  buffer;

    double  latMin;          // header record data
    double  lonMin;
    double  latDelta;
    double  lonDelta;
    long    latRowNum;
    long    lonColNum;
    long    iKind;
    double  latMax;          // calcd from header data
    double  lonMax;          // calcd from header data

    long    irown;
    long    icoln;
    double  irown_lat;
    double  icoln_lon;
    double  row_counter;
    double  col_counter;
    float   xx;
    float   yy;
    long    irec;            // binary data file element

    double  onemxx;
    double  onemyy;
    float   UL, UR, LL, LR;
    double  val;             // return value


    // ---------------------------------------------------------------
    // Define some necessary parameters
    // These header data elements are read in the main driver file,
    // are in a common block, and are already checked for endian condition
    // ---------------------------------------------------------------
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

    if (DEBUG > 0) {
        printf("\n\
  latMin      = %lf \n\
  lonMin      = %lf \n\
  latDelta    = %lf \n\
  lonDelta    = %lf \n\
  latRowNum   = %ld \n\
  lonColNum   = %ld \n\
  iKind       = %ld \n\
  latMax      = %lf \n\
  lonMax      = %lf \n", 
            latMin, lonMin, latDelta, lonDelta, latRowNum, lonColNum, 
            iKind,  latMax, lonMax);
    }


    // -------------------------------------------------------------------------
    // Find the row/col of the nearest grid node to the lat/lon point
    // This grid node is southwest from the lat/lon point
    // (Row_counter,Col_counter) = exact (lat,lon) grid coord location (double)
    // (Row,Col)                 = closest node (to sw) grid coord loc'n  (int)
    // -------------------------------------------------------------------------
    row_counter = ((latdd-latMin) / latDelta);
    col_counter = ((londd-lonMin) / lonDelta);

    irown = (long)floor(row_counter);         // the row just south from latdd
    icoln = (long)floor(col_counter);         // the col just west  from londd

    irown_lat = latMin + latDelta*(irown);    // lat just south(up) from latdd
    icoln_lon = lonMin + lonDelta*(icoln);    // lon just west      from londd

    // Find relative location of the point in the interpolation window
    yy = (row_counter - irown);               // 1x1 window
    xx = (col_counter - icoln);

    onemyy = 1.0 - yy;
    onemxx = 1.0 - xx;


    if (DEBUG > 0) {
        printf("\
  in bilin: --------------------------- \n\
  row_counter = %lf \n\
  col_counter = %lf \n\
  irown       = %ld \n\
  icoln       = %ld \n\
  xx          = %lf \n\
  yy          = %lf \n\
  latdd       = %lf \n\
  londd       = %lf \n\
  row_val     = %lf \n\
  (Nlat) should be: row_val < latdd \n\
  col_val     = %lf \n\
  (Elon) should be: col_val < londd \n", 
        row_counter,  col_counter, irown, icoln, xx,  yy, 
        latdd,  londd, irown_lat, icoln_lon);
    }


// ------------------------------------------------
// Extract 2x2 data array around the point 
// and do bilinear interpolation
// ------------------------------------------------
//           (icoln)   ----> East   (icoln+1)
//
// (irown-1)  LL ------------------------  LR
//               |     xx        | 1mxx |
//    |          | yy            |      |
//    |          |               |      |
//    |          |               |      |
//    |          |               |      |
//    \/         |----------------      |
//  North        | 1myy                 |
//               |                      |
// (irown)    UL ------------------------  UR
//

//  If point LL in on northern edge, then UL and UR are outside of dataset
//  Linear interpolation on or near northern edge only (1*e-4 = 0.3 arcsec)

    if ((latMax - latdd) < 0.0001) {
        irec = 44L + (long)(4*( (irown  )*lonColNum + icoln   ));
        fseek(infile, irec, SEEK_SET);             // SEEK_SET := beginning
        fread((char*)&buffer, sizeof(float), 1, infile);
        LL = buffer.ff;

        irec = 44L + (long)(4*( (irown  )*lonColNum + icoln+1 ));
        fseek(infile, irec, SEEK_SET);
        fread((char*)&buffer, sizeof(float), 1, infile);
        LR = buffer.ff;

        if (iKind != 1) {
            LL=flip_endian_f(LL);
            LR=flip_endian_f(LR);
        }

        val = (LL * onemxx / (xx + onemxx) )
            + (LR * xx     / (xx + onemxx) );

        return( val );
    }


    // Bilinear interpolation
    irec = 44L + (long)(4*( (irown  )*lonColNum + (icoln  ) ));
    fseek(infile, irec, SEEK_SET);
    fread((char*)&buffer, sizeof(float), 1, infile);
    LL = buffer.ff;

    irec = 44L + (long)(4*( (irown  )*lonColNum + (icoln+1) ));
    fseek(infile, irec, SEEK_SET);
    fread((char*)&buffer, sizeof(float), 1, infile);
    LR = buffer.ff;

    irec = 44L + (long)(4*( (irown+1)*lonColNum + (icoln  ) ));
    fseek(infile, irec, SEEK_SET);
    fread((char*)&buffer, sizeof(float), 1, infile);
    UL = buffer.ff;

    irec = 44L + (long)(4*( (irown+1)*lonColNum + (icoln+1) ));
    fseek(infile, irec, SEEK_SET);
    fread((char*)&buffer, sizeof(float), 1, infile);
    UR = buffer.ff;

    // Convert big endian values to little endian
    if (iKind != 1) {
        LL = flip_endian_f( LL );
        LR = flip_endian_f( LR );
        UL = flip_endian_f( UL );
        UR = flip_endian_f( UR );
    }

    val = LL * onemyy * onemxx 
        + LR * xx     * onemyy 
        + UL * onemxx * yy
        + UR * xx     * yy;

    if (DEBUG > 0) {
        printf("\
  in bilin: \n\
  --------------------------------------- \n\
  LoLeft = %lf              LoRight = %lf \n\
               val = %lf                  \n\
  UpLeft = %lf              UpRight = %lf \n\
  ---------------------------------------- ",
        LL, LR, val, UL, UR);
    }

    return( val );

}//~bilin

