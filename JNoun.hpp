#ifndef JNOUN_HPP
#define JNOUN_HPP

#include "JGrammar.hpp"
#include "Dimensions.hpp"

#include <iomanip>
#include <utility>
#include <cmath>
#include <iostream>

namespace J {
  using std::pair;

  class JNoun: public JWord {
    j_value_type value_type;
    Dimensions dims;

  public:
    JNoun(const Dimensions& d, j_value_type value_type);
    virtual string to_string() const = 0;
    virtual shared_ptr<JNoun> coordinate(int nr_coords, ...) const = 0;
    virtual shared_ptr<JNoun> clone() const = 0;

    virtual bool operator==(const JNoun& n) const = 0;
    virtual bool operator!=(const JNoun& n) const = 0;
    virtual shared_ptr<JNoun> extend(const Dimensions &d) const = 0;
    
    bool is_scalar() const { return get_dims().get_rank() == 0; }
    bool is_array() const { return !is_scalar(); }
    
    int get_rank() const { return get_dims().get_rank(); }
    j_value_type get_value_type() const { return value_type; }
    const Dimensions& get_dims() const { return dims; } 

    friend std::ostream& operator<<(std::ostream& os, const JNoun& d);
  };
  
  template <typename T> 
  class JArray: public JNoun {
  public:
    typedef vector<T> container;
    typedef typename container::iterator iter;

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
    JArray();
    JArray(const Dimensions &d, ...);

    JArray<T> operator[](int n) const;
    shared_ptr<JNoun> coordinate(int nr_coords, ...) const;
    

    bool operator==(const JNoun& j) const;
    bool operator!=(const JNoun& arr) const { 
      return !(*this == arr); 
    }

    string to_string() const;
    string content_string() const;

    shared_ptr<JNoun> clone() const;
    virtual shared_ptr<JNoun> extend(const Dimensions &d) const;
    void extend_into(const Dimensions& d, iter new_begin) const;

    shared_ptr<container> get_content() const { return content; }
    iter begin() const { return begin_iter; }
    iter end() const { return end_iter; }
  };
}

#endif
