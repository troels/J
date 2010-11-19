#ifndef SHAPEVERBS_HPP
#define SHAPEVERBS_HPP

#include "JVerbs.hpp"
#include "VerbHelpers.hpp"
#include "JExceptions.hpp"

namespace J {

template <typename T>
struct ShapeDyadOp {
  JNoun::Ptr operator()(const JArray<T>& rarg, const JNoun& noun) const { 
    JArray<JInt> larg(require_ints(noun));
    Dimensions from_larg(larg.is_scalar() ? 
			 Dimensions(1, *(larg.begin())) : 
			 Dimensions(larg.get_content()));

    Dimensions final_dims(from_larg + rarg.get_dims().suffix(-1));
    int rarg_number_of_elems = rarg.get_dims().number_of_elems();

    if (final_dims.number_of_elems() != 0 && rarg_number_of_elems == 0) {
      throw JIllegalDimensionsException("First dimension must be above 0");
    }
    
    shared_ptr<vector<T> > container(new vector<T>(final_dims.number_of_elems()));

    if (rarg_number_of_elems != 0) {
      typename vector<T>::iterator out_iter(container->begin());
      typename vector<T>::iterator out_end(container->end());
    
      while(distance(out_iter, out_end) >= rarg_number_of_elems) {
	copy(rarg.begin(), rarg.end(), out_iter);
	advance(out_iter, rarg_number_of_elems);
      }
      
      int distance_left(distance(out_iter, out_end));
      copy(rarg.begin(), rarg.begin() + distance_left, out_iter);
    }
    return JNoun::Ptr(new JArray<T>(final_dims, container));
  }
};
    
class ShapeVerb : public JVerb {
  struct MonadOp { 
    JNoun::Ptr operator()(JMachine::Ptr, const JNoun& arg) const { 
      Dimensions dims(arg.get_dims());
      shared_ptr<vector<JInt> > v(new vector<JInt>(dims.begin(), dims.end()));
      
      return JNoun::Ptr(new JArray<JInt>(Dimensions(1, v->size()), v));
    }
  };

  struct DyadOp {
    JNoun::Ptr operator()(JMachine::Ptr, const JNoun& larg, const JNoun& rarg) const { 
      return JArrayCaller<ShapeDyadOp, JNoun::Ptr>()(rarg, larg);
    }
  };

public:
  ShapeVerb(): JVerb(DefaultMonad<MonadOp>::Instantiate(rank_infinity, MonadOp()),
		     DefaultDyad<DyadOp>::Instantiate(1, rank_infinity, DyadOp())) {}
};
}

#endif
