#ifndef JCONJUNCTIONS_HPP
#define JCONJUNCTIONS_HPP

#include "JVerbs.hpp"
#include "VerbHelpers.hpp"
#include "JMachine.hpp"
#include <boost/weak_ptr.hpp>
namespace J {
  using boost::weak_ptr;

  class JConjunction: public JWord {
    weak_ptr<JMachine> jmachine;

  public:
    virtual ~JConjunction() {}
    JConjunction(weak_ptr<JMachine> jmachine): JWord(grammar_class_conjunction), jmachine(jmachine) {}
    virtual shared_ptr<JWord> operator()(shared_ptr<JWord> lword, shared_ptr<JWord> rword) const = 0;

    shared_ptr<JMachine> get_machine() const { 
      return jmachine.lock();
    }
  };
}

#endif
