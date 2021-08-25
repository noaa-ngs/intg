// %P%
// ----- constants ---------------------------------------------------
static const char SCCSID[]="$Id: intg_fn.c 35388 2010-06-11 14:02:16Z Srinivas.Reddy $	20$Date: 2010/03/24 14:55:26 $ NGS";

// ----- standard library --------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>  // isdigit()

// ----- classes, structures, types ----------------------------------
#include "grid_header.h"

// ----- functions ---------------------------------------------------
#include "dms_dd.h"
#include "getdir_geoid.h"
#include "getgrd_geoid.h"
#include "getheaders.h"
#include "interg.h"
#include "which1.h"


char*  intg_fn( char* lat_c, char* lon_c, int imodel ) {  
/*******************************************************************************
* NGS Program INTG - "INTerpolate Geoid"   modified to run as a function
* Interpolates the geoid height for a user specified position and geoid model.
* Estimates within gridded data models use spline or bilinear interpolation.
*
* Grided data model files (*.bin) are direct access, unformatted, binary format.
* The order of bytes in the geoid model data files are
* --- depends on which platform the file was created ---
* Platform dependant endian condition is corrected for the binary data.
*   in - lat_c  : North latitude,                            char[10] minimum
*   in - lon_c  : Longitude, West or East (changed to East)  char[10] min
*   in - imodel : Geoid model - see program intg for list
*   out- nothing 
*   ret- interpolated geoid height, as char*, XXXX.xxx (%8.3s)
* 
*******************************************************************************/
    FILE* vec_ifp[50];            // vector of FILE* holding grid file ptrs
    char  vec_fnames[50][256];    // vector of grid filenames
    GRID_HEADER vec_hdr[50];      // vector of header file data

    double  lat_dd  = 0.0;
    double  lon_dd  = 0.0;
    double  geoidHt = 0.0;

    char dirnam[256];
    char geoidHt_c[10];

    int  is_fn   = 1;        // run as function: false=0; true=(not zero)
    int  nfiles  = 0;        // number of files for the specific geoid model
    int  nff     = 0;        // number of files found
    int  kk      = -1;       // which grid file to use
    int  ii      = 0;

    // Initialize local variables
    strncpy(dirnam,    "\0", 256);
    strncpy(geoidHt_c, "\0",  10);
    for (ii = 0; ii < 50; ++ii) {
        strncpy(vec_fnames[ii], "\0", 256);
    }


    // ---------------------------------------------------------
    // Which directory are the geoid files in?
    // For Unix platform, set the directory location for the user.
    // ---------------------------------------------------------
    getdir_geoid(imodel, dirnam);

    // ---------------------------------------------------------
    // Create the list of files that must be opened, and open them.
    // Return with a count of how many files were actually opened.
    // is_fn : run as function: false=0; true=(not zero) (c std notation)
    // ---------------------------------------------------------
    is_fn = 1;

    getgrd_geoid(imodel, dirnam, is_fn, &nfiles, &nff,
                 vec_fnames, vec_ifp );

    // ---------------------------------------------------------
    // Read the headers of all geoid files which
    // where opened, and store that information.
    // Compute the max lat/lon from the header information.
    // Apply endian correction if required.
    // ---------------------------------------------------------
    getheaders( vec_ifp, vec_hdr, nfiles );


    // --------------------------------------------------------
    // Do NOT change lon_c or lat_c !!
    // Their pointers are passed back to the calling program.
    // --------------------------------------------------------
    // Convert Lat from char to decimal degrees
    // Convert South Latitudes to negative North.
    // --------------------------------------------------------
    lat_dd = dms_dd( lat_c );
    if (strncmp(&lat_c[0], "S", 1) == 0)  lat_dd *= -1.0;

    // --------------------------------------------------------
    // Convert Lon from char to decimal degrees
    // Convert West Longitudes to Positive East.
    // --------------------------------------------------------
    lon_dd = dms_dd( lon_c );
    if (lon_c[0] == 'W')  lon_dd = 360.0 - lon_dd;

    // Find which geoid file to use, based on the lat/lon input
    kk = which1( lat_dd, lon_dd, nfiles, kk, imodel, vec_hdr, vec_ifp );

    if (kk == -1) {          // If the point isn't in any grid area
        geoidHt = -999.0;
    } else {                 // Otherwise, do the interpolation ---
        geoidHt = interg(lat_dd, lon_dd, vec_hdr, vec_ifp, kk);
    }

    // -----------------------------------------------------
    // Convert the computed geoid height to character.
    // -----------------------------------------------------
    strncpy(geoidHt_c, "\0", 10);

    if (kk == -1)  strcpy( geoidHt_c, "-999" );
    else          sprintf( geoidHt_c, "%7.3lf", geoidHt );

    for (ii = 0; ii < nff; ++ii) {
       fclose(vec_ifp[ii]);
    }

    return( geoidHt_c );

}//~intg_fn

