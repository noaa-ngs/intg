// %P%
// ----- constants ---------------------------------------------------
static const char SCCSID[]="$Id: getgrd_geoid.c 35381 2010-06-11 13:32:48Z Srinivas.Reddy $	20$Date: 2010/04/26 13:51:08 $ NGS";

// ----- standard library --------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ----- classes, structures, types ----------------------------------
// ----- functions ---------------------------------------------------
#include "getgrd_geoid.h"
#include "trim_c.h"


void getgrd_geoid(int imodel, char* dirnam, int is_subr, int* nfiles, int* nff,
                  char vec_fnames[50][256], FILE* vec_ifp[50]) {
/*******************************************************************************
* "get (geoid) grids"
* Fills a vector with the file names of the gridded input data,
* and open all files that can be found in the given directory.
* Additional geoid models may be added as developed.
*  in - imodel  : integer key to specific geoid model
*  in - dirnam  : directory location of binary geoid model files
*               : special- "ngs" is key to use NGS network files
*  in - IS_SUBR : run as subroutine: false=0;  true=1; (c std notation)
*  out- nfiles  : number of binary files used for the specific geoid model
*  out- fnam    : array (vector of character vectors) of geoid model filenames
*  out- lin     : vector of file unit numbers  (type = long int)
*               : of geoid model files opened in this subroutine
*  out- nff     : "Number of Files Found" and --successfully opened--
* 
* Notes: 
* was: (imodel, dirnam, is_subr, nfiles, fnam, lin, nff )
*      int* imodel; char* dirnam; int*  is_subr; int*  nfiles;
*      char* fnam;   int*  lin;   int*  nff;
*******************************************************************************/
    char  this_fname[256];
    char  cval[3];
    const char suffix[] = ".bin";

    int  dirlen = 0;
    int  ii;

    *nff = 0;

    trim_c(dirnam, 'b');
    dirlen = strlen(dirnam);

    // -----------------------------------------------------
    // The GEOID99 file names
    // -----------------------------------------------------
    if (imodel == 1) {
        *nfiles = 14;

        // First 8 files are CONUS -----
        for (ii = 1; ii <= 8; ++ii) {
            strncpy(this_fname, "\0", 256);
            strncpy(cval,       "\0",   3);
            sprintf(cval, "%02d", ii);
            if (dirlen > 0) {
                strcpy(this_fname, dirnam);
            } 
            strcat(this_fname, "g1999u");
            strcat(this_fname, cval);
            strcat(this_fname, suffix);

            strcpy(vec_fnames[ii-1], this_fname);
        }

        // Next 4 files are ALASKA -----
        for (ii = 9; ii <= 12; ++ii) {
            strncpy(this_fname, "\0", 256);
            strncpy(cval,       "\0",   3);
            sprintf(cval, "%02d", ii-8);
            if (dirlen > 0) {
                strcpy(this_fname, dirnam);
            } 
            strcat(this_fname, "g1999a");
            strcat(this_fname, cval);
            strcat(this_fname, suffix);

            strcpy(vec_fnames[ii-1], this_fname);
        }

        // Next 1 file is HAWAII -----
        for (ii = 13; ii <= 13; ++ii) {
            strncpy(this_fname, "\0", 256);
            strncpy(cval,       "\0",   3);
            sprintf(cval, "%02d", ii-12);
            if (dirlen > 0) {
                strcpy(this_fname, dirnam);
            } 
            strcat(this_fname, "g1999h");
            strcat(this_fname, cval);
            strcat(this_fname, suffix);

            strcpy(vec_fnames[ii-1], this_fname);
        }

        // Next 1 file is PR/VI -----
        for (ii = 14; ii <= 14; ++ii) {
            strncpy(this_fname, "\0", 256);
            strncpy(cval,       "\0",   3);
            sprintf(cval, "%02d", ii-13);
            if (dirlen > 0) {
                strcpy(this_fname, dirnam);
            } 
            strcat(this_fname, "g1999p");
            strcat(this_fname, cval);
            strcat(this_fname, suffix);

            strcpy(vec_fnames[ii-1], this_fname);
        }

    // -----------------------------------------------------
    // The G99SSS file names
    // -----------------------------------------------------
    } else if (imodel == 2) {
        *nfiles = 8;

        // First 8 files are CONUS -----
        for (ii = 1; ii <= 8; ++ii) {
            strncpy(this_fname, "\0", 256);
            strncpy(cval,       "\0",   3);
            sprintf(cval, "%02d", ii);
            if (dirlen > 0) {
                strcpy(this_fname, dirnam);
            } 
            strcat(this_fname, "s1999u");
            strcat(this_fname, cval);
            strcat(this_fname, suffix);

            strcpy(vec_fnames[ii-1], this_fname);
        }

    // -----------------------------------------------------
    // The GEOID03 file names
    // -----------------------------------------------------
    } else if (imodel == 3) {
        *nfiles = 14;

        // First 8 files are CONUS -----
        for (ii = 1; ii <= 8; ++ii) {
            strncpy(this_fname, "\0", 256);
            strncpy(cval,       "\0",   3);
            sprintf(cval, "%02d", ii);
            if (dirlen > 0) {
                strcpy(this_fname, dirnam);
            } 
            strcat(this_fname, "g2003u");
            strcat(this_fname, cval);
            strcat(this_fname, suffix);

            strcpy(vec_fnames[ii-1], this_fname);
        }

        // Next 4 files are ALASKA -----
        for (ii = 9; ii <= 12; ++ii) {
            strncpy(this_fname, "\0", 256);
            strncpy(cval,       "\0",   3);
            sprintf(cval, "%02d", ii-8);
            if (dirlen > 0) {
                strcpy(this_fname, dirnam);
            } 
            strcat(this_fname, "g2003a");
            strcat(this_fname, cval);
            strcat(this_fname, suffix);

            strcpy(vec_fnames[ii-1], this_fname);
        }

        // Next 1 file is HAWAII -----
        for (ii = 13; ii <= 13; ++ii) {
            strncpy(this_fname, "\0", 256);
            strncpy(cval,       "\0",   3);
            sprintf(cval, "%02d", ii-12);
            if (dirlen > 0) {
                strcpy(this_fname, dirnam);
            } 
            strcat(this_fname, "g2003h");
            strcat(this_fname, cval);
            strcat(this_fname, suffix);

            strcpy(vec_fnames[ii-1], this_fname);
        }

        // Next 1 file is PR/VI
        for (ii = 14; ii <= 14; ++ii) {
            strncpy(this_fname, "\0", 256);
            strncpy(cval,       "\0",   3);
            sprintf(cval, "%02d", ii-13);
            if (dirlen > 0) {
                strcpy(this_fname, dirnam);
            } 
            strcat(this_fname, "g2003p");
            strcat(this_fname, cval);
            strcat(this_fname, suffix);

            strcpy(vec_fnames[ii-1], this_fname);
        }

    // -----------------------------------------------------
    // The USGG2003 file names
    // -----------------------------------------------------
    } else if (imodel == 4) {
        *nfiles = 14;

        // First 8 files are CONUS -----
        for (ii = 1; ii <= 8; ++ii) {
            strncpy(this_fname, "\0", 256);
            strncpy(cval,       "\0",   3);
            sprintf(cval, "%02d", ii);
            if (dirlen > 0) {
                strcpy(this_fname, dirnam);
            } 
            strcat(this_fname, "s2003u");
            strcat(this_fname, cval);
            strcat(this_fname, suffix);

            strcpy(vec_fnames[ii-1], this_fname);
        }

        // Next 4 files are ALASKA -----
        for (ii = 9; ii <= 12; ++ii) {
            strncpy(this_fname, "\0", 256);
            strncpy(cval,       "\0",   3);
            sprintf(cval, "%02d", ii-8);
            if (dirlen > 0) {
                strcpy(this_fname, dirnam);
            } 
            strcat(this_fname, "s2003a");
            strcat(this_fname, cval);
            strcat(this_fname, suffix);

            strcpy(vec_fnames[ii-1], this_fname);
        }

        // Next 1 file is HAWAII -----
        for (ii = 13; ii <= 13; ++ii) {
            strncpy(this_fname, "\0", 256);
            strncpy(cval,       "\0",   3);
            sprintf(cval, "%02d", ii-12);
            if (dirlen > 0) {
                strcpy(this_fname, dirnam);
            } 
            strcat(this_fname, "s2003h");
            strcat(this_fname, cval);
            strcat(this_fname, suffix);

            strcpy(vec_fnames[ii-1], this_fname);
        }

        // Next 1 file is PR/VI -----
        for (ii = 14; ii <= 14; ++ii) {
            strncpy(this_fname, "\0", 256);
            strncpy(cval,       "\0",   3);
            sprintf(cval, "%02d", ii-13);
            if (dirlen > 0) {
                strcpy(this_fname, dirnam);
            } 
            strcat(this_fname, "s2003p");
            strcat(this_fname, cval);
            strcat(this_fname, suffix);

            strcpy(vec_fnames[ii-1], this_fname);
        }

    // -----------------------------------------------------
    // The GEOID06 file names
    // -----------------------------------------------------
    } else if (imodel == 5) {
        *nfiles = 4;

        // First n files are CONUS ----- does not yet exist

        // First 4 files are ALASKA -----
        for (ii = 1; ii <= 4; ++ii) {
            strncpy(this_fname, "\0", 256);
            strncpy(cval,       "\0",   3);
            sprintf(cval, "%02d", ii);
            if (dirlen > 0) {
                strcpy(this_fname, dirnam);
            } 
            strcat(this_fname, "g2006a");
            strcat(this_fname, cval);
            strcat(this_fname, suffix);

            strcpy(vec_fnames[ii-1], this_fname);
        }

    // -----------------------------------------------------
    // The USGG2009 file names
    // -----------------------------------------------------
    } else if (imodel == 6) {
        *nfiles = 16;

        // First 8 files are CONUS -----
        for (ii = 1; ii <= 8; ++ii) {
            strncpy(this_fname, "\0", 256);
            strncpy(cval,       "\0",   3);
            sprintf(cval, "%02d", ii);
            if (dirlen > 0) {
                strcpy(this_fname, dirnam);
            }
            strcat(this_fname, "s2009u");
            strcat(this_fname, cval);
            strcat(this_fname, suffix);

            strcpy(vec_fnames[ii-1], this_fname);
        }

        // Next 4 files are ALASKA -----
        for (ii = 9; ii <= 12; ++ii) {
            strncpy(this_fname, "\0", 256);
            strncpy(cval,       "\0",   3);
            sprintf(cval, "%02d", ii-8);
            if (dirlen > 0) {
                strcpy(this_fname, dirnam);
            }
            strcat(this_fname, "s2009a");
            strcat(this_fname, cval);
            strcat(this_fname, suffix);

            strcpy(vec_fnames[ii-1], this_fname);
        }

        // Next 1 file is HAWAII -----
        for (ii = 13; ii <= 13; ++ii) {
            strncpy(this_fname, "\0", 256);
            strncpy(cval,       "\0",   3);
            sprintf(cval, "%02d", ii-12);
            if (dirlen > 0) {
                strcpy(this_fname, dirnam);
            }
            strcat(this_fname, "s2009h");
            strcat(this_fname, cval);
            strcat(this_fname, suffix);

            strcpy(vec_fnames[ii-1], this_fname);
        }

        // Next 1 file is GUAM -----
        for (ii = 14; ii <= 14; ++ii) {
            strncpy(this_fname, "\0", 256);
            strncpy(cval,       "\0",   3);
            sprintf(cval, "%02d", ii-13);
            if (dirlen > 0) {
                strcpy(this_fname, dirnam);
            }
            strcat(this_fname, "s2009g");
            strcat(this_fname, cval);
            strcat(this_fname, suffix);

            strcpy(vec_fnames[ii-1], this_fname);
        }

        // Next 1 file is SAMOA -----
        for (ii = 15; ii <= 15; ++ii) {
            strncpy(this_fname, "\0", 256);
            strncpy(cval,       "\0",   3);
            sprintf(cval, "%02d", ii-14);
            if (dirlen > 0) {
                strcpy(this_fname, dirnam);
            }
            strcat(this_fname, "s2009s");
            strcat(this_fname, cval);
            strcat(this_fname, suffix);

            strcpy(vec_fnames[ii-1], this_fname);
        }

        // Next 1 file is PR/VI -----
        for (ii = 16; ii <= 16; ++ii) {
            strncpy(this_fname, "\0", 256);
            strncpy(cval,       "\0",   3);
            sprintf(cval, "%02d", ii-15);
            if (dirlen > 0) {
                strcpy(this_fname, dirnam);
            }
            strcat(this_fname, "s2009p");
            strcat(this_fname, cval);
            strcat(this_fname, suffix);

            strcpy(vec_fnames[ii-1], this_fname);
        }

    // -----------------------------------------------------
    // The GEOID09 file names
    // -----------------------------------------------------
    } else if (imodel == 7) {
        // *nfiles = 16;      PR/VI NOT YET RELEASED
        *nfiles = 15;

        // First 8 files are CONUS -----
        for (ii = 1; ii <= 8; ++ii) {
            strncpy(this_fname, "\0", 256);
            strncpy(cval,       "\0",   3);
            sprintf(cval, "%02d", ii);
            if (dirlen > 0) {
                strcpy(this_fname, dirnam);
            }
            strcat(this_fname, "g2009u");
            strcat(this_fname, cval);
            strcat(this_fname, suffix);

            strcpy(vec_fnames[ii-1], this_fname);
        }

        // Next 4 files are ALASKA -----
        for (ii = 9; ii <= 12; ++ii) {
            strncpy(this_fname, "\0", 256);
            strncpy(cval,       "\0",   3);
            sprintf(cval, "%02d", ii-8);
            if (dirlen > 0) {
                strcpy(this_fname, dirnam);
            }
            strcat(this_fname, "g2009a");
            strcat(this_fname, cval);
            strcat(this_fname, suffix);

            strcpy(vec_fnames[ii-1], this_fname);
        }

        // Next 1 file is HAWAII -----
        for (ii = 13; ii <= 13; ++ii) {
            strncpy(this_fname, "\0", 256);
            strncpy(cval,       "\0",   3);
            sprintf(cval, "%02d", ii-12);
            if (dirlen > 0) {
                strcpy(this_fname, dirnam);
            }
            strcat(this_fname, "g2009h");
            strcat(this_fname, cval);
            strcat(this_fname, suffix);

            strcpy(vec_fnames[ii-1], this_fname);
        }

        // Next 1 file is GUAM - NORTHERN MARIANAS -----
        for (ii = 14; ii <= 14; ++ii) {
            strncpy(this_fname, "\0", 256);
            strncpy(cval,       "\0",   3);
            sprintf(cval, "%02d", ii-13);
            if (dirlen > 0) {
                strcpy(this_fname, dirnam);
            }
            strcat(this_fname, "g2009g");
            strcat(this_fname, cval);
            strcat(this_fname, suffix);

            strcpy(vec_fnames[ii-1], this_fname);
        }

        // Next 1 file is SAMOA -----
        for (ii = 15; ii <= 15; ++ii) {
            strncpy(this_fname, "\0", 256);
            strncpy(cval,       "\0",   3);
            sprintf(cval, "%02d", ii-14);
            if (dirlen > 0) {
                strcpy(this_fname, dirnam);
            }
            strcat(this_fname, "g2009s");
            strcat(this_fname, cval);
            strcat(this_fname, suffix);

            strcpy(vec_fnames[ii-1], this_fname);
        }

    /***********************************************
        // Next 1 file is PR/VI -----
        for (ii = 16; ii <= 16; ++ii) {
            strncpy(this_fname, "\0", 256);
            strncpy(cval,       "\0",   3);
            sprintf(cval, "%02d", ii-15);
            if (dirlen > 0) {
                strcpy(this_fname, dirnam);
            }
            strcat(this_fname, "g2009p");
            strcat(this_fname, cval);
            strcat(this_fname, suffix);

            strcpy(vec_fnames[ii-1], this_fname);
        }
    ***********************************************/

    // -----------------------------------------------------
    // Experimental Geoid: XXUSG
    // -----------------------------------------------------
    } else if (imodel == 9) {
        *nfiles = 8;

        // First 8 files are CONUS -----
        for (ii = 1; ii <= 8; ++ii) {
            strncpy(this_fname, "\0", 256);
            strncpy(cval,       "\0",   3);
            sprintf(cval, "%02d", ii);
            if (dirlen > 0) {
                strcpy(this_fname, dirnam);
            } 
            strcat(this_fname, "xxusgu");
            strcat(this_fname, cval);
            strcat(this_fname, suffix);

            strcpy(vec_fnames[ii-1], this_fname);
        }

    }//~if(imodel)

    // -----------------------------------------------------
    // Open all the files that were found
    // Iterate thru the Filenames vector
    // Open files, store ifstreams to ifstream vector
    // Grid file format is unformatted, direct access
    // -----------------------------------------------------
    for (ii = 0; ii < *nfiles; ++ii) {
        FILE* ifp;
        ifp = fopen(vec_fnames[ii], "rb");
        vec_ifp[ii] = ifp;             // Store FILE, success -or- fail (NULL)

        if( (ifp == NULL) && (is_subr == 0) ) {
            printf("Open file failed for %s\n", vec_fnames[ii]);
        }

        if (ifp != NULL) {
            ++(*nff);
            if (is_subr == 0) {   // (0 := false)
                fprintf(stdout, " *** Opening File: %s\n", vec_fnames[ii]);
            }
        }
    }

    // -----------------------------------------------------
    // Check and see if at least ONE file was opened,
    // and make a count of how many WERE opened.
    // Abort if we find no geoid files.
    // -----------------------------------------------------
    if (*nff == 0) {
        fprintf(stderr, "\
ERROR(209):  No files found -- aborting \n\
    Input directory  = %s \n\
    Input model code = %d \n", dirnam, imodel);
        abort();
    }

    return;

}//~getgrd_geoid

