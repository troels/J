#include "JArithmeticVerbs.hpp"

namespace J {
  shared_ptr<JNoun> IDotVerb::IDotMonad::operator()(const JNoun& arg) const { 
    JArray<JInt> int_arg = require_ints(arg);
    
    return monadic_apply(get_rank(), int_arg, MonadOp<JInt, JInt>());
  }

  template <>
  shared_ptr<JArray<JInt> > IDotVerb::MonadOp<JInt, JInt>::operator()(const JArray<JInt>& arg ) const {
    shared_ptr<vector<int> > v(new vector<int>(arg.begin(), arg.end()));
    shared_ptr<vector<JInt> > res(new vector<JInt>(accumulate(v->begin(), v->end(), 1, std::multiplies<int>())));

    typename vector<JInt>::iterator iter = res->begin();
    typename vector<JInt>::iterator end = res->end();
    
    for(DimensionCounter dc(v); iter != end; ++dc, ++iter) {
      *iter = *dc;
    }

    return shared_ptr<JArray<JInt> >(new JArray<JInt>(Dimensions(v), res));
  }
}
