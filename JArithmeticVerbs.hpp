#ifndef JARITMETICVERBS_HPP
#define JARITMETICVERBS_HPP

#include "JVerbs.hpp"
#include "JExceptions.hpp"
#include "VerbHelpers.hpp"
#include <functional>
#include <numeric>

namespace J {
  class PlusVerb: public JVerb { 
    template <typename Arg, typename Res>
    struct MonadOp: std::unary_function<Arg, Res> {
      Res operator()(Arg arg) const { 
	return arg;
      }
    };

    struct PlusMonad: public ScalarMonad<MonadOp> {};
    
    template <typename LArg, typename RArg, typename Res>
    struct DyadOp: std::binary_function<LArg, RArg, Res> {
      Res operator()(LArg larg, RArg rarg) const {
	return larg + rarg;
      }
    };

    struct PlusDyad: public ScalarDyad<DyadOp> {};
      
  public:
    PlusVerb(): JVerb(shared_ptr<Monad>(new PlusMonad()), shared_ptr<Dyad>(new PlusDyad())) {}
  };

  class MinusVerb: public JVerb { 
    template <typename Arg, typename Res>
    struct MonadOp: std::unary_function<Arg, Res> {
      Res operator()(Arg arg) const { 
	return -arg;
      }
    };
    
    struct MinusMonad: public ScalarMonad<MonadOp> {};
    
    template <typename LArg, typename RArg, typename Res>
    struct DyadOp: std::binary_function<LArg, RArg, Res> {
      Res operator()(LArg larg, RArg rarg) const {
	return larg - rarg;
      }
    };

    struct MinusDyad: public ScalarDyad<DyadOp> {};
      
  public:
    MinusVerb(): JVerb(shared_ptr<Monad>(new MinusMonad()), shared_ptr<Dyad>(new MinusDyad())) {}
  };    

  class IDotVerb: public JVerb { 
    template <typename Arg, typename Res>
    struct MonadOp: public std::unary_function<Arg, Res> {
      shared_ptr<JArray<Res> > operator()(const JArray<Arg>& arg) const;
    };

    struct IDotMonad: Monad { 
      IDotMonad(): Monad(1) {}
      shared_ptr<JNoun> operator()(const JNoun& arg) const;
    };
    
    struct IDotDyad: Dyad {
      IDotDyad(): Dyad(rank_infinity, rank_infinity) {}
      shared_ptr<JNoun> operator()(const JNoun& larg, const JNoun& rarg) const;
    };

  public:
    IDotVerb(): JVerb(shared_ptr<Monad>(new IDotMonad()), 
		      shared_ptr<Dyad>(new IDotDyad())) {}
  };
      
}
		  
#endif
