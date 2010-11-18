#include "JBasicAdverbs.hpp"

namespace J {
JWord::Ptr JInsertTableAdverb::operator()(JMachine::Ptr, JWord::Ptr word) const { 
  if (word->get_grammar_class() != grammar_class_verb)
    throw JIllegalGrammarClassException();
    
  return JWord::Ptr(new JInsertTableVerb(boost::static_pointer_cast<JVerb>(word)));
}

JInsertTableAdverb::JInsertTableVerb::MyMonad::MyMonad(shared_ptr<JVerb> verb):
  Monad(rank_infinity), verb(verb) {}

  
JNoun::Ptr JInsertTableAdverb::JInsertTableVerb::MyMonad::operator()(JMachine::Ptr m, 
								     const JNoun& arg) const { 
  if (arg.is_scalar()) return arg.clone();
  
  int first_dim = arg.get_dims()[0];
  if (first_dim == 1) { 
    return arg.coordinate(1, 0);
  }
  if (first_dim == 0) {
    return verb->unit(arg.get_dims().suffix(-1));
  }
    
  JNoun::Ptr res(arg.coordinate(1, first_dim - 1));
  for (int i = first_dim - 2; i >= 0; --i) {
    res = (*verb)(m, *arg.coordinate(1, i), *res);
  }
  return res;
}

JInsertTableAdverb::JInsertTableVerb::MyDyad::MyDyad(JVerb::Ptr verb): 
  Dyad(verb->get_dyad_lrank(), rank_infinity), verb(verb) {}
  
JNoun::Ptr JInsertTableAdverb::JInsertTableVerb::MyDyad::operator()(JMachine::Ptr m,
								    const JNoun& larg, 
								    const JNoun& rarg) const {
  return dyadic_apply(get_lrank(), get_rrank(), m, larg, rarg, *verb);
}

JInsertTableAdverb::JInsertTableVerb::JInsertTableVerb(shared_ptr<JVerb> verb): 
  JVerb(shared_ptr<Monad>(new MyMonad(verb)), 
	shared_ptr<Dyad>(new MyDyad(verb))) {}
}



    
