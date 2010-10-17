#ifndef JVERBS_HPP
#define JVERBS_HPP

#include "JExceptions.hpp"
#include "JNoun.hpp"
#include "JMachine.hpp"
#include "utils.hpp"
#include <cassert>
#include <algorithm>
#include <boost/optional.hpp>

namespace J {
  class Dyad {
    int lrank,  rrank;
  public:
    virtual ~Dyad() {};
    Dyad(int lrank, int rrank): lrank(lrank), rrank(rrank) {}
    
    int get_lrank() const { return lrank; }
    int get_rrank() const { return rrank; }

    virtual shared_ptr<JNoun> operator()(shared_ptr<JMachine> m, const JNoun& larg, const JNoun& rarg) const = 0;
  };

  class Monad { 
    int rank;
  public:
    virtual ~Monad()  {}
    Monad(int rank): rank(rank) {}
    
    int get_rank() const { return rank;}
    
    virtual shared_ptr<JNoun> operator()(shared_ptr<JMachine> m, const JNoun& arg) const = 0;
  };

  class JVerb: public JWord {
    shared_ptr<Monad> monad;
    shared_ptr<Dyad> dyad;

  public:
    virtual ~JVerb() {}
    JVerb(shared_ptr<Monad> monad, shared_ptr<Dyad> dyad):
      JWord(grammar_class_verb), monad(monad), dyad(dyad) {}
    
    shared_ptr<JNoun> operator()(shared_ptr<JMachine> m, const JNoun& larg, const JNoun& rarg) const {
      return (*dyad)(m, larg, rarg);
    }
    
    shared_ptr<JNoun> operator()(shared_ptr<JMachine> m, const JNoun& arg) const { 
      return (*monad)(m, arg);
    }

    int get_dyad_lrank() const { return dyad->get_lrank(); }
    int get_dyad_rrank() const { return dyad->get_rrank(); }
    int get_monad_rank() const { return monad->get_rank(); }
      
    virtual shared_ptr<JNoun> unit(const Dimensions&) const { 
      throw JNoUnitException();
    }
  };
}

#endif
