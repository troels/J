#ifndef VERBHELPERS_HPP
#define VERBHELPERS_HPP

#include "JNoun.hpp"
#include "JVerbs.hpp"
#include <boost/shared_ptr.hpp>

namespace J {
using boost::shared_ptr;
using boost::optional;

std::auto_ptr<OperationIteratorBase> get_operation_iterator(const JNoun& arg, const Dimensions& frame, 
							    int output_rank);

class JResult { 
  typedef vector<JNoun::Ptr > JNounList;

  Dimensions frame;
  JNounList nouns;
  JNounList::iterator nouns_ptr;
  shared_ptr<vector<int> > max_dims;
  optional<int> rank;
  optional<j_value_type> value_type;

  template <typename T>
  JNoun::Ptr assemble_result_internal() const;

public:
  JResult(const Dimensions& frame);
    
  Dimensions get_frame() const { return frame; }
  shared_ptr<vector<int> > get_max_dims() const { return max_dims; }
  optional<j_value_type> get_value_type() const { return value_type; }
  void add_noun(JNoun::Ptr noun);
  const JNounList& get_nouns() const { return nouns; }
  JNoun::Ptr assemble_result() const;
};
  
Dimensions find_frame(int lrank, int rrank, const Dimensions& larg, const Dimensions& rarg);

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

template <template <typename, typename> class OpType, typename Arg, typename Res> 
shared_ptr<JArray<Res> > scalar_monadic_apply(const JArray<Arg>& arg, OpType<Arg, Res> op) {
  Dimensions d(arg.get_dims());
  shared_ptr<vector<Res> > v(new vector<Res>(d.number_of_elems()));
  transform(arg.begin(), arg.end(), v->begin(), op);
    
  return shared_ptr<JArray<Res> >(new JArray<Res>(d, v));
}

template <typename OpType>
shared_ptr<JNoun > monadic_apply(int rank, JMachine::Ptr m, const JNoun& arg, OpType op) {
  if (rank < 0) {
    rank = std::max(0, arg.get_rank() + rank);
  }

  if ( rank >= arg.get_rank()) {
    return op(m, arg);
  }

  Dimensions frame = arg.get_dims().prefix(-rank);
  JResult res(frame);
      
  for (std::auto_ptr<OperationIteratorBase> input(get_operation_iterator(arg, frame, rank)); 
       !input->at_end(); ++(*input)) {
    res.add_noun(op(m, ***input));
  }

  return res.assemble_result();
}

template <template <typename, typename, typename> class Op>
struct ScalarDyad: public Dyad {
  ScalarDyad(): Dyad(0, 0) {}
  inline JNoun::Ptr operator()(shared_ptr<JMachine> m, const JNoun& larg, const JNoun& rarg) const;
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

template <template <typename, typename> class Op>
struct ScalarMonad: public Monad { 
  ScalarMonad(): Monad(0) {}
  inline JNoun::Ptr operator()(shared_ptr<JMachine> m, const JNoun& arg) const;
};

template <typename Op>
class DefaultMonad: public Monad { 
  Op op;
public:
  DefaultMonad(int rank, Op op): Monad(rank), op(op) {}
  
  JNoun::Ptr operator()(JMachine::Ptr m, const JNoun& arg) const { 
    return monad_apply(get_rank(), m, arg, op);
  }
};

template <template <typename, typename> class Op>
inline JNoun::Ptr ScalarMonad<Op>::operator()(shared_ptr<JMachine>, const JNoun& arg) const {
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
inline JNoun::Ptr ScalarDyad<Op>::operator()(shared_ptr<JMachine>, const JNoun& larg, 
						    const JNoun& rarg) const {
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

}

#endif
