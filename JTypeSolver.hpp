#ifndef JTYPESOLVER_HPP
#define JTYPESOLVER_HPP

#include <boost/mpl/fold.hpp>
#include <boost/mpl/list/list10.hpp>
#include <boost/shared_ptr.hpp>
#include <functional>
#include "JNoun.hpp"
#include "JGrammar.hpp"
#include "VerbHelpers.hpp"
#include "utils.hpp"
#include <map>
#include <utility>

namespace J {
  using boost::mpl::_;
  using boost::shared_ptr;

  typedef std::pair<j_value_type, j_value_type> DyadArgPair;
  typedef std::map<DyadArgPair, j_value_type> DyadTypeMap;

  template <typename Prev, typename This> 
  struct fill_map_iteration {
    static void apply(DyadTypeMap *map) {
      DyadArgPair p((j_value_type)JTypeTrait<typename This::first_argument_type>::value_type,
		    (j_value_type)JTypeTrait<typename This::second_argument_type>::value_type);
      (*map)[p] = JTypeTrait<typename This::result_type>::value_type;
      Prev::apply(map);
    }
  };
  
  struct fill_map_base_case {
    static void apply(DyadTypeMap *) {}
  };
  
  template <typename List>
  struct make_type_map : boost::mpl::fold<List, fill_map_base_case, fill_map_iteration<_, _> >::type {};

  template <typename From, typename To> 
  struct ConvertJArray {
    static JArray<To> apply(const JArray<From>& from) { 
      shared_ptr<vector<To> > to(new vector<To>(from.get_dims().number_of_elems()));
      copy(from.begin(), from.end(), to->begin());
      return JArray<To>(from.get_dims(), to);
    }
  };
  
  template <typename SingleType> 
  struct ConvertJArray<SingleType, SingleType> { 
    static JArray<SingleType> apply(const JArray<SingleType>& from) {
      return from;
    }
  };

  template <typename LArg, typename RArg, typename Res, 
	    typename LArgTo=LArg, typename RArgTo=RArg, typename ResTo=Res>
  struct conversion: public std::binary_function<LArg, RArg, Res> {
    typedef LArgTo first_argument_type_to;
    typedef RArgTo second_argument_type_to;
    typedef ResTo  result_type_to;
  };

  typedef boost::mpl::list4<std::binary_function<JInt, JInt, JInt>,
			    conversion<JFloat, JInt, JFloat, JFloat, JFloat>,
			    conversion<JInt, JFloat, JFloat, JFloat>,
			    std::binary_function<JFloat, JFloat, JFloat> > sample_list;

  shared_ptr<DyadTypeMap> setup_map() { 
    shared_ptr<DyadTypeMap> m(new DyadTypeMap);
    make_type_map<sample_list>::apply(&*m);
    return m;
  }
  
  void test_func() { 
    JArray<JInt> test(Dimensions(1, 5), 1,2,3,4,5);
    ConvertJArray<JInt, JFloat>::apply(test);
  }

  
  template <typename Op>
  shared_ptr<JNoun> dyadic_apply(int lrank, int rrank, const JNoun& larg, const JNoun& rarg,
				 Op op) {
    if (lrank >= larg.get_rank() && rrank >= rarg.get_rank()) {
      return op(larg, rarg)->clone();
    }
    
    Dimensions frame = find_frame(lrank, rrank, larg.get_dims(), rarg.get_dims());
    
    std::auto_ptr<OperationIteratorBase> liter(get_operation_iterator(larg, frame, lrank));
    std::auto_ptr<OperationIteratorBase> riter(get_operation_iterator(rarg, frame, rrank));
    
    JResult res(frame);
    
    while (!liter->at_end() && !riter->at_end()) { 
      res.add_noun(op(*liter, *riter));
    }
    assert(liter->at_end() && riter->at_end());
    
    return res.assemble_result();
  }
}      
    
									 
    

#endif
