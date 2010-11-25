#include "Aggregates.hpp"

namespace J { namespace Aggregates { 

JResult::JResult(const Dimensions& frame):
  frame(frame), nouns(frame.number_of_elems()), nouns_ptr(nouns.begin()) {}
  
void JResult::add_noun(JNoun::Ptr noun) { 
  assert(nouns_ptr != nouns.end());

  *nouns_ptr = noun;
  ++nouns_ptr;
}
  
shared_ptr<JNoun> JResult::assemble_result() const { 
  assert(nouns_ptr == nouns.end());

  Dimensions cell_dims(find_common_dims_from_nouns(nouns.begin(), nouns.end()));
  
  j_value_type type;
  if (cell_dims.number_of_elems() == 0) {
    type = j_value_type_int;
  } else {
    optional<j_value_type> otype(find_common_type_dropping_empty(nouns.begin(), nouns.end()));
    if (!otype) {
      throw JIllegalValueTypeException();
    }
    
    type = *otype;
  }
    
  return JTypeDispatcher<assemble_result_helper, JNoun::Ptr>()(type, *this, cell_dims);
}
    
template <typename T> 
JNoun::Ptr JResult::assemble_result_internal(const Dimensions& cell_dims) const {
  Dimensions res = frame + cell_dims;
  shared_ptr<vector<T> > v(make_shared<vector<T> >(res.number_of_elems(), JTypeTrait<T>::base_elem()));
  
  typename vector<T>::iterator ptr(v->begin());
  typename JNounList::const_iterator nounlist_ptr(nouns.begin()), nounlist_end(nouns.end());
    
  int cell_dims_len = cell_dims.number_of_elems();
  for (;nounlist_ptr != nounlist_end; ++nounlist_ptr, ptr += cell_dims_len) {
    require_type<T>(**nounlist_ptr).extend_into(cell_dims, ptr);
  }
  
  return boost::static_pointer_cast<JNoun>(make_shared<JArray<T> >(res, v));
}

}}
