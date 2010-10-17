#include "JBasicAdverbs.hpp"

namespace J {
  shared_ptr<JWord> JInsertTableAdverb::operator()(shared_ptr<JMachine>, shared_ptr<JWord> word) const { 
    if (word->get_grammar_class() != grammar_class_verb)
      throw JIllegalGrammarClassException();
    
    return shared_ptr<JWord>(new JInsertTableVerb(boost::static_pointer_cast<JVerb>(word)));
  }

  JInsertTableAdverb::JInsertTableVerb::MyMonad::MyMonad(shared_ptr<JVerb> verb):
    Monad(rank_infinity), verb(verb) {}

  
  shared_ptr<JNoun> JInsertTableAdverb::JInsertTableVerb::MyMonad::operator()(shared_ptr<JMachine> m, 
									      const JNoun& arg) const { 
    if (arg.is_scalar() || arg.get_dims()[0] == 1) return arg.clone();
    
    int first_dim = arg.get_dims()[0];
    if (first_dim == 0) {
      return verb->unit(arg.get_dims().suffix(-1));
    }
    
    shared_ptr<JNoun> res = arg.coordinate(1, first_dim - 1);
    for (int i = first_dim - 2; i >= 0; --i) {
      res = (*verb)(m, *arg.coordinate(1, i), *res);
    }
    return res;
  }

  JInsertTableAdverb::JInsertTableVerb::MyDyad::MyDyad(shared_ptr<JVerb> verb): 
    Dyad(verb->get_dyad_lrank(), rank_infinity), verb(verb) {}
  
  shared_ptr<JNoun> JInsertTableAdverb::JInsertTableVerb::MyDyad::operator()(shared_ptr<JMachine> m,
									     const JNoun& larg, 
									     const JNoun& rarg) const {
    return dyadic_apply(get_lrank(), get_rrank(), larg, rarg, VerbContainer(m, verb));
  }

  JInsertTableAdverb::JInsertTableVerb::JInsertTableVerb(shared_ptr<JVerb> verb): 
    JVerb(shared_ptr<Monad>(new MyMonad(verb)), 
	  shared_ptr<Dyad>(new MyDyad(verb))) {}
}



    
