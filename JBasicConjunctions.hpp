#ifndef JBASICCONJUNCTIONS_HPP
#define JBASICCONJUNCTIONS_HPP

#include "JConjunctions.hpp"
#include "JMachine.hpp"
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>

namespace J {
  using boost::weak_ptr;
  using boost::shared_ptr;

  class RankConjunction: public JConjunction { 
    class RankVerb: public JVerb { 
      class MyMonad: public Monad { 
	shared_ptr<JVerb> verb;

      public:
	MyMonad(int rank, shared_ptr<JVerb> verb): Monad(rank), verb(verb) {}
	shared_ptr<JNoun> operator()(const JNoun& arg) const {
	  return monadic_apply(get_rank(), arg, VerbContainer(verb));
	}
      };
	
      class MyDyad: public Dyad { 
	shared_ptr<JVerb> verb;
	
      public:
	MyDyad(int lrank, int rrank, shared_ptr<JVerb> verb): Dyad(lrank, rrank),  verb(verb) {}
	shared_ptr<JNoun> operator()(const JNoun& larg, const JNoun& rarg) const { 
	  return dyadic_apply(get_lrank(), get_rrank(), larg, rarg, VerbContainer(verb));
	}
      };
    public:
      RankVerb(weak_ptr<JMachine> jmachine, shared_ptr<JVerb> verb, int rank, int lrank, int rrank): 
	JVerb(jmachine, 
	      shared_ptr<Monad>(new MyMonad(rank, verb)),
	      shared_ptr<Dyad>(new MyDyad(lrank, rrank, verb))) {}
    };
  public:
    shared_ptr<JWord> operator()(shared_ptr<JWord> lword, shared_ptr<JWord> rword) const;
    RankConjunction(weak_ptr<JMachine> jmachine): JConjunction(jmachine) {}
  };
}

#endif
