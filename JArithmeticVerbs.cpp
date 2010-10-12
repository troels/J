#include "JArithmeticVerbs.hpp"

namespace J {
  PlusVerb::PlusVerb() : JVerb(shared_ptr<Monad>(new PlusMonad()), 
			       shared_ptr<Dyad>(new PlusDyad())) {}
}
