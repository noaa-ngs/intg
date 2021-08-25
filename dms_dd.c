// %P%
// ----- constants ---------------------------------------------------
static const char SCCSID[]="$Id: dms_dd.c 50355 2010-10-19 15:55:47Z Srinivas.Reddy $	20$Date: 2009/05/26 12:26:02 $ NGS";

// ----- standard library --------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


double dms_dd( const char* const dms_str ) {
/*******************************************************************************
* Conversion from dms string to decimal_degrees value
* in - char string in dms format
*      First char  must be hemisphere direction.
*      Hemisphere (N/S or W/E) is toggle for lat or lon parsing.
*      Max seconds =  8 digits right of decimal
*      Max chars   = 17   ie, WDDDMMSS.12345678
* ret- decimal degrees
*******************************************************************************/
   int deg = 0;
   int min = 0;

   double sec  = 0.0;
   double dd   = 0.0;

   char* err_ptr;

   char deg_str[4];
   char min_str[4];
   char sec_str[12];

   strncpy(deg_str, "\0",  4);
   strncpy(min_str, "\0",  4);
   strncpy(sec_str, "\0", 12);

   if( strlen(dms_str) > 17 ) {
      printf("String into function dms_dd() too long\n");
      printf("Max length := Hdddmmss.12345678\n");
      printf("String      = %s\n", dms_str );
      exit(1);
   }

   if( dms_str[0] == 'N' || dms_str[0] == 'S' ) {
      strncpy( deg_str, &dms_str[1], 2);
      strncpy( min_str, &dms_str[3], 2);
      strncpy( sec_str, &dms_str[5], (strlen(dms_str)-5) );
   } else 
   if( dms_str[0] == 'W' || dms_str[0] == 'E' ) {
      strncpy( deg_str, &dms_str[1], 3);
      strncpy( min_str, &dms_str[4], 2);
      strncpy( sec_str, &dms_str[6], (strlen(dms_str)-6) );
   } else {
      printf("ERROR: string into dms1_rad() not beginning with N/S/W/E\n");
      printf("       String = %s\n", dms_str );
      exit(1);
   }

   /*-----*/
   deg = strtol(deg_str, &err_ptr, 10);
   if (strlen(err_ptr) > 0) {
      printf("ERROR: Invalid degrees in dms_dd: %s\n", err_ptr );
      printf("       DMS entered = %s\n", dms_str );
      exit(1);
   }

   min = strtol(min_str, &err_ptr, 10);
   if( strlen(err_ptr) > 0 ) {
      printf("ERROR: Invalid minutes in dms_dd: %s\n", err_ptr );
      printf("       DMS entered = %s\n", dms_str );
      exit(1);
   }

   sec = strtod(sec_str, &err_ptr);
   if (strlen(err_ptr) > 0) {
      printf("ERROR: Invalid seconds in dms_dd: %s\n", err_ptr );
      printf("       DMS entered = %s\n", dms_str );
      exit(1);
   }
   /*-----*/

   dd = deg + min/60.0 + sec/3600.0;    // decimal degrees

   return( dd );

}//~dms_dd

