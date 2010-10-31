#ifndef UTILS_HPP
#define UTILS_HPP

#include "Dimensions.hpp"
#include "JNoun.hpp"
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
  friend int add_pos(VectorCounter& one, VectorCounter& two, int pos);
};

class OperationIteratorBase {
protected:
  Dimensions output;

private:
  Dimensions frame;
  int iterator_increment_periodicity, counter_end, counter;
    
public:
  virtual ~OperationIteratorBase() {}
  OperationIteratorBase(const Dimensions& frame, const Dimensions& object_dims, 
			int output_rank);
    
  bool at_end() const;
  OperationIteratorBase& operator++();
  virtual shared_ptr<JNoun> operator*() const = 0; 

protected:
  virtual void increment_iterator() = 0;
};    
    
template <typename T> 
class OperationIterator: public OperationIteratorBase { 
  typedef JArray<T> container;
  typedef typename container::iter iterator;

  container content;
  iterator ptr;
  int iterator_increment;

protected:
  void increment_iterator();
      
public:
  OperationIterator(const JArray<T>& c, const Dimensions& frame, int output_rank);

  shared_ptr<JNoun > operator*() const;
};

template <typename T>
class OperationScalarIterator { 
  typedef JArray<T> container;
  typedef typename container::iter iterator;

  JArray<T> content;
  Dimensions frame;
  int iterator_increment_periodicity;
  int counter_end, counter;
  iterator ptr;

public:
  OperationScalarIterator(const JArray<T>& content, const Dimensions& frame);
      
  bool at_end() const;
  OperationScalarIterator<T>& operator++();

  T operator*() const { return *ptr; }
};
    
  
JArray<JFloat> jarray_int_to_float(const JArray<JInt>& arr);
}


#endif
