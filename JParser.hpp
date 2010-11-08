#ifndef JPARSER_HPP
#define JPARSER_HPP


#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <cmath>
#include <functional>
#include <algorithm>

#include "JAST.hpp"
#include "ParsedNumbers.hpp"
#include "JGrammar.hpp"
#include "ParserCombinators.hpp"
#include "utils.hpp"

namespace J { namespace JParser { 
using namespace ::ParserCombinators;
using namespace ::J::JAST;
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
  static typename Parser<Iterator, ParsedNumberBase::Ptr>::Ptr Instantiate() {
    return typename Parser<Iterator, ParsedNumberBase::Ptr>::Ptr(new NumberParser<Iterator>());
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
  InterspersedParser1<Iterator, ParsedNumberBase::Ptr, void> parser;

public:
  ParseNoun(): parser(NumberParser<Iterator>::Instantiate(), 
		      WhitespaceParser<Iterator>::Instantiate()) {}
  
  JNoun::Ptr parse(Iterator* begin, Iterator end) const { 
    typename VecPtr<ParsedNumberBase::Ptr>::type v(parser.parse(begin, end));
    assert(v->size() > 0);
    typename vector<ParsedNumberBase::Ptr>::iterator iter = v->begin();
    
    j_value_type highest_j_value_type = (*iter)->get_value_type();
    ++iter;

    for(;iter != v->end(); ++iter) { 
      highest_j_value_type = highest_j_value(*iter, highest_j_value_type);
    }

    return create_jarray(highest_j_value_type, v);
  }
};

template <typename Iterator>
class NounParser: public Parser<Iterator, JASTBase::Ptr> {
public:
  typedef typename Parser<Iterator, JASTBase::Ptr>::Ptr Ptr;

  static Ptr Instantiate() { 
    return Ptr(new NounParser());
  }

private:  
  ParseNoun<Iterator> parser;

public:
  NounParser(): parser() {}
  
  JASTBase::Ptr parse(Iterator* begin, Iterator end) const { 
    return JASTBase::Ptr(new JASTNoun(parser.parse(begin, end)));
  }
};

template <typename Iterator>
class BuiltinParser: public Parser<Iterator, JASTBase::Ptr> { 
public:
  typedef typename Parser<Iterator, JASTBase::Ptr>::Ptr Ptr;
  
  template <typename T>
  static Ptr Instantiate(T begin, T end) { 
    return Ptr(new BuiltinParser(begin, end));
  }

private:
  shared_ptr<RegexParser<Iterator> > parser;

public:
  template <typename T>
  BuiltinParser(T begin, T end): parser() {
    vector<string> v(distance(begin, end));
    transform(begin, end, v.begin(), ptr_fun(&J::escape_regex));
    string s(join_str(v.begin(), v.end(), "|"));
    parser = shared_ptr<RegexParser<Iterator> >(new RegexParser<Iterator>(join_str(v.begin(), v.end(), "|")));
  }

  JASTBase::Ptr parse(Iterator* begin, Iterator end) const { 
    shared_ptr<vector<string> > s(parser->parse(begin, end));
    
    assert(s->size() > 0);
    
    return JASTBase::Ptr(new JASTBuiltin(s->at(0)));
  }
};

template <typename Iterator>
class UserDefinedParser: public Parser<Iterator, JASTBase::Ptr> {
  RegexParser<Iterator> parser;
  
public:
  typedef typename Parser<Iterator, JASTBase::Ptr>::Ptr Ptr;
  
  UserDefinedParser(): parser("\\w[\\w_\\d]*") {}
  
  JASTBase::Ptr parse(Iterator* begin, Iterator end) const { 
    shared_ptr<vector<string> > s(parser.parse(begin, end));
    assert(s->size() > 0);
    return JASTBase::Ptr(new JASTUserDefined(s->at(0)));
  }

  static Ptr Instantiate() { 
    return Ptr(new UserDefinedParser());
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
				     typename RegexParser<Iterator>::return_type, 
				     typename RegexParser<Iterator>::return_type >::Instantiate
    (RegexParser<Iterator>::Instantiate("\\s*\\(\\s*"),
     RegexParser<Iterator>::Instantiate("\\s*\\)\\s*"),
     ptr)) {}
  
  Res parse(Iterator* begin, Iterator end) const { 
    return parser->parse(begin, end);
  }
};
  

template <typename Iterator>
class SequenceParser: public Parser<Iterator, JASTBase::Ptr> {
public:
  typedef typename Parser<Iterator, JASTBase::Ptr>::Ptr Ptr;

private:
  typedef typename InterspersedParser1<Iterator, JASTBase::Ptr>::return_type parser_return_type;
  typedef typename Parser<Iterator, parser_return_type >::Ptr parser_ptr;
  parser_ptr parser;

  SequenceParser() {}

public:

  template <typename T>
  static Ptr Instantiate(T begin, T end) { 
    Ptr p(new SequenceParser<Iterator>());
    typename Parser<Iterator, JASTBase::Ptr>::Ptr rp
      (RecursiveParser<Iterator, JASTBase::Ptr>::Instantiate(p));
    typename Parser<Iterator, JASTBase::Ptr>::Ptr pep
      (ParenthesizedExpressionParser<Iterator, JASTBase::Ptr>::Instantiate(rp));

    SequenceParser<Iterator>* inst(static_cast<SequenceParser<Iterator>*>(p.get()));
    inst->parser = parser_ptr(new InterspersedParser1<Iterator, JASTBase::Ptr>
		       (ParseOr<Iterator, JASTBase::Ptr>::Instantiate()
			 ->add_or(pep)
			 ->add_or(BuiltinParser<Iterator>::Instantiate(begin, end))
			 ->add_or(NounParser<Iterator>::Instantiate())
			 ->add_or(UserDefinedParser<Iterator>::Instantiate()),
			WhitespaceParser<Iterator>::Instantiate()));
    return p;
  }
  
  JASTBase::Ptr parse(Iterator* begin, Iterator end) const {
    shared_ptr<vector<JASTBase::Ptr> > v(parser->parse(begin, end));
    
    return JASTSequence::Instantiate(v->begin(), v->end());
  }
};
}}

#endif
