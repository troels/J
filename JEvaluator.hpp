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
#include <iostream>
#include "JGrammar.hpp"
#include "JNoun.hpp"
#include "JVerbs.hpp"
#include "JAdverbs.hpp"
#include "JConjunctions.hpp"
#include "JToken.hpp"
#include "JMachine.hpp"
#include "Trains.hpp"
#include "JBasicAdverbs.hpp"

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
    switch (token->get_j_token_elem_type()) { 
    case JTokenTraits<T>::token_type:
      return true;
      break;
    case j_token_elem_type_operator:
      do {
	optional<JWord::Ptr> o
	  (get_machine()->lookup_symbol(static_cast<JTokenOperator*>(token.get())->get_operator_name()));
	return o && (*o)->get_grammar_class() == JTokenTraits<T>::grammar_class;
      } while(0);
      break;
    case j_token_elem_type_name:
      do {
	optional<JWord::Ptr> o(get_machine()->lookup_name(static_cast<JTokenName*>(token.get())->get_name()));
	return o && (*o)->get_grammar_class() == JTokenTraits<T>::grammar_class;
      } while (0);
      break;
    default:
      return false;
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

class JLParenCriteria: public JCriteria {
public:
  JLParenCriteria(JMachine::Ptr m): JCriteria(m) {}
  bool operator()(JTokenBase::Ptr token) const { 
    return token->get_j_token_elem_type() == j_token_elem_type_lparen;
  }
};

class JRParenCriteria: public JCriteria {
public:
  JRParenCriteria(JMachine::Ptr m): JCriteria(m) {}
  bool operator()(JTokenBase::Ptr token) const { 
    return token->get_j_token_elem_type() == j_token_elem_type_rparen;
  }
};

class JFillerCriteria: public JCriteriaCollection { 
public:
  JFillerCriteria(JMachine::Ptr m): JCriteriaCollection(m) {
    add_token(j_token_elem_type_start);
    add_token(j_token_elem_type_dummy);
  }
};

class JEdgeCriteria: public JCriteriaCollection { 
public:
  JEdgeCriteria(JMachine::Ptr m): JCriteriaCollection(m) {
    add_token(j_token_elem_type_start);
    add_token(j_token_elem_type_lparen);
    add_token(j_token_elem_type_assignment);
  }
};

class JEdgeAVNCriteria: public JCriteria { 
  JEdgeCriteria edge;
  JWordCriteria<JVerb> verb;
  JWordCriteria<JNoun> noun;
  JWordCriteria<JAdverb> adverb;
  JWordCriteria<JConjunction> conjunction;
  
public:
  JEdgeAVNCriteria(JMachine::Ptr m): JCriteria(m), edge(m), verb(m), noun(m), adverb(m), conjunction(m) {}

  bool operator()(JTokenBase::Ptr token) const { 
    return edge(token) || verb(token) || noun(token) || adverb(token) || conjunction(token);
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

class JVerbNounCapCriteria: public JCriteria { 
  JWordCriteria<JVerb> verb;
  JWordCriteria<JNoun> noun;
  
public:
  JVerbNounCapCriteria(JMachine::Ptr m): JCriteria(m), verb(m), noun(m) {}
  
  bool operator()(JTokenBase::Ptr token) const { 
    return verb(token) || noun(token) || token->get_j_token_elem_type() == j_token_elem_type_cap;
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

class JAssignmentCriteria: public JCriteria { 
public:
  JAssignmentCriteria(JMachine::Ptr m): JCriteria(m) {}
  bool operator()(JTokenBase::Ptr token) const { 
    return token->get_j_token_elem_type() == j_token_elem_type_assignment;
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
  typedef shared_ptr<JRule> Ptr;

  JRule(JMachine::Ptr m, JCriteria::Ptr first, JCriteria::Ptr second, JCriteria::Ptr third, JCriteria::Ptr fourth);
  virtual ~JRule() = 0;

  JMachine::Ptr get_machine() const { return jmachine; }

  template <typename Iterator>
  bool is_applicable(Iterator begin) const { 
    if (!(*first)(*begin)) return false;
    if (!(*second)(*(++begin))) return false; 
    if (!(*third)(*(++begin))) return false;
    if (!(*fourth)(*(++begin))) return false;
    return true;
  }
  
  bool operator()(list<JTokenBase::Ptr>* lst, list<JTokenBase::Ptr>::iterator iter) const;

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
  bool transform(list<JTokenBase::Ptr>* lst, list<JTokenBase::Ptr>::iterator iter) const;
};
  
class JRuleMonad1: public JRule { 
public:
  JRuleMonad1(JMachine::Ptr m): JRule(m, 
				      mc<JEdgeAVNCriteria>(m), mc<JWordCriteria<JVerb> >(m),
				      mc<JWordCriteria<JVerb> >(m), mc<JWordCriteria<JNoun> >(m))
  {}

protected:  
  bool transform(list<JTokenBase::Ptr>* lst, list<JTokenBase::Ptr>::iterator iter) const;
};

class JRuleDyad2: public JRule { 
public:
  JRuleDyad2(JMachine::Ptr m): JRule(m,
				     mc<JEdgeAVNCriteria>(m), mc<JWordCriteria<JNoun> >(m),
				     mc<JWordCriteria<JVerb> >(m), mc<JWordCriteria<JNoun> >(m)) {}
  
protected:
  bool transform(list<JTokenBase::Ptr>* lst, list<JTokenBase::Ptr>::iterator iter) const;
};

class JRuleAdverb3: public JRule { 
public:
  JRuleAdverb3(JMachine::Ptr m): JRule(m,
				       mc<JEdgeAVNCriteria>(m), mc<JVerbNounCriteria>(m),
				       mc<JWordCriteria<JAdverb> >(m), mc<JAnyCriteria>(m)) {}

protected:
  bool transform(list<JTokenBase::Ptr>* lst, list<JTokenBase::Ptr>::iterator iter) const;
};

class JRuleConjunction4: public JRule { 
public:
  JRuleConjunction4(JMachine::Ptr m): JRule(m,
					    mc<JEdgeAVNCriteria>(m), mc<JVerbNounCriteria>(m),
					    mc<JWordCriteria<JConjunction> >(m), mc<JVerbNounCriteria>(m)) {}
  
protected:
  bool transform(list<JTokenBase::Ptr>* lst, list<JTokenBase::Ptr>::iterator iter) const;
};

class JRuleFork5: public JRule { 
public:
  JRuleFork5(JMachine::Ptr m): JRule(m,
				     mc<JEdgeAVNCriteria>(m), mc<JVerbNounCapCriteria>(m),
				     mc<JWordCriteria<JVerb> >(m), mc<JWordCriteria<JVerb> >(m)) {}
protected:
  bool transform(list<JTokenBase::Ptr>* lst, list<JTokenBase::Ptr>::iterator iter) const;
};						       

class JRuleBident6: public JRule {
public:
  JRuleBident6(JMachine::Ptr m): 
    JRule(m,
	  mc<JEdgeCriteria>(m), mc<JCAVNCriteria>(m), 
	  mc<JCAVNCriteria>(m), mc<JAnyCriteria>(m)) {}
  
protected:
  bool transform(list<JTokenBase::Ptr>* lst, list<JTokenBase::Ptr>::iterator iter) const;
};


class JRuleAssignment7: public JRule { 
public:
  JRuleAssignment7(JMachine::Ptr m): 
    JRule(m, 
	  mc<JNameCriteria>(m), mc<JAssignmentCriteria>(m),
	  mc<JCAVNCriteria>(m), mc<JAnyCriteria>(m)) {};
  
protected:
  bool transform(list<JTokenBase::Ptr>* lst, list<JTokenBase::Ptr>::iterator iter) const;
};

class JRuleParens8: public JRule { 
public:
  JRuleParens8(JMachine::Ptr m): 
    JRule(m,
	  mc<JLParenCriteria>(m), mc<JCAVNCriteria>(m),
	  mc<JRParenCriteria>(m), mc<JAnyCriteria>(m)) {}
  
protected:
  bool transform(list<JTokenBase::Ptr>* lst, list<JTokenBase::Ptr>::iterator iter) const;
};

template <typename T>
JRule::Ptr mr(JMachine::Ptr m) {
  return JRule::Ptr(new T(m));
}


class JRuleSet { 
  vector<JRule::Ptr> rules;

public:			  
  JRuleSet(JMachine::Ptr m): rules(9) {
    rules[0] = mr<JRuleMonad0>(m);
    rules[1] = mr<JRuleMonad1>(m);
    rules[2] = mr<JRuleDyad2>(m);
    rules[3] = mr<JRuleAdverb3>(m);
    rules[4] = mr<JRuleConjunction4>(m);
    rules[5] = mr<JRuleFork5>(m);
    rules[6] = mr<JRuleBident6>(m);
    rules[7] = mr<JRuleAssignment7>(m);
    rules[8] = mr<JRuleParens8>(m);
  }

  bool operator()(list<JTokenBase::Ptr> *lst, list<JTokenBase::Ptr>::iterator lst_iter) const {
    for(vector<JRule::Ptr>::const_iterator iter = rules.begin(), end = rules.end(); iter != end; ++iter) { 
      if ((**iter)(lst, lst_iter)) return true;
    }
    return false;
  }
};

template <typename Iterator>
JWord::Ptr big_eval_loop(JMachine::Ptr m, Iterator iter, Iterator end) {
  list<JTokenBase::Ptr> lst;

  lst.insert(lst.end(), JTokenDummy::Instantiate());
  lst.insert(lst.end(), JTokenDummy::Instantiate());
  lst.insert(lst.end(), JTokenDummy::Instantiate());
  lst.insert(lst.end(), JTokenDummy::Instantiate());
  
  JRuleSet ruleset(m);
  list<JTokenBase::Ptr>::iterator safe_haven(lst.begin());
  for(;iter != end; ++iter) {
    lst.insert(lst.begin(), *iter);
    bool cont = true;
    list<JTokenBase::Ptr>::iterator lst_iter(lst.begin());
    while (cont) {
      if(ruleset(&lst, lst_iter)) {
	lst_iter = safe_haven;
	--lst_iter;
      } else {
	if (lst_iter == lst.begin()) {
	  cont = false;
	}  else {
	  --lst_iter;
	}
      }
    }
  }
  
  lst.remove_if(JFillerCriteria(m));
  if (lst.size() != 1) {
    throw JIllegalSyntaxException();
  }
  
  return get_bare_word(lst.front(), m);
}
  
}}

#endif
