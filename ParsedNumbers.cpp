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

shared_ptr<JNoun> create_jarray(j_value_type value_type, VecPtr<ParsedNumberBase::Ptr>::type vec) {
  typename VecPtr<ParsedNumberBase::Ptr>::type v2(new vector<ParsedNumberBase::Ptr>(vec->size()));
  
  vector<ParsedNumberBase::Ptr>::iterator iter = v2->begin();
  BOOST_FOREACH(ParsedNumberBase::Ptr p, *vec) { 
    *iter = p->convert_to(value_type);
    ++iter;
  }

  switch (value_type) { 
  case j_value_type_int:
    return create_noun<JInt>(v2);
  case j_value_type_float:
    return create_noun<JFloat>(v2);
  default:
    throw std::logic_error("Clauses missing");
  }
}

template <typename T>
shared_ptr<JNoun> create_noun(const VecPtr<ParsedNumberBase::Ptr>::type& vec) {
  shared_ptr<vector<T> > v(new vector<T>());
  BOOST_FOREACH(ParsedNumberBase::Ptr& p, *vec) { 
    v->push_back(static_cast<ParsedNumber<T>*>(p.get())->get_nr());
  }
  return shared_ptr<JNoun>(new JArray<T>(Dimensions(1, vec->size()), v));
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
