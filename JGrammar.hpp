#ifndef JGRAMMAR_HPP
#define JGRAMMAR_HPP

#include <climits>
#include <complex>
#include <boost/shared_ptr.hpp>

namespace J {
using boost::shared_ptr;
  const int rank_infinity = INT_MAX;

  enum j_grammar_class { 
    grammar_class_noun,
    grammar_class_verb,
    grammar_class_adverb,
    grammar_class_conjunction
  };

  enum j_value_type {
    j_value_type_int, 
    j_value_type_float,
    j_value_type_complex,
    j_value_type_char, 
    j_value_type_box
  };
  
  typedef int JInt;
  typedef double JFloat;
  typedef std::complex<JFloat> JComplex;
  typedef char JChar;
  
  template <typename T> 
  class JTypeTrait {};
  
  template <>
  struct JTypeTrait<JInt> {
    static JInt base_elem() { return 0; }
    static const j_value_type value_type = j_value_type_int;
  };

  template <>
  struct JTypeTrait<JFloat> {
    static JFloat base_elem() { return 0.0; }
    static const j_value_type value_type = j_value_type_float;
  };

  template <>
  struct JTypeTrait<JComplex> {
    static JFloat base_elem() { return 0.0; }
    static const j_value_type value_type = j_value_type_complex;
  };

  class JWord { 
    j_grammar_class grammar_class;
  public:
    typedef shared_ptr<JWord> Ptr;

    virtual ~JWord() {};
    JWord(j_grammar_class grammar_class): grammar_class(grammar_class) {}
    j_grammar_class get_grammar_class() const { return grammar_class; }
  };
}
    
#endif
