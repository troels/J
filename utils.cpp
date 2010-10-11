#include "utils.hpp"

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
  
  OperationIteratorBase::OperationIteratorBase(const Dimensions& frame, const Dimensions& object_dims,
					       int output_rank): 
    output(output_rank >= object_dims.get_rank() ? 
	   object_dims : object_dims.suffix(output_rank)),
    frame(frame), 
    iterator_increment_periodicity(output_rank >= object_dims.get_rank() ? 
				   0 : frame.suffix(output_rank - object_dims.get_rank()).number_of_elems()),
    counter_end(frame.number_of_elems()), counter(0) {}
  
  bool OperationIteratorBase::at_end() const {
    return counter >= counter_end;
  }
  
  OperationIteratorBase& OperationIteratorBase::operator++() {
    ++counter;
    
    if (iterator_increment_periodicity == 1) {
      increment_iterator();
    } else  if (iterator_increment_periodicity != 0 && 
		counter % iterator_increment_periodicity == 0) {
      increment_iterator();
    }
    
    return *this;
  }

  template <typename T>
  OperationIterator<T>::OperationIterator(const JArray<T>& c, const Dimensions& frame, int output_rank):
    OperationIteratorBase(frame, c.get_dims(), output_rank),
    content(c), ptr(c.begin()), iterator_increment(output.number_of_elems()) {}
  
  template <typename T>
  void OperationIterator<T>::increment_iterator() { 
    ptr += iterator_increment;
  }

  template <typename T>
  shared_ptr<JNoun> OperationIterator<T>::operator*() const {
    shared_ptr<JNoun> p(new JArray<T>(output, content, ptr, ptr + iterator_increment));
    return p;
  }

  template <typename T>
  OperationScalarIterator<T>::OperationScalarIterator(const JArray<T>& content, const Dimensions& frame):
    content(content), frame(frame), 
    iterator_increment_periodicity(content.get_rank() == 0 ? 0 : 
				   frame.suffix(-content.get_rank()).number_of_elems()),
    counter_end(frame.number_of_elems()),
    counter(0), ptr(content.begin()) {
    assert(frame.prefix_match(content.get_dims()));
  }
  
  template <typename T>
  bool OperationScalarIterator<T>::at_end() const {
    return counter >= counter_end;
  }

  template <typename T>
  OperationScalarIterator<T>& OperationScalarIterator<T>::operator++() {
    ++counter;
    if (iterator_increment_periodicity != 0 &&
	counter % iterator_increment_periodicity == 0) {
      ++ptr;
    }

    return *this;
  }

  template class OperationScalarIterator<JInt>;
  template class OperationIterator<JInt>;
}
