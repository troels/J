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
  typedef shared_ptr<Dyad> Ptr;

  virtual ~Dyad() {};
  Dyad(int lrank, int rrank): lrank(lrank), rrank(rrank) {}
    
  int get_lrank() const { return lrank; }
  int get_rrank() const { return rrank; }

  virtual JNoun::Ptr operator()(JMachine::Ptr m, const JNoun& larg, const JNoun& rarg) const = 0;
};

class Monad { 
  int rank;

public:
  typedef shared_ptr<Monad> Ptr;
  virtual ~Monad()  {}
  Monad(int rank): rank(rank) {}
    
  int get_rank() const { return rank;}
    
  virtual JNoun::Ptr operator()(JMachine::Ptr m, const JNoun& arg) const = 0;
};

class JVerb: public JWord {
  shared_ptr<Monad> monad;
  shared_ptr<Dyad> dyad;

public:
  typedef shared_ptr<JVerb> Ptr;

  virtual ~JVerb() {}
  JVerb(Monad::Ptr monad, Dyad::Ptr dyad):
    JWord(grammar_class_verb), monad(monad), dyad(dyad) {}
    
  JNoun::Ptr operator()(shared_ptr<JMachine> m, const JNoun& larg, const JNoun& rarg) const {
    return (*dyad)(m, larg, rarg);
  }
    
  JNoun::Ptr operator()(shared_ptr<JMachine> m, const JNoun& arg) const { 
    return (*monad)(m, arg);
  }

  int get_dyad_lrank() const { return dyad->get_lrank(); }
  int get_dyad_rrank() const { return dyad->get_rrank(); }
  int get_monad_rank() const { return monad->get_rank(); }
  
  string to_string() const;
  virtual JNoun::Ptr unit(const Dimensions&) const { 
    throw JNoUnitException();
  }
  
};

}

#endif
