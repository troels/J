#ifndef JCONJUNCTIONS_HPP
#define JCONJUNCTIONS_HPP

#include "JVerbs.hpp"
#include "VerbHelpers.hpp"
#include "JMachine.hpp"

namespace J {
class JConjunction: public JWord {
public:
  virtual ~JConjunction() {}
  JConjunction(): JWord(grammar_class_conjunction) {}
  virtual shared_ptr<JWord> operator()(shared_ptr<JMachine> m, shared_ptr<JWord> lword, 
				       shared_ptr<JWord> rword) const = 0;
};
}

#endif
