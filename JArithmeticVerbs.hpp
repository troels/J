#ifndef JARITMETICVERBS_HPP
#define JARITMETICVERBS_HPP

#include "JVerbs.hpp"
#include "JExceptions.hpp"
#include "VerbHelpers.hpp"
#include "JMachine.hpp"
#include <functional>
#include <numeric>
#include <functional>
#include <map>
#include <boost/optional.hpp>
#include <cmath>

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

template <typename Arg>
struct PlusDyadOp: std::binary_function<Arg, Arg, Arg> {
  Arg operator()(Arg larg, Arg rarg) const {
    return larg + rarg;
  }
};

template <>
struct PlusDyadOp<JBox>: BadScalarDyadOp<JBox> {};

class PlusVerb: public JArithmeticVerb<JInt> { 
public:
  PlusVerb(): 
    JArithmeticVerb(Monad::Ptr(new ScalarMonad<PlusMonadOp>()), 
		    Dyad::Ptr(new ScalarDyad<PlusDyadOp>()), 0) {}
};

template <typename Arg>
struct MinusMonadOp: public std::unary_function<Arg, Arg> {
  Arg operator()(Arg arg) const { 
    return -arg;
  }
};


template <>
struct MinusMonadOp<JBox>: public BadScalarMonadOp<JBox> {};

template <typename Arg>
struct MinusDyadOp: std::binary_function<Arg, Arg, Arg> {
  Arg operator()(Arg larg, Arg rarg) const {
    return larg - rarg;
  }
};

template <>
struct MinusDyadOp<JBox>: public BadScalarDyadOp<JBox> {};

class MinusVerb: public JArithmeticVerb<JInt> { 
public:
  MinusVerb(): JArithmeticVerb(Monad::Ptr(new ScalarMonad<MinusMonadOp>()), 
			       Dyad::Ptr(new ScalarDyad<MinusDyadOp>()), 0) {}
};    

template <typename T>
struct IDotDyadOp { 
  JNoun::Ptr operator()(const JArray<T>& larg, const JArray<T>& rarg, JMachine::Ptr m,
			const Dimensions& haystack_dims, const Dimensions& frame) const;
};

class IDotVerb: public JVerb { 
  struct MonadOp {
    JNoun::Ptr operator()(JMachine::Ptr m, const JNoun& arg) const;
  };

  struct DyadOp {
    JNoun::Ptr operator()(JMachine::Ptr m, const JNoun& larg, const JNoun& rarg) const;
  };
  
public:
  IDotVerb(): JVerb(DefaultMonad<MonadOp>::Instantiate(1, MonadOp()), 
		    DefaultDyad<DyadOp>::Instantiate(rank_infinity, rank_infinity, DyadOp())) {}

};

}
		  
#endif
