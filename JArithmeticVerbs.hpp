#ifndef JARITMETICVERBS_HPP
#define JARITMETICVERBS_HPP

#include "JVerbs.hpp"

namespace J {
  class PlusVerb: public JVerb { 
    class PlusMonad: public Monad { 
    public:
      PlusMonad(): Monad(0) {}
      shared_ptr<JNoun> operator()(const JNoun& arg) const {
	return arg.clone();
      }
    };
    
    class PlusDyad: public Dyad {
      template <typename T> 
      shared_ptr<JNoun> apply(const JArray<T>& larg, const JArray<T>& rarg) const;

    public:
      PlusDyad(): Dyad(0, 0) {}
      shared_ptr<JNoun> operator()(const JNoun& larg, const JNoun& rarg) const;
    };
      
  public:
    PlusVerb();
  };
}
		  
#endif
