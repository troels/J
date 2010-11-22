#ifndef JTYPES_HPP
#define JTYPES_HPP

#include "JGrammar.hpp"
#include "JExceptions.hpp"
#include "JNoun.hpp"
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>
#include <vector>
#include <utility>

namespace J {
using std::multimap;
using std::vector;
using boost::shared_ptr;
using boost::optional;
using std::pair;

template <template <typename> class Op, typename Ret>
struct JTypeDispatcher { 
  Ret operator()(j_value_type t) const { 
    switch (t) {
    case j_value_type_int:
      return Op<JInt>()();
    case j_value_type_float:
      return Op<JFloat>()();
    case j_value_type_box:
      return Op<JBox>()();
    default:
      throw JIllegalValueTypeException();
    }
  }
  
  template <typename Arg1>
  Ret operator()(j_value_type t, const Arg1& arg) const { 
    switch (t) {
    case j_value_type_int:
      return Op<JInt>()(arg);
    case j_value_type_float:
      return Op<JFloat>()(arg);
    case j_value_type_box:
      return Op<JBox>()(arg);
    default:
      throw JIllegalValueTypeException();
    }
  }

  template <typename Arg1, typename Arg2>
  Ret operator()(j_value_type t, const Arg1& arg1, const Arg2& arg2) const { 
    switch (t) {
    case j_value_type_int:
      return Op<JInt>()(arg1, arg2);
    case j_value_type_float:
      return Op<JFloat>()(arg1, arg2);
    case j_value_type_box:
      return Op<JBox>()(arg1, arg2);
    default:
      throw JIllegalValueTypeException();
    }
  }


  template <typename Arg1, typename Arg2, typename Arg3>
  Ret operator()(j_value_type t, const Arg1& arg1, const Arg2& arg2, const Arg3& arg3) const { 
    switch (t) {
    case j_value_type_int:
      return Op<JInt>()(arg1, arg2, arg3);
    case j_value_type_float:
      return Op<JFloat>()(arg1, arg2, arg3);
    case j_value_type_box:
      return Op<JBox>()(arg1, arg2, arg3);
    default:
      throw JIllegalValueTypeException();
    }
  }

  template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
  Ret operator()(j_value_type t, const Arg1& arg1, const Arg2& arg2, const Arg3& arg3, const Arg4& arg4) const { 
    switch (t) {
    case j_value_type_int:
      return Op<JInt>()(arg1, arg2, arg3, arg4);
    case j_value_type_float:
      return Op<JFloat>()(arg1, arg2, arg3, arg4);
    case j_value_type_box:
      return Op<JBox>()(arg1, arg2, arg3, arg4);
    default:
      throw JIllegalValueTypeException();
    }
  }

  template <typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
  Ret operator()(j_value_type t, const Arg1& arg1, const Arg2& arg2, const Arg3& arg3, const Arg4& arg4, 
		 const Arg5& arg5) const { 
    switch (t) {
    case j_value_type_int:
      return Op<JInt>()(arg1, arg2, arg3, arg4, arg5);
    case j_value_type_float:
      return Op<JFloat>()(arg1, arg2, arg3, arg4, arg5);
    case j_value_type_box:
      return Op<JBox>()(arg1, arg2, arg3, arg4, arg5);
    default:
      throw JIllegalValueTypeException();
    }
  }

};

template <template <typename> class Op, typename Ret>
struct JArrayCaller { 
  template <typename T>
  struct Converter {
    Ret operator()(const JNoun& noun) const {
      return Op<T>()(static_cast<const JArray<T>&>(noun));
    }

    template <typename Arg1>
    Ret operator()(const JNoun& noun, Arg1& arg1) const {
      return Op<T>()(static_cast<const JArray<T>&>(noun), arg1);
    }

    template <typename Arg1, typename Arg2>
    Ret operator()(const JNoun& noun, Arg1& arg1, Arg2& arg2) const {
      return Op<T>()(static_cast<const JArray<T>&>(noun), arg1, arg2);
    }

    template <typename Arg1, typename Arg2, typename Arg3>
    Ret operator()(const JNoun& noun, Arg1& arg1, Arg2& arg2, Arg3& arg3) const {
      return Op<T>()(static_cast<const JArray<T>&>(noun), arg1, arg2, arg3);
    }
  };
      
  Ret operator()(const JNoun& noun) const {
    return JTypeDispatcher<Converter, Ret>()(noun.get_value_type(), noun);
  };
 
  template <typename Arg1>
  Ret operator()(const JNoun& noun, Arg1& arg1) const { 
    return JTypeDispatcher<Converter, Ret>()(noun.get_value_type(), noun, arg1);
  }

  template <typename Arg1, typename Arg2>
  Ret operator()(const JNoun& noun, Arg1& arg1, Arg2& arg2) const { 
    return JTypeDispatcher<Converter, Ret>()(noun.get_value_type(), noun, arg1, arg2);
  }

  template <typename Arg1, typename Arg2, typename Arg3>
  Ret operator()(const JNoun& noun, Arg1& arg1, Arg2& arg2, Arg3& arg3) const { 
    return JTypeDispatcher<Converter, Ret>()(noun.get_value_type(), noun, arg1, arg2, arg3);
  }
};

template <typename From, typename To> 
struct ConvertType { 
  To operator()(From) const { 
    throw JIllegalTypeCastException();
  }
};

template <typename Arg>
struct ConvertType<Arg, Arg> {
  Arg operator()(Arg arg) const { 
    return arg;
  }
};


template <>
struct ConvertType<JInt, JFloat> {
  JFloat operator()(JInt from) {
    return static_cast<JFloat>(from);
  }
};

template <typename From>
struct CurriedConvertType {
  template <typename To>
  struct Impl {
    To operator()(From from) const {
      return ConvertType<From, To>()(from);
    }
  };
};
    
template <typename From, typename To>
struct ConvertJArray {
  shared_ptr<JArray<To> > operator()(const JArray<From>& from) const {
    shared_ptr<vector<To> > to(new vector<To>(from.get_dims().number_of_elems(),
					     JTypeTrait<To>::base_elem()));
    transform(from.begin(), from.end(), to->begin(), ConvertType<From, To>());
    return shared_ptr<JArray<To> >(new JArray<To>(from.get_dims(), to));
  }
};

class TypeConversions { 
  typedef multimap<j_value_type, j_value_type> our_map;
  our_map type_conversions;

  TypeConversions();

public:
  typedef shared_ptr<TypeConversions> Ptr;
  
  static Ptr get_instance();
    
  bool is_convertible_to(j_value_type from, j_value_type to) const;
  optional<j_value_type> find_best_type_conversion(j_value_type t1, j_value_type t2) const;
};

template <typename From>
struct ConversionOpPerformer {
  template <typename To>
  struct Impl {
    JNoun::Ptr operator()(const JArray<From>& from) const {
      return boost::static_pointer_cast<JNoun>(ConvertJArray<From, To>()(from));
    }
  };
};

template <typename T>
struct ConversionOp { 
  JNoun::Ptr operator()(const JArray<T>& from, j_value_type to_type) const { 
    return JTypeDispatcher<ConversionOpPerformer<T>::template Impl, JNoun::Ptr>()(to_type, from);
  }
};

struct GetNounAsJArrayOfType {
  JNoun::Ptr operator()(const JNoun& arg, j_value_type to_type) const;
};

template <template <typename> class Op, typename Res>
struct DualArrayConverter { 
  template <typename T>
  struct Impl {
    Res operator()(const JNoun& larg, const JNoun& rarg) const { 
      return Op<T>()(static_cast<const JArray<T>&>(larg), 
		     static_cast<const JArray<T>&>(rarg));
    }

    template <typename Arg1>
    Res operator()(const JNoun& larg, const JNoun& rarg, const Arg1& arg1) const { 
      return Op<T>()(static_cast<const JArray<T>&>(larg), 
		     static_cast<const JArray<T>&>(rarg),
		     arg1);
    }

    template <typename Arg1, typename Arg2>
    Res operator()(const JNoun& larg, const JNoun& rarg, const Arg1& arg1, const Arg2& arg2) const { 
      return Op<T>()(static_cast<const JArray<T>&>(larg), 
		     static_cast<const JArray<T>&>(rarg),
		     arg1, arg2);
    }

    template <typename Arg1, typename Arg2, typename Arg3>
    Res operator()(const JNoun& larg, const JNoun& rarg, 
		   const Arg1& arg1, const Arg2& arg2, const Arg3& arg3) const { 
      return Op<T>()(static_cast<const JArray<T>&>(larg), 
		     static_cast<const JArray<T>&>(rarg), 
		     arg1, arg2, arg3);
    }

    template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
    Res operator()(const JNoun& larg, const JNoun& rarg, 
		   const Arg1& arg1, const Arg2& arg2, const Arg3& arg3, Arg4& arg4) const { 
      return Op<T>()(static_cast<const JArray<T>&>(larg), static_cast<const JArray<T>&>(rarg), 
		     arg1, arg2, arg3, arg4);
    }

  };
};

template <template <typename> class Op, typename Res>
struct CallWithCommonType { 
  typedef pair<JNoun::Ptr, JNoun::Ptr> JNounCouple;

  JNounCouple do_conversion(const JNoun& larg, const JNoun& rarg) const { 
    optional<j_value_type> type(TypeConversions::get_instance()
				->find_best_type_conversion(larg.get_value_type(),
							    rarg.get_value_type()));
    if (!type) throw JIllegalValueTypeException();
    JNoun::Ptr larg_right_type(GetNounAsJArrayOfType()(larg, *type));
    JNoun::Ptr rarg_right_type(GetNounAsJArrayOfType()(rarg, *type));
    return JNounCouple(larg_right_type, rarg_right_type);
  }

  Res operator()(const JNoun& larg, const JNoun& rarg) const { 
    JNounCouple convertee(do_conversion(larg, rarg));
    return JTypeDispatcher<DualArrayConverter<Op, Res>::template Impl, Res>()(convertee.first->get_value_type(),
									      *convertee.first, *convertee.second);
  }
	
  template <typename Arg1>
  Res operator()(const JNoun& larg, const JNoun& rarg, const Arg1& arg1) {
    JNounCouple convertee(do_conversion(larg, rarg));
    return JTypeDispatcher<DualArrayConverter<Op, Res>::template Impl, Res>()(convertee.first->get_value_type(),
				    *convertee.first, *convertee.second, arg1);
  }

  template <typename Arg1, typename Arg2>
  Res operator()(const JNoun& larg, const JNoun& rarg, const Arg1& arg1, const Arg2& arg2) {
    JNounCouple convertee(do_conversion(larg, rarg));
    return JTypeDispatcher<DualArrayConverter<Op, Res>::template Impl, Res>()(convertee.first->get_value_type(),
				    *convertee.first, *convertee.second, arg1, arg2);
  }

  template <typename Arg1, typename Arg2, typename Arg3>
  Res operator()(const JNoun& larg, const JNoun& rarg, const Arg1& arg1, 
		 const Arg2& arg2, const Arg3& arg3) {
    JNounCouple convertee(do_conversion(larg, rarg));
    return JTypeDispatcher<DualArrayConverter<Op, Res>::template Impl, Res>()
			   (convertee.first->get_value_type(),
			    *convertee.first, *convertee.second, arg1, arg2, arg3);
  }

  template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
  Res operator()(const JNoun& larg, const JNoun& rarg, const Arg1& arg1, 
		 const Arg2& arg2, const Arg3& arg3, const Arg4& arg4) {
    JNounCouple convertee(do_conversion(larg, rarg));
    return JTypeDispatcher<DualArrayConverter<Op, Res>::template Impl, Res>()
			   (convertee.first->get_value_type(),
			    *convertee.first, *convertee.second, arg1, arg2, arg3, arg4);
  }
};

template <typename T>
JArray<T> require_type(const JNoun& noun);
}

#endif
