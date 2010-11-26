#include "ParsedNumbers.hpp"

namespace J { namespace JParser { 
      
template <typename Number>
ParsedNumberBase::Ptr ParsedNumber<Number>::convert_to(j_value_type type) const {
  return JTypeDispatcher<ParsedNumberConverter<Number>::template Impl, ParsedNumberBase::Ptr>()(type, get_nr());
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
template class ParsedNumber<JComplex>;
}}
