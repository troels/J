#ifndef JMACHINE_HPP
#define JMACHINE_HPP

#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>
#include <functional>
#include "JGrammar.hpp"

namespace J {
using boost::shared_ptr;
using std::map;
using std::string;
using std::pair;
using std::vector;
using boost::optional;

class JMachine { 
  typedef map<string, JWord::Ptr>::const_iterator map_iterator;
  map<string, JWord::Ptr> operators;
  JMachine();
  
public:
  typedef shared_ptr<JMachine> Ptr;
  static Ptr new_machine();
  
  optional<JWord::Ptr> lookup_symbol(const string& sym) const;
  shared_ptr<vector<string> > list_symbols() const;
};
}
#endif
