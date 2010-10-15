#ifndef JADVERBS_HPP
#define JADVERBS_HPP

namespace J {
  class JAdverb: public JWord {
  public:
    virtual ~JAdverb() {}
    JAdverb(): JWord(grammar_class_adverb) {}
    virtual shared_ptr<JWord> operator()(shared_ptr<JWord> word) const = 0;
  };
}
#endif
