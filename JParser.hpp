#ifndef JPARSER_HPP
#define JPARSER_HPP

#include <boost/spirit/include/qi.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <boost/fusion/include/vector.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_function.hpp>
#include "JGrammar.hpp"

namespace J { namespace JParser {

using std::vector;
using boost::shared_ptr;
namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;  

class ParserNumberBase { 
  j_value_type value_type;
public:
  ParserNumberBase(j_value_type value_type): value_type(value_type) {}
  virtual ~ParserNumberBase() {}

  j_value_type get_value_type() const { return value_type; }
};

template <typename Number> 
class ParserNumber: public ParserNumberBase {
  Number nr;
public:
  ParserNumber(Number nr): ParserNumberBase(JTypeTrait<Number>::value_type), nr(nr) {}
  
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

struct parse_int_impl { 
  template <typename Arg>
  struct Result { 
    typedef ParserNumber<JInt> result;
  };

  
  template <typename Arg>
  ParserNumber<JInt> operator()(Arg v) {
    return ParserNumber<JInt> (string_to_number<JInt, vector<char>::const_iterator>(v.begin(), v.end())); 
  }
};

struct add_int { 
  template <typename>
  struct result { 
    typedef void type;
  };
  
  add_int(vector<shared_ptr<ParserNumberBase> >& v): 
    output(v) {}

  void operator()(const vector<char>& a) const { 
    output.push_back(shared_ptr<ParserNumberBase>());
  };

private:
  vector<shared_ptr<ParserNumberBase> >& output;
};

template <typename Iterator>
struct parse_number : qi::grammar<Iterator, shared_ptr<ParserNumberBase> > 
{
  parse_number(vector<shared_ptr<ParserNumberBase> >& v): 
    parse_number::base_type(int_number), int_adder(v)
  {
    int_number =
      ('_' >> (+qi::digit) [int_adder(qi::_1)])
      | (+qi::digit) [int_adder(qi::_1)];
  }   

  qi::rule<Iterator, shared_ptr<ParserNumberBase> > number;
  qi::rule<Iterator, shared_ptr<ParserNumber<JFloat> > > float_number;
  qi::rule<Iterator, shared_ptr<ParserNumber<JComplex> > > complex_number;
  qi::rule<Iterator, shared_ptr<ParserNumberBase> > int_number;

  phoenix::function<add_int> int_adder;
};
}}

#endif
