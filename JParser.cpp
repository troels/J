#include "JParser.hpp"

namespace J { namespace JParser { 

std::ostream& operator<<(std::ostream& os, const ParsedNumberBase& p) { 
  return (os << p.to_string());
}

string trim_string(const string& str) {
  string::const_iterator start_iterator(str.begin());
  while (start_iterator != str.end() && std::isspace(*start_iterator)) ++start_iterator;
  string::const_iterator end_iterator(str.end());
  while (start_iterator != end_iterator && std::isspace(*end_iterator)) --end_iterator;
  
  return string(start_iterator, end_iterator);
}


TokenSequence parse_string(JMachine::Ptr m, const string& str ) {
  string real_str(trim_string(str));
  shared_ptr<vector<string> >    symbols(m->list_symbols());
    
  JTokenizer<string::iterator> tokenizer(symbols->begin(), symbols->end());
  string::iterator str_iter;
  TokenSequence res;
  try {
    res = tokenizer.parse(&str_iter, real_str.end());
  } catch (ParserFailure& mf) {
    throw JParserException();
  }
  
  if (str_iter != str.end()) {
    throw JParserException();
  }
  
  return res;
}

}}
  
