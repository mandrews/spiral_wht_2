#ifndef IC_COUNT_H
#define IC_COUNT_H

extern "C" {
#include "wht.h"
}

#include "labeled_vector.h"

labeled_vector * count(Wht *W, size_t max);

#endif/*IC_COUNT_H*/
