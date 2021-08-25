// /ngslib/source/Clib_billw/SCCS/s.spline6.c
// ----- constants ---------------------------------------------------

static const char SCCSID[]="$Id: spline6.c 50355 2010-10-19 15:55:47Z Srinivas.Reddy $";
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
#include "spline6.h"
#include "spline.h"
#include "initsp.h"
#include "flip_endian_f.h"


double spline6(double latdd,      double londd,
               FILE* vec_ifp[50], GRID_HEADER vec_hdr[50], int kk) {
/*******************************************************************************
* Spline interpolation using a 6x6 window around the given point(latdd, londd)
* The size of the window can be changed to any even number.
* Odd numbers are excluded since their use leads to non-symmetry
* in the distribution of data arount the interpolation point.
* Point in question must be inside of the outermost two data rings.
*     if( irown >= 3  &&  irown < (latRowNum - 2)
*     &&  icoln >= 3  &&  icoln < (lonColNum - 2) ) {
*         val = spline6(xlat, xlon, vec_ifp, vec_hdr, kk);
* 
*   in - latdd  : North lat of point in question  units = decdeg
*   in - londd  : East  lon of point in question  units = decdeg
*   in - lin    : vector of file unit numbers, pointing to grid files
*                 Data files must be direct access.
*   in - kk     : unit number of geoid grid  data file
*   out- interp : interpolated geoid height, units as per data file
*******************************************************************************/
    FILE*     infile;
    BUFFER    buffer;

    const int nn = 6;        // size of interpolation window

    double    latMin;        // header record data
    double    lonMin;
    double    latDelta;
    double    lonDelta;
    long      latRowNum;
    long      lonColNum;
    long      iKind;
    double    latMax;        // calcd from header data
    double    lonMax;        // calcd from header data

    long      irown;
    long      icoln;
    double    irown_lat;
    double    icoln_lon;
    double    row_counter;
    double    col_counter;
    float     xx;
    float     yy;
    long      irec;          // binary data file element
    double    interp;        // the interpolated geoid value

    float     f1,  f2,  f3,  f4,  f5,  f6;
    float     f7,  f8,  f9,  f10, f11, f12;
    float     f13, f14, f15, f16, f17, f18;
    float     f19, f20, f21, f22, f23, f24;
    float     f25, f26, f27, f28, f29, f30;
    float     f31, f32, f33, f34, f35, f36;

    double    AA[6];
    double    RR[6];
    double    QQ[6];
    double    HC[6];

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
  latMin    = %lf\n\
  lonMin    = %lf\n\
  latDelta  = %lf\n\
  lonDelta  = %lf\n\
  latRowNum = %ld\n\
  lonColNum = %ld\n\
  iKind     = %ld\n\
  latMax    = %lf\n\
  lonMax    = %lf\n",
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
    yy = (row_counter - irown) + 3.0;        //  3 := 6x6 spline window
    xx = (col_counter - icoln) + 3.0;

    if (DEBUG > 0) {
        fprintf(stderr, "\
  In function spline6: \n\
  kk          =  %d \n\
  latdd       =  %lf\n\
  londd       =  %lf\n\
  row_counter =  %lf\n\
  col_counter =  %lf\n\
  irown       =  %ld\n\
  icoln       =  %ld\n\
  xx          =  %lf\n\
  yy          =  %lf\n\
  irown_lat   =  %lf\n\
                (Nlat) should be: irown_lat < latdd \n\
  icoln_lon   = %lf\n\
                (Elon) should be: icoln_lon < londd \n",
            kk, latdd, londd, 
            row_counter, col_counter,  irown, icoln,  xx, yy,
            latdd, londd,  irown_lat, icoln_lon);
    }


    // ---------------------------------------------------------------
    // At this point, the (irown,icoln) coordinate pair represent
    // that grid node which is south and west from the lat/lon point,
    // about which we must get a 4x4 data matrix for spline interpolation
    // Extract 6x6 array and use it for the spline interpolation
    //   44L               gets past the header
    //   irown*lonColNum   gets to the row
    //   icoln             gets the data from the specific column
    // 
    // The fread positions the seek pointer after the read value, 
    // so subsequent freads already have the seek pointer correctly set. 
    // ---------------------------------------------------------------

    irec = 44L + (long)(4*( (irown-2)*lonColNum + (icoln-2) ));
    fseek(infile, irec, SEEK_SET);

    fread((char*)&buffer, (sizeof(float)), 1, infile);     // icoln-2
    f1 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);     // icoln-1
    f2 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);     // icoln
    f3 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);     // icoln+1
    f4 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);     // icoln+2
    f5 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);     // icoln+3
    f6 = buffer.ff;

    irec = 44L + (long)(4*( (irown-1)*lonColNum + (icoln-2) ));
    fseek(infile, irec, SEEK_SET);

    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f7 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f8 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f9 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f10 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f11 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f12 = buffer.ff;

    irec = 44L + (long)(4*( (irown  )*lonColNum + (icoln-2) ));
    fseek(infile, irec, SEEK_SET);

    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f13 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f14 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f15 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f16 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f17 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f18 = buffer.ff;

    irec = 44L + (long)(4*( (irown+1)*lonColNum + (icoln-2) ));
    fseek(infile, irec, SEEK_SET);

    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f19 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f20 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f21 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f22 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f23 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f24 = buffer.ff;

    irec = 44L + (long)(4*( (irown+2)*lonColNum + (icoln-2) ));
    fseek(infile, irec, SEEK_SET);

    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f25 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f26 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f27 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f28 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f29 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f30 = buffer.ff;

    irec = 44L + (long)(4*( (irown+3)*lonColNum + (icoln-2) ));
    fseek(infile, irec, SEEK_SET);

    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f31 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f32 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f33 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f34 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f35 = buffer.ff;
    fread((char*)&buffer, (sizeof(float)), 1, infile);
    f36 = buffer.ff;

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
        f17 = flip_endian_f( f17 );
        f18 = flip_endian_f( f18 );

        f19 = flip_endian_f( f19 );
        f20 = flip_endian_f( f20 );
        f21 = flip_endian_f( f21 );
        f22 = flip_endian_f( f22 );
        f23 = flip_endian_f( f23 );
        f24 = flip_endian_f( f24 );

        f25 = flip_endian_f( f25 );
        f26 = flip_endian_f( f26 );
        f27 = flip_endian_f( f27 );
        f28 = flip_endian_f( f28 );
        f29 = flip_endian_f( f29 );
        f30 = flip_endian_f( f30 );

        f31 = flip_endian_f( f31 );
        f32 = flip_endian_f( f32 );
        f33 = flip_endian_f( f33 );
        f34 = flip_endian_f( f34 );
        f35 = flip_endian_f( f35 );
        f36 = flip_endian_f( f36 );
    }

    if (DEBUG > 0) {
        fprintf(stderr, "\n\
 Grid data view (north is down, east to the right, zero offset numbering)\n\
 %3ld  %f  %f  %f | %f  %f  %f \n\
 %3ld  %f  %f  %f | %f  %f  %f \n\
 %3ld  %f  %f  %f | %f  %f  %f \n\
  ------------------------------------------------------------------------- \n\
 %3ld  %f  %f  %f | %f  %f  %f \n\
 %3ld  %f  %f  %f | %f  %f  %f \n\
 %3ld  %f  %f  %f | %f  %f  %f \n\
       %ld         %ld         %ld          %ld         %ld         %ld\n",
            irown-2,   f1,   f2,    f3,    f4,    f5,   f6,
            irown-1,   f7,   f8,    f9,    f10,   f11,  f12, 
            irown  ,   f13,  f14,   f15,   f16,   f17,  f18, 
            irown+1,   f19,  f20,   f21,   f22,   f23,  f24, 
            irown+2,   f25,  f26,   f27,   f28,   f29,  f30, 
            irown+3,   f31,  f32,   f33,   f34,   f35,  f36, 
                 icoln-2, icoln-1, icoln, icoln+1, icoln+2, icoln+3);
    }


    AA[0] = f1;
    AA[1] = f2;
    AA[2] = f3;
    AA[3] = f4;
    AA[4] = f5;
    AA[5] = f6;
    initsp(AA,nn,RR,QQ);
    HC[0] = spline (xx,AA,nn,RR);
    if (DEBUG > 0) { printf("HC[0]  = %12.8lf\n", HC[0]); }

    AA[0] = f7;
    AA[1] = f8;
    AA[2] = f9;
    AA[3] = f10;
    AA[4] = f11;
    AA[5] = f12;
    initsp(AA,nn,RR,QQ);
    HC[1] = spline (xx,AA,nn,RR);
    if (DEBUG > 0) { printf("HC[1]  = %12.8lf\n", HC[1]); }

    AA[0] = f13;
    AA[1] = f14;
    AA[2] = f15;
    AA[3] = f16;
    AA[4] = f17;
    AA[5] = f18;
    initsp(AA,nn,RR,QQ);
    HC[2] = spline (xx,AA,nn,RR);
    if (DEBUG > 0) { printf("HC[2]  = %12.8lf\n", HC[2]); }

    AA[0] = f19;
    AA[1] = f20;
    AA[2] = f21;
    AA[3] = f22;
    AA[4] = f23;
    AA[5] = f24;
    initsp(AA,nn,RR,QQ);
    HC[3] = spline (xx,AA,nn,RR);
    if (DEBUG > 0) { printf("HC[3]  = %12.8lf\n", HC[3]); }

    AA[0] = f25;
    AA[1] = f26;
    AA[2] = f27;
    AA[3] = f28;
    AA[4] = f29;
    AA[5] = f30;
    initsp(AA,nn,RR,QQ);
    HC[4] = spline (xx,AA,nn,RR);
    if (DEBUG > 0) { printf("HC[4]  = %12.8lf\n", HC[4]); }

    AA[0] = f31;
    AA[1] = f32;
    AA[2] = f33;
    AA[3] = f34;
    AA[4] = f35;
    AA[5] = f36;
    initsp(AA,nn,RR,QQ);
    HC[5] = spline (xx,AA,nn,RR);
    if (DEBUG > 0) { printf("HC[5]  = %12.8lf\n", HC[5]); }


    // variable interp is the spline interpolated value, returned by reference
    initsp(HC, nn, RR, QQ);
    interp = spline(yy, HC, nn, RR);
    if (DEBUG > 0) { printf("interp = %12.8lf\n", interp); }

    return( interp );

}//~spline6

