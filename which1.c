// /ngslib/source/Clib_billw/SCCS/s.which1.c
// ----- constants ---------------------------------------------------
static const char SCCSID[]="@(#)which1.c	1.13	2010/04/26 NGS";
static const int  DEBUG = 0;           // diagnostics print if != 0

// ----- standard library --------------------------------------------
#include <stdio.h>
#include <stdlib.h>

// ----- classes, structs, types -------------------------------------
#include "which1.h"
#include "grid_header.h"


int which1(double xlat, double xlon, int nfiles, int kk, int imodel, 
           GRID_HEADER vec_hdr[50], FILE* vec_ifp[50]) {
/*******************************************************************************
* Finds which of the open geoid files is best to be used 
* to interpolate to a point at xlat/xlon. 
* The returned value of "kk" means the "kkth" file will be used.
* For the GEOID99 models:
*   Alaska and CONUS overlap, and this code forces a "CONUS wins" scenario 
*   when interpolating the geoid at points in the overlap region.
* 
*   in - xlat   : latitude  of point in question
*   in - xlon   : longitude of point in question
*   in - nfiles : number of open files in vector of istreams
*   in - imodel : geoid model to be used
*   in -
*   out- kk     : file number to be used
*   ret- number of file in Filenames to use for interpolation
*******************************************************************************/
    double lat_min;   // local variables to hold grid_header values
    double lat_max;
    double lat_delta;
    double lon_min;
    double lon_max;
    double lon_delta;

    int  rank[50];
    int  ii;
    int  ne =0;
    int  se =0;
    int  we =0;
    int  ee =0;

    kk = -1;

    // -----------------------------------------------------------
    // This is the Alaska/CONUS overlap exception code.
    // The northwest CONUS grid extends into the Alaska panhandle.
    // The southeast Alaska grid is well north of Washinton state.
    // If in overlap region, then
    // use Alaska data for continuity in the panhandle.
    // -----------------------------------------------------------
    if (imodel == 1       // GEOID99      if in a model where overlap exists
    ||  imodel == 3       // GEOID03
    ||  imodel == 4       // USGG2003
    ||  imodel == 6       // USGG2009
    ||  imodel == 7 ) {   // GEOID09

        if (DEBUG != 0) {
            printf("1-In which1 Alaska in model block using kk = %d\n", kk);
            printf("2-xlat = %lf  xlon = %lf\n", xlat, xlon);
        }

        if (xlat >=  49  &&  xlat <=  58    // if in overlap region
        &&  xlon <= 234  &&  xlon >= 230)  {
            if (DEBUG != 0) {
                printf("In which1 Alaska overlap region using kk = %d\n", kk);
            }

            if ((vec_ifp[11] != NULL)) {    // if SouthEast Alaska grid is open
                kk = 11;                    // then use SE Alaska grid
                if (DEBUG != 0) {
                    printf("In which1 Alaska -success- using kk = %d\n", kk);
                }
                return( kk );
            } else {
                fprintf(stderr, "Error: In Alaska overlap region.\n");
                fprintf(stderr, "       SouthEast Alaska grid not open.\n");
                fprintf(stderr, "       Get SouthEast Alaska grid.\n");
                abort();
            }
        }
    }

    // -------------------------------------------
    // Spin through all *open* files, and *RANK* them
    // The file with the highest RANK decides the value of "kk".
    // Here's how the ranking goes:
    //   0 = Point does not lie in this file area
    //   1 = Point lies in this file, but at a corner
    //   2 = Point lies in this file, but at an edge
    //   3 = Point lies in this file, away from corners/edges
    // 
    // If a rank=3 or 4 file is found, kk is immediately set
    // and we return.  If no 3 or 4 files are found, the
    // file with the highest rank (2 or 1) sets kk.
    // If all files have rank=0, kk is set to -1
    // -------------------------------------------
    kk = -1;
    for (ii = 0; ii < nfiles; ++ii) {
        if (vec_ifp[ii] != NULL) {
            if (DEBUG != 0) {
                printf("In which1 grid file ii = %d is not NULL\n", ii);
            }

            rank[ii] = 0;

            lat_min   = vec_hdr[ii].lat_min;
            lat_delta = vec_hdr[ii].lat_delta;
            lat_max   = lat_min   + lat_delta * (vec_hdr[ii].lat_num - 1);

            lon_min   = vec_hdr[ii].lon_min;
            lon_delta = vec_hdr[ii].lon_delta;
            lon_max   = lon_min   + lon_delta * (vec_hdr[ii].lon_num - 1);

            if (xlat <= lat_max
            &&  xlat >= lat_min
            &&  xlon <= lon_max
            &&  xlon >= lon_min) {
                // At this point, we're inside a grid

                // 6x6 spline test
                if( (xlat     - lat_min ) > (2.0 * lat_delta) 
                &&  (lat_max  - xlat)     > (2.0 * lat_delta) 
                &&  (xlon     - lon_min ) > (2.0 * lon_delta) 
                &&  (lon_max  - xlon)     > (2.0 * lon_delta) ) {
                    rank[ii] = 4;
                    kk = ii;
                    if (DEBUG != 0) {
                        printf("In which1 found 6x6 spline kk = %d\n", kk);
                    }
                    return( kk );  // best condition - return right away

                // 4x4 spline test
                } else 
                if( (xlat   - lat_min) > lat_delta 
                &&  (lat_max - xlat)   > lat_delta 
                &&  (xlon   - lon_min) > lon_delta 
                &&  (lon_max - xlon)   > lon_delta ) {
                    rank[ii] = 3;

                // Edge and corner test
                } else {
                    // Find which of the 9 possible places this point resides
                    // NW corner, North Edge, NE corner
                    // West Edge, Center    , East edge
                    // SW corner, South Edge, SE corner
                    // c std: false = zero; not false is not zero (ie, 1)
                    ne = 0;
                    se = 0;
                    we = 0;
                    ee = 0;

                    // Near North edge
                    if (lat_max - xlat <= lat_delta)  ne = 1;

                    // Near South edge
                    if (xlat - lat_min <= lat_delta)  se = 1;
     
                    // Near East edge
                    if (lon_max - xlon <= lon_delta)  ee = 1;

                    // Near West edge
                    if (xlon - lon_min <= lon_delta)  we = 1;

                    // Set the rank of this file, based on edge-logic
                    if (ne == 1  &&  we == 0  &&  ee == 0)  rank[ii] = 2;
                    if (se == 1  &&  we == 0  &&  ee == 0)  rank[ii] = 2;
                    if (we == 1  &&  ne == 0  &&  se == 0)  rank[ii] = 2;
                    if (ee == 1  &&  ne == 0  &&  se == 0)  rank[ii] = 2;

                    if (ne == 1  &&  we == 1)  rank[ii] = 1;
                    if (se == 1  &&  we == 1)  rank[ii] = 1;
                    if (se == 1  &&  ee == 1)  rank[ii] = 1;
                    if (ne == 1  &&  ee == 1)  rank[ii] = 1;

                }//~tests

            }//~if (within grid)

        }//~if (vec_ifp[ii] != NULL)

    }//~for (ii)

    // If we reach this point, all possible files have been searched, 
    // and there's no open file which had a rank of 4. 
    // So now, see if we have any rank 3, 2 or 1 files to use.

    for (ii = 0; ii < nfiles; ++ii) {
        if (rank[ii] == 3) {
            kk = ii;
            return( kk );
        }
    }

    for (ii = 0; ii < nfiles; ++ii) {
        if (rank[ii] == 2) {
            kk = ii;
            return( kk );
        }
    }

    for (ii = 0; ii < nfiles; ++ii) {
        if (rank[ii] == 1) {
            kk = ii;
            return( kk );
        }
    }

    // If we come here, no files are acceptable for the given lat/lon value
    kk = -1;

    return( kk );

}//~which1

