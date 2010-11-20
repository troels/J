#include "Gerund.hpp"

namespace J {

struct is_verb_in_box {
  bool operator()(JBox& box) const {
    return box.get_contents()->get_grammar_class() == grammar_class_verb;
  }

  bool operator()(bool accum, JBox& box) const { 
    return accum && (*this)(box);
  }
};

bool is_gerund(JWord::Ptr word) {
  return 
    word->get_grammar_class() == grammar_class_noun &&
    static_cast<JNoun&>(*word).get_value_type() == j_value_type_box &&
    static_cast<JNoun&>(*word).get_dims().number_of_elems() > 0 &&
    static_cast<JNoun&>(*word).get_rank() <= 1 &&
    accumulate(static_cast<const JArray<JBox>&>(*word).begin(), static_cast<const JArray<JBox>&>(*word).end(),
	      true, is_verb_in_box());
}
}
