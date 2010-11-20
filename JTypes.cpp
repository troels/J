#include "JTypes.hpp"

namespace J {

TypeConversions::TypeConversions(): type_conversions() {
    typedef pair<j_value_type, j_value_type> p;
    
    type_conversions.insert(p(j_value_type_int, j_value_type_float));
    type_conversions.insert(p(j_value_type_int, j_value_type_complex));
    type_conversions.insert(p(j_value_type_float, j_value_type_complex));
}
 
TypeConversions::Ptr TypeConversions::get_instance() {
  static Ptr type_conversions = Ptr(new TypeConversions());
  return type_conversions;
}

bool TypeConversions::is_convertible_to(j_value_type from, j_value_type to) const { 
  for(our_map::const_iterator it(type_conversions.find(from)); it != type_conversions.end(); ++it) {
    if (it->second == to) return true;
  }
  return false;
}

optional<j_value_type> TypeConversions::find_best_type_conversion(j_value_type t1, j_value_type t2) const { 
  if (t1 == t2) return optional<j_value_type>(t1);
  if (is_convertible_to(t1, t2)) return optional<j_value_type>(t2);
  if (is_convertible_to(t2, t1)) return optional<j_value_type>(t1);
  return optional<j_value_type>();
}


JNoun::Ptr GetNounAsJArrayOfType::operator()(const JNoun& arg, j_value_type to_type) const {
  if (arg.get_value_type() == to_type) return arg.clone();
  return JArrayCaller<ConversionOp, JNoun::Ptr>()(arg, to_type);
}

}
