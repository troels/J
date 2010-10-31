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

shared_ptr<JMachine> JMachine::new_machine() { 
  return shared_ptr<JMachine>(new JMachine());
}
}
  
