#ifndef JBASICADVERBS_HPP
#define JBASICADVERBS_HPP

#include "JMachine.hpp"
#include "JExceptions.hpp"
#include "JVerbs.hpp"
#include "JConjunctions.hpp"
#include "JAdverbs.hpp"
#include "VerbHelpers.hpp"

namespace J {
class JInsertTableAdverb: public JAdverb {
  class JInsertTableVerb: public JVerb { 
    class MyMonad: public Monad { 
      JVerb::Ptr verb;

    public:
      MyMonad(JVerb::Ptr verb);
      shared_ptr<JNoun> operator()(JMachine::Ptr m, const JNoun& arg) const;
    };

    class MyDyad: public Dyad {
      JVerb::Ptr verb;
	
    public:
      MyDyad(JVerb::Ptr verb);
      shared_ptr<JNoun> operator()(JMachine::Ptr m, const JNoun& larg, const JNoun& rarg) const;
    };

  public:
    JInsertTableVerb(JVerb::Ptr verb);
      
  };	

public:
  JWord::Ptr operator()(JMachine::Ptr m, JWord::Ptr word) const;
};

class CompositeAdverb: public JAdverb {
  JAdverb::Ptr adverb0, adverb1;
public:
  static JAdverb::Ptr Instantiate(JAdverb::Ptr adverb0, JAdverb::Ptr adverb1) {
    return JAdverb::Ptr(new CompositeAdverb(adverb0, adverb1));
  }

  CompositeAdverb(JAdverb::Ptr adverb0, JAdverb::Ptr adverb1):
    JAdverb(), adverb0(adverb0), adverb1(adverb1) {}
  
  JWord::Ptr operator()(JMachine::Ptr m, JWord::Ptr word) const { 
    JWord::Ptr res0((*adverb0)(m, word));
    JWord::Ptr res1((*adverb1)(m, res0));
    return res1;
  }
};

class CompositeConjunctionAdverb: public JAdverb { 
  bool has_left_argument;
  JConjunction::Ptr conjunction;
  JWord::Ptr word;

public:
  static JAdverb::Ptr Instantiate(JConjunction::Ptr conjunction, JWord::Ptr word) {
    return JAdverb::Ptr(new CompositeConjunctionAdverb(conjunction, word));
  }

  static JAdverb::Ptr Instantiate(JWord::Ptr word, JConjunction::Ptr conjunction) {
    return JAdverb::Ptr(new CompositeConjunctionAdverb(word, conjunction));
  }

  CompositeConjunctionAdverb(JConjunction::Ptr conjunction, JWord::Ptr word):
    has_left_argument(false), conjunction(conjunction), word(word) {}

  CompositeConjunctionAdverb(JWord::Ptr word, JConjunction::Ptr conjunction): 
    has_left_argument(true), conjunction(conjunction), word(word) {}
  
  JWord::Ptr operator()(JMachine::Ptr m, JWord::Ptr new_word) const {
    if (has_left_argument) { 
      return (*conjunction)(m, word, new_word);
    } else {
      return (*conjunction)(m, new_word, word);
    }
  }
};

}
#endif
