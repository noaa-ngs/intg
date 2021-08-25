// %P%
// ----- constants ---------------------------------------------------
static const char SCCSID[]="$Id: expform.c 82438 2015-02-23 17:39:09Z bruce.tran $	20$Date: 2009/05/22 13:12:39 $ NGS";

// ----- standard library --------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ----- classes, structs, types -------------------------------------
// ----- functions ---------------------------------------------------
#include "expform.h"


void expform( void ) {
/*******************************************************************************
* Prints an explanation of the various formats to the display
*
*******************************************************************************/
    char  keyb[11];
    char  cformx[40];
    int   iformx =0;
    int   iii    =0;

    strncpy(keyb,   "\0", 11);
    strncpy(cformx, "\0", 40);


    while (iii == 0) {
    // -----------------------------------------------
    // Find out which format needs explaining
    // -----------------------------------------------
    printf("\
Which format would you like explained? \n\
  1 = Free Format Type 1               \n\
  2 = Free Format Type 2               \n\
  3 = NGS Blue Book Format             \n\
 99 = Return to the main program       \n\
  0 = End Program                      \n\
  -> ");

    fgets(cformx, 40, stdin);
    iformx = atoi( cformx );

    switch (iformx) {
    case 0:
        exit(0);
    case 99:
        ++iii;
        break;

    case 1:
    // -------------------------------------------
    // Explain Free Format (For Geoid) Type 1
    // -------------------------------------------
        printf("\
----------------------------------------------------------------------\n\
 1 = Free Format (For Geoid) Type 1 : \n\n\
ALL THE SAMPLES BELOW ARE VALID FORMAT 130 59.987654321 077 59.987654321  \n\
BM1027                                  50 75 \n\
XX8063                                          45.3 078 \n\
                 AIRPORT SC1               47 18.55 107 46 \n\
                                         30 17 089 01 \n\
           CORNER MARKER                 48 14 17 120 07 41.247 \n\
 DATA PT #4077                             42 42 42.4242   104 04 04.44445 \n\
        this one should be out of bounds    -89 359 \n\
Bangor ME                                  44 47 59.0 68 49 11 \n\
   Miami FL                                  25 49 11  80 16 47.0 \n\
San Diego CA                               32 43 47     117 10 11 \n\
Seattle WA                                 47.45     122.30 \n\
   Anchorage AK                                61.17 150.02\n\
Honolulu HI                               21 21 0     157 55 47\n\
           Guam                                       13.5000  215.2\n\
Northern Mariana Island                   17.1234 214.056\n\
BAKER ISLAND                             0 12 15    176 28 06.34534\n\
HOWLAND ISLAND                             0 48     176 38\n\
 JARVIS ISLAND                           -0 02     160 02\n\
LIHUE KAUAI ISLAND                         21 59    159 23\n\
PALMYRA ISLAND                            5 52      162 05\n\
American Samoa                                 -14 20     170 00\n\
San Juan PR                              18 28  066 07\n\
St. John USVI                               18.3333 64.7333\n\
SE Hawaii NAD83 (PA11) W0 from USGG2012 18.1    154.1\n\
NW Hawaii NAD83 (PA11) W0 from USGG2012 23.9    160.9\n\
SE Guam&Mariana NAD83(MA11)GUVD04&NMVD0311.1    214.1  \n\
NW Guam&Mariana NAD83(MA11)GUVD04&NMVD0317.9    216.9\n\
SE American Samoa NAD83 (PA11) ASVD02   -16.9   168.1\n\
NW American Samoa NAD83 (PA11) ASVD02   -11.1   173.9\n\
SE P.Rico/USVI NAD83(2011) PRVD02&VIVD0915.1    064.1  \n\
NW P.Rico/USVI NAD83(2011) PRVD02&VIVD0920.9     68.9\n\
SE Conterminous US NAD83 (2011) NAVD88  24.1    060.1\n\
NW Conterminous US NAD83 (2011) NAVD88  57.9    129.9\n\
SE Alaska          NAD83 (2011) NAVD88  49.1    126.1\n\
NW Alaska          NAD83 (2011) NAVD88  71.9    187.9   \n\n\
<----- Columns 01-40 for mark ID ------>\n\
                                        <------ Columns 41-80 for lat lon ----->\n\
         1         2         3         4         5         6         7         8\n\
12345678901234567890123456789012345678901234567890123456789012345678901234567890\n\n\
RULES FOR GEOID FREE FORMAT TYPE 1 : \n\n\
This is an ASCII format, where the first 40 characters  \n\
   of each record may contain the station name or be blank.\n\n\
The rest of the record (characters 41-80) must contain  \n\
   the latitude and longitude in one of three formats:     \n\
  (a) (two numbers)  decimal/integer degrees           \n\
  (b) (four numbers) integer degrees, decimal/integer minutes\n\
  (c) (six numbers)  integer degrees, integer minutes, decimal/integer seconds\n\n\
The latitude must be positive North. \n\
The longitude can be positive east or west; \n\
   the user selects which was used when running the program.\n\n\
                         (Hit RETURN to continue)\n");

        getchar();
        break;

    case 2:
    // -------------------------------------------
    // Explain Free Format (For Geoid) Type 2
    // -------------------------------------------
        printf("\
----------------------------------------------------------------------\n\
 2 = Free Format (For Geoid) Type 2 : \n\n\
30 59.987654321 077 59.987654321ALL THE SAMPLES BELOW ARE VALID FORMAT 2\n\
         45.3 078               XX8063 \n\
  47 18.55 107 46                               AIRPORT SC1 \n\
30 17 089 01 \n\
48 14 17 120 07 41.247                    CORNER MARKER \n\
 42 42 42.4242 104 04 04.44445  DATA PT #4077 \n\
   -89 359                                  this one should be out of bounds \n\
44 47 59.0    068 49 11                          Bangor ME \n\
 25 49 11  80 16 47.0                            Miami FL  \n\
 32 43 47     117 10 11         San Diego CA \n\
  47.45     122.30              Seattle WA  \n\
 61.17 150.02                             Anchorage AK \n\
 21 21 0     157 55 47                          Honolulu HI\n\
 13.5000  215.2                                     Guam \n\
17.1234 214.056                         Northern Mariana Island\n\
 0 12 15    176 28 06.34534                         BAKER ISLAND\n\
 0 48     176 38                         HOWLAND ISLAND\n\
  -0 02     160 02                            JARVIS ISLAND\n\
 21 59    159 23                            LIHUE KAUAI ISLAND\n\
 5 52      162 05                            PALMYRA ISLAND\n\
 -14 20     170 00                          American Samoa\n\
 18 28  066 07                          San Juan PR\n\
 18.3333 64.7333                          St. John USVI \n\
 18.1    154.1                  SE Hawaii          NAD83 (PA11) W0 from USGG2012\n\
 23.9    160.9                  NW Hawaii          NAD83 (PA11) W0 from USGG2012\n\
 11.1    214.1                  SE Guam & Mariana  NAD83 (MA11) GUVD04 & NMVD03\n\
 17.9    216.9                  NW Guam & Mariana  NAD83 (MA11) GUVD04 & NMVD03\n\
-16.9    168.1                  SE American Samoa  NAD83 (PA11) ASVD02 \n\
-11.1    173.9                  NW American Samoa  NAD83 (PA11) ASVD02 \n\
 15.1    064.1                  SE P.Rico/USVI     NAD83 (2011) PRVD02 & VIVD09\n\
 20.9    068.9                  NW P.Rico/USVI     NAD83 (2011) PRVD02 & VIVD09\n\
 24.1    060.1                  SE Conterminous US NAD83 (2011) NAVD88 \n\
 57.9    129.9                  NW Conterminous US NAD83 (2011) NAVD88 \n\
 49.1    126.1                  SE Alaska          NAD83 (2011) NAVD88 \n\
 71.9    187.9                  NW Alaska          NAD83 (2011) NAVD88 \n\n\
<-- Columns 1-32 for lat lon -->\n\
                                <--------- Columns 33-80 for mark ID ---------->\n\
         1         2         3         4         5         6         7         8\n\
12345678901234567890123456789012345678901234567890123456789012345678901234567890\n\n\
RULES FOR GEOID FREE FORMAT TYPE 2 : \n\n\
This is an ASCII format, where the last 40 characters  \n\
   of each record may contain the station name or be blank.\n\n\
The first 32 characters of the record (characters 1-32)\n\
   must contain the latitude and longitude in one of three formats:     \n\
  (a) (two numbers)  decimal/integer degrees           \n\
  (b) (four numbers) integer degrees, decimal/integer minutes\n\
  (c) (six numbers)  integer degrees, integer minutes, decimal/integer seconds\n\n\
The latitude must be positive North. \n\
The longitude can be positive east or west; \n\
   the user selects which was used when running the program.\n\n\
                         (Hit RETURN to continue)\n");

        getchar();
        break;

    case 3:
    // -------------------------------------------
    // Explain NGS Blue Book Format
    // -------------------------------------------
        printf("\
----------------------------------------------------------------------\n\
 3 = NGS Blue Book Format : \n\n\
 NGS Horizontal (Vol 1) Blue Book format - \
*80* (Control Point) and *86* (Combo Height) Records.\n\
INTG uses information from the *80* and *86* records in Blue \
Book files. \n\
The Station Serial Number(SSN), \
and the Latitude and the Longitude\n\
are read from the *80* records.  \n\
The rest of the record is ignored. \n\
Only the *86* record is modified by INTG - \
The *80* records and all other records are \
passed through without change to the output file.\n\
Any *84* records are deleted.\n\
Information in cols 36-43 in the *86* record is created by INTG.\n\
\n\
For more information on this format, please refer to: \n\
   \'Input Formats and Specifications of the \
National Geodetic Survey Data Base\' \n\
   \'Volume 1. Horizontal Control Data\'. \n\
Published by the Federal Geodetic Control Committee \
in September 1994 and \n\
available from: the National Geodetic Survey, NOAA, \
 Silver Spring, MD 20910. \n\n\
                         (Hit RETURN to continue)\n");

        getchar();

        printf("\
 The following input example is *80* and *86* records \
from a Blue Book file: \n\n\
004560*80*0096KNOXVILLE CT HSE              411906578  N0930548534  W 277  MIA33\n\
004565*86*0096 any geoid values in here     will be overwritten if ssn matches  \n\
                         (Hit RETURN to continue)\n");

        getchar();
        break;

    default:
        printf("Error: Invalid response\n");

    }//~switch
    }//~while(iii)

}//~expform

