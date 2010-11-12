#ifndef JEVALUATOR_HPP
#define JEVALUATOR

#include <stdexcept>
#include <boost/shared_ptr.hpp>
#include <deque>
#include <list>
#include <vector>
#include <string>
#include <cstdarg>
#include <set>
#include "JGrammar.hpp"
#include "JNoun.hpp"
#include "JVerbs.hpp"
#include "JAdverbs.hpp"
#include "JConjunctions.hpp"
#include "JToken.hpp"
#include "JMachine.hpp"

namespace J { namespace JEvaluator { 
using std::list;
using std::string;
using std::vector;
using std::set;
using namespace ::J::JTokens;

class JCriteria {
  JMachine::Ptr m;
public:
  virtual ~JCriteria() {}
  JCriteria(JMachine::Ptr m): m(m) {}
  virtual bool operator()(JTokenBase::Ptr token) const = 0;
  JMachine::Ptr get_machine() const { return m; }
};

class JAnyCriteria: public JCriteria {
  JAnyCriteria(JMachine::Ptr m): JCriteria(m) {}
  bool operator()(JTokenBase::Ptr) const {
    return true;
  }
};

class JNameCriteria: public JCriteria { 
  JNameCriteria(JMachine::Ptr m): JCriteria(m) {}
  bool operator()(JTokenBase::Ptr token) const {
    return token->get_j_token_elem_type() == j_token_elem_type_name;
  }
};

template <typename T>
class JWordCriteria: public JCriteria { 
public:
  JWordCriteria(JMachine::Ptr m): JCriteria(m) {}
  
  bool operator()(JTokenBase::Ptr token) const { 
    if(token->get_j_token_elem_type() == JTokenTraits<T>::token_type) return true;
    
    if (token->get_j_token_elem_type() == j_token_elem_type_name) {
      optional<JWord::Ptr> o(get_machine()->lookup_name(static_cast<JTokenName*>(token.get())->get_name()));
      if (o && (*o)->get_grammar_class() == JTokenTraits<T>::grammar_class) return true;
    }
    
    return false;
  }
};

class JCriteriaCollection: public JCriteria { 
  set<j_token_elem_type> tokens;
protected:
  void add_token(j_token_elem_type token) {
    tokens.insert(token);
  }

public:
  JCriteriaCollection(JMachine::Ptr m): JCriteria(m) {} 
  bool operator()(JTokenBase::Ptr token) const { 
    return tokens.find(token->get_j_token_elem_type()) != tokens.end();
  }
};

class JEdgeCriteria: public JCriteriaCollection { 
public:
  JEdgeCriteria(JMachine::Ptr m): JCriteriaCollection(m) {
    add_token(j_token_elem_type_lparen);
    add_token(j_token_elem_type_assignment);
  }
};

class JEdgeAVNCriteria: public JCriteria { 
  JEdgeCriteria edge;
  JWordCriteria<JVerb> verb;
  JWordCriteria<JNoun> noun;
  JWordCriteria<JAdverb> adverb;

public:
  JEdgeAVNCriteria(JMachine::Ptr m): JCriteria(m), edge(m), verb(m), noun(m), adverb(m) {}
  
  bool operator()(JTokenBase::Ptr token) const { 
    return edge(token) || verb(token) || noun(token) || adverb(token);
  }
};

class JVerbNounCriteria: public JCriteria { 
  JWordCriteria<JVerb> verb;
  JWordCriteria<JNoun> noun;
  
public:
  JVerbNounCriteria(JMachine::Ptr m): JCriteria(m), verb(m), noun(m) {}
  
  bool operator()(JTokenBase::Ptr token) const { 
    return verb(token) || noun(token);
  }
};

class JCAVNCriteria: public JCriteria { 
  JWordCriteria<JVerb> verb;
  JWordCriteria<JNoun> noun;  
  JWordCriteria<JAdverb> adverb;  
  JWordCriteria<JConjunction> conjunction;  

public:
  JCAVNCriteria(JMachine::Ptr m): 
    JCriteria(m), verb(m), noun(m), adverb(m), conjunction(m) {}
  
  bool operator()(JTokenBase::Ptr token) const { 
    return verb(token) || noun(token) || adverb(token) || conjunction(token);
  }
};
}}

#endif
