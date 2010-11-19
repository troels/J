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
#include <iostream>
#include <deque>
#include <list>

namespace J { namespace JTokens { 
using boost::shared_ptr;
using std::list;
using std::deque;
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
  j_token_elem_type_cap,
  j_token_elem_type_dummy,
  j_token_elem_type_start
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
  virtual string to_string() const = 0;
};

class JTokenDummy: public JTokenBase { 
public:
  static Ptr Instantiate() {
    return Ptr(new JTokenDummy());
  } 
  
  string to_string() const { 
    return "JTokenDummy[]";
  }

  JTokenDummy(): JTokenBase(j_token_elem_type_dummy) {}
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
  
  string to_string() const {
    std::stringstream ss;
    ss << "JTokenWord<";
    JWord::Ptr converted_word(boost::static_pointer_cast<JWord>(word));
    switch (converted_word->get_grammar_class()) {
    case grammar_class_verb:
      ss << "JVerb>[]";
      break;
    case grammar_class_conjunction:
      return "JConjunction>[]";
      break;
    case grammar_class_adverb:
      return "JAdverb>[]";
      break;
    case grammar_class_noun:
      ss << "JNoun>[" << static_cast<JNoun&>(*converted_word) << "]";    
      return ss.str();
    default:
      throw std::logic_error("Unknown grammar class");
    }
    return ss.str();
  }

  typename T::Ptr get_word() const { return word; } 
};

class JTokenOperator: public JTokenBase {
  string operator_name;
  
public:
  static Ptr Instantiate(const string& operator_name) {
    return Ptr(new JTokenOperator(operator_name));
  }
  JTokenOperator(const string& operator_name): 
    JTokenBase(j_token_elem_type_operator), operator_name(operator_name) {}
  
  string get_operator_name() const { 
    return operator_name;
  }

  string to_string() const { 
    std::stringstream ss;
    ss << "JTokenOperator[" << get_operator_name() << "]";
    return ss.str();
  }
};

class JTokenName: public JTokenBase {
  string name;
  
public:
  static JTokenBase::Ptr Instantiate(const string& name) {
    return JTokenBase::Ptr(new JTokenName(name));
  }

  JTokenName(const string& name): JTokenBase(j_token_elem_type_name), name(name) {} 
  
  string get_name() const { 
    return name;
  }

  string to_string() const { 
    std::stringstream ss;
    ss << "JTokenName[" << get_name() << "]";
    return ss.str();
  }
};

class JTokenCap: public JTokenBase { 
public:
  static JTokenBase::Ptr Instantiate() {
    return JTokenBase::Ptr(new JTokenCap());
  }
  
  string to_string() const { 
    std::stringstream ss;
    ss << "JTokenCap[]";
    return ss.str();
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

  
  string to_string() const { 
    std::stringstream ss;
    ss << "JTokenAssignment[" << get_assignment_name() << "]";
    return ss.str();
  }

  string get_assignment_name() const { return operator_name; }
};

class JTokenLParen: public JTokenBase {
public:
  static Ptr Instantiate() { 
    return Ptr(new JTokenLParen());
  }

  string to_string() const { 
    std::stringstream ss;
    ss << "JTokenLParen[]";
    return ss.str();
  }

  JTokenLParen():  JTokenBase(j_token_elem_type_lparen) {}
};

class JTokenRParen: public JTokenBase { 
public: 
  static Ptr Instantiate() { 
    return Ptr(new JTokenRParen());
  }

  string to_string() const { 
    std::stringstream ss;
    ss << "JTokenRParen[]";
    return ss.str();
  }

  JTokenRParen(): JTokenBase(j_token_elem_type_rparen) {}
};

class JTokenStart: public JTokenBase { 
public:
  static Ptr Instantiate() {
    return Ptr(new JTokenStart());
  }
  
  string to_string() const { 
    std::stringstream ss;
    ss << "JTokenStart[]";
    return ss.str();
  }

  JTokenStart(): JTokenBase(j_token_elem_type_start) {}
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

std::ostream& operator<<(std::ostream& os, JTokenBase::Ptr base);
std::ostream& operator<<(std::ostream& os, const list<JTokenBase::Ptr>& lst);
std::ostream& operator<<(std::ostream& os, const deque<JTokenBase::Ptr>& lst);
}}


#endif
