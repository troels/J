#ifndef JADVERBS_HPP
#define JADVERBS_HPP

#include "JMachine.hpp"

namespace J {
class JAdverb: public JWord {
public:
  JAdverb(): JWord(grammar_class_adverb) {}
  virtual shared_ptr<JWord> operator()(shared_ptr<JMachine> m, shared_ptr<JWord> word) const = 0;
};
}
#endif
