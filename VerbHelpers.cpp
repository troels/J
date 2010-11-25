#include "VerbHelpers.hpp"

namespace J {


Dimensions find_frame(int lrank, int rrank, const Dimensions& larg, const Dimensions& rarg) {
  if (lrank < 0) {
   lrank = std::max(0, larg.get_rank() + lrank);
  } 
  if (rrank < 0) {
    rrank = std::max(0, rarg.get_rank() + rrank);
  }

  if (larg.get_rank() <= lrank && rarg.get_rank() <= rrank)  {
    return Dimensions(0);
  } else if (larg.get_rank() <= lrank) {
    return rarg.prefix(rarg.get_rank() - rrank);
  } else if (rarg.get_rank() <= rrank) {
    return larg.prefix(larg.get_rank() - lrank);
  }
  
  Dimensions lframe = larg.prefix(larg.get_rank() - lrank);
  Dimensions rframe = rarg.prefix(rarg.get_rank() - rrank);
  
  if (lframe.get_rank() > rframe.get_rank()) {
    if (lframe.prefix_match(rframe)) {
      return lframe;
    } else {
      throw JIllegalDimensionsException("Incompatible arguments given");
    }
  } else {
    if (rframe.prefix_match(lframe)) {
      return rframe;
    } else {
      throw JIllegalDimensionsException("Incompatible arguments given");
    }
  }
}

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
  
OperationIteratorBase::Ptr get_operation_iterator(const JNoun& arg, const Dimensions& frame, 
						  int output_rank) { 
  return JArrayCaller<new_operation_iterator, OperationIteratorBase::Ptr>()(arg, frame, output_rank);
}

}
