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
#include "JTypes.hpp"

namespace J { namespace JParser { 
using std::vector;
using std::string;
using boost::shared_ptr;

template <typename T>
struct VecPtr {
  typedef shared_ptr<vector<T> > type;
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
  template <typename T>
  Ptr convert() const { 
    return convert_to(JTypeTrait<T>::value_type);
  }

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
  
  Ptr convert_to(j_value_type t) const;
  string to_string() const;

  Number get_nr() const { return nr; }
};

template <typename To>
struct ConvertParsedNumberTo { 
  To operator()(ParsedNumberBase::Ptr parsed) const { 
    return static_cast<ParsedNumber<To>&>(*parsed->convert<To>()).get_nr();
  }
};
  
template <typename T>
struct create_noun {
  template <typename Iterator>
  JNoun::Ptr operator()(Iterator begin, Iterator end) {
    int size(distance(begin, end));
    shared_ptr<vector<T> > vec(new vector<T>(size, JTypeTrait<T>::base_elem()));

    transform(begin, end, vec->begin(), ConvertParsedNumberTo<T>());
    return JNoun::Ptr(new JArray<T>(size == 1 ? Dimensions(0) : Dimensions(1, size), vec));
  }
};

template <typename Iterator>
JNoun::Ptr create_jarray(j_value_type value_type, Iterator begin, Iterator end) {
  return JTypeDispatcher<create_noun, JNoun::Ptr>()(value_type, begin, end);
}

template <typename From>
struct ParsedNumberConverter  {
  template <typename To>
  struct Impl {
    ParsedNumberBase::Ptr operator()(From from) const {
      return ParsedNumberBase::Ptr(new ParsedNumber<To>(ConvertType<From, To>()(from)));
    }
  };
};

}}

#endif
