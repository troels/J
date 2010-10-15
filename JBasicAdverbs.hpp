#ifndef JBASICADVERBS_HPP
#define JBASICADVERBS_HPP

#include "JExceptions.hpp"
#include "JVerbs.hpp"
#include "JAdverbs.hpp"

namespace J {
  // class JInsertTableAdverb: public Adverb {
  //   shared_ptr<JVerb> verb;

  //   class JInsertTableVerb: public JVerb { 
  //     class MyMonad: public JMonad { 
  // 	shared_ptr<JVerb> verb;
  //     public:
  // 	MyMonad(shared_ptr<JVerb> verb): 
  // 	  JMonad(const_rank_infinity), 
  // 	  verb(verb) {}
	
  // 	shared_ptr<JNoun> operator()(const JNoun& arg) const { 
  // 	  if (arg.is_scalar() || arg.get_dims()[0] == 1) return arg->clone();

  // 	  int first_dim = arg.get_dims()[0];
  // 	  if (first_dim == 0) {
  // 	    return verb->unit(arg.get_dims().suffix(-1));
  // 	  }

  // 	  shared_ptr<JNoun> res = arg.coordinate(1, first_dims - 1);
  // 	  for (int i = first_dims - 2; i >= 0; ++i) {
  // 	    res = (*verb)(*arg.coordinate(1, i), res);
  // 	  }
  // 	  return res;
  // 	}
  //     };

  //     class MyDyad: public JDyad {
  // 	shared_ptr<JVerb> verb;
  //     public:
  // 	template <typename LArg> 
  // 	shared_ptr<JNoun> 
  // 	shared_ptr<JNoun> operator()(const JNoun& larg, const JNoun& rarg) const {
	  
	

  //   public:
  //     JInsertTableVerb(JVerb& verb): verb(verb) {}
      
  //   }
      
  //     public:
  //   shared_ptr<JWord> operator()(const JWord& word) const;
  // };
}
#endif
