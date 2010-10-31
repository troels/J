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
  bool operator==(const ParsedNumberBase& that) const {
    return get_value_type() == that.get_value_type() && 
      static_cast<const ParsedNumber<Number>& >(that).get_nr() == get_nr();
  }
  
  Ptr convert_to(j_value_type type) const {
    if (type == j_value_type_int) { 
      return Ptr(new ParsedNumber<JInt>(ConvertNumber<Number, JInt>::convert(get_nr())));
    } else if (type == j_value_type_float) { 
      return Ptr(new ParsedNumber<JFloat>(ConvertNumber<Number, JFloat>::convert(get_nr())));
    } else { 
      throw std::logic_error("Invalid type conversion to type.");
    } 
  }

  string to_string() const { 
    std::ostringstream str;
    str << get_nr();
    return str.str();
  }

  Number get_nr() const { return nr; }
};

static const j_value_type j_number_type_hierarchy[] = {
  j_value_type_int,
  j_value_type_float,
  j_value_type_complex
};

static int get_j_value_type_place_in_hierarchy(j_value_type t) { 
  for (unsigned i = 0; i < (sizeof(j_number_type_hierarchy)/sizeof(j_value_type)); ++i) {
    if (t == j_number_type_hierarchy[i]) return i;
  } 
  
  throw std::logic_error("Never end here");
}

static j_value_type highest_j_value(ParsedNumberBase::Ptr a, j_value_type type) {
  int jval_a = get_j_value_type_place_in_hierarchy(a->get_value_type());
  int jval_b = get_j_value_type_place_in_hierarchy(type);
    
  if (jval_a <= jval_b) { 
    return type;
  } else if (jval_a > jval_b) { 
    return a->get_value_type();
  } 
}


template <typename T>
shared_ptr<JNoun> create_noun(const VecPtr<ParsedNumberBase::Ptr>::type& vec) {
  shared_ptr<vector<T> > v(new vector<T>());
  BOOST_FOREACH(ParsedNumberBase::Ptr& p, *vec) { 
    v->push_back(static_cast<ParsedNumber<T>*>(p.get())->get_nr());
  }
  return shared_ptr<JNoun>(new JArray<T>(Dimensions(1, vec->size()), v));
}

shared_ptr<JNoun> create_jarray(j_value_type value_type, VecPtr<ParsedNumberBase::Ptr>::type vec) {
  typename VecPtr<ParsedNumberBase::Ptr>::type v2(new vector<ParsedNumberBase::Ptr>(vec->size()));
  
  vector<ParsedNumberBase::Ptr>::iterator iter = v2->begin();
  BOOST_FOREACH(ParsedNumberBase::Ptr p, *vec) { 
    *iter = p->convert_to(value_type);
    ++iter;
  }

  switch (value_type) { 
  case j_value_type_int:
    return create_noun<JInt>(vec);
  case j_value_type_float:
    return create_noun<JFloat>(vec);
  default:
    throw std::logic_error("Clauses missing");
  }
}


}}

#endif
