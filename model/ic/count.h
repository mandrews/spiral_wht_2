#ifndef IC_COUNT_H
#define IC_COUNT_H

extern "C" {
#include "wht.h"
#include "registry.h"
}

#include <map>
#include <string>
#include <iostream>

using std::map;
using std::string;
using std::ostream;

class counts : public map<string, double> 
{
  public:

  counts(size_t max = WHT_MAX_UNROLL);

  counts& operator+=(counts &x);
  counts& operator*=(counts &x);

  friend ostream& operator<<(ostream& o, counts &x);

  void load_counts(FILE *fd);
  void load_basis(FILE *fd);

  size_t max;
};

typedef counts::iterator counts_iter;

void count(Wht *W, counts &x);

#endif/*IC_COUNT_H*/
