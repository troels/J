#include "VerbHelpers.hpp"

namespace J {
JResult::JResult(const Dimensions& frame):
  frame(frame), nouns(JNounList(frame.number_of_elems())), nouns_ptr(nouns.begin()), 
  elem_dims(), value_type() {}
  

void JResult::update_elem_dims(const Dimensions& dims) {
  if (!elem_dims) { 
    elem_dims = elem_dims_t(shared_ptr<vector<int> >
			    (new vector<int> (dims.begin(), dims.end())));
  } else { 
    int rank_diff = (*elem_dims)->size() - dims.get_rank();
    
    if (rank_diff >= 0) {
      std::transform(dims.begin(), dims.end(), (*elem_dims)->begin() + rank_diff, 
		     (*elem_dims)->begin() + rank_diff, std::ptr_fun(std::max<int>));
    }  else {
      std::transform(dims.begin() - rank_diff, dims.end(), (*elem_dims)->begin(), (*elem_dims)->begin(), 
		     std::ptr_fun(std::max<int>));
      (*elem_dims)->insert((*elem_dims)->begin(), dims.begin(), dims.begin() - rank_diff);
    }
  }
}

void JResult::add_noun(JNoun::Ptr noun) { 
  assert(nouns_ptr != nouns.end());
  
  if (get_value_type() && *get_value_type() != noun->get_value_type()) 
    throw JIllegalValueTypeException();

  update_elem_dims(noun->get_dims());

  if (!value_type) 
    value_type = noun->get_value_type();
  
  *nouns_ptr = noun;
  ++nouns_ptr;
}
  
shared_ptr<JNoun> JResult::assemble_result() const { 
  return JTypeDispatcher<assemble_result_helper, JNoun::Ptr>()(*get_value_type(), *this);
}
    
template <typename T> 
JNoun::Ptr JResult::assemble_result_internal() const {
  Dimensions content_dims(get_elem_dims());
  Dimensions res = get_frame() + content_dims;
  shared_ptr<vector<T> > v(new vector<T>(res.number_of_elems(), JTypeTrait<T>::base_elem()));
    
  typename vector<T>::iterator ptr = v->begin();
  typename JNounList::const_iterator nounlist_ptr = get_nouns().begin();
  typename JNounList::const_iterator nounlist_end = get_nouns().end();
    
  int content_dims_len = content_dims.number_of_elems();
  for (;nounlist_ptr != nounlist_end; ++nounlist_ptr, ptr += content_dims_len) {
    boost::static_pointer_cast<JArray<T>, JNoun>(*nounlist_ptr)->extend_into(content_dims, ptr);
  }
  
  return shared_ptr<JNoun>(new JArray<T>(res, v));
}


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

std::auto_ptr<OperationIteratorBase> get_operation_iterator(const JNoun& arg, const Dimensions& frame, 
							    int output_rank) { 
  switch (arg.get_value_type()) {
  case j_value_type_int:
    return std::auto_ptr<OperationIteratorBase>
      (new OperationIterator<JInt>(static_cast<const JArray<JInt>&>(arg), frame, output_rank));
  case j_value_type_float:
    return std::auto_ptr<OperationIteratorBase>
      (new OperationIterator<JFloat>(static_cast<const JArray<JFloat>&>(arg), frame, output_rank));
  default:
    assert(0);
  }
}

}
