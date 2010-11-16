#include "JEvaluator.hpp"

namespace J { namespace JEvaluator {

JRule::JRule(JMachine::Ptr m, JCriteria::Ptr first, JCriteria::Ptr second, 
	     JCriteria::Ptr third, JCriteria::Ptr fourth):  
  first(first), second(second), third(third), fourth(fourth), jmachine(m) {}

JRule::~JRule() {}

bool JRule::operator()(list<JTokenBase::Ptr>* lst, list<JTokenBase::Ptr>::iterator iter) const { 
  assert(distance(iter, lst->end()) >= 4);
  if (!is_applicable(iter)) return false;
  return transform(lst, iter);
}

bool JRuleMonad0::transform(list<JTokenBase::Ptr>* lst, list<JTokenBase::Ptr>::iterator iter) const { 
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

bool JRuleMonad1::transform(list<JTokenBase::Ptr>* lst, list<JTokenBase::Ptr>::iterator iter) const { 
  advance(iter, 2);
  list<JTokenBase::Ptr>::iterator saved_iter(iter);
  JVerb::Ptr verb(get_word<JVerb>(*iter, get_machine()));
  JNoun::Ptr noun(get_word<JNoun>(*++iter, get_machine()));
  ++iter;
    
  JNoun::Ptr res((*verb)(get_machine(), *noun));
    
  lst->erase(saved_iter, iter);
  lst->insert(iter, JTokenWord<JNoun>::Instantiate(res));
  return true;
}


bool JRuleDyad2::transform(list<JTokenBase::Ptr>* lst, list<JTokenBase::Ptr>::iterator iter) const { 
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

bool JRuleAdverb3::transform(list<JTokenBase::Ptr>* lst, list<JTokenBase::Ptr>::iterator iter) const { 
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

bool JRuleConjunction4::transform(list<JTokenBase::Ptr>* lst, list<JTokenBase::Ptr>::iterator iter) const {
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

bool JRuleFork5::transform(list<JTokenBase::Ptr>* lst, list<JTokenBase::Ptr>::iterator iter) const { 
  ++iter;
  list<JTokenBase::Ptr>::iterator saved_iter(iter);
  JTokenBase::Ptr first_arg(*iter);
  JVerb::Ptr verb0(get_word<JVerb>(*++iter, get_machine()));
  JVerb::Ptr verb1(get_word<JVerb>(*++iter, get_machine()));
  ++iter;

  if (first_arg->get_j_token_elem_type() == j_token_elem_type_cap) {
    JVerb::Ptr new_verb(new CappedFork(verb0, verb1));
    JTokenBase::Ptr new_token(construct_token(new_verb));
    lst->erase(saved_iter, iter);
    lst->insert(iter, new_token);
  } else if (JWordCriteria<JVerb>(get_machine())(first_arg)) {
    JVerb::Ptr first_verb(get_word<JVerb>(first_arg, get_machine()));
    JVerb::Ptr new_verb(new Fork(first_verb, verb0, verb1));
    JTokenBase::Ptr new_token(construct_token(new_verb));
    lst->erase(saved_iter, iter);
    lst->insert(iter, new_token);
  } else {
    JNoun::Ptr noun(get_word<JNoun>(first_arg, get_machine()));
    JVerb::Ptr new_verb(new BoundHook(noun, verb0, verb1));
    JTokenBase::Ptr new_token(construct_token(new_verb));
    lst->erase(saved_iter, iter);
    lst->insert(iter, new_token);
  }

  return true;
}

bool JRuleBident6::transform(list<JTokenBase::Ptr>* lst, list<JTokenBase::Ptr>::iterator iter) const {
  ++iter;
  list<JTokenBase::Ptr>::iterator saved_iter(iter);
  JWord::Ptr word0(get_bare_word(*iter, get_machine()));
  JWord::Ptr word1(get_bare_word(*++iter, get_machine()));
  ++iter;

  if(word0->get_grammar_class() == grammar_class_adverb &&
     word1->get_grammar_class() == grammar_class_adverb) {
    JAdverb::Ptr adverb(CompositeAdverb::Instantiate(boost::static_pointer_cast<JAdverb>(word0),
						     boost::static_pointer_cast<JAdverb>(word1)));
      
    lst->erase(saved_iter, iter);
    lst->insert(iter, JTokenWord<JAdverb>::Instantiate(adverb));
    return true;
  } else if (word0->get_grammar_class() == grammar_class_verb &&
	     word1->get_grammar_class() == grammar_class_verb) {
    JVerb::Ptr verb0(boost::static_pointer_cast<JVerb>(word0));
    JVerb::Ptr verb1(boost::static_pointer_cast<JVerb>(word1));
    JVerb::Ptr new_verb(new Hook(verb0, verb1));
    JTokenBase::Ptr new_token(construct_token(new_verb));
    lst->erase(saved_iter, iter);
    lst->insert(iter, new_token);
    return true;
  } else if ((word0->get_grammar_class() == grammar_class_conjunction &&
	      (word1->get_grammar_class() == grammar_class_noun || 
	       word1->get_grammar_class() == grammar_class_verb)) ||
	     (word1->get_grammar_class() == grammar_class_conjunction && 
	      (word0->get_grammar_class() == grammar_class_noun || 
	       word0->get_grammar_class() == grammar_class_verb))) {
    JAdverb::Ptr adverb;
    if (word0->get_grammar_class() == grammar_class_conjunction) {
      JConjunction::Ptr conjunction(boost::static_pointer_cast<JConjunction>(word0));
      adverb = CompositeConjunctionAdverb::Instantiate(conjunction, word1);
    } else {
      JConjunction::Ptr conjunction(boost::static_pointer_cast<JConjunction>(word1));
      adverb = CompositeConjunctionAdverb::Instantiate(word0, conjunction);
    }
    lst->erase(saved_iter, iter);
    lst->insert(iter, JTokenWord<JAdverb>::Instantiate(adverb));
    return true;
  } else {
    return false;
  }
}

bool JRuleAssignment7::transform(list<JTokenBase::Ptr>* lst, list<JTokenBase::Ptr>::iterator iter) const { 
  list<JTokenBase::Ptr>::iterator saved_iter(iter);
  string name(boost::static_pointer_cast<JTokenName>(*iter)->get_name());
  string assignment(boost::static_pointer_cast<JTokenAssignment>(*++iter)->get_assignment_name());
  ++iter;
  JWord::Ptr word(get_bare_word(*iter, get_machine()));
  
  if (assignment == "=:") {
    get_machine()->add_public_symbol(name, word);
    } else if (assignment == "=.") {
    get_machine()->add_private_symbol(name, word);
  } else {
    throw std::logic_error("Only =: and =. are valid assignments");
  }
  
  lst->erase(saved_iter, iter);
  return true;
}

bool JRuleParens8::transform(list<JTokenBase::Ptr>* lst, list<JTokenBase::Ptr>::iterator iter) const { 
  lst->erase(iter);
  advance(iter, 2);
  lst->erase(iter);
  return true;
}

}}
