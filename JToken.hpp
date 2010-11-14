#ifndef JTOKENS_HPP
#define JTOKENS_HPP

#include <vector>
#include <algorithm>
#include <string>
#include <boost/shared_ptr.hpp>
#include "JNoun.hpp"
#include "JVerbs.hpp"
#include "JAdverbs.hpp"
#include "JConjunctions.hpp"

namespace J { namespace JTokens { 
using boost::shared_ptr;
using std::vector;
using std::string;

enum j_token_elem_type { 
  j_token_elem_type_noun,
  j_token_elem_type_verb,
  j_token_elem_type_adverb,
  j_token_elem_type_conjunction,
  j_token_elem_type_operator,
  j_token_elem_type_name,
  j_token_elem_type_assignment,
  j_token_elem_type_lparen,
  j_token_elem_type_rparen,
  j_token_elem_type_cap
};

template <typename T>
struct JTokenTraits {} ;

template <> 
struct JTokenTraits<JNoun> {
  static const j_token_elem_type token_type = j_token_elem_type_noun;
  static const j_grammar_class grammar_class = grammar_class_noun;
};

template <>
struct JTokenTraits<JVerb> {
  static const j_token_elem_type token_type = j_token_elem_type_verb;
  static const j_grammar_class grammar_class = grammar_class_verb;
};

template <>
struct JTokenTraits<JAdverb> {
  static const j_token_elem_type token_type = j_token_elem_type_adverb;
  static const j_grammar_class grammar_class = grammar_class_adverb;
};

template <>
struct JTokenTraits<JConjunction> {
  static const j_token_elem_type token_type = j_token_elem_type_conjunction;
  static const j_grammar_class grammar_class = grammar_class_conjunction;
};

class JTokenBase {  
  j_token_elem_type type;

public:
  typedef shared_ptr<JTokenBase> Ptr;

  JTokenBase(j_token_elem_type type): type(type) {} 
  virtual ~JTokenBase() = 0;
  
  j_token_elem_type get_j_token_elem_type() const { 
    return type;
  }
};

template <typename T>
class JTokenWord: public JTokenBase {
  typename T::Ptr word;

public:
  typedef JTokenBase::Ptr Ptr;
  
  static Ptr Instantiate(typename T::Ptr word) {
    return Ptr(new JTokenWord<T>(word));
  }
    
  JTokenWord(typename T::Ptr word): 
    JTokenBase(JTokenTraits<T>::token_type), word(word) {}

  typename T::Ptr get_word() const { return word; } 
};

class JTokenOperator: public JTokenBase {
  string operator_name;
  
public:
  JTokenOperator(const string& operator_name): 
    JTokenBase(j_token_elem_type_operator), operator_name(operator_name) {}
  
  string get_operator_name() const { 
    return operator_name;
  }
};

class JTokenName: public JTokenBase {
  string name;
  
public:
  JTokenName(const string& name): JTokenBase(j_token_elem_type_name), name(name) {} 
  
  string get_name() const { 
    return name;
  }
};

class JTokenCap: public JTokenBase { 
public:
  static JTokenBase::Ptr Instantiate() {
    return JTokenBase::Ptr(new JTokenCap());
  }

  JTokenCap(): JTokenBase(j_token_elem_type_cap) {}
};
  
class JTokenAssignment: public JTokenBase { 
  string operator_name;

public:
  static JTokenBase::Ptr Instantiate(const string& operator_name) {
    return JTokenBase::Ptr(new JTokenAssignment(operator_name));
  }

  JTokenAssignment(const string& operator_name): 
    JTokenBase(j_token_elem_type_assignment), operator_name(operator_name) {}

  string get_assignment_name() const { return operator_name; }
};

class JTokenLParen: public JTokenBase {
public:
  typedef JTokenBase::Ptr Ptr;
  static Ptr Instantiate() { 
    return Ptr(new JTokenLParen());
  }

  JTokenLParen():  JTokenBase(j_token_elem_type_lparen) {}
};

class JTokenRParen: public JTokenBase { 
public: 
  typedef JTokenBase::Ptr Ptr;
  static Ptr Instantiate() { 
    return Ptr(new JTokenRParen());
  }

  JTokenRParen(): JTokenBase(j_token_elem_type_rparen) {}
};

template <typename T>
typename T::Ptr get_word(JTokenBase::Ptr token, JMachine::Ptr m) { 
  if(token->get_j_token_elem_type() == JTokenTraits<T>::token_type) {
    return static_cast<JTokenWord<T>*>(token.get())->get_word();
  } else { 
    optional<JWord::Ptr> o(
      token->get_j_token_elem_type() == j_token_elem_type_operator ?
      m->lookup_symbol(static_cast<JTokenOperator*>(token.get())->get_operator_name()) : 
      token->get_j_token_elem_type() == j_token_elem_type_name ?
      m->lookup_name(static_cast<JTokenName*>(token.get())->get_name()) :
      optional<JWord::Ptr>());
    
    if (o && (*o)->get_grammar_class() == JTokenTraits<T>::grammar_class) {
      return boost::static_pointer_cast<T>(*o);
    }
  }
  
  throw std::logic_error("Not a word of right type");
}

template <typename T>
JWord::Ptr get_word_as_word(JTokenBase::Ptr token) {
  return boost::static_pointer_cast<JWord>(static_cast<JTokenWord<T>*>(token.get())->get_word());
}

template <typename T>
typename T::Ptr get_word_as_type(JWord::Ptr word) { 
  assert(JTokenTraits<T>::grammar_class == word->get_grammar_class());
  return boost::static_pointer_cast<T>(word);
}
  
JWord::Ptr get_bare_word(JTokenBase::Ptr token, JMachine::Ptr m);
JTokenBase::Ptr construct_token(JWord::Ptr word);
}}


#endif
