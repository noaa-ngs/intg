// /ngslib/source/Clib_billw/SCCS/s.spline4.c
// ----- constants ---------------------------------------------------
static const char SCCSID[]="$Id: spline4.c 50357 2010-10-19 15:58:42Z Srinivas.Reddy $";
static const int  DEBUG = 0;           // diagnostics print if != 0

// ----- standard library --------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>    // floor

// ----- classes, structs, types -------------------------------------
#include "grid_header.h"

typedef struct {
    float ff;
} BUFFER;

// ----- functions ---------------------------------------------------
#include "spline.h"
#include "initsp.h"
#include "spline.h"
#include "flip_endian_f.h"


double spline4(double latdd,      double londd, 
               FILE* vec_ifp[50], GRID_HEADER vec_hdr[50], int kk) {
/*******************************************************************************
* Spline interpolation using a 4x4 window around the given point(latdd, londd)
* The size of the window can be changed to any even number. 
* Odd numbers are excluded since their use leads to non-symmetry 
* in the distribution of data arount the interpolation point.
* Point in question must be inside of the outermost data ring. 
*     if( irown >= 2  &&  irown < (latRowNum - 1)
*     &&  icoln >= 2  &&  icoln < (lonColNum - 1) ) {
*         val = spline4(xlat, xlon, vec_ifp, vec_hdr, kk);
* 
*   in - latdd   : North lat of point in question  units = decdeg
*   in - londd   : East  lon of point in question  units = decdeg
*   in - vec_ifp : Vector of file pointers, pointing to grid files
*                  Data files must be direct access. 
*   in - vec_hdr : Vector of structs with header data
*   in - kk      : Unit number of geoid grid  data file
*   out- interp  : interpolated geoid height, units as per data file
*   ret- interp  : interpolated geoid height, units as per data file
*******************************************************************************/
    FILE*    infile;
    BUFFER   buffer;

    const int nn = 4;        // size of interpolation window

    double   latMin;         // header record data
    double   lonMin;
    double   latDelta;
    double   lonDelta;
    long     latRowNum;
    long     lonColNum;
    long     iKind;
    double   latMax;         // calcd from header data
    double   lonMax;         // calcd from header data

    long     irown;
    long     icoln;
    double   irown_lat;
    double   icoln_lon;
    double   row_counter;
    double   col_counter;
    float    xx;
    float    yy;
    long     irec;           // binary data file element
    double   interp;         // the interpolated geoid value

    float    f1,  f2,  f3,  f4;
    float    f5,  f6,  f7,  f8;
    float    f9,  f10, f11, f12;
    float    f13, f14, f15, f16;

    double   AA[4];
    double   RR[4];
    double   QQ[4];
    double   HC[4];


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
        fprintf(stderr, "\
  In function spline4: \n\
  latMin      = %lf\n\
  lonMin      = %lf\n\
  latDelta    = %lf\n\
  lonDelta    = %lf\n\
  latRowNum   = %ld\n\
  lonColNum   = %ld\n\
  ikind       = %ld\n\
  latMax      = %lf\n\
  lonMax      = %lf\n", 
            latMin, lonMin, latDelta, lonDelta, latRowNum, lonColNum, 
            iKind, latMax,  lonMax);
    }


    // --------------------------------------------------------------------
    // Find the row/col of the nearest grid node to the lat/lon point
    // This grid node is southwest from the lat/lon point
    // (Row_counter,Col_counter) = exact (lat,lon) grid coord location (real*8)
    // (Row,Col)                 = closest node (to sw) grid coord loc'n (int)
    // --------------------------------------------------------------------
    row_counter = ((latdd-latMin) / latDelta);
    col_counter = ((londd-lonMin) / lonDelta);

    // Find the reference node row and col
    irown = (long)floor(row_counter);        // the row just south from latdd
    icoln = (long)floor(col_counter);        // the col just west  from londd

    // Find the latitude and longitude of the reference node
    irown_lat = latMin + latDelta*(irown);   // lat just south(up)  from latdd
    icoln_lon = lonMin + lonDelta*(icoln);   // lon just west(left) from londd

    // Find relative location of the point in the interpolation window
    yy = (row_counter - irown) + 2.0;          //  2 := 4x4 spline window
    xx = (col_counter - icoln) + 2.0;

    // original code from interg: 
    //   irown = nint( (latdd-glamn(kk)) / dla(kk) ) + 1
    //   icoln = nint( (londd-glomn(kk)) / dlo(kk) ) + 1
    // 
    //   xx = ( londd - (glomn(kk)+(icoln-2)*dlo(kk)) ) / dlo(kk)
    //   yy = ( latdd - (glamn(kk)+(irown-2)*dla(kk)) ) / dla(kk)

    if (DEBUG > 0) {
        fprintf(stderr, "\
  In function spline4: \n\
  row_counter =  %lf\n\
  col_counter =  %lf\n\
  irown       =  %ld\n\
  icoln       =  %ld\n\
  xx          =  %f\n\
  yy          =  %f\n\
  latdd       =  %lf\n\
  londd       =  %lf\n\
  irown_lat     =  %lf\n\
                (Nlat) should be: irown_lat < latdd \n\
  icoln_lon     =  %lf\n\
                (Elon) should be: icoln_lon < londd \n", 
            row_counter, col_counter, irown,   icoln,     xx, 
            yy,          latdd,       londd, irown_lat, icoln_lon);
    }

    // ---------------------------------------------------------------
    // At this point, the (irown,icoln) coordinate pair represent 
    // that grid node which is south and west from the lat/lon point, 
    // about which we must get a 4x4 data matrix for spline interpolation
    // Extract 4x4 array and use it for the spline interpolation
    //   44L               gets past the header
    //   irown*lonColNum   gets to the row
    //   icoln             gets the data from the specific column
    //
    // The fread positions the seek pointer after the value read, 
    // so subsequent freads already have the seek pointer correctly set.
    // ---------------------------------------------------------------

    irec = 44L + (long)(4*( (irown-1)*lonColNum + (icoln-1) ));
    fseek(infile, irec, SEEK_SET);

    fread((char*)&buffer, (sizeof(float)), 1, infile);     // icoln-1
    f1 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);     // icoln
    f2 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);     // icoln+1
    f3 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);     // icoln+2
    f4 = buffer.ff;

    irec = 44L + (long)(4*( (irown  )*lonColNum + (icoln-1) ));
    fseek(infile, irec, SEEK_SET);

    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f5 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f6 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f7 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f8 = buffer.ff;

    irec = 44L + (long)(4*( (irown+1)*lonColNum + (icoln-1) ));
    fseek(infile, irec, SEEK_SET);

    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f9 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f10 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f11 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f12 = buffer.ff;

    irec = 44L + (long)(4*( (irown+2)*lonColNum + (icoln-1) ));
    fseek(infile, irec, SEEK_SET);

    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f13 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f14 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f15 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f16 = buffer.ff;

    // Convert big endian values to little endian
    if (vec_hdr[kk].ikind != 1) {   
        f1  = flip_endian_f( f1  );
        f2  = flip_endian_f( f2  );
        f3  = flip_endian_f( f3  );
        f4  = flip_endian_f( f4  );
        f5  = flip_endian_f( f5  );
        f6  = flip_endian_f( f6  );
        f7  = flip_endian_f( f7  );
        f8  = flip_endian_f( f8  );
        f9  = flip_endian_f( f9  );
        f10 = flip_endian_f( f10 );
        f11 = flip_endian_f( f11 );
        f12 = flip_endian_f( f12 );
        f13 = flip_endian_f( f13 );
        f14 = flip_endian_f( f14 );
        f15 = flip_endian_f( f15 );
        f16 = flip_endian_f( f16 );
    }

    if (DEBUG != 0) {
        fprintf(stderr, "\n\
   Grid data view (north is down, east to the right)\n\
 %3ld  %f  %f | %f  %f  \n\
 %3ld  %f  %f | %f  %f  \n\
  ------------------------------------------------------------------------- \n\
 %3ld  %f  %f | %f  %f  \n\
 %3ld  %f  %f | %f  %f  \n\
       %ld          %ld           %ld           %ld\n",
            irown-1,   f1,  f2,   f3,   f4,
            irown  ,   f5,  f6,   f7,   f8,
            irown+1,   f9, f10,  f11,  f12,
            irown+2,  f13, f14,  f15,  f16,
                     icoln-1,  icoln, icoln+1, icoln+2);
    }

    AA[0] = f1;
    AA[1] = f2;
    AA[2] = f3;
    AA[3] = f4;
    initsp(AA, nn, RR, QQ);
    HC[0] = spline(xx, AA, nn, RR);
    if (DEBUG > 0)  printf("HC[0]  = %12.8lf\n", HC[0] );

    AA[0] = f5;
    AA[1] = f6;
    AA[2] = f7;
    AA[3] = f8;
    initsp(AA, nn, RR, QQ);
    HC[1] = spline(xx, AA, nn, RR);
    if (DEBUG > 0)  printf("HC[1]  = %12.8lf\n", HC[1] );

    AA[0] = f9;
    AA[1] = f10;
    AA[2] = f11;
    AA[3] = f12;
    initsp(AA, nn, RR, QQ);
    HC[2] = spline (xx, AA, nn, RR);
    if (DEBUG > 0)  printf("HC[2]  = %12.8lf\n", HC[2] );

    AA[0] = f13;
    AA[1] = f14;
    AA[2] = f15;
    AA[3] = f16;
    initsp(AA, nn, RR, QQ);
    HC[3] = spline(xx, AA, nn, RR);
    if (DEBUG > 0)  printf("HC[3]  = %12.8lf\n", HC[3] );

    // variable interp is the spline interpolated value, returned by reference
    initsp(HC, nn, RR, QQ);
    interp = spline(yy, HC, nn, RR);
    if (DEBUG > 0)  printf("interp = %12.8lf\n", interp);

    return( interp );

}//~spline4

