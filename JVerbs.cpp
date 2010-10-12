#include "JVerbs.hpp"

namespace J {
  JResultBase::JResultBase(const Dimensions& frame, j_value_type value_type):
    frame(frame), value_type(value_type), nouns(JNounList(frame.number_of_elems())),
    nouns_ptr(nouns.begin()), max_dims(),  rank() {}
  
  void JResultBase::add_noun(const JNoun& noun) { 
    assert(nouns_ptr != nouns.end());

    if (get_value_type() != noun.get_value_type()) 
      throw JIllegalValueTypeException();
    
    if (!rank) {
      rank = noun.get_rank();
      max_dims = shared_ptr<vector<int> >(new vector<int>(noun.get_dims().begin(), noun.get_dims().end()));
    } else {
      if (*rank != noun.get_rank()) 
	throw JIllegalRankException();

      vector<int>::iterator max_dims_ptr = max_dims->begin();
      vector<int>::iterator noun_ptr = noun.get_dims().begin();
      vector<int>::iterator noun_end = noun.get_dims().end();
      
      for (;noun_ptr != noun_end; ++noun_ptr, ++max_dims_ptr) {
	*max_dims_ptr = std::max(*max_dims_ptr, *noun_ptr);
      }
    }
    
    *nouns_ptr = noun.clone();
    ++nouns_ptr;
  }
  
  template <typename T>
  JResult<T>::JResult(const Dimensions& frame): 
    JResultBase(frame, JTypeTrait<T>::value_type) {}
  
  template <typename T>
  shared_ptr<JNoun> JResult<T>::assemble_result() const {
    Dimensions content_dims(get_max_dims());
    Dimensions res = get_frame() + content_dims;
    shared_ptr<vector<T> > v(new vector<T>(res.number_of_elems(), JTypeTrait<T>::base_elem()));
    
    typename vector<T>::iterator ptr = v->begin();
    typename JNounList::const_iterator nounlist_ptr = get_nouns().begin();
    typename JNounList::const_iterator nounlist_end = get_nouns().end();
    
    int content_dims_len = content_dims.number_of_elems();
    for (;nounlist_ptr != nounlist_end; ++nounlist_ptr, ptr += content_dims_len) {
      boost::static_pointer_cast<JArray<T>, JNoun>(*nounlist_ptr)->extend_into(content_dims, ptr);
    }
    
    return shared_ptr<JNoun>(new JArray<T>(res, v));
  }
    
  Dimensions find_frame(int lrank, int rrank, const Dimensions& larg, const Dimensions& rarg) {
    if (larg.get_rank() <= lrank && rarg.get_rank() <= rrank)  {
      return Dimensions(0);
    } else if (larg.get_rank() <= lrank) {
      return rarg.prefix(rarg.get_rank() - rrank);
    } else if (rarg.get_rank() <= rrank) {
      return larg.prefix(larg.get_rank() - lrank);
    }
    
    Dimensions lframe = larg.prefix(larg.get_rank() - lrank);
    Dimensions rframe = rarg.prefix(rarg.get_rank() - rrank);

    if (lframe.get_rank() > rframe.get_rank()) {
      if (lframe.prefix_match(rframe)) {
	return lframe;
      } else {
	throw JIllegalDimensionsException("Incompatible arguments given");
      }
    } else {
      if (rframe.prefix_match(lframe)) {
	return rframe;
      } else {
	throw JIllegalDimensionsException("Incompatible arguments given");
      }
    }
  }

  
  template class JResult<JInt>;
}

  
