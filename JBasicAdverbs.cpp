#include "JBasicAdverbs.hpp"

namespace J {
  shared_ptr<JWord> JInsertTableAdverb::operator()(shared_ptr<JWord> word) const { 
    if (word->get_grammar_class() != grammar_class_verb)
      throw JIllegalGrammarClassException();
    
    return shared_ptr<JWord>(new JInsertTableVerb(boost::static_pointer_cast<JVerb>(word)));
  }
}



    
