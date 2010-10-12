#ifndef JVERBS_HPP
#define JVERBS_HPP

#include "JExceptions.hpp"
#include "JNoun.hpp"
#include "utils.hpp"
#include <cassert>
#include <algorithm>
#include <boost/optional.hpp>

namespace J {
  using boost::optional;

  class JResultBase { 
    typedef vector<shared_ptr<JNoun> > JNounList;

    Dimensions frame;
    j_value_type value_type;
    JNounList nouns;
    JNounList::iterator nouns_ptr;
    shared_ptr<vector<int> > max_dims;
    optional<int> rank;

  public:
    virtual ~JResultBase() {}
    JResultBase(const Dimensions& frame, j_value_type value_type);
    
    Dimensions get_frame() const { return frame; }
    shared_ptr<vector<int> > get_max_dims() const { return max_dims; }
    j_value_type get_value_type() const { return value_type; }
    void add_noun(const JNoun& noun);
    virtual shared_ptr<JNoun> assemble_result() const = 0;
    const JNounList& get_nouns() const { return nouns; }
  };
  
  template <typename T>
  class JResult: public JResultBase {
  public:
    JResult(const Dimensions& frame);
    shared_ptr<JNoun> assemble_result() const;
  };
    
  Dimensions find_frame(int lrank, int rrank, const Dimensions& larg, const Dimensions& rarg);

  class Dyad {
    int lrank,  rrank;
  public:
    virtual ~Dyad() {};
    Dyad(int lrank, int rrank): lrank(lrank), rrank(rrank) {}
    
    int get_lrank() const { return lrank; }
    int get_rrank() const { return rrank; }

    virtual shared_ptr<JNoun> operator()(const JNoun& larg, const JNoun& rarg) const = 0;
  };

  class Monad { 
    int rank;
  public:
    virtual ~Monad()  {}
    Monad(int rank): rank(rank) {}
    
    int get_rank() const { return rank;}
    
    virtual shared_ptr<JNoun> operator()(const JNoun& arg) const = 0;
  };

  class JVerb: public JWord {
    shared_ptr<Monad> monad;
    shared_ptr<Dyad> dyad;

  public:
    virtual ~JVerb() {}
    JVerb(shared_ptr<Monad> monad, shared_ptr<Dyad> dyad):
      JWord(grammar_class_verb), monad(monad), dyad(dyad) {}
    
    shared_ptr<JNoun> operator()(const JNoun& larg, const JNoun& rarg) const {
      return (*dyad)(larg, rarg);
    }
    
    shared_ptr<JNoun> operator()(const JNoun& arg) const { 
      return (*monad)(arg);
    }

    int get_dyad_lrank() const { return dyad->get_lrank(); }
    int get_dyad_rrank() const { return dyad->get_rrank(); }
    int get_monad_rank() const { return monad->get_rank(); }
  };


}

#endif