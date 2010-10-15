#ifndef JBASICADVERBS_HPP
#define JBASICADVERBS_HPP

#include "JExceptions.hpp"
#include "JVerbs.hpp"
#include "JAdverbs.hpp"
#include "VerbHelpers.hpp"

namespace J {
  class JInsertTableAdverb: public JAdverb {
    shared_ptr<JVerb> verb;

    class JInsertTableVerb: public JVerb { 
      class MyMonad: public Monad { 
  	shared_ptr<JVerb> verb;
      public:
  	MyMonad(shared_ptr<JVerb> verb): 
  	  Monad(rank_infinity), 
  	  verb(verb) {}
	
  	shared_ptr<JNoun> operator()(const JNoun& arg) const { 
  	  if (arg.is_scalar() || arg.get_dims()[0] == 1) return arg.clone();

  	  int first_dim = arg.get_dims()[0];
  	  if (first_dim == 0) {
  	    return verb->unit(arg.get_dims().suffix(-1));
  	  }

  	  shared_ptr<JNoun> res = arg.coordinate(1, first_dim - 1);
  	  for (int i = first_dim - 2; i >= 0; ++i) {
  	    res = (*verb)(*arg.coordinate(1, i), *res);
  	  }
  	  return res;
  	}
      };

      class MyDyad: public Dyad {
  	shared_ptr<JVerb> verb;
	
	struct VerbContainer { 
	  shared_ptr<JVerb> verb;

	  shared_ptr<JNoun> operator()(const JNoun& larg, const JNoun& rarg) {
	    return (*verb)(larg, rarg);
	    
	  }
	  VerbContainer(shared_ptr<JVerb> verb): verb(verb) {}
	};

      public:
  	shared_ptr<JNoun> operator()(const JNoun& larg, const JNoun& rarg) const {
	  return dyadic_apply(verb->get_dyad_lrank(), rank_infinity, larg, rarg, VerbContainer(verb));
	}
      };
    };	
  public:
    JInsertTableAdverb(shared_ptr<JVerb>& verb): verb(verb) {}
  };
}
#endif
