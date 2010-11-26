#ifndef JEVALAPPLIER_HPP
#define JEVALAPPLIER_HPP

#include "JParser.hpp"
#include "JEvaluator.hpp"
#include "JMachine.hpp"

namespace J {

class JExecutor {
  typedef J::JParser::JTokenizer<string::iterator> parser_type;
  typedef parser_type::result_type token_sequence;

  JMachine::Ptr jmachine;
  parser_type::Ptr tokenizer;
  
  token_sequence parse_line(const string& line) const; 

public:
  JExecutor(JMachine::Ptr m);

  JWord::Ptr operator()(const string& line);
};

}


#endif 
