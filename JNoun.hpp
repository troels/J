#ifndef JNOUN_HPP
#define JNOUN_HPP

#include "JGrammar.hpp"
#include "Dimensions.hpp"

#include <stdexcept>
#include <iomanip>
#include <utility>
#include <cmath>
#include <iostream>
#include <boost/optional.hpp>

namespace J {
using boost::optional;
using std::pair;

class JNoun: public JWord {
public:
  typedef shared_ptr<JNoun> Ptr;

private:
  j_value_type value_type;
  Dimensions dims;

public:
  JNoun(const Dimensions& d, j_value_type value_type);
  virtual string to_string() const = 0;
  virtual JNoun::Ptr subarray(int start, int end) const = 0;
  virtual JNoun::Ptr coordinate(int nr_coords, ...) const = 0;
  virtual JNoun::Ptr clone() const = 0;

  virtual JNoun::Ptr extend(const Dimensions &d) const = 0;
  bool is_scalar() const { return get_rank() == 0; }
  bool is_array() const { return !is_scalar(); }
    
  int get_rank() const { return get_dims().get_rank(); }
  j_value_type get_value_type() const { return value_type; }
  const Dimensions& get_dims() const { return dims; } 
};
  
template <typename T> 
class JArray: public JNoun {
public:
  typedef vector<T> container;
  typedef typename container::iterator iter;
  typedef typename container::iterator iterator;

private:
  shared_ptr<container> content;

  iter begin_iter;
  iter end_iter;

  int get_field_width() const;
  void content_string(std::stringstream &s, int field_width) const;
    
public:
  JArray(const Dimensions& d, shared_ptr<container> v);
  JArray(const Dimensions& d, shared_ptr<container> v, iter begin, iter end);
  JArray(const Dimensions& d, const JArray<T>& arr, iter begin, iter end);
  JArray(const Dimensions &d, ...);
  JArray();

  JArray<T> operator[](int n) const;
  JNoun::Ptr coordinate(int nr_coords, ...) const;
    

  bool operator==(const JWord& j) const;
  bool operator!=(const JWord& arr) const { 
    return !(*this == arr); 
  }

  string to_string() const;
  string content_string() const;

  JNoun::Ptr clone() const;
  JNoun::Ptr subarray(int start, int end) const;
  JNoun::Ptr extend(const Dimensions &d) const;
  void extend_into(const Dimensions& d, iter new_begin) const;
  shared_ptr<container> get_content() const;

  T get_scalar_value() const { assert(is_scalar()); return *begin(); }
  iter begin() const { return begin_iter; }
  iter end() const { return end_iter; }
};


template <typename T>
shared_ptr<JArray<T> > filled_array(const Dimensions &dims, T val) {
  shared_ptr<vector<T> > v(new vector<T>(dims.number_of_elems(), val));
  return shared_ptr<JArray<T> >(new JArray<T>(dims, v));
}


}

#endif
