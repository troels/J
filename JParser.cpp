#include "JParser.hpp"

namespace J { namespace JParser { 

std::ostream& operator<<(std::ostream& os, const ParsedNumberBase& p) { 
  return (os << p.to_string());
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
  
