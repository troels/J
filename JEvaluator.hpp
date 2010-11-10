#ifndef JEVALUATOR_HPP
#define JEVALUATOR

#include <stdexcept>
#include <boost/shared_ptr.hpp>
#include <deque>
#include <vector>
#include <string>
#include "JGrammar.hpp"
#include "JNoun.hpp"
#include "JVerbs.hpp"
#include "JAdverbs.hpp"
#include "JConjunctions.hpp"

namespace J { namespace JEvaluator { 
using std::deque;
using std::string;
using std::vector;

enum j_evaluator_type { 
  j_evaluator_type_noun,
  j_evaluator_type_verb,
  j_evaluator_type_adverb,
  j_evaluator_type_conjunction,
  j_evaluator_type_dummy,
  j_evaluator_type_sequence_start,
  j_evaluator_type_assignment 
};

class JEvaluatorBase {
  j_evaluator_type type;
public:
  typedef shared_ptr<JEvaluatorBase> Ptr;
  
  JEvaluatorBase(j_evaluator_type type): type(type) {}
  virtual ~JEvaluatorBase() = 0;
  j_evaluator_type get_j_evaluator_type() const { return type; }
};

template <typename T>
struct JEvaluatorTraits { 
  static const j_evaluator_type evaluator_type;
  static const j_grammar_class grammar_class;
};

template <> 
struct JEvaluatorTraits<JNoun> { 
  static const j_evaluator_type evaluator_type = j_evaluator_type_noun;
  static const j_grammar_class grammar_class = grammar_class_noun;
};


template <> 
struct JEvaluatorTraits<JVerb> { 
  static const j_evaluator_type evaluator_type = j_evaluator_type_verb;
  static const j_grammar_class grammar_class = grammar_class_verb;
};

template <> 
struct JEvaluatorTraits<JAdverb> { 
  static const j_evaluator_type evaluator_type = j_evaluator_type_adverb;
  static const j_grammar_class grammar_class = grammar_class_adverb;
};

template <> 
struct JEvaluatorTraits<JConjunction> { 
  static const j_evaluator_type evaluator_type = j_evaluator_type_conjunction;
  static const j_grammar_class grammar_class = grammar_class_conjunction;
};


template <typename T>
class JEvaluatorContainer: public JEvaluatorBase { 
  JWord::Ptr elem;

public:
  JEvaluatorContainer(JWord::Ptr elem): JEvaluatorBase(JEvaluatorTraits<T>::evaluator_type), elem(elem) {
    assert(elem->get_grammar_class() == JEvaluatorTraits<T>::grammar_class);
  }
  
  shared_ptr<T> get_word() const { 
    return boost::static_pointer_cast<T>(elem);
  }
};

enum assignment_type { assignment_type_local,
		       assignment_type_public };

class JEvaluatorAssignment { 
  assignment_type type;

public:
  JEvaluatorAssignment(const string& assignment) {
    if (assignment == "=:") {
      type = assignment_type_public;
    } else if (assignment == "=.") {
      type = assignment_type_local;
    } else {
      throw std::logic_error("Unknown assignment operator: " + assignment);
    }
  }
};

class JEvaluatorDummy: public JEvaluatorBase { 
public:
  typedef JEvaluatorBase::Ptr Ptr;
  
  static Ptr Instantiate() { 
    return Ptr(new JEvaluatorDummy());
  }

  JEvaluatorDummy(): JEvaluatorBase(j_evaluator_type_dummy) {}
};

class JEvaluatorSequenceStart: public JEvaluatorBase { 
public:
  JEvaluatorSequenceStart(): JEvaluatorBase(j_evaluator_type_sequence_start) {}
};


void pad_evaluator(deque<JEvaluatorBase::Ptr>* evaluator) { 
  if (evaluator->size() < 4) {
    while (i = 4; i > evaluator->size(); --i) {
      evaluator.push_back(JEvaluatorDummy::Instantiate());
    }
  } else {
    while(evaluator.size() > 4 && evaluator.back()->get_j_evaluator_type() == j_evaluator_type_dummy) {
      evaluator.pop_back();
    }
  }
}


void evaluate(deque<JEvaluatorBase::Ptr>* evaluator) { 
  pad_evaluator(evaluator);
  
}
  
  
  
class JEvaluator { 
  deque<JEvaluatorBase> evaluator;
public:
  
  
};
}}

#endif
