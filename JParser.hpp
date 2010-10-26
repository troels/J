#ifndef JPARSER_HPP
#define JPARSER_HPP

#include <boost/spirit/include/qi.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <boost/fusion/include/vector.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include "JGrammar.hpp"
#include <string>
#include <sstream>
#include <iostream>
#include <cmath>


namespace J { namespace JParser { 

class ParserNumberBase;
template <typename T> class ParserNumber;
}}

namespace boost { namespace spirit { namespace traits { 
using boost::shared_ptr;
using J::JParser::ParserNumberBase;
using J::JParser::ParserNumber;

typedef shared_ptr<ParserNumberBase> NumberPtr;

template <typename T> 
struct transform_attribute<T&, NumberPtr > {
  typedef NumberPtr type;
  static NumberPtr pre(T& v) { return NumberPtr(new ParserNumber<T>(v)); }
  static void post(T&, const NumberPtr&) {}
  static void fail(NumberPtr&) {}
};

template <> 
struct transform_attribute<NumberPtr&, NumberPtr > {
  typedef NumberPtr type;
  static NumberPtr pre(NumberPtr& v) { return v; }
  static void post(NumberPtr&, const NumberPtr&) {}
  static void fail(NumberPtr&) {}
};

}}}

namespace J { namespace JParser {

using std::vector;
using std::string;
using boost::shared_ptr;
namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;  
namespace ascii = boost::spirit::ascii;

class ParserNumberBase { 
  j_value_type value_type;
public:
  ParserNumberBase(j_value_type value_type): value_type(value_type) {}
  virtual ~ParserNumberBase() {}
  virtual string to_string() const = 0;
  
  j_value_type get_value_type() const { return value_type; }
  friend std::ostream& operator<<(std::ostream& os, const ParserNumberBase& p);
};

template <typename Number> 
class ParserNumber: public ParserNumberBase {
  Number nr;
public:
  ParserNumber(Number nr): ParserNumberBase(JTypeTrait<Number>::value_type), nr(nr) {}

  string to_string() const { 
    std::ostringstream str;
    str << get_nr();
    return str.str();
  }

  Number get_nr() const { return nr; }
};

template <typename T>
T char2digit(char c) {
  assert ((c >= '0' && c <= '9') ||
	  (c >= 'a' && c <= 'z') ||
	  (c >= 'A' && c <= 'Z'));

  if (c >= '0' && c <= '9') {
    return c - '0';
  } else if (c >= 'a' && c <= 'z') {
    return c - 'a';
  } else if (c >= 'A' && c <= 'Z') {
    return c - 'A';
  } else {
    assert(0);
  }
}

template <typename NumberType, typename Iterator>
NumberType string_to_number(Iterator begin, Iterator end, NumberType base = 10) {
  NumberType res(0);
  for(;begin != end; ++begin) {
    res *= base;
    res += char2digit<NumberType>(*begin);
  }

  return res;
}

template <typename NumberType> 
struct parse_num_impl { 
  template <typename Arg, typename Base>
  struct result { 
    typedef NumberType type;
  };

  template <typename Arg, typename Base>
  NumberType operator()(const Arg& v, Base b = 10) const { 
    return string_to_number<NumberType, typename Arg::const_iterator>(v.begin(), v.end(), b);
  }
};

struct pack_number_impl { 
  template <typename Arg>
  struct result { 
    typedef shared_ptr<ParserNumberBase> type;
  };

  template <typename Arg>
  shared_ptr<ParserNumberBase> operator()(const Arg& arg) const { 
    return shared_ptr<ParserNumberBase>(new ParserNumber<Arg>(arg));
  }
};

template <typename Iterator>
struct white_space : qi::grammar<Iterator>
{
  white_space() : white_space::base_type(start) {
    start = qi::space;
  }
  
  qi::rule<Iterator> start;
};

typedef shared_ptr<ParserNumberBase> NumberPtr;

using qi::lit;
using qi::digit;
using qi::_1;
using qi::_val;
using phoenix::val;
using qi::_a;
using qi::_b;
using qi::_c;

template <typename Iterator>
struct parse_number : qi::grammar<Iterator, NumberPtr() > 
{

  parse_number(): parse_number::base_type(integer_type) 
  {
    integer_type = 
      (lit('_') >> (+digit) [ _val = pack_number(val(-1) * parse_int(_1, 10)) ])
      | (+digit) [ _val = pack_number(parse_int(_1, 10)) ];
    
    float_type = 
      ((lit('_') [ _a = val(-1.0) ] >> 
	(*digit) [ _b = parse_float(_1, 10.0) ]) |
       
       (*digit) [ _a = val(1.0) ] 
       [ _b = parse_float(_1, 10.0) ]) >> '.' >> 
      (*digit) [ _c = parse_float(_1, 1.0/10.0) ] 
      [ _val = pack_number(_a * (_b + _c)) ];
  }

  qi::rule< Iterator, NumberPtr() > integer_type;
			   qi::rule< Iterator, NumberPtr(), qi::locals<JFloat, JFloat, JFloat, JInt> > float_type;

  phoenix::function<parse_num_impl<JInt> > parse_int;
  phoenix::function<parse_num_impl<JFloat> > parse_float;
  phoenix::function<pack_number_impl> pack_number;
  
  // phoenix::function<parse_num_impl<JInt> > parse_int;
  // phoenix::function<negate<JInt> > negate_int;
  // phoenix::function<parse_float_impl<JFloat> > parse_float;
};
}}

#endif
