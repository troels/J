#include "JGrammar.hpp"
#include "JNoun.hpp"

namespace J {

std::ostream& operator<<(std::ostream& os, const JBox& b) {
  return os << "JBox[" << (b.get_contents()) << "]";
}

std::ostream& operator<<(std::ostream& os, const JWord& noun) {
  return os << noun.to_string();
}

std::ostream& operator<<(std::ostream& os, JWord::Ptr noun) {
  return os << (*noun);
}

JBox JTypeTrait<JBox>::base_elem() {
  static JBox jbox(JWord::Ptr(new JArray<JInt>(Dimensions(1, 0)))); 
  return jbox;
}

bool JBox::operator==(const JBox& box) const {
  return *box.get_contents() == *this->get_contents();
}
  
}			  
