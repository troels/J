#include "JExecutor.hpp"

namespace J {

JExecutor::JExecutor(JMachine::Ptr m): jmachine(m), tokenizer() {
  shared_ptr<vector<string> > symbols = jmachine->list_symbols();
  tokenizer = parser_type::Instantiate(symbols->begin(), symbols->end());
}

JExecutor::token_sequence JExecutor::parse_line(const string& line) const {
  string trimmed(trim_string(line));
    
  token_sequence res;
  string::iterator iter(trimmed.begin());
  try {
    res = tokenizer->parse(&iter, trimmed.end());
  } catch (J::JParser::ParserFailure& mf) {
    throw JParserException();
  }

  if (iter != trimmed.end()) {
    throw JParserException();
  }

  return res;
}

JWord::Ptr JExecutor::operator()(const string& line) {
  token_sequence seq(parse_line(line));
  
  return J::JEvaluator::big_eval_loop(jmachine, seq->begin(), seq->end());
}
}
