#ifndef TRAINS_HPP
#define TRAINS_HPP

#include "JVerbs.hpp"
#include <functional>
#include <numeric>
#include <boost/shared_ptr.hpp>

namespace J {
using boost::shared_ptr;

class Hook: public JVerb { 
  class MonadOp: public Monad {
    JVerb::Ptr verb0, verb1;
  public:
    MonadOp(JVerb::Ptr verb0, JVerb::Ptr verb1):
      Monad(rank_infinity), verb0(verb0), verb1(verb1) {}
    
    JNoun::Ptr operator()(JMachine::Ptr m, const JNoun& arg) const { 
      JNoun::Ptr noun((*verb1)(m, arg));
      JNoun::Ptr resnoun((*verb0)(m, arg, *noun));
      return resnoun;
    }
  };
  
  class DyadOp: public Dyad  {
    JVerb::Ptr verb0, verb1;
  public:
    DyadOp(JVerb::Ptr verb0, JVerb::Ptr verb1):
      Dyad(rank_infinity, rank_infinity), verb0(verb0), verb1(verb1) {}
    
    JNoun::Ptr operator()(JMachine::Ptr m, const JNoun& larg, const JNoun& rarg) const {
      JNoun::Ptr noun((*verb1)(m, rarg));
      JNoun::Ptr resnoun((*verb0)(m, larg, *noun));
      return resnoun;
    }
  };
  
public:
  Hook(JVerb::Ptr verb0, JVerb::Ptr verb1): 
    JVerb(Monad::Ptr(new MonadOp(verb0, verb1)), 
	  Dyad::Ptr(new DyadOp(verb0, verb1))) {}
};

class BoundHook: public JVerb {
  class MonadOp: public Monad { 
    JVerb::Ptr verb0, verb1;
    JNoun::Ptr noun;
  public:
    static Ptr Instantiate(JNoun::Ptr noun, JVerb::Ptr verb0, JVerb::Ptr verb1) {
      return Ptr(new MonadOp(noun, verb0, verb1));
    }

    MonadOp(JNoun::Ptr noun, JVerb::Ptr verb0, JVerb::Ptr verb1):
      Monad(rank_infinity), verb0(verb0), verb1(verb1), noun(noun) {}
    
    JNoun::Ptr operator()(JMachine::Ptr m, const JNoun& arg) const {
      JNoun::Ptr res0((*verb1)(m, arg));
      JNoun::Ptr res1((*verb0)(m, *noun, *res0));
      return res1;
    }
  };

  class DyadOp: public Dyad {
    JVerb::Ptr verb0, verb1;
    JNoun::Ptr noun;
  public:
    static Ptr Instantiate(JNoun::Ptr noun, JVerb::Ptr verb0, JVerb::Ptr verb1) {
      return Ptr(new DyadOp(noun, verb0, verb1));
    }

    DyadOp(JNoun::Ptr noun, JVerb::Ptr verb0, JVerb::Ptr verb1):
      Dyad(rank_infinity, rank_infinity), verb0(verb0), verb1(verb1), noun(noun) {}
    
    JNoun::Ptr operator()(JMachine::Ptr m, const JNoun& larg, const JNoun& rarg) const { 
      JNoun::Ptr res0((*verb1)(m, larg, rarg));
      JNoun::Ptr res1((*verb0)(m, *noun, *res0));
      return res1;
    }
  };

public:
  BoundHook(JNoun::Ptr noun, JVerb::Ptr verb0, JVerb::Ptr verb1):
    JVerb(MonadOp::Instantiate(noun, verb0, verb1),
	  DyadOp::Instantiate(noun, verb0, verb1)) {}
};
   
      
class Fork: public JVerb { 
  class MonadOp: public Monad { 
    JVerb::Ptr verb0, verb1, verb2;
  public:
    static Ptr Instantiate(JVerb::Ptr verb0, JVerb::Ptr verb1, JVerb::Ptr verb2) {
      return Ptr(new MonadOp(verb0, verb1, verb2));
    }

    MonadOp(JVerb::Ptr verb0, JVerb::Ptr verb1, JVerb::Ptr verb2):
      Monad(rank_infinity), verb0(verb0), verb1(verb1), verb2(verb2) {} 
    
    JNoun::Ptr operator()(JMachine::Ptr m, const JNoun& arg) const { 
      JNoun::Ptr noun0((*verb0)(m, arg));
      JNoun::Ptr noun1((*verb2)(m, arg));
      JNoun::Ptr resnoun((*verb1)(m, *noun0, *noun1));
      return resnoun;
    }
  };

  class DyadOp: public Dyad { 
    JVerb::Ptr verb0, verb1, verb2; 
  public:
    static Ptr Instantiate(JVerb::Ptr verb0, JVerb::Ptr verb1, JVerb::Ptr verb2) {
      return Ptr(new DyadOp(verb0, verb1, verb2));
    }

    DyadOp(JVerb::Ptr verb0, JVerb::Ptr verb1, JVerb::Ptr verb2): 
      Dyad(rank_infinity, rank_infinity), verb0(verb0), verb1(verb1), verb2(verb2) {}
    
    JNoun::Ptr operator()(JMachine::Ptr m, const JNoun& larg, const JNoun& rarg) const { 
      JNoun::Ptr noun0((*verb0)(m, larg, rarg));
      JNoun::Ptr noun1((*verb2)(m, larg, rarg));
      JNoun::Ptr resnoun((*verb1)(m, *noun0, *noun1));
      return resnoun;
    }
  };
 
public:
  Fork(JVerb::Ptr verb0, JVerb::Ptr verb1, JVerb::Ptr verb2): 
    JVerb(MonadOp::Instantiate(verb0, verb1, verb2),
	  DyadOp::Instantiate(verb0, verb1, verb2)) {}
};

class CappedFork: public JVerb {
  class MonadOp: public Monad { 
    JVerb::Ptr verb0, verb1;
  public:
    MonadOp(JVerb::Ptr verb0, JVerb::Ptr verb1): 
      Monad(rank_infinity), verb0(verb0), verb1(verb1) {}

    JNoun::Ptr operator()(JMachine::Ptr m, const JNoun& arg) const {
      JNoun::Ptr noun0((*verb1)(m, arg));
      JNoun::Ptr resnoun((*verb0)(m, *noun0));
      return resnoun;
    }
  };

  class DyadOp: public Dyad { 
    JVerb::Ptr verb0, verb1;

  public:
    DyadOp(JVerb::Ptr verb0, JVerb::Ptr verb1):
      Dyad(rank_infinity, rank_infinity), verb0(verb0), verb1(verb1) {}
      
    JNoun::Ptr operator()(JMachine::Ptr m, const JNoun& larg, const JNoun& rarg) const { 
      JNoun::Ptr noun0((*verb1)(m, larg, rarg));
      JNoun::Ptr resnoun((*verb0)(m, *noun0));
      return resnoun;
    }
  };

public:
  CappedFork(JVerb::Ptr verb0, JVerb::Ptr verb1): 
    JVerb(Monad::Ptr(new MonadOp(verb0, verb1)),
	  Dyad::Ptr(new DyadOp(verb0, verb1))) {}
};
}

#endif
