#ifndef JGRAMMAR_HPP
#define JGRAMMAR_HPP

#include <climits>
#include <complex>
#include <boost/shared_ptr.hpp>
#include <string>

namespace J {
using std::string;
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

class JNoun;
class JBox {
  shared_ptr<JNoun> contents;
public:
  JBox(shared_ptr<JNoun> contents): contents(contents) { assert(contents); }
  
  bool operator==(const JBox& box) const;
  shared_ptr<JNoun> get_contents() const { return contents; } ;
};

std::ostream& operator<<(std::ostream& os, const JBox& b);

template <typename T> 
class JTypeTrait {};
  
template <>
struct JTypeTrait<JInt> {
  static JInt base_elem() { return 0; }
  static const j_value_type value_type = j_value_type_int;
};

template <>
struct JTypeTrait<JChar> {
  static JChar base_elem() { return ' '; }
  static const j_value_type value_type = j_value_type_char;
};

template <>
struct JTypeTrait<JFloat> {
  static JFloat base_elem() { return 0.0; }
  static const j_value_type value_type = j_value_type_float;
};

template <>
struct JTypeTrait<JComplex> {
  static JComplex base_elem() { return JComplex(0, 0); }
  static const j_value_type value_type = j_value_type_complex;
};

template <>
struct JTypeTrait<JBox> {
  static JBox base_elem(); 
  static const j_value_type value_type = j_value_type_box;
};

class JWord { 
  j_grammar_class grammar_class;
public:
  typedef shared_ptr<JWord> Ptr;

  virtual ~JWord() {};
  virtual string to_string() const = 0;
  virtual bool operator==(const JWord&) const { 
    return false;
  }
  virtual bool operator!=(const JWord& word) const {
    return !(*this == word);
  }
  JWord(j_grammar_class grammar_class): grammar_class(grammar_class) {}
  j_grammar_class get_grammar_class() const { return grammar_class; }
};


std::ostream& operator<<(std::ostream& os, const JWord& d);
std::ostream& operator<<(std::ostream& os, JWord::Ptr d);


}
    
#endif
