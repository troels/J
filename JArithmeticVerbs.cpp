#include "JArithmeticVerbs.hpp"

namespace J {
  template <typename T>
  JArithmeticVerb<T>::JArithmeticVerb(shared_ptr<Monad> monad, shared_ptr<Dyad> dyad, T unit_value): 
    JVerb(monad, dyad), unit_value(unit_value), dyad_type_map() {

    dyad_type_map[dyad_pair(j_value_type_int, j_value_type_int)] = j_value_type_int;
    dyad_type_map[dyad_pair(j_value_type_float, j_value_type_float)] = j_value_type_float;
    dyad_type_map[dyad_pair(j_value_type_float, j_value_type_int)] = j_value_type_float;
    dyad_type_map[dyad_pair(j_value_type_int, j_value_type_float)] = j_value_type_float;
  }
  
  template <typename T>
  optional<j_value_type> JArithmeticVerb<T>::res_type(j_value_type larg, j_value_type rarg) const { 
    DyadTypeMap::const_iterator it = dyad_type_map.find(dyad_pair(larg, rarg));
    
    if (it != dyad_type_map.end()) {
      return optional<j_value_type>(it->second);
    } else {
      return optional<j_value_type>();
    }
  }

  template <typename T>
  shared_ptr<JNoun> JArithmeticVerb<T>::unit(const Dimensions& dims) const {
    return filled_array(dims, unit_value);
  }

  template class JArithmeticVerb<JInt>;
  
  shared_ptr<JNoun> IDotVerb::IDotMonad::operator()(const JNoun& arg) const { 
    JArray<JInt> int_arg = require_ints(arg);

    return monadic_apply< MonadOp<JInt, JInt> >(get_rank(), int_arg, MonadOp<JInt, JInt>());
  }

  template <>
  shared_ptr<JNoun > IDotVerb::MonadOp<JInt, JInt>::operator()(const JNoun& _arg) const {
    const JArray<JInt>& arg = static_cast<const JArray<JInt>&>(_arg);
    vector<int> v(arg.begin(), arg.end());
    shared_ptr<vector<JInt> > res(new vector<JInt>(std::abs(accumulate(v.begin(), v.end(), 
								       1, std::multiplies<int>()))));
    typename vector<JInt>::iterator iter = res->begin();
    typename vector<JInt>::iterator end = res->end();
    
    for(DimensionCounter dc(v); iter != end; ++dc, ++iter) {
      *iter = *dc;
    }

    shared_ptr<vector<int> > dims_vec(new vector<int>(v.size()));
    for (vector<int>::iterator input = v.begin(), output = dims_vec->begin(); input != v.end(); ++output, ++input) { 
      *output = std::abs(*input);
    }
      
    return shared_ptr<JArray<JInt> >(new JArray<JInt>(Dimensions(dims_vec), res));
  }

  shared_ptr<JNoun> IDotVerb::IDotDyad::operator()(const JNoun& larg, const JNoun& rarg) const {
    if (larg.get_value_type() == j_value_type_int &&
	rarg.get_value_type() == j_value_type_int) {
      return DyadOp<JInt, JInt, JInt>()(static_cast<const JArray<JInt> &>(larg),
					static_cast<const JArray<JInt> &>(rarg));
    } else if (larg.get_value_type() == j_value_type_float &&
	       rarg.get_value_type() == j_value_type_float) {
      return DyadOp<JFloat, JFloat, JInt>()(static_cast<const JArray<JFloat> &>(larg),
					    static_cast<const JArray<JFloat> &>(rarg));
    } else if (larg.get_value_type() == j_value_type_char &&
	       rarg.get_value_type() == j_value_type_char) {
      return DyadOp<JChar, JChar, JInt>()(static_cast<const JArray<JChar> &>(larg),
					  static_cast<const JArray<JChar> &>(rarg));
    }

    throw JIllegalValueTypeException();
  }

  template <typename Arg>
  shared_ptr<JArray<JInt> > IDotVerb::DyadOp<Arg, Arg, JInt>::operator()(const JArray<Arg>& larg,
									 const JArray<Arg>& rarg) const {
    Dimensions haystack_dims(larg.get_dims().suffix(-1));
    Dimensions frame(rarg.get_dims().prefix(-haystack_dims.get_rank()));
    
    
    if (!rarg.get_dims().suffix_match(haystack_dims)) {
      shared_ptr<vector<JInt> > v(new vector<JInt>(frame.number_of_elems(), larg.get_dims()[0]));
      return shared_ptr<JArray<JInt> >(new JArray<JInt>(frame, v));
    }

    shared_ptr<vector<JInt> > res(new vector<JInt>(frame.number_of_elems()));
    
    typename vector<JInt>::iterator output = res->begin();
    typename vector<Arg>::iterator needle_iterator = rarg.begin();
    typename vector<Arg>::iterator needle_end = rarg.end();

    int needle_increment = haystack_dims.number_of_elems();

    for(;needle_iterator != needle_end; needle_iterator += needle_increment, ++output) {
      typename vector<Arg>::iterator haystack_iterator = larg.begin();
      typename vector<Arg>::iterator haystack_end = larg.end();

      JInt i = 0;
      for(; haystack_iterator != haystack_end; ++i, haystack_iterator += needle_increment) {
	if (equal(haystack_iterator, haystack_iterator + needle_increment, needle_iterator)) break;
      }
      *output = i;
    }

    return shared_ptr<JArray<JInt> >(new JArray<JInt>(frame, res));
  }
}
