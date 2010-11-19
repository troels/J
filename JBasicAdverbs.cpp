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

JNoun::Ptr PrefixInfixAdverb::PrefixInfixVerb::MonadOp::operator()(JMachine::Ptr m, const JNoun& arg) const { 
  Dimensions dims(arg.get_rank() == 0 ? Dimensions(1,1) : arg.get_dims() );
  
  JResult res(dims.prefix(1));
	
  if (dims[0] == 0) {
    return arg.clone();
  }

  for (int i = 0; i < dims[0]; ++i) {
    JNoun::Ptr slice(arg.subarray(0, i + 1)); 
    JNoun::Ptr ans((*verb)(m, *slice));
    res.add_noun(ans);
  }
	
  return res.assemble_result();
}

JNoun::Ptr 
PrefixInfixAdverb::PrefixInfixVerb::DyadOp::operator()(JMachine::Ptr m, const JNoun& larg, const JNoun& rarg) const { 
  JArray<JInt> ints(require_ints(larg));
  int len = *(ints.begin());
  
  Dimensions dims(rarg.get_rank() == 0 ? Dimensions(1,1): rarg.get_dims());
  
  int first_elem = dims[0];
  
  if (len < 0) { 
    len = -len;
    int nr_of_cycles = first_elem % len == 0 ? first_elem / len : first_elem / len + 1;
    JResult res(Dimensions(1, nr_of_cycles));
	  
    for(int i = 0; i < nr_of_cycles; ++i) { 
      JNoun::Ptr slice(rarg.subarray(i * len, std::min((i + 1) * len, first_elem)));
      res.add_noun((*verb)(m, *slice));
    } 
	  
    return res.assemble_result();
  } else {
    if (first_elem < len) {
      return rarg.subarray(0, 0);
    } 
	  
    JResult res(Dimensions(1, first_elem - len + 1));
    for (int i = 0; i + len <= first_elem; ++i) {
      res.add_noun((*verb)(m, *rarg.subarray(i, i + len)));
    }
	  
    return res.assemble_result();
  }
}

}



    
