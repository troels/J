#ifndef JARITMETICVERBS_HPP
#define JARITMETICVERBS_HPP

#include "JVerbs.hpp"
#include "JExceptions.hpp"
#include "VerbHelpers.hpp"
#include "JMachine.hpp"
#include <functional>
#include <numeric>
#include <map>
#include <boost/optional.hpp>

namespace J {
using boost::optional;

template <typename T>
class JArithmeticVerb: public JVerb {
  T unit_value;
    
public:
  JArithmeticVerb(shared_ptr<Monad> monad, shared_ptr<Dyad> dyad, T unit_value);
  shared_ptr<JNoun> unit(const Dimensions& dims) const;
};

class PlusVerb: public JArithmeticVerb<JInt> { 
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
  PlusVerb(): 
    JArithmeticVerb(shared_ptr<Monad>(new PlusMonad()), 
		    shared_ptr<Dyad>(new PlusDyad()),
		    0) {}
};

class MinusVerb: public JArithmeticVerb<JInt> { 
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
  MinusVerb(): JArithmeticVerb(shared_ptr<Monad>(new MinusMonad()), 
			       shared_ptr<Dyad>(new MinusDyad()), 0) {}
};    

class IDotVerb: public JVerb { 
  template <typename Arg, typename Res>
  struct MonadOp: public std::unary_function<Arg, Res> {
    shared_ptr<JNoun > operator()(const JNoun& arg) const;
  };

  struct IDotMonad: Monad { 
    IDotMonad(): Monad(1) {}
    shared_ptr<JNoun> operator()(shared_ptr<JMachine>, const JNoun& arg) const;
  };
    
  template <typename LArg, typename RArg, typename Res>
  struct DyadOp: public std::binary_function<LArg, RArg, Res> {
    shared_ptr<JArray<Res> > operator()(const JArray<LArg>& larg,
					const JArray<RArg>& rarg) const;
  };

  template <typename Arg>
  struct DyadOp<Arg, Arg, JInt>: public std::binary_function<Arg, Arg, JInt> {
    shared_ptr<JArray<JInt> > operator()(const JArray<Arg>& larg,
					 const JArray<Arg>& rarg) const;
  };


  struct IDotDyad: Dyad {
    IDotDyad(): Dyad(rank_infinity, rank_infinity) {}
    shared_ptr<JNoun> operator()(shared_ptr<JMachine> m, const JNoun& larg, const JNoun& rarg) const;
  };

public:
  IDotVerb(): JVerb(shared_ptr<Monad>(new IDotMonad()), 
		    shared_ptr<Dyad>(new IDotDyad())) {}
};
      
}
		  
#endif
