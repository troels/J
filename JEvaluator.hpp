#ifndef JEVALUATOR_HPP
#define JEVALUATOR

#include <stdexcept>
#include <boost/shared_ptr.hpp>
#include <deque>
#include <list>
#include <vector>
#include <string>
#include <cstdarg>
#include <set>
#include "JGrammar.hpp"
#include "JNoun.hpp"
#include "JVerbs.hpp"
#include "JAdverbs.hpp"
#include "JConjunctions.hpp"

namespace J { namespace JEvaluator { 
using std::list;
using std::string;
using std::vector;
using std::set;

enum j_evaluator_type { 
  j_evaluator_type_noun,
  j_evaluator_type_verb,
  j_evaluator_type_adverb,
  j_evaluator_type_conjunction,
  j_evaluator_type_dummy,
  j_evaluator_type_name,
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
  JEvaluatorAssignment(const string& assignment);
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


void pad_evaluator(list<JEvaluatorBase::Ptr>* evaluator);


class JEvaluatorTypeClass {
  shared_ptr<set<j_evaluator_type> > types;
public:
  typedef shared_ptr<JEvaluatorTypeClass> Ptr;
  JEvaluatorTypeClass(int nr, ...);
  virtual ~JEvaluatorTypeClass();

  bool contains(j_evaluator_type t) const; 
};

extern const JEvaluatorTypeClass::Ptr EDGE; 
extern const JEvaluatorTypeClass::Ptr VERB;
extern const JEvaluatorTypeClass::Ptr NOUN;
extern const JEvaluatorTypeClass::Ptr ADVERB;
extern const JEvaluatorTypeClass::Ptr VERBNOUN;
extern const JEvaluatorTypeClass::Ptr ANY;
extern const JEvaluatorTypeClass::Ptr ASGN;
extern const JEvaluatorTypeClass::Ptr CAVN;
extern const JEvaluatorTypeClass::Ptr NAME;

class JRule {
  JEvaluatorTypeClass::Ptr first;
  JEvaluatorTypeClass::Ptr second;
  JEvaluatorTypeClass::Ptr third;
  JEvaluatorTypeClass::Ptr fourth;

public:
  JRule(JEvaluatorTypeClass::Ptr first, JEvaluatorTypeClass::Ptr second,
	JEvaluatorTypeClass::Ptr third, JEvaluatorTypeClass::Ptr fourth): 
    first(first), second(second), third(third), fourth(fourth) {}
  
  template <typename Iterator>
  bool match(Iterator begin) {
    return first->contains(*begin) && second->contains(*(begin + 1)) &&
           third->contains(*(begin + 2)) && fourth->contains(*(begin + 3));
  }
};
  
  
}}

#endif
