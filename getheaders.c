// %P%
// ----- constants ---------------------------------------------------
static const char SCCSID[]="$Id: getheaders.c 86264 2015-11-16 13:52:29Z bruce.tran $	20$Date: 2010/04/26 13:41:10 $ NGS";
static const int  DEBUG = 0;           // diagnostics print if != 0

// ----- standard library --------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

// ----- classes, structures, types ----------------------------------
#include "grid_header.h"            // struct

typedef struct {
    double fd;
} BUF_DBL;

#ifdef NGS_DB_ORACLE
  typedef struct {
      int fl;
  } BUF_LONG;

#else

  typedef struct {
      long fl;
  } BUF_LONG;
#endif

// ----- functions ---------------------------------------------------
#include "getheaders.h"
#include "flip_endian_d.h"
#include "flip_endian_l.h"

static int32_t swap_int32( int32_t val );

int getheaders(FILE* vec_ifp[50], GRID_HEADER vec_hdr[50], int nfiles) {
/*******************************************************************************
* "get (grid) headers"
* Read the headers of all geoid files which where opened, 
* and store that information.
* DOES NOT compute the max lat/lon from the header information.
*   in - vec_ifp     : 1-D array of type FILE*, filled with ifp's to grid files
*   in - vec_gridHdr : 1-D array of type GRID_HEADER, empty
*   out- vec_gridHdr : 1-D array of type GRID_HEADER, filled, by reference
*   ret- number of total header structures that can be found. --NOT-- nff!
* fseek positions the get pointer. 
* fread performs the read, and repositions the get pointer.
*******************************************************************************/
    int  sizeofdouble;
    int  sizeoflong;
#ifdef NGS_DB_ORACLE
    //64-bit but grids were created with 32-bit
    sizeofdouble = 8; //based on 64 or 32 bit mode this could change
    sizeoflong = 4; //based on 64 or 32 bit mode this could change. 4 for 32-bit and 8 for 64-bit
#else
    sizeofdouble = sizeof(double); 
    sizeoflong = sizeof(long); 
#endif
    BUF_DBL   buf_dbl;
    BUF_LONG  buf_long;
    int  ii =0;

    for (ii = 0; ii < nfiles; ++ii) {
        if (vec_ifp[ii] != NULL) {
            fseek( vec_ifp[ii],  0L, SEEK_SET );

            fread( (char*)&buf_dbl,  sizeofdouble, 1, vec_ifp[ii] );
            vec_hdr[ii].lat_min    = buf_dbl.fd;

            fread( (char*)&buf_dbl,  sizeofdouble, 1, vec_ifp[ii] );
            vec_hdr[ii].lon_min    = buf_dbl.fd;

            fread( (char*)&buf_dbl,  sizeofdouble, 1, vec_ifp[ii] );
            vec_hdr[ii].lat_delta  = buf_dbl.fd;

            fread( (char*)&buf_dbl,  sizeofdouble, 1, vec_ifp[ii] );
            vec_hdr[ii].lon_delta  = buf_dbl.fd;

            fread( (char*)&buf_long, sizeoflong,   1, vec_ifp[ii] );
            vec_hdr[ii].lat_num    = buf_long.fl;

            fread( (char*)&buf_long, sizeoflong,   1, vec_ifp[ii] );
            vec_hdr[ii].lon_num    = buf_long.fl;

            fread( (char*)&buf_long, sizeoflong,   1, vec_ifp[ii] );
            vec_hdr[ii].ikind      = buf_long.fl;

            if (vec_hdr[ii].ikind != 1) {
                vec_hdr[ii].lat_min   = flip_endian_d( vec_hdr[ii].lat_min   );
                vec_hdr[ii].lon_min   = flip_endian_d( vec_hdr[ii].lon_min   );
                vec_hdr[ii].lat_delta = flip_endian_d( vec_hdr[ii].lat_delta );
                vec_hdr[ii].lon_delta = flip_endian_d( vec_hdr[ii].lon_delta );
                //vec_hdr[ii].lat_num   = flip_endian_l( vec_hdr[ii].lat_num   );
                //vec_hdr[ii].lon_num   = flip_endian_l( vec_hdr[ii].lon_num   );
                vec_hdr[ii].lat_num   = swap_int32( vec_hdr[ii].lat_num   );
                vec_hdr[ii].lon_num   = swap_int32( vec_hdr[ii].lon_num   );
                //Don't reset back to 1. ikind is used in other routines
                //vec_hdr[ii].ikind     = flip_endian_l( vec_hdr[ii].ikind     );
            }

            if (DEBUG != 0) {
                printf("in function getheaders: \n");
                printf("ii          = %d\n",  ii );
                printf("Lat_min     = %lf\n", vec_hdr[ii].lat_min  );
                printf("Lon_min     = %lf\n", vec_hdr[ii].lon_min  );
                printf("Lat_delta   = %lf\n", vec_hdr[ii].lat_delta);
                printf("Lon_delta   = %lf\n", vec_hdr[ii].lon_delta);
                printf("Lat_num     = %ld\n", vec_hdr[ii].lat_num  );
                printf("Lon_num     = %ld\n", vec_hdr[ii].lon_num  );
                printf("Ikind       = %ld\n", vec_hdr[ii].ikind    );
            }

        }//~if
    }//~for (ii)

    return( ii );

}//~getheaders

//! Byte swap unsigned short
uint16_t swap_uint16( uint16_t val )
{
    return (val << 8) | (val >> 8 );
}

//! Byte swap short
int16_t swap_int16( int16_t val )
{
    return (val << 8) | ((val >> 8) & 0xFF);
}

//! Byte swap unsigned int
uint32_t swap_uint32( uint32_t val )
{
    val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF );
    return (val << 16) | (val >> 16);
}

//! Byte swap int
int32_t swap_int32( int32_t val )
{
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF );
    return (val << 16) | ((val >> 16) & 0xFFFF);
}

int64_t swap_int64( int64_t val )
{
    val = ((val << 8) & 0xFF00FF00FF00FF00ULL ) | ((val >> 8) & 0x00FF00FF00FF00FFULL );
    val = ((val << 16) & 0xFFFF0000FFFF0000ULL ) | ((val >> 16) & 0x0000FFFF0000FFFFULL );
    return (val << 32) | ((val >> 32) & 0xFFFFFFFFULL);
}

uint64_t swap_uint64( uint64_t val )
{
    val = ((val << 8) & 0xFF00FF00FF00FF00ULL ) | ((val >> 8) & 0x00FF00FF00FF00FFULL );
    val = ((val << 16) & 0xFFFF0000FFFF0000ULL ) | ((val >> 16) & 0x0000FFFF0000FFFFULL );
    return (val << 32) | (val >> 32);
}
