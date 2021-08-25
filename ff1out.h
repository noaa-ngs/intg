#ifndef FF1OUT_H
#define FF1OUT_H

// %P%
// ----- constants ---------------------------------------------------
#pragma ident "$Id: ff1out.h 43804 2010-07-12 16:43:49Z bruce.tran $	20$Date: 2009/07/06 17:55:11 $ NGS"

// ----- standard library --------------------------------------------
#include <stdio.h>

// ----- classes, structs, types -------------------------------------
#include "dataset1.h"

// ----- functions ---------------------------------------------------
int ff1out(FILE* ofp, DATASET1 vec_data, double geoidHt);

#endif //~FF1OUT_H

