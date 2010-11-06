#include "JMachine.hpp"
#include "JArithmeticVerbs.hpp"
#include "JBasicAdverbs.hpp"
#include "JBasicConjunctions.hpp"

namespace J {
JMachine::JMachine(): operators() {
  typedef pair<string, JWord::Ptr> p;

  operators.insert(p("+", JWord::Ptr(new PlusVerb())));
  operators.insert(p("-", JWord::Ptr(new MinusVerb())));
  operators.insert(p("i.", JWord::Ptr(new IDotVerb())));
  operators.insert(p("/", JWord::Ptr(new JInsertTableAdverb())));
  operators.insert(p("\"", JWord::Ptr(new RankConjunction())));
}

JMachine::Ptr JMachine::new_machine() { 
  return shared_ptr<JMachine>(new JMachine());
}

optional<JWord::Ptr> JMachine::lookup_symbol(const string& sym) const {
  map_iterator iter = operators.find(sym);

  if (iter == operators.end()) {
    return optional<JWord::Ptr>();
  }

  return optional<JWord::Ptr>(iter->second);
}


shared_ptr<vector<string>  >JMachine::list_symbols() const { 
  shared_ptr<vector<string> > strs(new vector<string>(operators.size()));
  transform(operators.begin(), operators.end(), strs->begin(),
	    attr_fun(&pair<string, JWord::Ptr>::first));
  return strs;
}
}
  
