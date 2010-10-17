#include "JBasicConjunctions.hpp"

namespace J {
  shared_ptr<JWord> RankConjunction::operator()(shared_ptr<JMachine>, 
						shared_ptr<JWord> lword, shared_ptr<JWord> rword) const {
    if (lword->get_grammar_class() != grammar_class_verb || 
	rword->get_grammar_class() != grammar_class_noun) {
      throw JIllegalGrammarClassException();
    }
    
    shared_ptr<JVerb> verb = boost::static_pointer_cast<JVerb>(lword);
    shared_ptr<JNoun> new_rank = boost::static_pointer_cast<JNoun>(rword);
    
    JArray<JInt> array = require_ints(*new_rank);
    if (array.get_rank() > 1 || 
	(array.get_rank() == 1 && (array.get_dims()[0] > 3 || array.get_dims()[0] <= 0))) {
      throw JIllegalDimensionsException();
    }
    
    JArray<JInt>::iter ptr = array.begin();
    if (array.get_rank() == 0 || 
	(array.get_rank() == 1 && array.get_dims()[0] == 1)) {
      return shared_ptr<JWord>(new RankVerb(verb, *ptr, *ptr, *ptr));
    } else if (array.get_dims()[0] == 2) {
      return shared_ptr<JWord>(new RankVerb(verb, *ptr, *(ptr + 1), *(ptr + 1)));
    } else if (array.get_dims()[0] == 3) {
      return shared_ptr<JWord>(new RankVerb(verb, *ptr, *(ptr + 1), *(ptr + 2)));
    }

    assert(0);
  }
}
