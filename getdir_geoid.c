// %P%
// ----- constants ---------------------------------------------------
static const char SCCSID[]="$Id: getdir_geoidux.c 35346 2010-06-11 13:30:25Z Srinivas.Reddy $	20$Date: 2010/02/22 09:39:37 $ NGS";

// ----- standard library --------------------------------------------
#include <stdio.h>
#include <string.h>

// ----- classes, structs, types -------------------------------------
// ----- functions ---------------------------------------------------
#include "getdir_geoid.h"


int getdir_geoid(int imodel, char* dirnam) {
#ifdef NGS_PC_ENV
   /*******************************************************************************
    * For PC network system, returns the directory location 
    * where geoid binary model grid files are located. 
    *  in - imodel : user selected geoid model   int
    *  in - dirnam : char vector, empty          character*256
    *  out- dirnam : char vector, filled
    *******************************************************************************/

    strncpy(dirnam, "\0", 256);

    printf("\n\
      What is the **FULL** directory name (including trailing slashes) \n\
      where the geoid (*.bin) files may be found? \n\
         (Unix Example:  /export/home/geoid09/) \n\
         (PC   Example:  C:\\GEOID09\\) \n\
         Hit <RETURN> to default to this directory \n\
         -> ");

    fgets(dirnam, 256, stdin);

#else
   /*******************************************************************************
    * For the NGS unix network system, returns the directory location 
    * where geoid binary model grid files are located. 
    *  in - imodel : user selected geoid model   int
    *  in - dirnam : char vector, empty          character*256 dirnam
    *  out- dirnam : char vector, filled
    *******************************************************************************/
    switch (imodel) {
    case 1: 
        strcpy(dirnam, "/ngslib/data/Geoid/Geoid99/");
        break;
    case 2: 
        strcpy(dirnam, "/ngslib/data/Geoid/G99sss/");
        break;
    case 3: 
        strcpy(dirnam, "/ngslib/data/Geoid/Geoid03/");
        break;
    case 4: 
        strcpy(dirnam, "/ngslib/data/Geoid/USGG2003/");
        break;
    case 5: 
        strcpy(dirnam, "/ngslib/data/Geoid/Geoid06/");
        break;
    case 6: 
        strcpy(dirnam, "/ngslib/data/Geoid/USGG2009/Format_unix/");
        break;
    case 7: 
        strcpy(dirnam, "/ngslib/data/Geoid/Geoid09/Format_unix/");
        break;

    case 9: 
        strcpy(dirnam, "/ngslib/data/Geoid/Geoid99/");
        break;

    case 11: 
        strcpy(dirnam, "/ngslib/data/Geoid/USGG2012/");
        break;

    case 12: 
        strcpy(dirnam, "/ngslib/data/Geoid/Geoid12A/");
        break;
        
    case 13: 
        strcpy(dirnam, "/ngslib/data/Geoid/Geoid12C/");
        break;
    case 14:
        strcpy(dirnam, "/ngslib/data/Geoid/Geoid18/");
        break;
        
    default: 
        fprintf(stderr, "ERROR: in file getdirux, invalid option %d\n", imodel);
        return( -1 );
    }//~switch

#endif

    return(0);

}//~getdir_geoid

