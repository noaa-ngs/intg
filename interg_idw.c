// %P%
// ----- constants ---------------------------------------------------
static const char SCCSID[]="$Id: interg_idw.c 35366 2010-06-11 13:31:48Z Srinivas.Reddy $	20$Date: 2010/03/15 14:52:08 $ NGS";
static const int  DEBUG = 0;           // diagnostics print if > 0

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
#include "interg_idw.h"
#include "flip_endian_f.h"
#include "idw_interp.h"
#include "idw_point.h"
 
// ----- prototypes ---------------------------------------------------
int extract(int startrow, int startcol, int numrows, int numcols, 
                  IDW_POINT points[50], FILE *ifp, GRID_HEADER header);

double interg_idw(double xlat, double xlon, 
            GRID_HEADER  vec_hdr[50], FILE* vec_ifp[50], int kk) {
/*******************************************************************************
* Interpolates a value  from the kk-th numbered geoid model grid
* using an Inverse Distance Weighting algorithm. 
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
    IDW_POINT points[50];

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
    int    nump;
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
    // Not on a node, so interpolate using sample of
    //   1) 6x6 grid
    //   2) 4x4 grid
    //   3) 2x2 grid
    // -----------------------------------------------------

    if( irown >= 2  &&  irown < (latRowNum - 3) 
    &&  icoln >= 2  &&  icoln < (lonColNum - 3) ) {
        // extract 6x6 grid around xlat xlon
        nump = extract(irown-2, icoln-2, 6, 6, points, vec_ifp[kk], vec_hdr[kk]);
    }
    else
    if( irown >= 1  &&  irown < (latRowNum - 2) 
    &&  icoln >= 1  &&  icoln < (lonColNum - 2) ) {
        // extract 4x4 grid around xlat xlon
        nump = extract(irown-1, icoln-1, 4, 4, points, vec_ifp[kk], vec_hdr[kk]);
    }
    else {
        // extract 2x2 grid around xlat xlon
        nump = extract( irown, icoln, 2, 2, points, vec_ifp[kk], vec_hdr[kk]);
    }

    val = idw_interp( xlat, xlon, points, nump);
    return( val );

}//~interg_idw


int extract(int startrow, int startcol, int numrows, int numcols, 
                  IDW_POINT points[50], FILE *ifp, GRID_HEADER header )  {

    int count;
    int currentrow, currentcol;
    long irec;
    float  f1;
    double currentlat, currentlon;
    BUFFER buffer;
    double  latMin;
    double  lonMin;
    double  latDelta;
    double  lonDelta;
    long    latRowNum;
    long    lonColNum;
    long    iKind;
     
    latMin    = header.lat_min;
    lonMin    = header.lon_min;
    latDelta  = header.lat_delta;
    lonDelta  = header.lon_delta;
    latRowNum = header.lat_num;
    lonColNum = header.lon_num;
    iKind     = header.ikind;

    count = 0;
    for ( int i=0; i < numrows; i++ ) {
       currentrow = startrow + i;
       currentlat = latMin + latDelta*(currentrow);

       // set position to start read
       irec = 44L + (long)(4*( (currentrow)*lonColNum + (startcol) ));
       fseek(ifp, irec, SEEK_SET);

       // for each column, read a value and store
       for ( int j=0; j < numcols; j++ ) {
          currentcol = startcol + j;
          currentlon = lonMin + lonDelta*(currentcol);
          size_t bytes_read=fread((char*)&buffer, (sizeof(float)), 1, ifp);
          //printf("SAGT:bytes-read=%d\n",bytes_read);
          f1 = buffer.ff;
          if (iKind != 1)  f1 = flip_endian_f( f1 );
          points[count].value = f1;
          points[count].latitude = currentlat;
          points[count].longitude = currentlon;
          count++;
          if ( DEBUG > 0 ) fprintf(stderr,"  extracting %d,%d at %lf,%lf = %lf\n",currentrow, currentcol, currentlat, currentlon, f1);
       }

    }

    return(count);

}//~extract
