#ifndef RUN_BBK_H
#define RUN_BBK_H

// %P%
// ----- constants ---------------------------------------------------
#pragma ident "$Id: run_bbk.h 43800 2010-07-12 16:43:35Z bruce.tran $	20$Date: 2009/05/15 15:19:29 $ NGS"

// ----- standard library --------------------------------------------
#include <stdio.h>

// ----- classes, structs, types -------------------------------------
#include "grid_header.h"

// ----- functions ---------------------------------------------------
void run_bbk(FILE* ifp, FILE* ofp, FILE* vec_ifp[50], GRID_HEADER vec_hdr[50], 
             int nfiles, int imodel);

#endif //~RUN_BBK_H

