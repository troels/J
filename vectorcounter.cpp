#include "vectorcounter.hpp"

namespace J {
  VectorCounter::VectorCounter(const Dimensions &dims): 
    dims(dims), v(vector<int>(dims.get_rank(), 0)) {}
  
  pair<int, int> add_row(VectorCounter &one, VectorCounter& two) { 
    assert(one.get_rank() == two.get_rank());
    int onerank = one.get_rank();
    int tworank = two.get_rank();
    return pair<int, int>(one.dims[onerank - 1], (add_pos(one, two, onerank - 2) + 1) * two.dims[tworank - 1]);
  }

  int add_pos(VectorCounter &one, VectorCounter& two, int pos) { 
    if (pos < 0) return 0;
    assert(one.get_rank() == two.get_rank());
    assert(one.dims[pos] <= two.dims[pos]);
    
    if (one.v[pos] < one.dims[pos] - 1) {
      ++one.v[pos];
      ++two.v[pos];
      return 0;
    } else {
      one.v[pos] = 0;
      two.v[pos] = 0;
      return add_pos(one, two, pos - 1) * two.dims[pos] + (two.dims[pos] - one.dims[pos]);
    }
  }
}



