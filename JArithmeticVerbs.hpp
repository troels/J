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

template <typename Arg>
struct PlusMonadOp: public std::unary_function<Arg, Arg> {
  Arg operator()(Arg arg) const { 
    return arg;
  }
};

template <>
struct PlusMonadOp<JBox>: public std::unary_function<JBox, JBox> {
  JBox operator()(JBox) const { 
    throw JIllegalValueTypeException();
  }
};


class PlusVerb: public JArithmeticVerb<JInt> { 
  template <typename LArg, typename RArg, typename Res>
  struct DyadOp: std::binary_function<LArg, RArg, Res> {
    Res operator()(LArg larg, RArg rarg) const {
      return larg + rarg;
    }
  };
      
public:
  PlusVerb(): 
    JArithmeticVerb(Monad::Ptr(new ScalarMonad<PlusMonadOp>()), 
		    Dyad::Ptr(new ScalarDyad<DyadOp>()), 0) {}
};

template <typename Arg>
struct MinusMonadOp: public std::unary_function<Arg, Arg> {
  Arg operator()(Arg arg) const { 
    return -arg;
  }
};

template <>
struct MinusMonadOp<JBox>: public std::unary_function<JBox, JBox> {
  JBox operator()(JBox) const { 
    throw JIllegalValueTypeException();
  }
};

class MinusVerb: public JArithmeticVerb<JInt> { 
  template <typename LArg, typename RArg, typename Res>
  struct DyadOp: std::binary_function<LArg, RArg, Res> {
    Res operator()(LArg larg, RArg rarg) const {
      return larg - rarg;
    }
  };

public:
  MinusVerb(): JArithmeticVerb(Monad::Ptr(new ScalarMonad<MinusMonadOp>()), 
			       Dyad::Ptr(new ScalarDyad<DyadOp>()), 0) {}
};    

class IDotVerb: public JVerb { 
  template <typename Arg, typename Res>
  struct MonadOp: public std::unary_function<Arg, Res> {
    JNoun::Ptr operator()(JMachine::Ptr m, const JNoun& arg) const;
  };

  struct IDotMonad: Monad { 
    IDotMonad(): Monad(1) {}
    JNoun::Ptr operator()(JMachine::Ptr, const JNoun& arg) const;
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
    JNoun::Ptr operator()(JMachine::Ptr m, const JNoun& larg, const JNoun& rarg) const;
  };

public:
  IDotVerb(): JVerb(shared_ptr<Monad>(new IDotMonad()), 
		    shared_ptr<Dyad>(new IDotDyad())) {}
};
      
}
		  
#endif
