#ifndef PARSED_NUMBERS_HPP
#define PARSED_NUMBERS_HPP

#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <vector>
#include <string>
#include <stdexcept>
#include <functional>
#include <algorithm>
#include "JGrammar.hpp"
#include "JNoun.hpp"

namespace J { namespace JParser { 
using std::vector;
using std::string;
using boost::shared_ptr;

template <typename T>
struct VecPtr {
  typedef shared_ptr<vector<T> > type;
};

template <typename From, typename To> 
struct ConvertNumber { 
  static To convert(From from) { 
    return static_cast<To>(from);
  }
};

class ParsedNumberBase { 
public:
  typedef shared_ptr<ParsedNumberBase> Ptr;

private:
  j_value_type value_type;

public:
  ParsedNumberBase(j_value_type value_type): value_type(value_type) {}
  virtual ~ParsedNumberBase() {}
  virtual string to_string() const = 0;
  virtual Ptr convert_to(j_value_type type) const = 0;
  virtual bool operator==(const ParsedNumberBase& that) const = 0;

  j_value_type get_value_type() const { return value_type; }
  friend std::ostream& operator<<(std::ostream& os, const ParsedNumberBase& p);
};


template <typename Number> 
class ParsedNumber: public ParsedNumberBase {
  Number nr;
public:
  ParsedNumber(Number nr): ParsedNumberBase(JTypeTrait<Number>::value_type), nr(nr) {}
  bool operator==(const ParsedNumberBase& that) const;
  
  Ptr convert_to(j_value_type type) const;

  string to_string() const;

  Number get_nr() const { return nr; }
};

static const j_value_type j_number_type_hierarchy[] = {
  j_value_type_int,
  j_value_type_float,
  j_value_type_complex
};

int get_j_value_type_place_in_hierarchy(j_value_type t);
j_value_type highest_j_value(ParsedNumberBase::Ptr a, j_value_type type);

template <typename T>
struct create_noun {
  template <typename Iterator>
  static JNoun::Ptr apply(Iterator begin, Iterator end) {
    shared_ptr<vector<T> > v(new vector<T>(distance(begin, end)));
    typename vector<T>::iterator out_iter(v->begin());
    
    for (Iterator in_iter(begin); in_iter != end; ++in_iter, ++out_iter) {
      *out_iter = static_cast<ParsedNumber<T>*>(in_iter->get())->get_nr();
    }
    
    return JNoun::Ptr(new JArray<T>(Dimensions(1, v->size()), v));
  }
};

template <typename Iterator>
shared_ptr<JNoun> create_jarray(j_value_type value_type, Iterator begin, Iterator end) {
  typename VecPtr<ParsedNumberBase::Ptr>::type v2(new vector<ParsedNumberBase::Ptr>(distance(begin, end)));
  vector<ParsedNumberBase::Ptr>::iterator iter = v2->begin();

  for (Iterator arg_iter = begin; arg_iter != end; ++arg_iter) {
    *iter = (*arg_iter)->convert_to(value_type);
    ++iter;
  }

  switch (value_type) { 
  case j_value_type_int:
    return create_noun<JInt>::apply(v2->begin(), v2->end());
  case j_value_type_float:
    return create_noun<JFloat>::apply(v2->begin(), v2->end());
  default:
    throw std::logic_error("Clauses missing");
  }
}

}}

#endif
