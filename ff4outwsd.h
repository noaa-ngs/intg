#ifndef FF4OUTWSD_H
#define FF4OUTWSD_H

// %P%
// ----- constants ---------------------------------------------------
#pragma ident "$Id: ff4outwsd.h 67430 2012-12-18 19:09:13Z stephen.gilbert $	20$Date: 2009/07/06 17:55:39 $ NGS"

// ----- standard library --------------------------------------------
#include <stdio.h>

// ----- classes, structs, types -------------------------------------
#include "dataset1.h"

// ----- functions ---------------------------------------------------
int ff4outwsd(FILE* ofp, DATASET1 vec_data, double geoidHt, double stddev,
           double distance);

#endif //~FF4OUTWSD_H

