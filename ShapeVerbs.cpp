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

JNoun::Ptr RavelAppendVerb::DyadOp::operator()(JMachine::Ptr m, const JNoun& larg, const JNoun& rarg) const { 
  if (larg.get_value_type() != rarg.get_value_type()) {
    throw JIllegalValueTypeException("Needs the same types");
  }
  
  return JArrayCaller<AppendOp, JNoun::Ptr>()(larg, rarg, m);
}

template <typename T>
JNoun::Ptr AppendOp<T>::operator()(const JArray<T>& larg, const JNoun& rarg_, JMachine::Ptr) const { 
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
    
    fill_n(res->begin(), item_size, (*larg.begin()));
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
}

// struct AppendOp { 
//   template <typename Iterator>
//   JNoun::Ptr operator()(Iterator in_begin, Iterator in_end) const { 
//     if (begin == end) {
//       return JNoun::Ptr(new JArray<JInt>(Dimensions(1, 0)));
//     }
//   typedef get_dimensions<get_boxed::iterator> get_dims;
//   get_dims::result_type dims_iters(get_dims()(content_iters.first, content_iters.second));

//   Dimensions dims(find_common_dims(dims_iters.first, dims_iters.second));
//   if (dims.get_rank() == 0) {
//     dims = Dimensions(1, arg.get_dims().number_of_elems());
//   }
    
    
    

JNoun::Ptr RazeVerbNS::perform_op(JMachine::Ptr m, const JNoun& arg) {
  using boost::make_transform_iterator;

  if (arg.get_value_type() != j_value_type_box)  {
    return RavelAppendVerb()(m, arg);
  } 

  if (arg.get_dims().number_of_elems() == 0) {
    return JNoun::Ptr(new JArray<JInt>(Dimensions(1, 0)));
  }
  
  const JArray<JBox>& box_arr = static_cast<const JArray<JBox>&>(arg);

  typedef get_boxed_content<JArray<JBox>::iter> get_boxed;
  get_boxed::result_type content_iters(get_boxed()(box_arr.begin(), box_arr.end()));
  
  typedef get_dimensions<get_boxed::iterator> get_dims;
  get_dims::result_type dims_iters(get_dims()(content_iters.first, content_iters.second));

  Dimensions dims(find_common_dims(dims_iters.first, dims_iters.second));
  if (dims.get_rank() == 0) {
    dims = Dimensions(1, arg.get_dims().number_of_elems());
  }

  typedef filter_empty<get_boxed::iterator> empty_filter;
  empty_filter::result_type filtered(empty_filter()(content_iters.first, content_iters.second));

  optional<j_value_type> otype(find_common_type_for_collection(filtered.first, filtered.second));
  j_value_type type;

  if (!otype) {
    if (dims.number_of_elems() != 0) 
      throw JIllegalValueTypeException();
    
    type = j_value_type_int;
  } else {
    type = *otype;
  }
  
  int highest_coord = 0;

  for (get_dims::iterator iter(dims_iters.first); iter != dims_iters.second; ++iter) {
    if ((*iter).get_rank() == dims.get_rank()) {
      highest_coord += (*iter)[0];
    } else {
      ++highest_coord;
    }
  };

  shared_ptr<vector<int> > new_dims_vec(boost::make_shared<vector<int> >(dims.begin(), dims.end()));
  (*new_dims_vec)[0] = highest_coord;

  Dimensions new_dims(new_dims_vec);

  return JTypeDispatcher<RazeVerbNS::AllocateArray, JNoun::Ptr>()(type, new_dims, 
								  content_iters.first, content_iters.second);
}


}
