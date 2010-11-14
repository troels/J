#include "JToken.hpp"

namespace J { namespace JTokens { 

JTokenBase::~JTokenBase() {}

JWord::Ptr get_bare_word(JTokenBase::Ptr token, JMachine::Ptr m) {
  switch (token->get_j_token_elem_type()) {
  case j_token_elem_type_noun:
    return get_word_as_word<JNoun>(token);
    break;
  case j_token_elem_type_verb:
    return get_word_as_word<JVerb>(token);
    break;
  case j_token_elem_type_adverb:
    return get_word_as_word<JAdverb>(token);
    break;
  case j_token_elem_type_conjunction:
    return get_word_as_word<JConjunction>(token);
    break;
  case j_token_elem_type_operator:
    do {
      optional<JWord::Ptr> o(m->lookup_symbol(static_cast<JTokenOperator*>(token.get())->get_operator_name()));
      assert(o);
      return *o;
    } while(0);
    break;
  case j_token_elem_type_name:
    do { 
      optional<JWord::Ptr> o(m->lookup_name(static_cast<JTokenName*>(token.get())->get_name()));
      assert(o);
      return *o;
    } while(0);
    break;
  default:
    throw std::logic_error("get_bare_word was called with wrong arguments");
  }
}

JTokenBase::Ptr construct_token(JWord::Ptr word) { 
  switch (word->get_grammar_class()) {
  case grammar_class_noun:
    return JTokenWord<JNoun>::Instantiate(get_word_as_type<JNoun>(word));
    break;
  case grammar_class_verb:
    return JTokenWord<JVerb>::Instantiate(get_word_as_type<JVerb>(word));
    break;
  case grammar_class_adverb:
    return JTokenWord<JAdverb>::Instantiate(get_word_as_type<JAdverb>(word));
    break;
  case grammar_class_conjunction:
    return JTokenWord<JAdverb>::Instantiate(get_word_as_type<JAdverb>(word));
    break;
  default:
    throw std::logic_error("Word is of illegal type");
  }
}
}}


