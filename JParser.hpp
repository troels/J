#ifndef JPARSER_HPP
#define JPARSER_HPP

#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <cmath>
#include <vector>

#include "JGrammar.hpp"
#include "ParserCombinators.hpp"

namespace J { namespace JParser { 
using namespace ::ParserCombinators;
using std::vector;
using std::string;
using boost::shared_ptr;

class ParsedNumberBase { 
public:
  typedef shared_ptr<ParsedNumberBase> Ptr;

private:
  j_value_type value_type;

public:
  ParsedNumberBase(j_value_type value_type): value_type(value_type) {}
  virtual ~ParsedNumberBase() {}
  virtual string to_string() const = 0;
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

  string to_string() const { 
    std::ostringstream str;
    str << get_nr();
    return str.str();
  }

  Number get_nr() const { return nr; }
};

  
template <typename Iterator, typename Res>
Res parse_number(Iterator begin, Iterator end, Res base) { 
  Res res(0);
  for(;begin != end; ++begin) { 
    res *= base;
    if (*begin >= '0' && *begin <= '9') {
      res += (*begin - '0');
    } else if (*begin >= 'a' && *begin <= 'z') {
      res += (*begin - 'a') + 10;
    } else if (*begin >= 'A' && *begin <= 'Z') {
      res += (*begin - 'A') + 10;
    } else {
      throw std::logic_error("Expected a number-like character");
    }
  }
  return res;
}

template <typename Iterator>
class IntegerParser: public Parser<Iterator, JInt>  { 
  RegexParser<Iterator> parser; 
public:
  IntegerParser(): parser("(_?)(\\d+)") {}
  JInt parse(Iterator* begin, Iterator end) const { 
    shared_ptr<vector<string> > v(parser.parse(begin, end));
    JInt sign = (((*v)[1]) == "_") ? (-1) : 1;
    return sign * parse_number<string::iterator, JInt>((*v)[2].begin(), (*v)[2].end(), 10);
  }
};

template <typename Iterator>
class IntegerParserWrapper: public Parser<Iterator, ParsedNumberBase::Ptr> {
public:
  typedef shared_ptr<IntegerParserWrapper<Iterator> > Ptr;
  static Ptr Instantiate() { 
    return Ptr(new IntegerParserWrapper<Iterator>());
  }

public:
  IntegerParser<Iterator> parser;
  
public:
  ParsedNumberBase::Ptr parse(Iterator *begin, Iterator end) const {
    return ParsedNumberBase::Ptr(new ParsedNumber<JInt>(parser.parse(begin, end)));
  }
};
    
template <typename Iterator>
class FloatingPointParser: public Parser<Iterator, JFloat> {
  RegexParser<Iterator> parser;
public:
  FloatingPointParser(): parser("(_?)(?=\\d|\\.\\d)(\\d*)(?=\\.\\d|[eE]_?\\d)(?:\\.(\\d*))?(?:[eE](_?)(\\d+))?") {}

  JFloat parse(Iterator *begin, Iterator end) const { 
    Iterator cached_begin = *begin;
    shared_ptr<vector<string> > v(parser.parse(begin, end));
    
    JFloat sign = (*v)[1] == "_" ? -1 : 1;
    JFloat integer_part = parse_number<string::iterator, JFloat>((*v)[2].begin(), (*v)[2].end(), 10.0);
    JFloat float_part = 
      parse_number<string::reverse_iterator, JFloat>((*v)[3].rbegin(), (*v)[3].rend(), 1.0/10.0) * 1.0/10.0;
    int exponent_sign = (*v)[4] == "_" ? -1 : 1;
    int exponent_number = parse_number<string::iterator, int>((*v)[5].begin(),
							      (*v)[5].end(), 10);

    return sign * (integer_part + float_part) * pow(10.0, exponent_number * exponent_sign);
  }
};

template <typename Iterator>
class FloatingPointParserWrapper: public Parser<Iterator, ParsedNumberBase::Ptr> {
public:
  typedef shared_ptr<FloatingPointParserWrapper<Iterator> > Ptr;

  static Ptr Instantiate() { 
    return Ptr(new FloatingPointParserWrapper<Iterator>());
  }

private:
  FloatingPointParser<Iterator> parser;

public:
  ParsedNumberBase::Ptr parse(Iterator *begin, Iterator end) const {
    return ParsedNumberBase::Ptr(new ParsedNumber<JFloat>(parser.parse(begin, end)));
  }
};


template <typename Iterator>
class NumberParser: public Parser<Iterator, ParsedNumberBase::Ptr> {
  typedef ParseOr<Iterator, ParsedNumberBase::Ptr> ParserType;
  typename ParserType::Ptr parser;

public:
  NumberParser(): parser(ParserType::Instantiate()->
			 add_or(FloatingPointParserWrapper<Iterator>::Instantiate())->
			 add_or(IntegerParserWrapper<Iterator>::Instantiate())) {}
  
  ParsedNumberBase::Ptr parse(Iterator* begin, Iterator end) const { 
    return parser->parse(begin, end);
  }
};

// template <typename Iterator>
// class ParseNoun: public Parser<Iterator, JNoun> { 
// };
}}

#endif
