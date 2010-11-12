#include "JEvaluator.hpp"

namespace J { namespace JEvaluator {

JEvaluatorBase::~JEvaluatorBase() {}

JEvaluatorTypeClass::JEvaluatorTypeClass(int nr, ...): types(new set<j_evaluator_type>()) {
  va_list va;
  
  va_start(va, nr);
  while (nr--) { 
    types->insert(static_cast<j_evaluator_type>(va_arg(va, int)));
  }
  
  va_end(va);
}

bool JEvaluatorTypeClass::contains(j_evaluator_type t) const { 
  return find(types->begin(), types->end(), t) != types->end();
}

JEvaluatorTypeClass::~JEvaluatorTypeClass() {}

const JEvaluatorTypeClass::Ptr EDGE(new JEvaluatorTypeClass(3, j_evaluator_type_dummy, 
							       j_evaluator_type_sequence_start,
							    j_evaluator_type_assignment));
const JEvaluatorTypeClass::Ptr VERB(new JEvaluatorTypeClass(1, j_evaluator_type_verb));
const JEvaluatorTypeClass::Ptr NOUN(new JEvaluatorTypeClass(1, j_evaluator_type_noun));
const JEvaluatorTypeClass::Ptr ADVERB(new JEvaluatorTypeClass(1, j_evaluator_type_adverb));
const JEvaluatorTypeClass::Ptr VERBNOUN(new JEvaluatorTypeClass(2, j_evaluator_type_verb, j_evaluator_type_noun));
const JEvaluatorTypeClass::Ptr ANY(new JEvaluatorTypeClass(7, j_evaluator_type_verb, j_evaluator_type_noun,
							      j_evaluator_type_adverb, j_evaluator_type_assignment,
							      j_evaluator_type_conjunction, 
							      j_evaluator_type_sequence_start,
							      j_evaluator_type_dummy));
const JEvaluatorTypeClass::Ptr ASGN(new JEvaluatorTypeClass(1, j_evaluator_type_assignment));
const JEvaluatorTypeClass::Ptr CAVN(new JEvaluatorTypeClass(4, j_evaluator_type_conjunction, j_evaluator_type_adverb,
							       j_evaluator_type_verb, j_evaluator_type_noun));


JEvaluatorAssignment::JEvaluatorAssignment(const string& assignment) {
  if (assignment == "=:") {
    type = assignment_type_public;
  } else if (assignment == "=.") {
    type = assignment_type_local;
  } else {
    throw std::logic_error("Unknown assignment operator: " + assignment);
  }
}

void pad_evaluator(list<JEvaluatorBase::Ptr>* evaluator) { 
  while (evaluator->size() < 4) {
    evaluator->push_back(JEvaluatorDummy::Instantiate());
  }
  while(evaluator->size() > 4 && evaluator->back()->get_j_evaluator_type() == j_evaluator_type_dummy) {
    evaluator->pop_back();
  }
}


}}
