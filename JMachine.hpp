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
#include "Locale.hpp"

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
  shared_ptr<Locale> cur_locale;
  JMachine();
  
public:
  typedef shared_ptr<JMachine> Ptr;
  static Ptr new_machine();
  
  optional<JWord::Ptr> lookup_symbol(const string& sym) const;
  shared_ptr<vector<string> > list_symbols() const;

  optional<JWord::Ptr> lookup_name(const string&) const; 
  void add_public_symbol(const string& name, JWord::Ptr word);
  void add_private_symbol(const string& name, JWord::Ptr word);
};
}
#endif
