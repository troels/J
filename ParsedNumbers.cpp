#include "ParsedNumbers.hpp"

namespace J { namespace JParser { 

int get_j_value_type_place_in_hierarchy(j_value_type t) { 
  for (unsigned i = 0; i < (sizeof(j_number_type_hierarchy)/sizeof(j_value_type)); ++i) {
    if (t == j_number_type_hierarchy[i]) return i;
  } 
  
  throw std::logic_error("Never end here");
}

j_value_type highest_j_value(ParsedNumberBase::Ptr a, j_value_type type) {
  int jval_a = get_j_value_type_place_in_hierarchy(a->get_value_type());
  int jval_b = get_j_value_type_place_in_hierarchy(type);
    
  if (jval_a <= jval_b) { 
    return type;
  } 
  return a->get_value_type();
}




template <typename Number>
ParsedNumberBase::Ptr ParsedNumber<Number>::convert_to(j_value_type type) const {
  if (type == j_value_type_int) { 
    return ParsedNumberBase::Ptr(new ParsedNumber<JInt>(ConvertNumber<Number, JInt>::convert(get_nr())));
  } else if (type == j_value_type_float) { 
    return ParsedNumberBase::Ptr(new ParsedNumber<JFloat>(ConvertNumber<Number, JFloat>::convert(get_nr())));
  } else { 
    throw std::logic_error("Invalid type conversion to type.");
  } 
}

template <typename Number>
string ParsedNumber<Number>::to_string() const { 
  std::ostringstream str;
  str << get_nr();
  return str.str();
}

template <typename Number>
bool ParsedNumber<Number>::operator==(const ParsedNumberBase& that) const {
  return get_value_type() == that.get_value_type() && 
    static_cast<const ParsedNumber<Number>& >(that).get_nr() == get_nr();
}

template class ParsedNumber<JInt>;
template class ParsedNumber<JFloat>;
}}
