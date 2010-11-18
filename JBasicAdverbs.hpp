#ifndef JBASICADVERBS_HPP
#define JBASICADVERBS_HPP

#include <cmath>
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
      JNoun::Ptr operator()(JMachine::Ptr m, const JNoun& arg) const;
    };

    class MyDyad: public Dyad {
      JVerb::Ptr verb;
	
    public:
      MyDyad(JVerb::Ptr verb);
      JNoun::Ptr operator()(JMachine::Ptr m, const JNoun& larg, const JNoun& rarg) const;
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

class PrefixInfixAdverb: public JAdverb { 
  class PrefixInfixVerb: public JVerb {
    class MyMonad: public Monad { 
      JVerb::Ptr verb;

    public:
      static Ptr Instantiate(JVerb::Ptr verb) { 
	return Ptr(new MyMonad(verb));
      }

      MyMonad(JVerb::Ptr verb): 
	Monad(rank_infinity), verb(verb) {}
      
      JNoun::Ptr operator()(JMachine::Ptr m, const JNoun& arg) const { 
	Dimensions dims(arg.get_rank() == 0 ? Dimensions(1,1) : arg.get_dims() );
	
	JResult res(dims.prefix(1));
	
	if (dims[0] == 0) {
	  return arg.clone();
	}

	for (int i = 0; i < dims[0]; ++i) {
	  JNoun::Ptr slice(arg.subarray(0, i + 1)); 
	  JNoun::Ptr ans((*verb)(m, *slice));
	  res.add_noun(ans);
	}
	
	return res.assemble_result();
      }
    };

    class DyadOp {
      JVerb::Ptr verb;

    public:
      DyadOp(JVerb::Ptr verb): verb(verb) {}
      
      JNoun::Ptr operator()(JMachine::Ptr m, const JNoun& larg, const JNoun& rarg) const { 
	JArray<JInt> ints(require_ints(larg));
	int len = *(ints.begin());
	
	Dimensions dims(rarg.get_rank() == 0 ? Dimensions(1,1): rarg.get_dims());
	
	int first_elem = dims[0];
	
	if (len < 0) { 
	  len = -len;
	  int nr_of_cycles = first_elem % len == 0 ? first_elem / len : first_elem / len + 1;
	  JResult res(Dimensions(1, nr_of_cycles));
	  
	  for(int i = 0; i < nr_of_cycles; ++i) { 
	    JNoun::Ptr slice(rarg.subarray(i * len, std::min((i + 1) * len, first_elem)));
	    res.add_noun((*verb)(m, *slice));
	  } 
	  
	  return res.assemble_result();
	} else {
	  if (first_elem < len) {
	    return rarg.subarray(0, 0);
	  } 
	  
	  JResult res(Dimensions(1, first_elem - len + 1));
	  for (int i = 0; i + len <= first_elem; ++i) {
	    res.add_noun((*verb)(m, *rarg.subarray(i, i + len)));
	  }
	  
	  return res.assemble_result();
	}
      }
    };
    
  public:
    static Ptr Instantiate(JVerb::Ptr verb) {
      return Ptr(new PrefixInfixVerb(verb));
    }
    
    PrefixInfixVerb(JVerb::Ptr verb):
      JVerb(MyMonad::Instantiate(verb), 
	    DefaultDyad<DyadOp>::Instantiate(0, rank_infinity, DyadOp(verb))) {}
  };
  
public:
  JWord::Ptr operator()(JMachine::Ptr, JWord::Ptr word) const { 
    if (word->get_grammar_class() != grammar_class_verb) { 
      throw std::logic_error("Need a verb");
    }
    
    JVerb::Ptr verb(boost::static_pointer_cast<JVerb>(word));

    return JWord::Ptr(new PrefixInfixVerb(verb));
  }
};

}
#endif
