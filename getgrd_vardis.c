// %P%
// ----- constants ---------------------------------------------------
// $Rev:: 6#$
// $Date::  $
// $Id:: ge#$

// ----- standard library --------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ----- classes, structures, types ----------------------------------
// ----- functions ---------------------------------------------------
#include "getgrd_vardis.h"
#include "trim_c.h"


void getgrd_vardis(int imodel, char* dirnam, int is_subr, int* nfiles,
                   int* nvff, int *ndff, char var_fnames[50][256],
                   FILE* var_ifp[50] ){
/*******************************************************************************
* "get (variance and distance) grids"
* Fills vectors with the file names of the gridded variance and distance data,
* and opens all files that can be found in the given directory.
* Additional geoid models may be added as developed.
*  in - imodel  : integer key to specific geoid model
*  in - dirnam  : directory location of binary geoid model files
*               : special- "ngs" is key to use NGS network files
*  in - IS_SUBR : run as subroutine: false=0;  true=1; (c std notation)
*  out- nfiles  : number of binary files used for the specific geoid model
*  out- nvff    : "Number of Variance Files Found" and --successfully opened--
*  out- ndff    : "Number of Distance Files Found" and --successfully opened--
*  out- var_fnam: array (vector of character vectors) of variance filenames
*  out- dis_fnam: array (vector of character vectors) of distance filenames
*  out- var_ifp : vector of file pointers to the variance files
*  out- dis_ifp : vector of file pointers to the distance files
*
* Notes:
*******************************************************************************/
    char  this_vfname[256];
    char  this_dfname[256];
    char  cval[3];
    const char suffix[] = ".bin";

    int  dirlen = 0;
    int  ii;

    *nvff = 0;
    *ndff = 0;
    *nfiles = 0;

    trim_c(dirnam, 'b');
    dirlen = strlen(dirnam);

    // -----------------------------------------------------
    // The GEOID12A file names
    // -----------------------------------------------------
    if (imodel == 12) {
        *nfiles = 8;
        int numVarFiles = 0;
        int numDisFiles = 0;

        // CONUS -----
        // Attempt to open the one file for both variance and distance
        FILE* ifp_var_conus;
        FILE* ifp_dis_conus;
        strncpy(this_vfname, "\0", 256);
        strncpy(this_dfname, "\0", 256);
        if (dirlen > 0) {
            strcpy(this_vfname, dirnam);
            strcpy(this_dfname, dirnam);
        }
        strcat(this_vfname, "v2012au0.bin");

        if ( ((ifp_var_conus = fopen(this_vfname, "rb")) != NULL) &&
             ((ifp_dis_conus = fopen(this_dfname, "rb")) != NULL) ) {
            *nfiles -= 7;
            strcpy(var_fnames[numVarFiles++], this_vfname);
	    fclose(ifp_var_conus);
	    fclose(ifp_dis_conus);

        } else {
            // First 8 files are CONUS -----
            for (ii = 1; ii <= 8; ++ii) {
                sprintf(var_fnames[numVarFiles++], "%sv2012au%01d.bin",
                        dirnam, ii);
            }
        }

    // -----------------------------------------------------
    // The GEOID12B file names
    // -----------------------------------------------------
    } else if (imodel == 13) {
        *nfiles = 0;
        int numVarFiles = 0;
        int numDisFiles = 0;

        // CONUS -----
        // Attempt to open the one file for both variance and distance
        FILE* ifp_var_conus;
        FILE* ifp_dis_conus;
        strncpy(this_vfname, "\0", 256);
        strncpy(this_dfname, "\0", 256);
        if (dirlen > 0) {
            strcpy(this_vfname, dirnam);
            strcpy(this_dfname, dirnam);
        }
        strcat(this_vfname, "v2012bu0.bin");

        if ( ((ifp_var_conus = fopen(this_vfname, "rb")) != NULL) &&
             ((ifp_dis_conus = fopen(this_dfname, "rb")) != NULL) ) {
            *nfiles += 1;
            strcpy(var_fnames[numVarFiles++], this_vfname);
	    fclose(ifp_var_conus);
	    fclose(ifp_dis_conus);

        }

        // Alaska -----
        // Attempt to open the one file model
        FILE* ifp_var_ak;
        FILE* ifp_dis_ak;
        strncpy(this_vfname, "\0", 256);
        strncpy(this_dfname, "\0", 256);
        if (dirlen > 0) {
            strcpy(this_vfname, dirnam);
            strcpy(this_dfname, dirnam);
        }
        strcat(this_vfname, "v2012AK.bin");

        if ((ifp_var_ak = fopen(this_vfname, "rb")) != NULL &&
             ((ifp_dis_ak = fopen(this_dfname, "rb")) != NULL) ) {
            *nfiles += 1;
            strcpy(var_fnames[numVarFiles++], this_vfname);
	    fclose(ifp_var_ak);
	    fclose(ifp_dis_ak);

        }


        // Next 1 file is GUAM - NORTHERN MARIANAS -----
        FILE* ifp_var_guam;
        FILE* ifp_dis_guam;
        strncpy(this_vfname, "\0", 256);
        strncpy(this_dfname, "\0", 256);
        if (dirlen > 0) {
            strcpy(this_vfname, dirnam);
            strcpy(this_dfname, dirnam);
        }
        strcat(this_vfname, "v2012GUAM.bin");

        if ((ifp_var_guam = fopen(this_vfname, "rb")) != NULL &&
             ((ifp_dis_guam = fopen(this_dfname, "rb")) != NULL) ) {
            *nfiles += 1;
            strcpy(var_fnames[numVarFiles++], this_vfname);
	    fclose(ifp_var_guam);
	    fclose(ifp_dis_guam);

        }

        // Next 1 file is SAMOA -----
        FILE* ifp_var_as;
        FILE* ifp_dis_as;
        strncpy(this_vfname, "\0", 256);
        strncpy(this_dfname, "\0", 256);
        if (dirlen > 0) {
            strcpy(this_vfname, dirnam);
            strcpy(this_dfname, dirnam);
        }
        strcat(this_vfname, "v2012AS.bin");

        if ((ifp_var_as = fopen(this_vfname, "rb")) != NULL &&
             ((ifp_dis_as = fopen(this_dfname, "rb")) != NULL) ) {
            *nfiles += 1;
            strcpy(var_fnames[numVarFiles++], this_vfname);
	    fclose(ifp_var_as);
	    fclose(ifp_dis_as);

        }

        // Next 1 file is PR/VI -----
        FILE* ifp_var_prvi;
        FILE* ifp_dis_prvi;
        strncpy(this_vfname, "\0", 256);
        strncpy(this_dfname, "\0", 256);
        if (dirlen > 0) {
            strcpy(this_vfname, dirnam);
            strcpy(this_dfname, dirnam);
        }
        strcat(this_vfname, "v2012PRVI.bin");

        if ((ifp_var_prvi = fopen(this_vfname, "rb")) != NULL &&
             ((ifp_dis_prvi = fopen(this_dfname, "rb")) != NULL) ) {
            *nfiles += 1;
            strcpy(var_fnames[numVarFiles++], this_vfname);
	    fclose(ifp_var_prvi);
	    fclose(ifp_dis_prvi);

        }

   // -----------------------------------------------------
    // The GEOID18 file names
    // -----------------------------------------------------
    } else if (imodel == 14) {
        *nfiles = 0;
        int numVarFiles = 0;
        int numDisFiles = 0;

        // CONUS -----
        // Attempt to open the one file for both variance and distance
        FILE* ifp_var_conus;
        FILE* ifp_dis_conus;
        strncpy(this_vfname, "\0", 256);
        strncpy(this_dfname, "\0", 256);
        if (dirlen > 0) {
            strcpy(this_vfname, dirnam);
            strcpy(this_dfname, dirnam);
        }
        strcat(this_vfname, "v2018u0.bin");

        if ( ((ifp_var_conus = fopen(this_vfname, "rb")) != NULL) &&
             ((ifp_dis_conus = fopen(this_dfname, "rb")) != NULL) ) {
            *nfiles += 1;
            strcpy(var_fnames[numVarFiles++], this_vfname);
	    fclose(ifp_var_conus);
	    fclose(ifp_dis_conus);

        }

        // Next 1 file is PR/VI -----
        FILE* ifp_var_prvi;
        FILE* ifp_dis_prvi;
        strncpy(this_vfname, "\0", 256);
        strncpy(this_dfname, "\0", 256);
        if (dirlen > 0) {
            strcpy(this_vfname, dirnam);
            strcpy(this_dfname, dirnam);
        }
        strcat(this_vfname, "v2018PRVI.bin");

        if ((ifp_var_prvi = fopen(this_vfname, "rb")) != NULL &&
             ((ifp_dis_prvi = fopen(this_dfname, "rb")) != NULL) ) {
            *nfiles += 1;
            strcpy(var_fnames[numVarFiles++], this_vfname);
	    fclose(ifp_var_prvi);
	    fclose(ifp_dis_prvi);

        }

    }//~if(imodel)

    // -----------------------------------------------------
    // Open all the files that were found
    // Iterate thru the Filenames vector
    // Open files, store ifstreams to ifstream vector
    // Grid file format is unformatted, direct access
    // -----------------------------------------------------
    for (ii = 0; ii < *nfiles; ++ii) {
        FILE* ivfp;
        FILE* idfp;

        ivfp = fopen(var_fnames[ii], "rb");
        var_ifp[ii] = ivfp;             // Store FILE, success -or- fail (NULL)


        if( (ivfp == NULL) && (is_subr == 0) ) {
            printf("Open file failed for %s\n", var_fnames[ii]);
        }


        if (ivfp != NULL) {
            ++(*nvff);
            if (is_subr == 0) {   // (0 := false)
                fprintf(stdout, " *** Opening File: %s\n", var_fnames[ii]);
            }
        }

    }

    if ((*nvff == 0) && (*ndff == 0)) {
       fprintf(stdout, " *** No variance and distance grid files were found for this model.\n");
    }

    return;

}//~getgrd_vardis

