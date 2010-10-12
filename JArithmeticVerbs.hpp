#ifndef JARITMETICVERBS_HPP
#define JARITMETICVERBS_HPP

#include "JVerbs.hpp"
#include "JExceptions.hpp"
#include "VerbHelpers.hpp"
#include <functional>

namespace J {
  class PlusVerb: public JVerb { 
    struct PlusMonad: public Monad { 
      PlusMonad(): Monad(0) {}
      shared_ptr<JNoun> operator()(const JNoun& arg) const {
	return arg.clone();
      }
    };
    
    template <typename LArg, typename RArg, typename Res>
    struct DyadOp: std::binary_function<LArg, RArg, Res> {
      Res operator()(LArg larg, RArg rarg) const {
	return larg + rarg;
      }
    };

    struct PlusDyad: public ScalarDyad<DyadOp> {};
      
  public:
    PlusVerb();
  };
}
		  
#endif
