#ifndef JTOKENS_HPP
#define JTOKENS_HPP

#include <vector>
#include <algorithm>
#include <string>
#include <boost/shared_ptr.hpp>
#include "JNoun.hpp"

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
  j_token_elem_type_sequence,
  j_token_elem_type_assignment,
  j_token_elem_type_lparen,
  j_token_elem_type_rparen
};

template <typename T>
struct JTokenTraits {} ;

template <> 
struct JTokenTraits<JNoun> {
  static const j_token_elem_type token_type = j_token_elem_type_noun;
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

class JTokenAssignment: public JTokenBase { 
  string operator_name;

public:
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
  JTokenRParen(): JTokenBase(j_token_elem_type_rparen) {}
};

}}


#endif
