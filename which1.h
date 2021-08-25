#ifndef WHICH1_H
#define WHICH1_H

// %P%
// ----- constants ---------------------------------------------------
#pragma ident "$Id: which1.h 35299 2010-06-11 13:17:05Z Srinivas.Reddy $	20$Date: 2009/05/15 14:01:13 $ NGS"

// ----- standard library --------------------------------------------
#include <stdio.h>

// ----- classes, structs, types -------------------------------------
#include "grid_header.h"

// ----- functions ---------------------------------------------------
int which1(double xlat, double xlon, int nfiles, int kk, int imodel,
           GRID_HEADER vec_hdr[50], FILE* vec_ifp[50]);

#endif //~WHICH1_H

