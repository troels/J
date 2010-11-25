#include "JMachine.hpp"
#include "JArithmeticVerbs.hpp"
#include "JBasicAdverbs.hpp"
#include "JBasicConjunctions.hpp"
#include "ShapeVerbs.hpp"

namespace J {
JMachine::JMachine(): operators(), cur_locale(Locale::Instantiate()) {
  typedef pair<string, JWord::Ptr> p;

  operators.insert(p("+", JWord::Ptr(new PlusVerb())));
  operators.insert(p("-", JWord::Ptr(new MinusVerb())));
  operators.insert(p("i.", JWord::Ptr(new IDotVerb())));
  operators.insert(p("/", JWord::Ptr(new JInsertTableAdverb())));
  operators.insert(p("\"", JWord::Ptr(new RankConjunction())));
  operators.insert(p("\\", JWord::Ptr(new PrefixInfixAdverb())));
  operators.insert(p("$", JWord::Ptr(new ShapeVerb())));
  operators.insert(p(",", JWord::Ptr(new RavelAppendVerb())));
  operators.insert(p(";", JWord::Ptr(new RazeLinkVerb())));
  operators.insert(p("*", JWord::Ptr(new SignumTimesVerb())));
  operators.insert(p("%", JWord::Ptr(new ReciprocalDivideVerb())));
  operators.insert(p("<", JWord::Ptr(new LessBoxVerb())));
  operators.insert(p(">", JWord::Ptr(new MoreUnboxVerb())));
  operators.insert(p("<.", JWord::Ptr(new FloorLesserofVerb())));
  operators.insert(p(">.", JWord::Ptr(new CeilingGreaterofVerb())));
  operators.insert(p("<:", JWord::Ptr(new DecrementLessequalVerb())));
  operators.insert(p(">:", JWord::Ptr(new IncrementMoreequalVerb())));
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


shared_ptr<vector<string> > JMachine::list_symbols() const { 
  shared_ptr<vector<string> > strs(new vector<string>(operators.size()));
  transform(operators.begin(), operators.end(), strs->begin(),
	    attr_fun(&pair<string, JWord::Ptr>::first));
  return strs;
}

optional<JWord::Ptr> JMachine::lookup_name(const string& name) const {
  return cur_locale->lookup_symbol(name);
}

void JMachine::add_public_symbol(const string& name, JWord::Ptr word) {
  cur_locale->add_public_symbol(name, word);
}

void JMachine::add_private_symbol(const string& name, JWord::Ptr word) {
  cur_locale->add_private_symbol(name, word);
}
}
  
