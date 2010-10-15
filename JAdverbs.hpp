#ifndef JADVERBS_HPP
#define JADVERBS_HPP

namespace J {
  class JAdverb: public JWord {
  public:
    JAdverb(): JWord(grammar_class_adverb) {}
    virtual shared_ptr<JWord> operator()(const JWord& word) const = 0;
  };
}
#endif
