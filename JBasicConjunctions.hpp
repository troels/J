#ifndef JBASICCONJUNCTIONS_HPP
#define JBASICCONJUNCTIONS_HPP

#include "JConjunctions.hpp"
#include "JMachine.hpp"
#include <boost/shared_ptr.hpp>

namespace J {
using boost::shared_ptr;

class RankConjunction: public JConjunction { 
  class RankVerb: public JVerb { 
    class MyMonad: public Monad { 
      JVerb::Ptr verb;

    public:
      MyMonad(int rank, JVerb::Ptr verb): Monad(rank), verb(verb) {}
      JNoun::Ptr operator()(JMachine::Ptr m, const JNoun& arg) const {
	return monadic_apply(get_rank(), m, arg, *verb);
      }
    };
	
    class MyDyad: public Dyad { 
      JVerb::Ptr verb;
	
    public:
      MyDyad(int lrank, int rrank, JVerb::Ptr verb): Dyad(lrank, rrank),  verb(verb) {}
      JNoun::Ptr operator()(JMachine::Ptr m, const JNoun& larg, const JNoun& rarg) const { 
	return dyadic_apply(get_lrank(), get_rrank(), m, larg, rarg, *verb);
      }
    };
  public:
    RankVerb(JVerb::Ptr verb, int rank, int lrank, int rrank): 
      JVerb(Monad::Ptr(new MyMonad(rank, verb)),
	    Dyad::Ptr(new MyDyad(lrank, rrank, verb))) {}
  };

public:
  JWord::Ptr operator()(JMachine::Ptr m, JWord::Ptr lword, JWord::Ptr rword) const;
  RankConjunction(): JConjunction() {}
};
}

#endif
