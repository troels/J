#ifndef SHAPEVERBS_HPP
#define SHAPEVERBS_HPP

#include "JVerbs.hpp"
#include "JNoun.hpp"
#include "VerbHelpers.hpp"
#include "JExceptions.hpp"
#include <boost/bind.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/iterator/filter_iterator.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

namespace J {

template <typename T>
struct ShapeDyadOp {
  JNoun::Ptr operator()(const JArray<T>& rarg, const JNoun& noun) const;
};
    
class ShapeVerb : public JVerb {
  struct MonadOp { 
    JNoun::Ptr operator()(JMachine::Ptr, const JNoun& arg) const;
  };

  struct DyadOp {
    JNoun::Ptr operator()(JMachine::Ptr, const JNoun& larg, const JNoun& rarg) const;
  };

public:
  ShapeVerb(): JVerb(DefaultMonad<MonadOp>::Instantiate(rank_infinity, MonadOp()),
		     DefaultDyad<DyadOp>::Instantiate(1, rank_infinity, DyadOp())) {}
};

template <typename T>
struct RavelOp {
  JNoun::Ptr operator()(const JArray<T>& arg) const;
};

template <typename T>
struct AppendOp {
  JNoun::Ptr operator()(const JArray<T>& larg, const JNoun& rarg_, JMachine::Ptr) const;
};

class RavelAppendVerb: public JVerb { 
  struct MonadOp {
    JNoun::Ptr operator()(JMachine::Ptr, const JNoun& arg) const;
  };

  struct DyadOp {
    JNoun::Ptr operator()(JMachine::Ptr m, const JNoun& larg, const JNoun& rarg) const;
  };

public:
  RavelAppendVerb(): JVerb(DefaultMonad<MonadOp>::Instantiate(rank_infinity, MonadOp()),
			   DefaultDyad<DyadOp>::Instantiate(rank_infinity, rank_infinity, DyadOp())) {}
};

class RazeLinkVerb: public JVerb { 
  struct MonadOp { 
    JNoun::Ptr operator()(JMachine::Ptr m, const JNoun& arg) const;
  };

  struct DyadOp {
    JNoun::Ptr operator()(JMachine::Ptr m, const JNoun& larg, const JNoun& rarg) const {
      return JNoun::Ptr();
    }
  };

public:
  RazeLinkVerb(): JVerb(DefaultMonad<MonadOp>::Instantiate(rank_infinity, MonadOp()),
			DefaultDyad<DyadOp>::Instantiate(rank_infinity, rank_infinity, DyadOp())) {}
};

}	

#endif
