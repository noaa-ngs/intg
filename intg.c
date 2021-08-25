// %P%
// ----- constants ---------------------------------------------------
static const char SCCSID[]="$Id: intg.c 86256 2015-11-16 13:07:52Z bruce.tran $	20$Date: 2010/06/21 16:47:51 $ NGS";
static const char PGMVER[]="3.3";
static const char PGMDAT[]="2015/11/16";
static const int  DEBUG = 0;           // diagnostics print if != 0
static const int  MEM_STEP = 40;       // dynamic allocation increment

// ----- standard library --------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>      // sqrt()

// ----- classes, structures, types ----------------------------------
#include "grid_header.h"
#include "dataset1.h"

// ----- functions ---------------------------------------------------
#include "bb80ll.h"
#include "expform.h"
#include "c2v.h"
#include "ff1.h"
#include "ff1out.h"
#include "ff2.h"
#include "ff2out.h"
#include "ff4out.h"
#include "getdir_geoid.h"
#include "which1.h"
#include "getgrd_geoid.h"
#include "getgrd_vardis.h"
#include "getheaders.h"
#include "intro.h"
#include "interg.h"
#include "interg_idw.h"
#include "run_bbk.h"
#include "trim_c.h"


/*********************************************************************
* For further technical information, questions, or comments:
*   NOAA, National Geodetic Survey, N/NGS6, Silver Spring, MD  U.S.A.
*   Attn   : Daniel R. Roman, Ph.D.
*   Phone  : 301-713-3202
*   Fax    : 301-713-4172
*   e-mail : dan.roman@noaa.gov
*********************************************************************/

/*********************************************************************
*                                                                    *
*                            DISCLAIMER                              *
*                                                                    *
*   THIS PROGRAM AND SUPPORTING INFORMATION IS FURNISHED BY THE      *
* GOVERNMENT OF THE UNITED STATES OF AMERICA, AND IS ACCEPTED AND    *
* USED BY THE RECIPIENT WITH THE UNDERSTANDING THAT THE UNITED STATES*
* GOVERNMENT MAKES NO WARRANTIES, EXPRESS OR IMPLIED, CONCERNING THE *
* ACCURACY, COMPLETENESS, RELIABILITY, OR SUITABILITY OF THIS        *
* PROGRAM, OF ITS CONSTITUENT PARTS, OR OF ANY SUPPORTING DATA.      *
*                                                                    *
*   THE GOVERNMENT OF THE UNITED STATES OF AMERICA SHALL BE UNDER NO *
* LIABILITY WHATSOEVER RESULTING FROM ANY USE OF THIS PROGRAM.  THIS *
* PROGRAM SHOULD NOT BE RELIED UPON AS THE SOLE BASIS FOR SOLVING A  *
* PROBLEM WHOSE INCORRECT SOLUTION COULD RESULT IN INJURY TO PERSON  *
* OR PROPERTY.                                                       *
*                                                                    *
*   THIS PROGRAM IS PROPERTY OF THE GOVERNMENT OF THE UNITED STATES  *
* OF AMERICA.  THEREFORE, THE RECIPIENT FURTHER AGREES NOT TO ASSERT *
* PROPRIETARY RIGHTS THEREIN AND NOT TO REPRESENT THIS PROGRAM TO    *
* ANYONE AS BEING OTHER THAN A GOVERNMENT PROGRAM.                   *
*                                                                    *
*********************************************************************/

//Global variables
FILE *efp;
char old_cluster_rec[255];
int fatal_error;
int   vc_unit;
int   car97_unit;

int main( const int argc, const char* argv[] ) {
/*******************************************************************************
* NGS Program INTG - "INTerpolate Geoid"
* Interpolates the geoid height for a user specified position and geoid model.
* Estimates within gridded data models use spline or bilinear interpolation.
*
* Grided data model files (*.bin) are direct access, unformatted, binary format.
* The order of bytes in the geoid model data files are
* --- depends on which platform the file was created ---
* Platform dependant endian condition is corrected for the binary data.
*******************************************************************************/
    FILE* ifp;
    FILE* ofp;
    FILE* vec_ifp[50];            // vector of FILE* of height grids
    FILE* var_ifp[50];            // vector of FILE* of variance grids
    FILE* dis_ifp[50];            // vector of FILE* of distance grids
    char  vec_fnames[50][256];    // vector of filenames of height grids
    char  var_fnames[50][256];    // vector of filenames of variance grids
    char  dis_fnames[50][256];    // vector of filenames of distance grids

    GRID_HEADER vec_hdr[50];      // vector of header file data
    GRID_HEADER var_hdr[50];      // vector of header file data
    GRID_HEADER dis_hdr[50];      // vector of header file data
    DATASET1*   vec_data;  // ptr to vector of lat-lon-text input data

    char    dash70[]="----------------------------------------------------------------------";
    char    card[90];
    char    card2[90];
    char    card86[90];
    char    dirnam[256];
    char    rec_in[90];
    char    ifname[256];  //  input file name
    char    ofname[256];  // output file name
    char    ofyn[2];
    char    cinput[42];
    char    text[42];

    double  xlat;
    double  xlon;

    // Variables for statistics of the run
    double  minght =  999999.0; // smallest geoid value
    double  minlat =      90.0;    // lat at min location
    double  minlon =     360.0;    // lon at min location
    double  maxght = -999999.0; // largest  geoid value
    double  maxlat =     -90.0;    // lat at max location
    double  maxlon =    -180.0;    // lon at max location
    double  xn     = -999999.0;    // northernmost lat
    double  xs     =  999990.0;   // southernmost lat
    double  xe     = -999999.0;    // easternmost lon
    double  xw     =  999999.0;    // westernmost lon
    double  fact   = 0.0;
    double  ave    = 0.0;
    double  std    = 0.0;
    double  rms    = 0.0;
    double  geoidHt= 0.0;         // solution
    double  variance = -999.;
    double  stddev = -999.;
    double  distance = -999.;

    int  iform  = 0;
    int  ipos;
    int  keep   = 0;
    int  kount  = 0;
    int  mem_limit = 0;
    int  kk  = 0;
    int  mm  = 0;
    int  iii = 0;
    int  iinput;
    int  imodel;
    int  is_subr;
    int  nfiles, nvdfiles;
    int  nff, nvff, ndff;
    int  ii;
    int  jj;
    int  poseast = 0;  // Assume west longitude
    int  outfil  = 0;  // Cstd:  0 = false;  !0 = not false


    // -----------------------------------------------------
    // Initialize variables - Zero out the statistics for this run
    // -----------------------------------------------------
    strncpy(dirnam, "\0", 256);
    strncpy(cinput, "\0",  42);
    strncpy(text,   "\0",  42);
    strncpy(card,   "\0",  90);
    strncpy(card2,  "\0",  90);
    strncpy(card86, "\0",  90);
    strncpy(rec_in, "\0",  90);
    strncpy(ofyn,   "\0",   2);

    for (ii = 0; ii < 50; ++ii) {
        strncpy(vec_fnames[ii], "\0", 256);
        strncpy(var_fnames[ii], "\0", 256);
        strncpy(dis_fnames[ii], "\0", 256);
    }

    // ---------------------------------------------------------
    // Write out the introductory/disclaimer screens
    // ---------------------------------------------------------
    // printf("NGS program intg    version: %s     date(ccyy/mm/dd): %s\n",
    //         PGMVER, PGMDAT);

    intro( PGMVER, PGMDAT );    // prints intro and disclaimer to stdout

    // ---------------------------------------------------------
    // Select geoid model
    // Experimental has no prompt, yet no error message
    // ---------------------------------------------------------
    iii = 0;
    while (iii == 0) {
        printf("%s\n", dash70);
        printf("\
Which geoid model do you wish to use?\n\n\
   1 = GEOID99                  2 = G99SSS\n\
   3 = GEOID03                  4 = USGG2003\n\
   5 = GEOID06                  6 = USGG2009\n\
   7 = GEOID09\n\
   9 = Experimental Geoid (XUSHG)\n\n\
   11 = USGG2012\n\
   12 = GEOID12A\n\
   13 = GEOID12B\n\
  99 = END PROGRAM\n\n\
   -> ");
        strncpy(cinput, "\0", 42);
        fgets( cinput, 40, stdin);
        imodel = atoi(cinput);

        if (imodel == 99) return(0);

        if ((imodel >= 1 && imodel <= 7)  || imodel == 9 || imodel == 11 || imodel == 12 || imodel == 13) {
            ++iii;
        } else {
            fprintf(stderr,"Error: Not a valid response. Try again.\n");
#ifndef NGS_PC_ENV
            exit(-1);
#endif
        }
    }//~while

    // ---------------------------------------------------------
    // Which directory are the geoid files in?
    // For Unix platform, set the directory location for the user.
    // ---------------------------------------------------------
    getdir_geoid(imodel, dirnam);

    // ---------------------------------------------------------
    // Create the list of files that must be opened, and open them.
    // Return with a count of how many files were opened,
    // and a flag (ios) of which files are open and which are not.
    // IS_SUBR : run as subroutine: false=0; true=(not zero) (c std notation)
    // ---------------------------------------------------------
    is_subr = 0;
    getgrd_geoid(imodel, dirnam, is_subr, &nfiles, &nff,
                 vec_fnames, vec_ifp);
    getgrd_vardis(imodel, dirnam, is_subr, &nvdfiles, &nvff, &ndff,
                 var_fnames, dis_fnames, var_ifp, dis_ifp);

    // ---------------------------------------------------------
    // Read the headers of all geoid files which
    // where opened, and store that information.
    // Compute the max lat/lon from the header information.
    // Apply endian correction if required
    // ---------------------------------------------------------
    getheaders( vec_ifp, vec_hdr, nfiles );
    getheaders( var_ifp, var_hdr, nvdfiles );
    getheaders( dis_ifp, dis_hdr, nvdfiles );

    // ---------------------------------------------------------
    // How to input?
    // ---------------------------------------------------------
    iii = 0;
    while (iii == 0) {
        printf("%s\n", dash70);
        printf("\
How would you like to input the data? \n\
    1 = By Keyboard (with prompts) \n\
    2 = By File     (using allowed formats) \n\n\
    -> ");

        strncpy(cinput, "\0", 42);
        fgets(  cinput, 40, stdin);
        iinput = atoi(cinput);

        if ((iinput > 0 && iinput < 3)) {
            ++iii;
        } else {
            fprintf(stderr, "Error: not an option - try again\n");
#ifndef NGS_PC_ENV
            exit(-1);
#endif
        }
    }

    // ---------------------------------------------------------
    // If using an input file, select format
    // ---------------------------------------------------------
    if (iinput == 2) {
        iii = 0;
        while (iii == 0) {
            printf("\n%s\n", dash70);
            printf("\
Which format will you use for input? \n\
    1 = File - Free Format (For Geoid) Type 1 \n\
    2 = File - Free Format (For Geoid) Type 2 \n\
    3 = File - NGS Blue Book Vol 1 (*80* and *86* records) \n\n\
    0 = End Program \n\
   99 = Please explain to me the formats \n\n\
    -> ");

            strncpy(cinput, "\0", 42);
            fgets(  cinput, 40, stdin);
            iform = atoi(cinput);

            if (iform == 0) {
                exit(0);
            } else if (iform == 99) {
                expform();
            }
            else if ((iform > 0 && iform < 4)) {
                ++iii;
            } else {
                fprintf(stderr, "Error: not an option - try again\n");
#ifndef NGS_PC_ENV
                exit(-1);
#endif
            }
        }
    }

    // ---------------------------------------------------------
    // Select positive longitude convention.
    // DON'T ask this if the input is Blue Book
    // ---------------------------------------------------------
    iii = 0;
    if ((iform == 1 || iform == 2 || iinput == 1)) {
        while (iii == 0) {
            printf("\n");
            printf("%s\n", dash70);
            printf("\
Which longitude convention will you use? \n\
    1 - Positive EAST \n\
    2 - Positive WEST \n\n\
    -> ");

            strncpy(cinput, "\0", 42);
            fgets(  cinput, 40, stdin);
            ipos = atoi(cinput);

            if (ipos == 1) {
                poseast = 1;     // C-std: Nonzero = true
                ++iii;
            }
            else if (ipos == 2) {
                poseast = 0;
                ++iii;
            } else {
                fprintf(stderr, "Error: not an option - try again\n");
#ifndef NGS_PC_ENV
                exit(-1);
#endif
            }
        }
    }

    // ---------------------------------------------------------
    // Open the Input file if necessary
    // ---------------------------------------------------------
    iii = 0;
    if (iinput == 2) {
        while (iii == 0) {
            printf("\n");
            printf("Enter your  input file name   : ");
            strncpy(ifname, "\0", 256);
            fgets(  ifname, 256, stdin);
            trim_c( ifname, 'b' );

            if ((ifp = fopen( ifname, "r" )) == NULL) {
                printf("Error: Cannot find input file: %s\nTry again", ifname);
#ifndef NGS_PC_ENV
                exit(-1);
#endif
            } else {
                ++iii;
            }
        }//~while
    }//~if

    // ---------------------------------------------------------
    // Output file? (Forced to 'yes' if there was an input file)
    // ---------------------------------------------------------
    if (iinput == 1) {  // 1 = keyboard
        printf("\n");
        printf("Write output to a file (y/n)? : ");

        strncpy(cinput, "\0", 42);
        fgets(  cinput, 40, stdin);
        trim_c( cinput, 'b' );
        strcpy(ofyn, cinput);
    } else {
        strcpy(ofyn, "y");
    }

    // ---------------------------------------------------------
    // Open Output file if necessary
    // ---------------------------------------------------------
    outfil = 0;       // Cstd:  0 := false   !0 := not false
    iii    = 0;
    if (strcmp(ofyn, "Y") == 0 || strcmp(ofyn, "y") == 0) {
        while (iii == 0) {
            outfil = 1;
            printf("Enter your output file name   : ");

            strncpy(ofname, "\0", 256);
            fgets(  ofname, 256, stdin);
            trim_c( ofname, 'b');

            if ((ofp = fopen(ofname, "w")) == NULL) {
                printf("Error: File exists - try again\n");
#ifndef NGS_PC_ENV
                exit(-1);
#endif
            } else {
                ++iii;
            }
        }
    }


    // ======================================================================
    // Now handle the 4 input types
    // ======================================================================

    mem_limit += MEM_STEP;
    vec_data = (DATASET1*) calloc(mem_limit, sizeof(DATASET1));
    if (vec_data == NULL ) {
        fprintf(stderr, "Out of system memory - allocation fails\n");
#ifndef NGS_PC_ENV
        exit(-1);
#endif
    }

    // -----------------------------------------------
    // Input by file, free format type 1
    // -----------------------------------------------
    if (iform == 1) {
        while( fgets(rec_in, 90, ifp) ) {
            trim_c(rec_in, 'r');
            if (strlen(rec_in) < 5) continue;

            // Find the lat/lon value
            // Longitude always returns as 0->360...whether this
            // is positive east or west is fixed a few lines down
            strncpy(text, "\0", 42);
            ff1(rec_in, &xlat, &xlon, text);

            // If the lat/lon values came back as -999, set the
            // geoid value to -999 and skip the interpolation
            if (xlat == -999. || xlon == -999.) {
                geoidHt = (double)-999.;
                continue;
            }

            // Force input West longitude to be positive East
            if (poseast == 0) {           // C std: 0 = false; is NOT east lon
                xlon = 360. - xlon;
            }
            // Now have the lat/lon pair from input file type 2

            DATASET1 thisSet;
            strncpy(thisSet.text, "\0", 50);
            thisSet.lat     = xlat;
            thisSet.lon     = xlon;
            thisSet.poseast = poseast;
            strcpy(thisSet.text, text);
            vec_data[kount] = thisSet;
            ++kount;
            if (kount >= mem_limit) {
                mem_limit += MEM_STEP;
            vec_data = (DATASET1*)realloc(vec_data, mem_limit*sizeof(DATASET1));
                if (vec_data == NULL) {
                   printf("Out of system memory - allocation fails\n");
                   exit(-1);
                }
            }
            strncpy(rec_in, "\0",  90);            
        }//~while
    }//~if (iform = 1)

    // -----------------------------------------------
    // Input file, free format type 2
    // -----------------------------------------------
    else if (iform == 2) {
        while( fgets(rec_in, 90, ifp) ) {
            trim_c(rec_in, 'r');

            // Find the lat/lon value
            // Longitude always returns as 0->360...whether this
            // is positive east or west is fixed a few lines down
            strncpy(text, "\0", 42);
            ff2(rec_in, &xlat, &xlon, text);

            // If the lat/lon values came back as -999, set the
            // geoid value to -999 and skip the interpolation
            if (xlat == -999. || xlon == -999.) {
                geoidHt = (double) -999.;
                continue;
            }

            // Force input West longitude to be positive East
            if (poseast == 0) {           // C std: 0 = false; is NOT east lon
                xlon = 360. - xlon;
            }
            // Now have the lat/lon pair from input file type 2

            DATASET1 thisSet;
            strncpy(thisSet.text, "\0", 50);
            thisSet.lat     = xlat;
            thisSet.lon     = xlon;
            thisSet.poseast = poseast;
            strcpy(thisSet.text, text);
            vec_data[kount] = thisSet;
            ++kount;
            if (kount >= mem_limit) {
                mem_limit += MEM_STEP;
            vec_data = (DATASET1*)realloc(vec_data, mem_limit*sizeof(DATASET1));
                if (vec_data == NULL) {
                    printf("Out of system memory - allocation fails\n");
                    exit(-1);
                }
            }
            strncpy(rec_in, "\0",  90);

        }//~while
    }//~if (iform = 2)

    // -----------------------------------------------
    // Input by file, horizontal bluebook
    // Function contains all processes.
    // -----------------------------------------------
    else if (iform == 3) {
        run_bbk( ifp, ofp, vec_ifp, vec_hdr, vec_fnames, nff, imodel );

        fclose(ifp);
        fclose(ofp);
        return(0);

    }//~(iform == 3) bluebook

    // -----------------------------------------------
    // Input by prompts
    // -----------------------------------------------
    else if (iinput == 1) {
        jj = 0;
        do {
            printf("\n");
            printf("What is the name of this point?           : ");
            strncpy(cinput, "\0", 42);
            fgets(  cinput, 40, stdin);
            strncpy(text, "\0", 42);
            strncpy( text, cinput, (strlen(cinput)-1) );  // strip linefeed

            ii = 0;
            while (ii == 0) {
                printf("\n");
                printf("What is the North Latitude of this point? : ");
                strncpy(cinput, "\0", 42);
                fgets(  cinput, 40, stdin);
                trim_c( cinput, 'b');
                xlat = c2v(cinput, 1);
                if (fabs(xlat + 999) < 0.001) {
                    printf("Error(501): Bad Latitude ... try again\n");
#ifndef NGS_PC_ENV
                    exit(-1);
#endif
                } else {
                    ++ii;
                }
            }

            ii = 0;
            while (ii == 0) {
                if (poseast == 1) {
                    printf("\n");
                    printf("What is the East Longitude of this point? : ");
                } else {
                    printf("\n");
                    printf("What is the West Longitude of this point? : ");
                }
                strncpy(cinput, "\0", 42);
                fgets(  cinput, 40, stdin);
                trim_c( cinput, 'b');
                xlon = c2v(cinput, 2);
                if (fabs(xlon + 999) < 0.001) {
                    printf("Error(501): Bad Longitude ... try again\n");
#ifndef NGS_PC_ENV
                    exit(-1);
#endif
                } else {
                    ++ii;
                }

                // Force input West longitude to be positive East
                if (poseast == 0) {       // C std: 0 = false; is NOT east lon
                    xlon = 360. - xlon;
                }
            }

            // load lat/lon struct to 1-D array
            DATASET1 thisSet;
            strncpy(thisSet.text, "\0", 50);
            thisSet.lat     = xlat;
            thisSet.lon     = xlon;
            thisSet.poseast = poseast;
            strcpy(thisSet.text, text);
            vec_data[kount] = thisSet;
            ++kount;
            if (kount >= mem_limit) {
                mem_limit += MEM_STEP;
                vec_data =
                    (DATASET1*)realloc(vec_data, mem_limit * sizeof(DATASET1));
                if (vec_data == NULL) {
                    printf("Out of system memory - allocation fails\n");
                    exit(-1);
                }
            }

            printf("\n");
            printf("Enter another station? (y/n): ");
            strncpy(cinput, "\0",  42);
            fgets(  cinput, 40, stdin);
            trim_c( cinput, 'b');

            if( (strncmp(cinput, "y", 1) == 0)
            ||  (strncmp(cinput, "Y", 1) == 0) ) {
                ;  // do nothing
            } else {
                ++jj;
            }
        } while (jj == 0);

    }//~(iinput == 1) keyboard prompt

    // ---------------------------------------------
    // Now have the lat/lon pair(s) in a data vector
    // ---------------------------------------------


    // ===== COMMON TO NON-BLUEBOOK INPUT TYPES ============

    // ===== INTERPOLATE GEOID VALUE =======================

    // iterate thru the array -----
    for (ii = 0; ii < kount; ++ii) {
        xlat = vec_data[ii].lat;
        xlon = vec_data[ii].lon;

        if (DEBUG != 0) {
            printf("kount = %d  ii = %d    xlat  = %lf   xlon = %lf\n",
                    kount, ii, xlat, xlon);
        }

        // If the lat/lon values came back as -999, set the
        // geoid value to -999 and skip the interpolation
        if (xlat == -999. || xlon == -999.) {
            geoidHt = (double) -999.;
            stddev = (double) -999.;
            distance = (double) -999.;
            continue;
        } else {

            // Find which geoid file to use, based on the lat/lon input
            kk = which1( xlat, xlon, nfiles, kk, imodel, vec_fnames, vec_hdr, vec_ifp );

            if (DEBUG != 0) { printf("kk = %d \n", kk); }

            // If the point isn't in any of our grid areas, set to -999
            if (kk == -1) {
                geoidHt = (double) -999.;
                stddev = (double) -999.;
                distance = (double) -999.;

            // Otherwise, do the interpolation
            } else {
                if (DEBUG != 0) {
                    printf("xlat = %lf  xlon = %lf  kk = %d\n ", xlat,xlon,kk);
                }

                geoidHt = interg(xlat, xlon, vec_hdr, vec_ifp, kk );

                if (DEBUG != 0) { printf("geoidHt = %lf \n", geoidHt ); }

                //  If we have variance and distance grids available,
                //  interpolate from those as well
                stddev = (double) -999.;
                distance = (double) -999.;
                if ( nvdfiles > 0 ) {
                   mm = which1( xlat, xlon, nvdfiles, mm, imodel, var_fnames,
                                var_hdr, var_ifp );
                   if ( mm != -1 ) {
                      variance = interg_idw(xlat, xlon, var_hdr, var_ifp, mm );
                      stddev = sqrt(variance);
                   }

                   mm = which1( xlat, xlon, nvdfiles, mm, imodel, dis_fnames,
                                dis_hdr, dis_ifp );
                   if ( mm != -1 ) {
                      distance = interg_idw(xlat, xlon, dis_hdr, dis_ifp, mm );
                   }
                }
                //printf("SAGout: %lf +/-%lf m %lf \n",geoidHt,stddev,distance);

                ++keep;
                ave += geoidHt;
                rms += geoidHt * geoidHt;
                if (xlat > xn)  xn = xlat;
                if (xlat < xs)  xs = xlat;
                if (xlon > xe)  xe = xlon;
                if (xlon < xw)  xw = xlon;

                if (geoidHt < minght) {
                    minght = geoidHt;
                    minlat = xlat;
                    minlon = xlon;
                }
                if (geoidHt > maxght) {
                    maxght = geoidHt;
                    maxlat = xlat;
                    maxlon = xlon;
                }
            }

            if (strcmp(ofyn, "Y") == 0 || strcmp(ofyn, "y") == 0) {
                if      (iform == 1)  ff1out(ofp, vec_data[ii], geoidHt, imodel, stddev, distance);
                else if (iform == 2)  ff2out(ofp, vec_data[ii], geoidHt, imodel, stddev, distance);
                // else if (iform == 3)  // bluebook - do nothing -----

                else if (iinput == 1) ff1out(ofp, vec_data[ii], geoidHt, imodel, stddev, distance);

            }//~if(ofyn)

            if( (iinput == 1) && (strcmp(ofyn, "Y") != 0) ){
                ff4out(ofp, vec_data[ii], geoidHt, imodel, stddev, distance);
            }


            // if( (iinput == 1) && (outfil == 1) )
            //     ff4out(ofp, vec_data[ii], geoidHt, imodel, stddev, distance);

        }//~if(xlat == -999. || xlon == -999.)

    }//~for(ii)

    free( (void*)vec_data );

    // Finally, write out the record to screen and possibly to an output file

    // ===== OUTPUT =======================
    // write output to screen and possibly to an output file
    // go get another input dataset


    // =============================================================
    // This is for input file format = 1
    // When finished, give a little report to the screen and end program.
    //
    ave = ave/keep;
    rms = sqrt(rms/keep);
    if (keep > 1) {
        fact = ((double)(keep)/(double)(keep-1));
        std  = sqrt( fact*(rms*rms - ave*ave) );
    } else {
        std = 0;
    }

    printf("\n%s\n", dash70);
    printf("\
FINAL REPORT: \n\
  Number of points input: %8d \n\
  Number of good points : %8d \n\
  Northernmost Latitude : %10.6lf  \n\
  Southernmost Latitude : %10.6lf \n", kount, keep, xn, xs);

    if (poseast == 0) { // poseast = 0  := Westlon
        printf("\
  Westernmost Longitude : %10.6lf \n\
  Easternmost Longitude : %10.6lf \n\
  Minimum Geoid Height  :  %8.3lf \n\
    Lat/Lon of Minimum  : %10.6lf  %10.6lf \n\
  Maximum Geoid Height  :  %8.3lf \n\
    Lat/Lon of Maximum  : %10.6lf  %10.6lf \n\
  Average Geoid Height  :  %8.3lf \n\
  Standard Deviation    :  %8.3lf \n\
  Root Mean Square      :  %8.3lf \n",
            (360. - xw), (360. - xe), minght, minlat, (360. - minlon),
            maxght, maxlat, (360. - maxlon), ave, std, rms);

    } else {       // using East longitude

        printf("\
  Westernmost Longitude : %10.6lf \n\
  Easternmost Longitude : %10.6lf \n\
  Minimum Geoid Height  :  %8.3lf \n\
    Lat/Lon of Minimum  : %10.6lf  %10.6lf \n\
  Maximum Geoid Height  :  %8.3lf \n\
    Lat/Lon of Maximum  : %10.6lf  %10.6lf \n\
  Average Geoid Height  :  %8.3lf \n\
  Standard Deviation    :  %8.3lf \n\
  Root Mean Square      :  %8.3lf \n",
            xw, xe, minght, minlat, minlon,
            maxght, maxlat, maxlon, ave, std, rms);

    }

    if (iinput == 2) {  // input by file
        fclose(ifp);
    }
    if (strcmp(ofyn, "Y") == 0 || strcmp(ofyn, "y") == 0) {
        fclose(ofp);
    }

    return(0);

}//~intg

