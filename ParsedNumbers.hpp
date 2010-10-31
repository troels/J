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
shared_ptr<JNoun> create_noun(const VecPtr<ParsedNumberBase::Ptr>::type& vec);

shared_ptr<JNoun> create_jarray(j_value_type value_type, VecPtr<ParsedNumberBase::Ptr>::type vec);


}}

#endif
