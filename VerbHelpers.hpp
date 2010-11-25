#ifndef VERBHELPERS_HPP
#define VERBHELPERS_HPP

#include "JNoun.hpp"
#include "JVerbs.hpp"
#include "JTypes.hpp"
#include "utils.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <cmath>
#include <functional>
#include <algorithm>
#include <iterator>
#include <boost/bind.hpp>
#include <boost/iterator/filter_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/foreach.hpp>

namespace J {
using boost::shared_ptr;
using boost::make_shared;
using boost::optional;

std::auto_ptr<OperationIteratorBase> get_operation_iterator(const JNoun& arg, const Dimensions& frame, 
							    int output_rank);

class JResult { 
  typedef vector<JNoun::Ptr > JNounList;

  Dimensions frame;
  JNounList nouns;
  JNounList::iterator nouns_ptr;
  typedef optional<shared_ptr<vector<int> > > elem_dims_t;
  elem_dims_t elem_dims;
  optional<j_value_type> value_type;

  template <typename T>
  JNoun::Ptr assemble_result_internal() const;
  
  void update_elem_dims(const Dimensions& dims);

  template <typename T>
  struct assemble_result_helper { 
    JNoun::Ptr operator()(const JResult& res) const {
      return res.assemble_result_internal<T>();
    }
  };

public:
  JResult(const Dimensions& frame);
    
  Dimensions get_frame() const { return frame; }
  shared_ptr<vector<int> > get_elem_dims() const { return *elem_dims; }
  optional<j_value_type> get_value_type() const { return value_type; }
  void add_noun(JNoun::Ptr noun);
  const JNounList& get_nouns() const { return nouns; }
  JNoun::Ptr assemble_result() const;
};
  
Dimensions find_frame(int lrank, int rrank, const Dimensions& larg, const Dimensions& rarg);


  
template <typename Op>
JNoun::Ptr dyadic_apply(int lrank, int rrank, 
			JMachine::Ptr m, const JNoun& larg, const JNoun& rarg,
			Op op) {
  if (lrank >= larg.get_rank() && rrank >= rarg.get_rank()) {
    return op(m, larg, rarg)->clone();
  }
    
  Dimensions frame = find_frame(lrank, rrank, larg.get_dims(), rarg.get_dims());
    
  std::auto_ptr<OperationIteratorBase> liter(get_operation_iterator(larg, frame, lrank));
  std::auto_ptr<OperationIteratorBase> riter(get_operation_iterator(rarg, frame, rrank));
    
  JResult res(frame);
    
  for (;!liter->at_end() && !riter->at_end(); ++(*liter), ++(*riter)) {
    res.add_noun(op(m, ***liter, ***riter));
  }

  assert(liter->at_end() && riter->at_end());
  return res.assemble_result();
}


template <template <typename> class Op> 
struct scalar_monadic_apply {
  template <typename T>
  struct Impl {
    typedef Op<T> our_op;
    typedef typename our_op::result_type result_type;
    typedef typename our_op::argument_type argument_type;
    
    JNoun::Ptr
    operator()(const JArray<argument_type>& arg) {
      Dimensions d(arg.get_dims());
      shared_ptr<vector<result_type> > v
	(new vector<result_type>(d.number_of_elems(), JTypeTrait<result_type>::base_elem()));
      transform(arg.begin(), arg.end(), v->begin(), our_op());
      
      return JNoun::Ptr(new JArray<result_type>(d, v));
    }
  };

};

template <typename OpType>
JNoun::Ptr monadic_apply(int rank, JMachine::Ptr m, const JNoun& arg, OpType op) {
  if (rank < 0) {
    rank = std::max(0, arg.get_rank() + rank);
  }

  if ( rank >= arg.get_rank()) {
    return op(m, arg);
  }

  Dimensions frame(rank == 0 ? arg.get_dims() : arg.get_dims().prefix(-rank));
  if (frame.number_of_elems() == 0) {
    return JNoun::Ptr(new JArray<JInt>(frame));
  }

  JResult res(frame);
  
  for (std::auto_ptr<OperationIteratorBase> input(get_operation_iterator(arg, frame, rank)); 
       !input->at_end(); ++(*input)) {
    res.add_noun(op(m, ***input));
  }
  
  return res.assemble_result();
}

template <template <typename> class OpType>
struct scalar_dyadic_apply {
  template <typename T>
  struct Impl {
    JNoun::Ptr operator()(const JArray<T>& larg, const JArray<T>& rarg, JMachine::Ptr) const { 
      typedef typename OpType<T>::result_type result_type;
      typedef vector<result_type> res_vec;
      
      if (larg.get_dims() == rarg.get_dims()) {
	Dimensions d(larg.get_dims());
	shared_ptr<res_vec > v(new res_vec(d.number_of_elems(), JTypeTrait<T>::base_elem()));
	transform(larg.begin(), larg.end(), rarg.begin(), v->begin(), OpType<T>());
	return JNoun::Ptr(new JArray<result_type>(d, v));
      }
      
      Dimensions frame(find_frame(0, 0, larg.get_dims(), rarg.get_dims()));
    
      if (frame.number_of_elems() == 0) { 
	return JNoun::Ptr(new JArray<JInt>(frame));
      }

      OperationScalarIterator<T> liter(larg, frame), riter(rarg, frame);
      
      shared_ptr<res_vec > v(new res_vec(frame.number_of_elems(), JTypeTrait<T>::base_elem()));
      
      OpType<T> op;
      for(typename res_vec::iterator output(v->begin()), output_end(v->end()); output != output_end; 
	  ++output, ++liter, ++riter) {
	*output = op(*liter, *riter);
      }
      
      return JNoun::Ptr(new JArray<result_type>(frame, v));
    }
  };
};

template <template <typename> class Op>
struct ScalarDyad: public Dyad {
  ScalarDyad(): Dyad(0, 0) {}

  static Ptr Instantiate() {
    return Ptr(new ScalarDyad<Op>());
  }

  JNoun::Ptr operator()(JMachine::Ptr m, const JNoun& larg, const JNoun& rarg) const {
    return CallWithCommonType<scalar_dyadic_apply<Op>::template Impl, JNoun::Ptr>()(larg, rarg, m);
  }
};

  
template <typename Op>
class DefaultDyad: public Dyad {
  Op op;
public:
  DefaultDyad(int lrank, int rrank, Op op): Dyad(lrank, rrank), op(op) {}

  static Ptr Instantiate(int lrank, int rrank, Op op) {
    return Ptr(new DefaultDyad<Op>(lrank, rrank, op));
  }

  JNoun::Ptr operator()(JMachine::Ptr m, const JNoun& larg, const JNoun& rarg) const {
    return dyadic_apply(get_lrank(), get_rrank(), m, larg, rarg, op);
  }
};

template <template <typename> class Op>
struct ScalarMonad: public Monad { 
  ScalarMonad(): Monad(0) {}
  
  static Ptr Instantiate() {
    return Ptr(new ScalarMonad<Op>());
  }

  JNoun::Ptr operator()(JMachine::Ptr, const JNoun& arg) const {
    JArrayCaller<scalar_monadic_apply<Op>::template Impl, JNoun::Ptr> caller;
    return caller(arg);
  }
};
  
template <typename Op>
class DefaultMonad: public Monad { 
  Op op;
public:
  DefaultMonad(int rank, Op op): Monad(rank), op(op) {}
  
  static Ptr Instantiate(int rank, Op op) {
    return Ptr(new DefaultMonad(rank, op));
  }

  JNoun::Ptr operator()(JMachine::Ptr m, const JNoun& arg) const { 
    return monadic_apply(get_rank(), m, arg, op);
  }
};


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


class VerbContainer { 
  JVerb::Ptr verb;
  JMachine::Ptr jmachine; 

public:
  JNoun::Ptr operator()(const JNoun& larg, const JNoun& rarg) const {
    return (*verb)(jmachine, larg, rarg);
  }
    
  JNoun::Ptr operator()(const JNoun& arg) const { 
    return (*verb)(jmachine, arg);
  }

  VerbContainer(JMachine::Ptr jmachine, JVerb::Ptr verb): verb(verb), jmachine(jmachine) {}
};

template <typename Arg>
struct BadScalarMonadOp: std::unary_function<Arg, Arg> { 
  Arg operator()(Arg) {
    throw JIllegalValueTypeException();
  }
};

template <typename Arg>
struct BadScalarDyadOp: std::binary_function<Arg, Arg, Arg> { 
  Arg operator()(Arg, Arg) {
    throw JIllegalValueTypeException();
  }
};  


template <typename T>
struct new_operation_iterator { 
  std::auto_ptr<OperationIteratorBase> operator()(const JArray<T>& arg, 
						  const Dimensions& frame,  int output_rank) const {
    return std::auto_ptr<OperationIteratorBase>(new OperationIterator<T>(arg, frame, output_rank));
  }
};



}
  
#endif
