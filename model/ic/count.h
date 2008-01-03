#ifndef IC_COUNT_H
#define IC_COUNT_H

extern "C" {
#include "wht.h"
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
  counts& operator+=(counts &v);
  friend ostream& operator<<(ostream& o, counts &v);
};
typedef counts::iterator counts_iter;

counts * count(Wht *W, size_t max);

#endif/*IC_COUNT_H*/
