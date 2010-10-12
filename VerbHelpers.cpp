#include "VerbHelpers.hpp"

namespace J {
  void  DimensionCounter::increment(int pos)  {
    if (pos < 0) { turned_around = true; return; }
    assert(pos < static_cast<int>(reference.size()));
    
    if (reference[pos] > 0) {
      ++current_count[pos];
      if (current_count[pos] == reference[pos]) {
	current_count[pos] = 0;
	increment(pos - 1);
      }
    } else if (reference[pos] < 0) {
      --current_count[pos];
      if (current_count[pos] == -1) {
	current_count[pos] = -(reference[pos] + 1);
	increment(pos - 1);
      }
    }
  }

  DimensionCounter::DimensionCounter(const vector<int>& ref): reference(ref), 
							      current_count(reference.size()),
							      suffix_array(reference.size()),
							      turned_around(false) {
    for (int i = 0, len = reference.size(); i < len; ++i) {
      suffix_array[i] = std::abs(accumulate(reference.begin() + i + 1, reference.end(), 
					    1, std::multiplies<int>()));
      if (reference[i] == 0) turned_around = true;
      if (reference[i] < 0) current_count[i] = -(reference[i] + 1);
    }
  }
  
  DimensionCounter& DimensionCounter::operator++() {
    increment(reference.size() - 1);
    return *this;
  }
  
  int DimensionCounter::operator*() const {
    int res = 0;
    for (int i = 0, len = current_count.size(); i < len; ++i) {
      res += current_count[i]*suffix_array[i];
    }
    
    return res;
  }

  JArray<JInt> require_ints(const JNoun& noun) {
    if (noun.get_value_type() == j_value_type_int) {
      return static_cast<const JArray<JInt> &>(noun);
    }
    
    throw JIllegalValueTypeException();
  }
}
