#include "JParser.hpp"

namespace J { namespace JParser { 

std::ostream& operator<<(std::ostream& os, const ParserNumberBase& p) { 
  return (os << p.to_string());
}

}}
  
