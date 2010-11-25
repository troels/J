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
    JArithmeticVerb(ScalarMonad<PlusMonadOp>::Instantiate(),
		    ScalarDyad<PlusDyadOp>::Instantiate(), 0) {}
};

namespace SignumTimesVerbNS {

template <typename Arg>
struct SignumMonadOp: public std::unary_function<Arg, JInt> {
  JInt operator()(Arg arg) const {
    return arg < 0 ? -1 : arg == 0 ? 0 : 1;
  }
};

template <> 
struct SignumMonadOp<JBox>: BadScalarMonadOp<JBox> {};

template <> 
struct SignumMonadOp<JChar>: BadScalarMonadOp<JChar> {};

template <typename Arg>
struct TimesDyadOp: public std::binary_function<Arg, Arg, Arg> {
  Arg operator()(Arg arg1, Arg arg2) const {
    return arg1 * arg2;
  }
};

template <>
struct TimesDyadOp<JBox>: BadScalarDyadOp<JBox> {};

template <>
struct TimesDyadOp<JChar>: BadScalarDyadOp<JChar> {};

}


class SignumTimesVerb: public JArithmeticVerb<JInt> {
public:
  SignumTimesVerb(): 
    JArithmeticVerb(ScalarMonad<SignumTimesVerbNS::SignumMonadOp>::Instantiate(),
		    ScalarDyad<SignumTimesVerbNS::TimesDyadOp>::Instantiate(), 1) {}
};

namespace ReciprocalDivideVerbNS {
template <typename Arg>
struct ReciprocalMonadOp: public std::unary_function<Arg, JFloat> {
  JFloat operator()(Arg arg) const {
    return static_cast<JFloat>(1.0) / static_cast<JFloat>(arg);
  }
};

template <>
struct ReciprocalMonadOp<JBox>: public BadScalarMonadOp<JBox> {};

template <>
struct ReciprocalMonadOp<JChar>: public BadScalarMonadOp<JChar> {};

template <typename Arg>
struct DivideDyadOp: public std::binary_function<Arg, Arg, JFloat> {
  JFloat operator()(Arg arg1, Arg arg2) const {
    return static_cast<JFloat>(arg1)/static_cast<JFloat>(arg2);
  }
};

template <>
struct DivideDyadOp<JBox>: public BadScalarDyadOp<JBox> {};

template <>
struct DivideDyadOp<JChar>: public BadScalarDyadOp<JChar> {};

}

class ReciprocalDivideVerb: public JArithmeticVerb<JInt> {
public:
  ReciprocalDivideVerb(): 
    JArithmeticVerb(ScalarMonad<ReciprocalDivideVerbNS::ReciprocalMonadOp>::Instantiate(),
		    ScalarDyad<ReciprocalDivideVerbNS::DivideDyadOp>::Instantiate(), 1) {}
};
    
template <typename Arg>
struct MinusMonadOp: public std::unary_function<Arg, Arg> {
  Arg operator()(Arg arg) const { 
    return -arg;
  }
};

namespace FloorLesserofVerbNS {
template <typename Arg>
struct FloorMonadOp : public BadScalarMonadOp<Arg> {};

template <> 
struct FloorMonadOp<JInt>: public std::unary_function<JInt, JInt> {
  JInt operator()(JInt arg) const {
    return arg;
  }
};

template <>
struct FloorMonadOp<JFloat>: public std::unary_function<JFloat, JInt> {
  JInt operator()(JFloat arg) const {
    return static_cast<JInt>(std::floor(arg));
  }
};

template <typename Arg> 
struct LesserofDyadOp: public std::binary_function<Arg, Arg, Arg> {
  Arg operator()(Arg arg1, Arg arg2) const { 
    return std::min(arg1, arg2);
  }
};

template <>
struct LesserofDyadOp<JBox>: public BadScalarDyadOp<JBox> {};

template <>
struct LesserofDyadOp<JChar>: public BadScalarDyadOp<JChar> {};

}
  
class FloorLesserofVerb: public JArithmeticVerb<JInt> {
public:
  FloorLesserofVerb(): 
    JArithmeticVerb<JInt>(ScalarMonad<FloorLesserofVerbNS::FloorMonadOp>::Instantiate(),
			  ScalarDyad<FloorLesserofVerbNS::LesserofDyadOp>::Instantiate(), 0) {}
};

namespace CeilingGreaterofVerbNS {
template <typename Arg>
struct CeilingMonadOp : public BadScalarMonadOp<Arg> {};

template <> 
struct CeilingMonadOp<JInt>: public std::unary_function<JInt, JInt> {
  JInt operator()(JInt arg) const {
    return arg;
  }
};

template <>
struct CeilingMonadOp<JFloat>: public std::unary_function<JFloat, JInt> {
  JInt operator()(JFloat arg) const {
    return static_cast<JInt>(std::ceil(arg));
  }
};

template <typename Arg> 
struct GreaterofDyadOp: public std::binary_function<Arg, Arg, Arg> {
  Arg operator()(Arg arg1, Arg arg2) const { 
    return std::max(arg1, arg2);
  }
};

template <>
struct GreaterofDyadOp<JBox>: public BadScalarDyadOp<JBox> {};

template <>
struct GreaterofDyadOp<JChar>: public BadScalarDyadOp<JChar> {};

}
  
class CeilingGreaterofVerb: public JArithmeticVerb<JInt> {
public:
  CeilingGreaterofVerb(): 
    JArithmeticVerb<JInt>(ScalarMonad<CeilingGreaterofVerbNS::CeilingMonadOp>::Instantiate(),
			  ScalarDyad<CeilingGreaterofVerbNS::GreaterofDyadOp>::Instantiate(), 0) {}
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
  MinusVerb(): JArithmeticVerb(ScalarMonad<MinusMonadOp>::Instantiate(),
			       ScalarDyad<MinusDyadOp>::Instantiate(),0) {}

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

namespace LessBoxVerbNS {

template <typename T>
struct DyadOp: public std::binary_function<T, T, JInt> { 
  JInt operator()(const T& arg1, const T& arg2) const  {
    return arg1 < arg2;
  }
};

template <>
struct DyadOp<JBox>: public BadScalarDyadOp<JBox> {};

template <>
struct DyadOp<JChar>: public BadScalarDyadOp<JChar> {};

}


class LessBoxVerb: public JVerb { 
  struct MonadOp { 
    JNoun::Ptr operator()(JMachine::Ptr, const JNoun& noun) const;
  };
  
public:
  LessBoxVerb(): JVerb(DefaultMonad<MonadOp>::Instantiate(rank_infinity, MonadOp()),
		       ScalarDyad<J::LessBoxVerbNS::DyadOp>::Instantiate()) {};
};

namespace MoreUnboxVerbNS {

template <typename T>
struct DyadOp: public std::binary_function<T, T, JInt> {
  JInt operator()(const T& arg1, const T& arg2) const { 
    return arg1 > arg2;
  }
};

template <> 
struct DyadOp<JBox> : public BadScalarDyadOp<JBox> {};

template <>
struct DyadOp<JChar>: public BadScalarDyadOp<JChar> {};
}
    

class MoreUnboxVerb: public JVerb {
  struct MonadOp { 
    JNoun::Ptr operator()(JMachine::Ptr m, const JNoun& noun) const;
  };

public:  
  MoreUnboxVerb(): JVerb(DefaultMonad<MonadOp>::Instantiate(0, MonadOp()),
			 ScalarDyad<J::MoreUnboxVerbNS::DyadOp>::Instantiate()) {}
};

namespace DecrementLessequalVerbNS {

template <typename Arg>
struct DecrementMonadOp: public std::unary_function<Arg, Arg> {
  Arg operator()(Arg arg) const { 
    return arg - 1;
  }
};

template <>
struct DecrementMonadOp<JBox>: BadScalarMonadOp<JBox> {};

template <>
struct DecrementMonadOp<JChar>: BadScalarMonadOp<JChar> {};

template <typename Arg>
struct LessequalDyadOp: public std::binary_function<Arg, Arg, JInt> {
  JInt operator()(Arg arg1, Arg arg2) const { 
    return arg1 <= arg2 ? 1 : 0;
  }
};

template <>
struct LessequalDyadOp<JBox>: BadScalarDyadOp<JBox> {};

template <>
struct LessequalDyadOp<JChar>: BadScalarDyadOp<JChar> {};
}

class DecrementLessequalVerb: public JArithmeticVerb<JInt> {
public:
  DecrementLessequalVerb():
    JArithmeticVerb<JInt>(ScalarMonad<DecrementLessequalVerbNS::DecrementMonadOp>::Instantiate(),
			  ScalarDyad<DecrementLessequalVerbNS::LessequalDyadOp>::Instantiate(), 0) {}
};

namespace IncrementMoreequalVerbNS {

template <typename Arg>
struct IncrementMonadOp: public std::unary_function<Arg, Arg> {
  Arg operator()(Arg arg) const { 
    return arg + 1;
  }
};

template <>
struct IncrementMonadOp<JBox>: BadScalarMonadOp<JBox> {};

template <>
struct IncrementMonadOp<JChar>: BadScalarMonadOp<JChar> {};

template <typename Arg>
struct MoreequalDyadOp: public std::binary_function<Arg, Arg, JInt> {
  JInt operator()(Arg arg1, Arg arg2) const { 
    return arg1 >= arg2 ? 1 : 0;
  }
};

template <>
struct MoreequalDyadOp<JBox>: BadScalarDyadOp<JBox> {};

template <>
struct MoreequalDyadOp<JChar>: BadScalarDyadOp<JChar> {};
}

class IncrementMoreequalVerb: public JArithmeticVerb<JInt> {
public:
  IncrementMoreequalVerb():
    JArithmeticVerb<JInt>(ScalarMonad<IncrementMoreequalVerbNS::IncrementMonadOp>::Instantiate(),
			  ScalarDyad<IncrementMoreequalVerbNS::MoreequalDyadOp>::Instantiate(), 0) {}
};
			  
}
		  
#endif
