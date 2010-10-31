#ifndef JPARSER_HPP
#define JPARSER_HPP


#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <cmath>


#include "ParsedNumbers.hpp"
#include "JGrammar.hpp"
#include "ParserCombinators.hpp"

namespace J { namespace JParser { 
using namespace ::ParserCombinators;
using std::vector;
using std::string;
using boost::shared_ptr;

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
  typedef shared_ptr<NumberParser<Iterator> > Ptr;
  static Ptr Instantiate() {
    return Ptr(new NumberParser<Iterator>());
  }

  NumberParser(): parser(ParserType::Instantiate()->
			 add_or(FloatingPointParserWrapper<Iterator>::Instantiate())->
			 add_or(IntegerParserWrapper<Iterator>::Instantiate())) {}
  
  ParsedNumberBase::Ptr parse(Iterator* begin, Iterator end) const { 
    return parser->parse(begin, end);
  }
};


template <typename Iterator>
class ParseNoun: public Parser<Iterator, JNoun::Ptr > { 
  InterspersedParser1<Iterator, vector<ParsedNumberBase::Ptr> > parser;

public:
  ParseNoun(): parser(NumberParser<Iterator>::Instantiate(), 
		      WhitespaceParser<Iterator>::Instantiate) {}
  
  JNoun::Ptr parse(Iterator* begin, Iterator end) { 
    typename VecPtr<ParsedNumberBase::Ptr>::type v(parser.parse(begin, end));
    assert(v->size() > 0);
    typename vector<ParsedNumberBase::Ptr>::iterator iter = v->begin();
    
    int highest_j_value_type = begin->get_value_type();
    ++begin;

    for(;begin != end; ++begin) { 
      highest_j_value_type = highest_j_value(*begin, highest_j_value_type);
    }

    return create_jarray(highest_j_value_type, v);
  }
};
}}

#endif
