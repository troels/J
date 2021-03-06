#ifndef JPARSER_HPP
#define JPARSER_HPP


#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <cmath>
#include <functional>
#include <algorithm>
#include <cctype>

#include "JToken.hpp"
#include "ParsedNumbers.hpp"
#include "JGrammar.hpp"
#include "ParserCombinators.hpp"
#include "Aggregates.hpp"
#include "utils.hpp"

namespace J { namespace JParser { 
using namespace ::ParserCombinators;
using namespace ::J::JTokens;
using std::vector;
using std::string;
using boost::shared_ptr;
using boost::make_shared;
using boost::static_pointer_cast;

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
  typedef typename Parser<Iterator, ParsedNumberBase::Ptr>::Ptr Ptr; 
  static  Ptr Instantiate() {
    return static_pointer_cast<Parser<Iterator, ParsedNumberBase::Ptr> >(make_shared<NumberParser<Iterator> >());
  }

  NumberParser(): parser(ParserType::Instantiate()->
			 add_or(FloatingPointParserWrapper<Iterator>::Instantiate())->
			 add_or(IntegerParserWrapper<Iterator>::Instantiate())) {}
  
  ParsedNumberBase::Ptr parse(Iterator* begin, Iterator end) const { 
    return parser->parse(begin, end);
  }
};

template <typename Iterator>
class ComplexNumberParser: public Parser<Iterator, JComplex> {
  typename NumberParser<Iterator>::Ptr number_parser;
  typename ParseConstant<Iterator>::Ptr constant_parser;

public:
  typedef typename Parser<Iterator, JComplex>::Ptr Ptr;

  static Ptr Instantiate() {
    return static_pointer_cast<ComplexNumberParser<Iterator> >(make_shared<ComplexNumberParser<Iterator> >());
  }

  ComplexNumberParser(): number_parser(NumberParser<Iterator>::Instantiate()), 
			 constant_parser(ParseConstant<Iterator>::Instantiate("j")) {}
  
  JComplex parse(Iterator* begin, Iterator end) const { 
    ParsedNumberBase::Ptr real_part(number_parser->parse(begin, end));
    constant_parser->parse(begin, end);
    ParsedNumberBase::Ptr imaginary_part(number_parser->parse(begin, end));
      
    JFloat real_part_float = static_cast<ParsedNumber<JFloat>&>(*real_part->convert<JFloat>()).get_nr();
    JFloat imaginary_part_float = static_cast<ParsedNumber<JFloat>&>(*imaginary_part->convert<JFloat>()).get_nr();
    
    return JComplex(real_part_float, imaginary_part_float);
  }
};

template <typename Iterator>
class ComplexNumberWrapper: public Parser<Iterator, ParsedNumberBase::Ptr> {
  typename ComplexNumberParser<Iterator>::Ptr parser;
public:
  typedef Parser<Iterator, ParsedNumberBase::Ptr> BaseType;
  static typename BaseType::Ptr Instantiate() {
    return static_pointer_cast<BaseType>(make_shared<ComplexNumberWrapper<Iterator> >());
  }

  ComplexNumberWrapper(): parser(ComplexNumberParser<Iterator>::Instantiate()) {}
  
  ParsedNumberBase::Ptr parse(Iterator* begin, Iterator end) const { 
    return static_pointer_cast<ParsedNumberBase>(make_shared<ParsedNumber<JComplex> >(parser->parse(begin, end)));
  }
};

template <typename Iterator>
class NounPartParser: public Parser<Iterator, ParsedNumberBase::Ptr> {
  typedef ParseOr<Iterator, ParsedNumberBase::Ptr> ParserType;
  typename ParserType::Ptr parser;
  
public:
  typedef typename Parser<Iterator, ParsedNumberBase::Ptr>::Ptr  Ptr;
  static Ptr Instantiate() {
    return make_shared<NounPartParser<Iterator> >();
  }

  NounPartParser(): 
    parser (ParserType::Instantiate()
	    ->add_or(ComplexNumberWrapper<Iterator>::Instantiate())
	    ->add_or(NumberParser<Iterator>::Instantiate())) {}

  ParsedNumberBase::Ptr parse(Iterator* begin, Iterator end) const { 
    return parser->parse(begin, end);
  }
};

template <typename Iterator>
class ParseNoun: public Parser<Iterator, JNoun::Ptr > { 
  typedef InterspersedParser1<Iterator, ParsedNumberBase::Ptr, void> parser_type;
  parser_type parser;
  
public:
  ParseNoun(): parser(NounPartParser<Iterator>::Instantiate(), 
		      WhitespaceParser<Iterator>::Instantiate()) {}
  
  JNoun::Ptr parse(Iterator* begin, Iterator end) const { 
    typename parser_type::result_type v(parser.parse(begin, end));
    assert(v->size() > 0);
    
    typedef J::Aggregates::get_value_type<typename parser_type::result_type::element_type::iterator> 
      value_type_iterator;
    typename value_type_iterator::result_type value_type_iter(value_type_iterator()(v->begin(), v->end()));
    
    optional<j_value_type> best_type(J::Aggregates::find_common_type(value_type_iter.first, value_type_iter.second));
    
    if (!best_type) 
      throw JParserException("Invalid combination of types");

    return create_jarray(*best_type, v->begin(), v->end());
  }
};

template <typename Iterator>
class NounParser: public Parser<Iterator, JTokenBase::Ptr> {
  ParseNoun<Iterator> parser;
  
public:
  typedef typename Parser<Iterator, JTokenBase::Ptr>::Ptr Ptr;

  static Ptr Instantiate() { 
    return static_pointer_cast<Parser<Iterator, JTokenBase::Ptr> >(make_shared<NounParser<Iterator> >());
  }
  
public:
  NounParser(): parser() {}
  
  JTokenBase::Ptr parse(Iterator* begin, Iterator end) const { 
    return JTokenBase::Ptr(new JTokenWord<JNoun>(parser.parse(begin, end)));
  }
};

template <typename Iterator>
class OperatorParser: public Parser<Iterator, JTokenBase::Ptr> { 
public:
  typedef typename Parser<Iterator, JTokenBase::Ptr>::Ptr Ptr;
  
  template <typename T>
  static Ptr Instantiate(T begin, T end) { 
    return Ptr(new OperatorParser(begin, end));
  }

private:
  shared_ptr<RegexParser<Iterator> > parser;

public:
  template <typename T>
  OperatorParser(T begin, T end): parser() {
    vector<string> v(distance(begin, end));
    transform(begin, end, v.begin(), ptr_fun(&J::escape_regex));
    string s(join_str(v.begin(), v.end(), "|"));
    parser = shared_ptr<RegexParser<Iterator> >(new RegexParser<Iterator>(join_str(v.begin(), v.end(), "|")));
  }

  JTokenBase::Ptr parse(Iterator* begin, Iterator end) const { 
    shared_ptr<vector<string> > s(parser->parse(begin, end));
    
    assert(s->size() > 0);
    
    return JTokenBase::Ptr(new JTokenOperator(s->at(0)));
  }
};

template <typename Iterator>
class NameParser: public Parser<Iterator, JTokenBase::Ptr> {
  RegexParser<Iterator> parser;
  
public:
  typedef typename Parser<Iterator, JTokenBase::Ptr>::Ptr Ptr;
  
  NameParser(): parser("\\w[\\w_\\d]*") {}
  
  JTokenBase::Ptr parse(Iterator* begin, Iterator end) const { 
    shared_ptr<vector<string> > s(parser.parse(begin, end));
    assert(s->size() > 0);
    return JTokenBase::Ptr(new JTokenName(s->at(0)));
  }

  static Ptr Instantiate() { 
    return Ptr(new NameParser());
  }
};

template <typename Iterator, typename Res>
class ParenthesizedExpressionParser: public Parser<Iterator, Res> {
public:
  typedef typename Parser<Iterator, Res>::Ptr Ptr;

private:
  Ptr parser;
  
public:
  static Ptr Instantiate(Ptr ptr) {
    return Ptr(new ParenthesizedExpressionParser(ptr));
  }

  ParenthesizedExpressionParser(Ptr ptr): 
    parser(DelimitedExpressionParser<Iterator, Res, 
				     typename RegexParser<Iterator>::result_type, 
				     typename RegexParser<Iterator>::result_type >::Instantiate
    (RegexParser<Iterator>::Instantiate("\\s*\\(\\s*"),
     RegexParser<Iterator>::Instantiate("\\s*\\)\\s*"),
     ptr)) {}
  
  Res parse(Iterator* begin, Iterator end) const { 
    return parser->parse(begin, end);
  }
};
  

template <typename Iterator>
class JTokenizer: public Parser<Iterator, 
				typename InterspersedParser1<Iterator, JTokenBase::Ptr>::result_type>  {
  typedef InterspersedParser1<Iterator, JTokenBase::Ptr> parser_type;

public:
  typedef typename parser_type::result_type result_type;
  typedef typename Parser<Iterator, result_type>::Ptr Ptr;

private:
  Ptr parser;

public:
  template <typename T>
  JTokenizer(T begin, T end): parser(new parser_type
				     (ParseOr<Iterator, JTokenBase::Ptr>::Instantiate()
				      ->add_or(ParseConstant<Iterator>::Instantiate("(") >>
					       ConstantParser<Iterator, JTokenBase::Ptr>::Instantiate
					       (JTokenLParen::Instantiate()))
				      ->add_or(ParseConstant<Iterator>::Instantiate(")") >>
					       ConstantParser<Iterator, JTokenBase::Ptr>::Instantiate
					       (JTokenRParen::Instantiate()))
				      ->add_or(ParseConstant<Iterator>::Instantiate("=:") >>
					       ConstantParser<Iterator, JTokenBase::Ptr>::Instantiate
					       (JTokenAssignment::Instantiate("=:")))
				      ->add_or(ParseConstant<Iterator>::Instantiate("=.") >>
					       ConstantParser<Iterator, JTokenBase::Ptr>::Instantiate
					       (JTokenAssignment::Instantiate("=.")))
				      ->add_or(ParseConstant<Iterator>::Instantiate("[:") >>
					       ConstantParser<Iterator, JTokenBase::Ptr>::Instantiate
					       (JTokenCap::Instantiate()))
				      ->add_or(OperatorParser<Iterator>::Instantiate(begin, end))
				      ->add_or(NounParser<Iterator>::Instantiate())
				      ->add_or(NameParser<Iterator>::Instantiate()),
				      WhitespaceParser<Iterator>::Instantiate())) {}

  template <typename T>
  static Ptr Instantiate(T begin, T end) { 
    return Ptr(new JTokenizer(begin, end));
  }
  
  result_type parse(Iterator* begin, Iterator end) const {
    result_type res(parser->parse(begin, end));
    res->push_front(JTokenStart::Instantiate());
    return res;
  }
};

}}

#endif
