#ifndef SHAPEVERBS_HPP
#define SHAPEVERBS_HPP

#include "JVerbs.hpp"
#include "JNoun.hpp"
#include "VerbHelpers.hpp"
#include "JExceptions.hpp"

namespace J {

template <typename T>
struct ShapeDyadOp {
  JNoun::Ptr operator()(const JArray<T>& rarg, const JNoun& noun) const { 
    JArray<JInt> larg(require_type<JInt>(noun));
    Dimensions from_larg(larg.is_scalar() ? 
			 Dimensions(1, *(larg.begin())) : 
			 Dimensions(larg.get_content()));

    Dimensions final_dims(from_larg + rarg.get_dims().suffix(-1));
    int rarg_number_of_elems = rarg.get_dims().number_of_elems();

    if (final_dims.number_of_elems() != 0 && rarg_number_of_elems == 0) {
      throw JIllegalDimensionsException("Must have more than zero elements in input, when wanted in output.");
    }
    
    shared_ptr<vector<T> > container(new vector<T>(final_dims.number_of_elems(), 
						   JTypeTrait<T>::base_elem()));

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

template <typename T>
struct RavelOp {
  JNoun::Ptr operator()(const JArray<T>& arg) const {
    return JNoun::Ptr(new JArray<T>(Dimensions(1, arg.get_dims().number_of_elems()), 
				    arg.get_content()));
  }
};

template <typename T>
JArray<T> expand_to_rank(int rank, const JArray<T>& array) {
  assert(rank >= array.get_rank());
  Dimensions old_dims(array.get_dims());
  shared_ptr<vector<int> > new_dims_vector(new vector<int>(rank, 1));

  copy(old_dims.begin(), old_dims.end(), new_dims_vector->begin() + (rank - array.get_rank()));
  return JArray<T>(new_dims_vector, array.get_content());
}
  
template <typename T>
struct AppendOp {
  JNoun::Ptr operator()(const JArray<T>& larg, const JNoun& rarg_, JMachine::Ptr) const { 
    const JArray<T>& rarg = static_cast<const JArray<T>&>(rarg_);
    
    if (larg.is_scalar() && rarg.is_scalar()) {
      shared_ptr<vector<T> > res(new vector<T>(2, JTypeTrait<T>::base_elem()));
      (*res)[0] = (*larg.begin());
      (*res)[1] = (*rarg.begin());
      return JNoun::Ptr(new JArray<T>(Dimensions(1, 2), res));
    } 
    
    if (rarg.is_scalar()) {
      int total_larg_elems(larg.get_dims().number_of_elems());
      Dimensions larg_dims(larg.get_dims());

      shared_ptr<vector<T> > res(new vector<T>(total_larg_elems + larg_dims.suffix(-1).number_of_elems(),
					       JTypeTrait<T>::base_elem()));
      
      copy(larg.begin(), larg.end(), res->begin());
      fill(res->begin() + total_larg_elems, res->end(), (*rarg.begin()));
      
      shared_ptr<vector<int> > dim_vector(new vector<int>(larg_dims.begin(), larg_dims.end()));
      (*dim_vector->begin())++;
      return JNoun::Ptr(new JArray<T>(Dimensions(dim_vector), res));
    } else if (larg.is_scalar()) {
      Dimensions rarg_dims(rarg.get_dims());
      int item_size = rarg_dims.suffix(-1).number_of_elems();
      shared_ptr<vector<T> > res(new vector<T>(rarg_dims.number_of_elems() + item_size,
					       JTypeTrait<T>::base_elem()));

      fill(res->begin(), res->begin() + item_size, (*larg.begin()));
      copy(rarg.begin(), rarg.end(), res->begin() + item_size);

      shared_ptr<vector<int> > dim_vector(new vector<int>(rarg.get_dims().begin(), rarg.get_dims().end()));
      (*dim_vector->begin())++;

      return JNoun::Ptr(new JArray<T>(Dimensions(dim_vector), res));      
    }
    
    JArray<T> new_larg(larg.get_rank() < rarg.get_rank() ? 
		       expand_to_rank(rarg.get_rank(), larg) : larg);
    JArray<T> new_rarg(rarg.get_rank() < larg.get_rank() ?
		       expand_to_rank(larg.get_rank(), rarg) : rarg);
    
    int larg_first_dim(new_larg.get_dims()[0]);
    int rarg_first_dim(new_rarg.get_dims()[0]);

    Dimensions frame(Dimensions(1, larg_first_dim + rarg_first_dim));
    JResult res(frame);

    for (int i = 0; i < larg_first_dim; ++i) {
      res.add_noun(new_larg.coordinate(1, i));
    }
    
    for (int i = 0; i < rarg_first_dim; ++i) {
      res.add_noun(new_rarg.coordinate(1, i));
    }
    return res.assemble_result();
  };
};

class RavelAppendVerb: public JVerb { 
  struct MonadOp {
    JNoun::Ptr operator()(JMachine::Ptr, const JNoun& arg) const { 
      return JArrayCaller<RavelOp, JNoun::Ptr>()(arg);
    }
  };

  struct DyadOp {
    JNoun::Ptr operator()(JMachine::Ptr m, const JNoun& larg, const JNoun& rarg) const { 
      if (larg.get_value_type() != rarg.get_value_type()) {
	throw JIllegalValueTypeException("Needs the same types");
      }

      return JArrayCaller<AppendOp, JNoun::Ptr>()(larg, rarg, m);
    }
  };

public:
  RavelAppendVerb(): JVerb(DefaultMonad<MonadOp>::Instantiate(rank_infinity, MonadOp()),
			   DefaultDyad<DyadOp>::Instantiate(rank_infinity, rank_infinity, DyadOp())) {}
};
}	

#endif
