// $Id: trim_c.c 35388 2010-06-11 14:02:16Z Srinivas.Reddy $ 
// %P%

// ----- standard library --------------------------------------------
#include <string.h>
#include <malloc.h>
#include <ctype.h>

// global variables ------------------------------------------------------------
static char sccsid[] = "$Id: trim_c.c 35388 2010-06-11 14:02:16Z Srinivas.Reddy $	20$Date: 2010/05/04 14:27:01 $ NGS";

char* trim_c(char* str, const char w) {
//---------------------------------------------------------------------
// Removes leading and/or trailing white space chars from the input string.
// --- NON - STDC++ version ---
// Parameters:
//  in -  str : ptr to char string to be trimmed.
//  in -  w   : where to trim: left='L'or'l'; right='R'or'r'; --no default--
//  out-  str : ptr to the trimmed char string     by reference
//  ret-      : ptr to the trimmed char string
//---------------------------------------------------------------------
    int   ii=0;
    char *ptr;

    // Trim on the left
    if ( strchr("LlBb", w) ) {
       for (ptr = str; (isspace(*ptr) != 0); ++ptr) {
          ;
       }
       if (ptr != str) {
          strcpy(str, ptr);
       }
    }

    // Trim on the right
    if (strchr("RrBb", w) != 0) {
       for (ii = (strlen(str)-1); (ii >= 0 && isspace(str[ii]) != 0); --ii) {
          str[ii] = '\0';
       }
    }

    return(str); // Return the trimmed string

} // trim_c()
