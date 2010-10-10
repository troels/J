#ifndef VECTORCOUNTER_HPP
#define VECTORCOUNTER_HPP

#include "Dimensions.hpp"
#include <utility>

namespace J {
  using std::pair;
  class VectorCounter { 
    Dimensions dims;
    vector<int> v;
    
  public:
    VectorCounter(const Dimensions& dims);
    
    int get_rank() const { return dims.get_rank(); }

    friend pair<int, int> add_row(VectorCounter &one, VectorCounter& two);
    friend int add_pos(VectorCounter &one, VectorCounter& two, int pos);
  };
}

#endif
