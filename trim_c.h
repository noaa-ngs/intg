#ifndef TRIM_C_H
#define TRIM_C_H

#pragma ident "$Id: trim_c.h 34347 2010-06-09 21:03:58Z Srinivas.Reddy $	20$Date: 2010/06/03 15:23:52 $ NGS"
#include <string.h>

//-----------------------------------------------------------------------------
// Removes leading and/or trailing white space chars from the input string.
// The second argument indicates whether to trim
// blanks from both ends (the default), the left side, or the right side.
// The trimmed string is returned both by value and reference.
//  in - str : ptr to char string to be trimmed
//  in - w   : which end(s) to trim:   both ends (B or b, the default),
//           :  left side (L or l), or right side (R or r).
//  out- str : ptr to the trimmed string by reference         
//  ret- str : ptr to the trimmed string by value
//-----------------------------------------------------------------------------
char* trim_c(char* str, const char w);

#endif // end TRIM_C_H
