#ifndef JTYPES_HPP
#define JTYPES_HPP

#include "JGrammar.hpp"
#include "JExceptions.hpp"
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
  Ret operator()(j_value_type t, Arg1& arg1, Arg2& arg2) const { 
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
  Ret operator()(j_value_type t, Arg1& arg1, Arg2& arg2, Arg3& arg3) const { 
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
  Ret operator()(j_value_type t, Arg1& arg1, Arg2& arg2, Arg3& arg3, Arg4& arg4) const { 
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

  TypeConversions(): type_conversions() {
    typedef pair<j_value_type, j_value_type> p;
    
    type_conversions.insert(p(j_value_type_int, j_value_type_float));
    type_conversions.insert(p(j_value_type_int, j_value_type_complex));
    type_conversions.insert(p(j_value_type_float, j_value_type_complex));
  }

public:
  typedef shared_ptr<TypeConversions> Ptr;
  
  static Ptr get_instance() { 
    static Ptr type_conversions = Ptr(new TypeConversions());
    return type_conversions;
  }
    
  bool is_convertible_to(j_value_type from, j_value_type to) const { 
    for(our_map::const_iterator it(type_conversions.find(from)); it != type_conversions.end(); ++it) {
      if (it->second == to) return true;
    }
    return false;
  }

  optional<j_value_type> find_best_type_conversion(j_value_type t1, j_value_type t2) const { 
    if (t1 == t2) return optional<j_value_type>(t1);
    if (is_convertible_to(t1, t2)) return optional<j_value_type>(t2);
    if (is_convertible_to(t2, t1)) return optional<j_value_type>(t1);
    return optional<j_value_type>();
  }
};

template <typename T>
struct ConversionOp { 
  JNoun::Ptr operator()(const JArray<T>& from, j_value_type to_type) const { 
    switch(to_type) {
    case j_value_type_int:
      return boost::static_pointer_cast<JNoun>(ConvertJArray<T, JInt>()(from));
    case j_value_type_float:
      return boost::static_pointer_cast<JNoun>(ConvertJArray<T, JFloat>()(from));
    case j_value_type_complex:
      return boost::static_pointer_cast<JNoun>(ConvertJArray<T, JComplex>()(from));
    case j_value_type_char:
      return boost::static_pointer_cast<JNoun>(ConvertJArray<T, JChar>()(from));
    case j_value_type_box:
      return boost::static_pointer_cast<JNoun>(ConvertJArray<T, JBox>()(from));
    default:
      throw JIllegalTypeCastException();
    }
  }
};

struct GetNounAsJArray {
  JNoun::Ptr operator()(const JNoun& arg, j_value_type to_type) const {
    if (arg.get_value_type() == to_type) return arg.clone();
    return JArrayCaller<ConversionOp, JNoun::Ptr>()(arg, to_type);
  }
};
}  

#endif
