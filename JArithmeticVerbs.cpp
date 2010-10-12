#include "JArithmeticVerbs.hpp"

namespace J {
  PlusVerb::PlusVerb() : JVerb(shared_ptr<Monad>(new PlusMonad()), 
			       shared_ptr<Dyad>(new PlusDyad())) {}

  shared_ptr<JNoun> PlusVerb::PlusDyad::operator()(const JNoun& larg, const JNoun& rarg) const {
    return shared_ptr<JNoun>();
  }

  template <typename T> 
  shared_ptr<JNoun> PlusVerb::PlusDyad::apply(const JArray<T>& larg, const JArray<T>& rarg) const {
    return shared_ptr<JNoun>();
  }
}
