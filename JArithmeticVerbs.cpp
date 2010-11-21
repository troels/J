#include "JArithmeticVerbs.hpp"

namespace J {
template <typename T>
JArithmeticVerb<T>::JArithmeticVerb(shared_ptr<Monad> monad, shared_ptr<Dyad> dyad, T unit_value): 
  JVerb(monad, dyad), unit_value(unit_value) {}
  
template <typename T>
shared_ptr<JNoun> JArithmeticVerb<T>::unit(const Dimensions& dims) const {
  return filled_array(dims, unit_value);
}

template class JArithmeticVerb<JInt>;

JNoun::Ptr IDotVerb::MonadOp::operator()(JMachine::Ptr, const JNoun& noun) const { 
  const JArray<JInt>& arg = require_type<JInt>(noun);
  vector<int> v(arg.begin(), arg.end());
  int new_size(std::abs(accumulate(v.begin(), v.end(), 1, std::multiplies<int>())));
  shared_ptr<vector<JInt> > res(new vector<JInt>(new_size));
  
  typename vector<JInt>::iterator iter(res->begin()), end(res->end());
  
  for(DimensionCounter dc(v); iter != end; ++dc, ++iter) {
    *iter = *dc;
  }
  
  shared_ptr<vector<int> > dims_vec(new vector<int>(v.size()));
  transform(v.begin(), v.end(), dims_vec->begin(), std::ptr_fun<JInt, JInt>(std::abs));
  
  return JNoun::Ptr(new JArray<JInt>(Dimensions(dims_vec), res));
}
  
JNoun::Ptr IDotVerb::DyadOp::operator()(JMachine::Ptr m, const JNoun& larg, const JNoun& rarg) const { 
  Dimensions haystack_dims(larg.get_dims().suffix(-1));
  Dimensions frame(rarg.get_dims().prefix(-haystack_dims.get_rank()));

  if (haystack_dims.get_rank() > rarg.get_dims().get_rank()) {
    return JNoun::Ptr(new JArray<JInt>(frame, larg.is_scalar() ? 1 : larg.get_dims()[0]));
  } 

  if (!TypeConversions::get_instance()
      ->find_best_type_conversion(larg.get_value_type(), rarg.get_value_type()) ||
      !rarg.get_dims().suffix_match(haystack_dims)) {
    shared_ptr<vector<JInt> > v(new vector<JInt>(frame.number_of_elems(), larg.is_scalar() ? 1 : larg.get_dims()[0]));
    return JNoun::Ptr(new JArray<JInt>(frame, v));
  }
  
  return CallWithCommonType<IDotDyadOp, JNoun::Ptr>()(larg, rarg, m, haystack_dims, frame);
}

template <typename T>
JNoun::Ptr IDotDyadOp<T>::operator()(const JArray<T>& larg, const JArray<T>& rarg, JMachine::Ptr,
				     const Dimensions& haystack_dims, const Dimensions& frame) const { 

  shared_ptr<vector<JInt> > res(new vector<JInt>(frame.number_of_elems()));
  int increment = haystack_dims.number_of_elems();

  vector<JInt>::iterator output(res->begin());
  typename vector<T>::iterator needle_iter(rarg.begin()), needle_end(rarg.end());
  
  for(;needle_iter != needle_end; advance(needle_iter, increment), ++output) {
    typename vector<T>::iterator haystack_iter(larg.begin()), haystack_end(larg.end());

    JInt i = 0;
    for(; haystack_iter != haystack_end; ++i, advance(haystack_iter, increment)) {
      if (equal(haystack_iter, haystack_iter + increment, needle_iter)) break;
    }
    *output = i;
  }
  
  return JNoun::Ptr(new JArray<JInt>(frame, res));
}
}
