#ifndef FF2OUT_H
#define FF2OUT_H

// %P%
// ----- constants ---------------------------------------------------
#pragma ident "$Id: ff2out.h 43805 2010-07-12 16:43:53Z bruce.tran $	20$Date: 2009/07/06 17:55:27 $ NGS"

// ----- standard library --------------------------------------------
#include <stdio.h>

// ----- classes, structs, types -------------------------------------
#include "dataset1.h"

// ----- functions ---------------------------------------------------
int ff2out(FILE* ofp, DATASET1 vec_data, double geoidHt);

#endif //~FF2OUT_H

