#ifndef JGRAMMAR_HPP
#define JGRAMMAR_HPP

namespace J {
  enum j_grammar_class { 
    grammar_class_noun,
    grammar_class_verb,
    grammar_class_adverb,
    grammar_class_conjunction
  };

  enum j_value_type {
    j_value_type_int,
    j_value_type_float,
    j_value_type_char,
    j_value_type_box
  };
  
  typedef int JInt;
  typedef double JFloat;
  typedef char JChar;
  
  class JNoun {};
};

    
#endif
