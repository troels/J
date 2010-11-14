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
#include <iterator>
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
  typedef shared_ptr<JCriteria> Ptr;

  virtual ~JCriteria() {}
  JCriteria(JMachine::Ptr m): m(m) {}
  virtual bool operator()(JTokenBase::Ptr token) const = 0;
  JMachine::Ptr get_machine() const { return m; }
};

class JAnyCriteria: public JCriteria {
public:
  JAnyCriteria(JMachine::Ptr m): JCriteria(m) {}
  bool operator()(JTokenBase::Ptr) const {
    return true;
  }
};

class JNameCriteria: public JCriteria { 
public:
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

template <typename Criteria>
JCriteria::Ptr mc(JMachine::Ptr m) {
  return JCriteria::Ptr(new Criteria(m));
}

class JRule { 
  JCriteria::Ptr first;
  JCriteria::Ptr second;
  JCriteria::Ptr third;
  JCriteria::Ptr fourth;
  JMachine::Ptr jmachine;
public:
  JRule(JMachine::Ptr m, JCriteria::Ptr first, JCriteria::Ptr second, JCriteria::Ptr third, JCriteria::Ptr fourth): 
    first(first), second(second), third(third), fourth(fourth), jmachine(m) {}
  virtual ~JRule() {}

  JMachine::Ptr get_machine() const { return jmachine; }

  template <typename Iterator>
  bool is_applicable(Iterator begin) const { 
    if (!(*first)(*begin)) return false;
    if (!(*second)(*(++begin))) return false;
    if (!(*third)(*(++begin))) return false;
    if (!(*fourth)(*(++begin))) return false;
    return true;
  }
  
  bool operator()(list<JTokenBase::Ptr>* lst, list<JTokenBase::Ptr>::iterator iter) const { 
    assert(distance(iter, lst->end()) >= 4);
    if (!is_applicable(iter)) return false;
    return transform(lst, iter);
  }

protected:
  virtual bool transform(list<JTokenBase::Ptr>* lst, list<JTokenBase::Ptr>::iterator iter) const = 0;
};

class JRuleMonad0: public JRule {
public:
  JRuleMonad0(JMachine::Ptr m): JRule(m,
				      mc<JEdgeCriteria>(m), mc<JWordCriteria<JVerb> >(m), 
				      mc<JWordCriteria<JNoun> >(m), mc<JAnyCriteria>(m))
				{}
  
protected:
  bool transform(list<JTokenBase::Ptr>* lst, list<JTokenBase::Ptr>::iterator iter) const { 
    ++iter;
    list<JTokenBase::Ptr>::iterator saved_iter(iter);

    JVerb::Ptr verb(get_word<JVerb>(*iter, get_machine()));
    JNoun::Ptr noun(get_word<JNoun>(*++iter, get_machine()));
    ++iter;
    JNoun::Ptr res((*verb)(get_machine(), *noun));
    
    lst->erase(saved_iter, iter);
    lst->insert(iter, JTokenWord<JNoun>::Instantiate(res));
    return true;
  }
};
  
class JRuleMonad1: public JRule { 
public:
  JRuleMonad1(JMachine::Ptr m): JRule(m, 
				      mc<JEdgeAVNCriteria>(m), mc<JWordCriteria<JVerb> >(m),
				      mc<JWordCriteria<JVerb> >(m), mc<JWordCriteria<JNoun> >(m))
  {}
  
  bool transform(list<JTokenBase::Ptr>* lst, list<JTokenBase::Ptr>::iterator iter) const { 
    advance(iter, 2);
    list<JTokenBase::Ptr>::iterator saved_iter(iter);
    JVerb::Ptr verb(get_word<JVerb>(*iter, get_machine()));
    JNoun::Ptr noun(get_word<JNoun>(*++iter, get_machine()));
    ++iter;
    
    JNoun::Ptr res((*verb)(get_machine(), *noun));
    
    lst->erase(saved_iter, iter);
    lst->insert(iter, JTokenWord<JNoun>::Instantiate(res));
    return true;
  };
};

class JRuleDyad2: public JRule { 
public:
  JRuleDyad2(JMachine::Ptr m): JRule(m,
				     mc<JEdgeAVNCriteria>(m), mc<JWordCriteria<JNoun> >(m),
				     mc<JWordCriteria<JVerb> >(m), mc<JWordCriteria<JNoun> >(m)) {}
  
  bool transform(list<JTokenBase::Ptr>* lst, list<JTokenBase::Ptr>::iterator iter) const { 
    ++iter;
    list<JTokenBase::Ptr>::iterator saved_iter(iter);
    
    JNoun::Ptr noun0(get_word<JNoun>(*iter, get_machine()));
    JVerb::Ptr verb(get_word<JVerb>(*++iter, get_machine()));
    JNoun::Ptr noun1(get_word<JNoun>(*++iter, get_machine()));
    ++iter;

    JNoun::Ptr res((*verb)(get_machine(), *noun0, *noun1));
    lst->erase(saved_iter, iter);
    lst->insert(iter, JTokenWord<JNoun>::Instantiate(res));
    return true;
  }
};

class JRuleAdverb3: public JRule { 
public:
  JRuleAdverb3(JMachine::Ptr m): JRule(m,
				       mc<JEdgeAVNCriteria>(m), mc<JVerbNounCriteria>(m),
				       mc<JWordCriteria<JAdverb> >(m), mc<JAnyCriteria>(m)) {}

  bool transform(list<JTokenBase::Ptr>* lst, list<JTokenBase::Ptr>::iterator iter) const { 
    ++iter;
    list<JTokenBase::Ptr>::iterator saved_iter(iter);
    
    JWord::Ptr arg(get_bare_word(*iter, get_machine()));
    JAdverb::Ptr adverb(get_word<JAdverb>(*++iter, get_machine()));
    ++iter;

    JWord::Ptr res((*adverb)(get_machine(), arg));
    JTokenBase::Ptr new_token(construct_token(res));

    lst->erase(saved_iter, iter);
    lst->insert(iter, new_token);
    return true;
  }
};

class JRuleConjunction4: public JRule { 
public:
  JRuleConjunction4(JMachine::Ptr m): JRule(m,
					    mc<JEdgeAVNCriteria>(m), mc<JVerbNounCriteria>(m),
					    mc<JWordCriteria<JConjunction> >(m), mc<JVerbNounCriteria>(m)) {}
  
  bool transform(list<JTokenBase::Ptr>* lst, list<JTokenBase::Ptr>::iterator iter) const {
    ++iter;
    list<JTokenBase::Ptr>::iterator saved_iter(iter);
    
    JWord::Ptr arg0(get_bare_word(*iter, get_machine()));
    JConjunction::Ptr conj(get_word<JConjunction>(*++iter, get_machine()));
    JWord::Ptr arg1(get_bare_word(*++iter, get_machine()));
    ++iter;

    JWord::Ptr res((*conj)(get_machine(), arg0, arg1));
    JTokenBase::Ptr new_token(construct_token(res));
    
    lst->erase(saved_iter, iter);
    lst->insert(iter, new_token);
    return true;
  }
};

// class JRuleFork5: public JRule { 
// public:
//   JRuleFork5(JMachine::Ptr m): JRule(m,
// 				     mc<JEdgeAVNCriteria>(m), mc<JVerbNounCriteria>(m),
// 				     mc<JWordCriteria<JVerb> >(m), mc<JWordCriteria<JVerb> >(m)) {}
  
//   bool transform(list<JTokenBase::Ptr>* lst, list<JTokenbase::Ptr>::iterator iter) const { 
//     ++iter;
//     list<JTokenBase::Ptr>::iterator saved_iter(iter);
    
}}

#endif
