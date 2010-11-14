#ifndef JCONJUNCTIONS_HPP
#define JCONJUNCTIONS_HPP

#include "JVerbs.hpp"
#include "VerbHelpers.hpp"
#include "JMachine.hpp"

namespace J {
class JConjunction: public JWord {
public:
  typedef shared_ptr<JConjunction> Ptr;
  virtual ~JConjunction() {}
  JConjunction(): JWord(grammar_class_conjunction) {}
  virtual JWord::Ptr operator()(JMachine::Ptr m, JWord::Ptr lword, JWord::Ptr rword) const = 0;
};
}

#endif
