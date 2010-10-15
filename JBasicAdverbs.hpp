#ifndef JBASICADVERBS_HPP
#define JBASICADVERBS_HPP

#include "JExceptions.hpp"
#include "JVerbs.hpp"
#include "JAdverbs.hpp"
#include "VerbHelpers.hpp"

namespace J {
  class JInsertTableAdverb: public JAdverb {
    class JInsertTableVerb: public JVerb { 
      class MyMonad: public Monad { 
  	shared_ptr<JVerb> verb;

      public:
  	MyMonad(shared_ptr<JVerb> verb);
  	shared_ptr<JNoun> operator()(const JNoun& arg) const;
      };

      class MyDyad: public Dyad {
  	shared_ptr<JVerb> verb;
	
      public:
	MyDyad(shared_ptr<JVerb> verb);
  	shared_ptr<JNoun> operator()(const JNoun& larg, const JNoun& rarg) const;
      };

    public:
      JInsertTableVerb(shared_ptr<JVerb> verb);
      
    };	

  public:
    shared_ptr<JWord> operator()(shared_ptr<JWord> word) const;
  };
}
#endif
