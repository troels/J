#ifndef UTILS_HPP
#define UTILS_HPP

#include "Dimensions.hpp"
#include "JNoun.hpp"
#include <utility>
#include <sstream>

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
  virtual JNoun::Ptr operator*() const = 0; 

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

  JNoun::Ptr operator*() const;
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
    

template <typename S, typename T>
struct attr_fun_t {
  S T::* attrib;

  attr_fun_t(S T::* attrib): attrib(attrib) {}

  S operator()(const T& elem) { 
    return elem.*attrib;
  }
};

template <typename S, typename T>
attr_fun_t<S, T> attr_fun(S T::* attrib) { 
  return attr_fun_t<S, T>(attrib);
}

template <typename T>
string join_str(T begin, T end, const string& joiner) { 
  if (begin == end) return string("");
  std::stringstream ss(std::stringstream::out);
  ss << *begin;
  ++begin;
  
  for(;begin != end; ++begin) { 
    ss << joiner;
    ss << *begin;
  }
  
  return ss.str();
}

template <typename T>
JArray<T> expand_to_rank(int rank, const JArray<T>& array) {
  assert(rank >= array.get_rank());
  Dimensions old_dims(array.get_dims());
  shared_ptr<vector<int> > new_dims_vector(new vector<int>(rank, 1));

  copy(old_dims.begin(), old_dims.end(), new_dims_vector->begin() + (rank - array.get_rank()));
  return JArray<T>(new_dims_vector, array.get_content());
}
  
bool escape_char_p(char c);
string escape_regex(const string& s);
string trim_string(const string& s); 
}

#endif
