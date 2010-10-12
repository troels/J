#ifndef VERBHELPERS_HPP
#define VERBHELPERS_HPP

#include "JNoun.hpp"
#include "JVerbs.hpp"
#include <boost/shared_ptr.hpp>

namespace J {
  using boost::shared_ptr;
  template <template <typename, typename, typename> class OpType, typename LArg, typename RArg, typename Res>
  shared_ptr<JArray<Res> > scalar_dyadic_apply(const JArray<LArg>& larg, const JArray<RArg>& rarg, 
					       OpType<LArg, RArg, Res> op) {
    if (larg.get_dims() == rarg.get_dims()) {
      Dimensions d(larg.get_dims());
      shared_ptr<vector<Res> > v(new vector<Res>(d.number_of_elems()));
      transform(larg.begin(), larg.end(), rarg.begin(), v->begin(), op);
      return shared_ptr<JArray<Res> >(new JArray<Res>(d, v));
    }

    Dimensions frame(find_frame(0, 0, larg.get_dims(), rarg.get_dims()));
    
    OperationScalarIterator<LArg> liter(larg, frame);
    OperationScalarIterator<RArg> riter(rarg, frame);
    
    shared_ptr<vector<Res> > v(new vector<Res>(frame.number_of_elems()));
    
    typename vector<Res>::iterator output = v->begin();
    typename vector<Res>::iterator end = v->end();
    
    for(;output != end; ++liter, ++riter, ++output) {
      *output = op(*liter, *riter);
    }
    
    return shared_ptr<JArray<Res> >(new JArray<Res>(frame, v));
  }

  template <template <typename, typename, typename> class OpType, typename LArg, typename RArg, typename Res>
  shared_ptr<JArray<Res> > dyadic_apply(int lrank, int rrank,
					const JArray<LArg>& larg, const JArray<RArg>& rarg, 
					OpType<LArg, RArg, Res> op) {
    if (lrank >= larg.rank && rrank >= rarg.rank) {
      return shared_ptr<JArray<Res> >(new JArray<Res>(op(larg, rarg)));
    }

    Dimensions frame = find_frame(lrank, rrank, larg.get_dims(), rarg.get_dims());
    
    OperationIterator<LArg> liter(larg, frame);
    OperationIterator<RArg> riter(rarg, frame);
    
    JResult<Res> res(frame);
    
    while (!liter.at_end() && !riter.at_end()) {
      res.add_noun(*liter, *riter);
    }
    
    return res.assemble_result();
  }

  template <template <typename, typename> class OpType, typename Arg, typename Res> 
    shared_ptr<JArray<Res> > scalar_monadic_apply(const JArray<Arg>& arg, OpType<Arg, Res> op) {
    
    Dimensions d(arg.get_dims());
    shared_ptr<vector<Res> > v(new vector<Res>(d.number_of_elems()));
    transform(arg.begin(), arg.end(), v->begin(), op);
    
    return shared_ptr<JArray<Res> >(new JArray<Res>(d, v));
  }
  
  template <template <typename, typename> class OpType, typename Arg, typename Res>
  shared_ptr<JArray<Res> > monadic_apply(int rank, const JArray<Arg>& arg, OpType<Arg, Res> op) {
    if ( rank >= arg.get_rank()) {
      return op(arg);
    }

    Dimensions frame = arg.get_dims().prefix(-rank);
    JResult<Res> res(frame);
    
    OperationIterator<Arg> input(arg, frame, rank);
    while (!input.at_end()) {
      res.add_noun(*op(static_cast<JArray<Arg> &>(**input)));
      ++input;
    }

    return boost::static_pointer_cast<JArray<Res> >(res.assemble_result());
  }


  template <template <typename, typename, typename> class Op>
  struct ScalarDyad: public Dyad {
    ScalarDyad(): Dyad(0, 0) {}
    inline shared_ptr<JNoun> operator()(const JNoun& larg, const JNoun& rarg) const;
  };

  template <template <typename, typename> class Op>
  struct ScalarMonad: public Monad { 
    ScalarMonad(): Monad(0) {}
    inline shared_ptr<JNoun> operator()(const JNoun& arg) const;
  };

  template <template <typename, typename> class Op>
  inline shared_ptr<JNoun> ScalarMonad<Op>::operator()(const JNoun& arg) const {
    if (arg.get_value_type() == j_value_type_int) {
      return scalar_monadic_apply<Op, JInt, JInt>(static_cast<const JArray<JInt> &>(arg),
						  Op<JInt, JInt>());
    } else if (arg.get_value_type() == j_value_type_float) {
      return scalar_monadic_apply<Op, JFloat, JFloat>(static_cast<const JArray<JFloat> &>(arg),
						      Op<JFloat, JFloat>());
    }
    throw JIllegalValueTypeException();
  }
      
  template <template <typename, typename, typename> class Op>  
  inline shared_ptr<JNoun> ScalarDyad<Op>::operator()(const JNoun& larg, const JNoun& rarg) const {
    if (larg.get_value_type() == j_value_type_int && 
	rarg.get_value_type() == j_value_type_int) {
      return scalar_dyadic_apply<Op, JInt, JInt, JInt>(static_cast<const JArray<JInt>& >(larg),  
						       static_cast<const JArray<JInt>& >(rarg),
						       Op<JInt, JInt, JInt>());
    } else if (larg.get_value_type() == j_value_type_float && rarg.get_value_type() == j_value_type_float) {
      return scalar_dyadic_apply<Op, JFloat, JFloat, JFloat>(static_cast<const JArray<JFloat>& >(larg),
							     static_cast<const JArray<JFloat>& >(rarg),
							     Op<JFloat, JFloat, JFloat>());
    }
    
    if(larg.get_value_type() != rarg.get_value_type()) {
      if (larg.get_value_type() == j_value_type_float && rarg.get_value_type() == j_value_type_int) {
	
	return scalar_dyadic_apply<Op, JFloat, JFloat, JFloat>
	  (static_cast<const JArray<JFloat>& >(larg), 
	   jarray_int_to_float(static_cast<const JArray<JInt>&>(rarg)),
	   Op<JFloat, JFloat, JFloat>());
	
      } else if (larg.get_value_type() == j_value_type_int && 
		 rarg.get_value_type() == j_value_type_float) {
	
	return scalar_dyadic_apply<Op, JFloat, JFloat, JFloat>
	  (jarray_int_to_float(static_cast<const JArray<JInt>&>(larg)), 
	   static_cast<const JArray<JFloat>& >(rarg),
	   Op<JFloat, JFloat, JFloat>());
	  
      }
    }
    throw JIllegalValueTypeException();
  }

  JArray<JInt> require_ints(const JNoun& noun); 

  class DimensionCounter { 
    vector<int> reference;
    vector<int> current_count;
    vector<int> suffix_array;
    bool turned_around;

    void increment(int pos); 

  public:
    DimensionCounter(const vector<int>& ref);
    
    DimensionCounter& operator++();
    int operator*() const;
    bool at_end() const { 
      return turned_around; 
    }
  };
}

#endif
