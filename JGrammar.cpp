#include "JGrammar.hpp"

namespace J {

std::ostream& operator<<(std::ostream& os, const JWord& noun) {
  return os << noun.to_string();
}

std::ostream& operator<<(std::ostream& os, JWord::Ptr noun) {
  return os << (*noun);
}

}			  
