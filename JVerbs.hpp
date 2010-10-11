#ifndef JVERBS_HPP
#define JVERBS_HPP

#include "JExceptions.hpp"
#include "JNoun.hpp"
#include <cassert>
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
}

#endif
