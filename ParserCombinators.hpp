#ifndef PARSERCOMBINATORS_HPP
#define PARSERCOMBINATORS_HPP

#include <iostream>
#include <string>
#include <boost/regex.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <algorithm>
#include <functional>
#include <cassert>
#include <utility>

namespace ParserCombinators { 
using std::pair;
using std::string;
using std::vector;
using boost::regex;
using boost::optional;
using boost::shared_ptr;

class ParserFailure {
  string msg;
public:
  virtual ~ParserFailure() {}
  ParserFailure(const string& msg): msg(msg) {}
  ParserFailure(): msg("Unknown error") {}

  string get_msg() const { return msg; }
};

class MatchFailure: public ParserFailure {
public:
  MatchFailure(const string& msg): ParserFailure(msg) {}
};

class EndOfBufferFailure: public ParserFailure { 
public:
  EndOfBufferFailure(const string& msg): ParserFailure(msg) {}
};

template <typename Iterator>
void end_of_buffer_p(Iterator* begin, Iterator end) {
  if (*begin == end) throw  EndOfBufferFailure("Unexpected end of buffer");
}

typedef shared_ptr<vector<string> > StringVecPtr;

template <typename Iterator, typename Res>
class Parser { 
public:
  typedef shared_ptr<Parser<Iterator, Res> > Ptr;

  virtual ~Parser() {}
  virtual Res parse(Iterator* begin, Iterator end) const = 0;
};
  
template <typename Iterator>
class RegexParser: public Parser<Iterator, StringVecPtr> { 
  string str;
  regex our_regex;

public:
  RegexParser(const string& regex): str(regex), our_regex(boost::regex(regex)) {}
  StringVecPtr parse(Iterator* begin, Iterator end) const { 
    boost::match_results<Iterator> m;
    if (!regex_search(*begin, end, m, our_regex, boost::regex_constants::match_continuous)) {
      throw MatchFailure("Regex: " + str);
    }
    
    StringVecPtr res = StringVecPtr(new vector<string>(m.size()));
    for (size_t i = 0; i < m.size(); ++i) { 
      (*res)[i] = m.str(i);
    }
    
    advance(*begin, m.length());
    return res;
  }
};    

template <typename Iterator>
class ParseConstant: public Parser<Iterator, void> { 
  string str;
public:
  ParseConstant(const string& str): str(str) {}
  void parse(Iterator* begin, Iterator end) const { 
    if (distance(*begin, end) < distance(str.begin(), str.end()) ||
	!equal(str.begin(), str.end(), *begin)) {
      throw MatchFailure("Expected: " + str);
    } 
    
    advance(*begin, distance(str.begin(), str.end()));
  }
};

using boost::enable_shared_from_this;

template <typename ParserType> 
class ParserList: public enable_shared_from_this<ParserList<ParserType> > { 
public:
  typedef shared_ptr<ParserType> ParserTypePtr;
  typedef shared_ptr<ParserList<ParserType> > Ptr;
  
private:
  ParserTypePtr parser;
  Ptr next;

public:
  ParserList(ParserTypePtr parser): parser(parser), next() {} 
  ParserList(ParserTypePtr parser, Ptr pl): parser(parser), next(pl) {} 

  Ptr get_shared() { 
    return enable_shared_from_this<ParserList<ParserType> >::shared_from_this(); 
  }
  Ptr get_next() const { return next; }
  ParserTypePtr get_parser() const { return parser; }
  Ptr cons(ParserTypePtr pt) { 
    return Ptr(new ParserList<ParserType>(pt, get_shared()));
  }
  
  Ptr reverse() const { 
    Ptr lst = Ptr(new ParserList<ParserType>(parser));
    Ptr next = this->next;
    while(next) {
      lst = lst->cons(next->get_parser());
      next = next->get_next();
    }
    return lst;
  }
};

template <typename Iterator, typename Res>
class ParseOr: public Parser<Iterator, Res> { 
public:
  typedef shared_ptr< ParseOr<Iterator, Res> > Ptr;
  typedef Parser<Iterator, Res> OurParser;
  typedef shared_ptr<OurParser> OurParserPtr;
  typedef typename ParserList<OurParser>::Ptr ParserListPtr;

  static Ptr Instantiate() {
    return Ptr(new ParseOr<Iterator, Res>());
  }

private:
  mutable ParserListPtr reverse_list;
  ParserListPtr parser_list;

  ParseOr(ParserListPtr parser_list): parser_list(parser_list) {} 

  ParserListPtr get_reverse_list() const {
    if (!parser_list) return parser_list;

    if (!reverse_list) { 
      reverse_list = parser_list->reverse();
    } 
    
    return reverse_list;
  }

public:
  ParseOr(): reverse_list(), parser_list() {} 
  ParseOr(OurParserPtr parser): reverse_list(), parser_list(new ParserList<OurParser>(parser)) {}
  
  Res parse(Iterator *begin, Iterator end) const {
    assert(*begin != end);
    Iterator cached_begin = *begin;
    
    ParserListPtr ptr = get_reverse_list();
    while(ptr) { 
      try {
	return ptr->get_parser()->parse(begin, end);
      } catch (MatchFailure& m) {
	*begin = cached_begin;
	ptr = ptr->get_next();
      } 
    } 
    
    throw MatchFailure("ParseOr failed");
  }

  Ptr add_or(OurParserPtr parser) const {
    if (parser_list) 
      return Ptr(new ParseOr<Iterator, Res>(parser_list->cons(parser)));
    return Ptr(new ParseOr<Iterator, Res>(parser));
  }
};

template <typename Iterator, typename Res, typename InterRes = void>
class InterspersedParser: public Parser<Iterator, shared_ptr<vector<Res> > > { 
  typename Parser<Iterator, Res>::Ptr parser;
  typename Parser<Iterator, InterRes>::Ptr inter_parser;

public:
  InterspersedParser(typename Parser<Iterator, Res>::Ptr parser, 
		     typename Parser<Iterator, InterRes>::Ptr inter_parser): 
    parser(parser), inter_parser(inter_parser) {}

  shared_ptr<vector<Res> > parse(Iterator* begin, Iterator end) const {
    shared_ptr<vector<Res> > output(new vector<Res>());
    Iterator cached_begin = *begin;
    try { 
      output->push_back(parser->parse(begin, end));
    } catch (MatchFailure& f) {
      *begin = cached_begin;
      return output;
    }
    
    while (*begin != end) {
      Iterator cached_begin = *begin;
      try { 
	inter_parser->parse(begin, end);
	output->push_back(parser->parse(begin, end));
      } catch (MatchFailure& m) {
	*begin = cached_begin;
	return output;
      }
    };
    
    return output;
  }
};

template <typename T>
struct VecPtr {
  typedef shared_ptr<vector<T> > type;
};

template <typename Iterator, typename Res, typename InterRes = void>
class InterspersedParser1: public Parser<Iterator, shared_ptr<vector<Res> > > { 
  InterspersedParser<Iterator, Res, InterRes> parser;
public:
  InterspersedParser1(typename Parser<Iterator, Res>::Ptr parser,
		      typename Parser<Iterator, InterRes>::Ptr inter_parser):
    parser(parser, inter_parser) {}

  shared_ptr<vector<Res> > parse(Iterator* begin, Iterator end) const { 
    Iterator cached_begin = *begin;
    shared_ptr<vector<Res> > res(parser.parse(begin, end));
    if (res->size() == 0) { 
      throw MatchFailure("Failed to match InterspersedParser1");
    }  else {
      return res;
    }
  }
};
  
template <typename Iterator>
class WhitespaceParser: public Parser<Iterator, void> {
public:
  static typename Parser<Iterator, void>::Ptr Instantiate() {
    return typename Parser<Iterator, void>::Ptr(new WhitespaceParser<Iterator>());
  }

private:
  RegexParser<Iterator> regexp;

public:
  WhitespaceParser(): regexp("\\s*") {}
  void parse(Iterator* begin, Iterator end) const {
    regexp.parse(begin, end);
  }
};
}

#endif