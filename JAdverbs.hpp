#ifndef JADVERBS_HPP
#define JADVERBS_HPP

#include "JMachine.hpp"
#include <boost/weak_ptr.hpp>
namespace J {
  using boost::weak_ptr;

  class JAdverb: public JWord {
    weak_ptr<JMachine> jmachine;
    
  public:
    JAdverb(weak_ptr<JMachine> jmachine): JWord(grammar_class_adverb), jmachine(jmachine) {}
    virtual shared_ptr<JWord> operator()(shared_ptr<JWord> word) const = 0;
    
    shared_ptr<JMachine> get_machine() const { 
      return jmachine.lock();
    }
  };
}
#endif
