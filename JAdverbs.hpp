#ifndef JADVERBS_HPP
#define JADVERBS_HPP

#include "JMachine.hpp"

namespace J {
class JAdverb: public JWord {
public:
  typedef shared_ptr<JAdverb> Ptr;

  JAdverb(): JWord(grammar_class_adverb) {}
  virtual JWord::Ptr operator()(JMachine::Ptr m, JWord::Ptr word) const = 0;
};
}
#endif
