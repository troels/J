#include "ShapeVerbs.hpp"

namespace J {

JNoun::Ptr ShapeVerb::MonadOp::operator()(JMachine::Ptr, const JNoun& arg) const { 
  Dimensions dims(arg.get_dims());
  shared_ptr<vector<JInt> > v(new vector<JInt>(dims.begin(), dims.end()));
  
  return JNoun::Ptr(new JArray<JInt>(Dimensions(1, v->size()), v));
}

JNoun::Ptr ShapeVerb::DyadOp::operator()(JMachine::Ptr, const JNoun& larg, const JNoun& rarg) const { 
  return JArrayCaller<ShapeDyadOp, JNoun::Ptr>()(rarg, larg);
}

template <typename T>
JNoun::Ptr ShapeDyadOp<T>::operator()(const JArray<T>& rarg, const JNoun& noun) const { 
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

template <typename T>
JNoun::Ptr RavelOp<T>::operator()(const JArray<T>& arg) const {
  return JNoun::Ptr(new JArray<T>(Dimensions(1, arg.get_dims().number_of_elems()), 
				  arg.get_content()));
}

JNoun::Ptr RavelAppendVerb::MonadOp::operator()(JMachine::Ptr, const JNoun& arg) const { 
  return JArrayCaller<RavelOp, JNoun::Ptr>()(arg);
}

JNoun::Ptr RavelAppendVerb::DyadOp::operator()(JMachine::Ptr, const JNoun& larg, const JNoun& rarg) const { 
  if (larg.get_value_type() != rarg.get_value_type()) {
    throw JIllegalValueTypeException("Needs the same types");
  }

  JNoun::Ptr ptrs[2];
  ptrs[0] = larg.clone();
  ptrs[1] = rarg.clone();

  return J::Aggregates::concatenate_nouns(ptrs, ptrs + 2);
}

JNoun::Ptr RazeLinkVerb::MonadOp::operator()(JMachine::Ptr m, const JNoun& arg) const {
  
  if (arg.get_dims().number_of_elems() == 0) {
    return JNoun::Ptr(new JArray<JInt>(Dimensions(1, 0)));
  }

  if (arg.get_value_type() != j_value_type_box)  {
    return RavelAppendVerb()(m, arg);
  } 
  
  const JArray<JBox>& box_arr = static_cast<const JArray<JBox>&>(arg);

  typedef J::Aggregates::get_boxed_content<JArray<JBox>::iter> get_boxed;
  get_boxed::result_type content_iters(get_boxed()(box_arr.begin(), box_arr.end()));
  
  return J::Aggregates::concatenate_nouns(content_iters.first, content_iters.second);
}

}
