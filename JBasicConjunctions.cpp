#include "JBasicConjunctions.hpp"

namespace J {
JWord::Ptr RankConjunction::operator()(JMachine::Ptr, JWord::Ptr lword, JWord::Ptr rword) const {
  if (lword->get_grammar_class() != grammar_class_verb || 
      rword->get_grammar_class() != grammar_class_noun) {
    throw JIllegalGrammarClassException();
  }
    
  shared_ptr<JVerb> verb = boost::static_pointer_cast<JVerb>(lword);
  shared_ptr<JNoun> new_rank = boost::static_pointer_cast<JNoun>(rword);
    
  JArray<JInt> array = require_type<JInt>(*new_rank);
  if (array.get_rank() > 1 || 
      (array.get_rank() == 1 && (array.get_dims()[0] > 3 || array.get_dims()[0] <= 0))) {
    throw JIllegalDimensionsException();
  }
    
  JArray<JInt>::iter ptr = array.begin();
  if (array.get_rank() == 0 || 
      (array.get_rank() == 1 && array.get_dims()[0] == 1)) {
    return JWord::Ptr(new RankVerb(verb, *ptr, *ptr, *ptr));
  } else if (array.get_dims()[0] == 2) {
    return JWord::Ptr(new RankVerb(verb, *(ptr + 1), *ptr, *(ptr + 1)));
  } else if (array.get_dims()[0] == 3) {
    return JWord::Ptr(new RankVerb(verb, *ptr, *(ptr + 1), *(ptr + 2)));
  }

  assert(0);
}
}
