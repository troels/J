#ifndef JPARSER_HPP
#define JPARSER_HPP

#include <boost/spirit/include/qi.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/boost_tuple.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include "JGrammar.hpp"
#include <string>
#include <sstream>
#include <iostream>
#include <cmath>
#include <vector>

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



// namespace boost { namespace spirit { namespace traits { 
// using boost::tuple;

// typedef boost::shared_ptr<J::JParser::ParserNumberBase > NumberPtr;
// typedef tuple<optional<unused_type>, std::vector<char> > ParserResultType;

// template <typename T> 
// struct transform_attribute<T, NumberPtr> {
//   typedef NumberPtr type;

//   static type pre(T& v) { return NumberPtr(new J::JParser::ParserNumber<J::JInt>(10)); }
//   static void post(T&, const NumberPtr&) {}
//   static void fail(NumberPtr&) {}
// };

// }}}


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
using qi::char_;
using qi::_1;
using qi::_2;
using qi::_3;
using qi::_val;
using qi::_a;
using qi::_b;
using qi::_c;
using qi::eps;
using phoenix::val;

struct if_impl { 
  template <typename Conditional, typename Arg1, typename Arg2> 
  struct result { 
    typedef Arg1 type;
  };

  template <typename Conditional, typename Arg1, typename Arg2>
  Arg1 operator()(const Conditional& cond, Arg1 a, Arg2 b) const {
    return cond ? a : b;
  }
};

struct my_pow {
  template <typename Arg1, typename Arg2>
  struct result { 
    typedef Arg1 type;
  };

  template <typename Arg1, typename Arg2>
  Arg1 operator()(Arg1 a, Arg2 b) const { 
    return std::pow(a, b);
  }
};
    

template <typename Iterator>
struct parse_number : qi::grammar<Iterator, NumberPtr() > 
{

  parse_number(): parse_number::base_type(integer_type) 
  {
    integer_type = 
      (-char_('_') >> (+digit)) 
      [ _val = pack_number(either_or(_1, -1, 1) * parse_int(_2, 10)) ] ;

    exponent_type = (char_("eE") >> -char_('_') >> (+digit)) 
      [ _val = phx_pow(10.0, either_or(_2, -1, 1) * parse_int(_3, 10)) ];
      
    float_type = 
      ((((-char_('_') >> (*digit) >> lit('.') >> (+digit)) 
	[ 
	 _a = either_or(_1, -1.0, 1.0) * (parse_float(_2, 10.0) + parse_float(_3, 1.0/10.0)) 
	]) |
       (-char_('_') >> (+digit) >> lit('.') >> (*digit))
       [ 
	_a = either_or(_1, -1.0, 1.0) * (parse_float(_2, 10.0) + parse_float(_3, 1.0/10.0)) 
       ]) >> 
       (exponent_type [ _val = pack_number(_1 * _a) ] | eps [ _val = pack_number(_a) ])) | 
      (
       (-char_('_') >> (+digit) >> exponent_type) 
       [ 
        _val = pack_number(either_or(_1, -1, 1) * parse_float(_2, 10.0) * _3)
       ]);
	


    // float_type = 
    //   (-char_('_') >> (*digit) 
    //    >> lit(".") >> (+digit) 
    //    >> 
    //    (*digit) [ _a = 1.0 ] 
    //    [ _b = parse_float(_1, 10.0) ]) >> '.' >> 
    //   (*digit) [ _c = parse_float(_1, 1.0/10.0) ] 
    //   [ _val = pack_number(_a * (_b + _c)) ];
  }

  qi::rule< Iterator, JFloat() > exponent_type;
  qi::rule< Iterator, NumberPtr() > integer_type;
  qi::rule< Iterator, NumberPtr(), qi::locals<JFloat> > float_type;


  phoenix::function<parse_num_impl<JInt> > parse_int;
  phoenix::function<parse_num_impl<JFloat> > parse_float;
  phoenix::function<pack_number_impl> pack_number;
  phoenix::function<if_impl> either_or;
  phoenix::function<my_pow> phx_pow;
  // phoenix::function<parse_num_impl<JInt> > parse_int;
  // phoenix::function<negate<JInt> > negate_int;
  // phoenix::function<parse_float_impl<JFloat> > parse_float;
};
}}

#endif
