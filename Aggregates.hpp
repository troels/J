#ifndef AGGREGATES_HPP
#define AGGREGATES_HPP

#include <boost/bind.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/iterator/filter_iterator.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <cmath>
#include <vector>
#include <algorithm>
#include "JTypes.hpp"
#include "JGrammar.hpp"
#include "Dimensions.hpp"
#include "JNoun.hpp"

namespace J { namespace Aggregates { 

using boost::bind;
using boost::transform_iterator;
using boost::filter_iterator;
using boost::shared_ptr;
using boost::make_shared;
using boost::static_pointer_cast;
using boost::function;

class JResult { 
  typedef vector<JNoun::Ptr> JNounList;

  Dimensions frame;
  JNounList nouns;
  JNounList::iterator nouns_ptr;

  template <typename T>
  JNoun::Ptr assemble_result_internal(const Dimensions& dims) const;
  
  template <typename T>
  struct assemble_result_helper { 
    JNoun::Ptr operator()(const JResult& res, const Dimensions& dims) const {
      return res.assemble_result_internal<T>(dims);
    }
  };

public:
  JResult(const Dimensions& frame);
    
  void add_noun(JNoun::Ptr noun);
  JNoun::Ptr assemble_result() const;
};

template <typename Iterator>
struct get_boxed_content { 
  typedef boost::function<JNoun::Ptr (JBox)> func_t;
  typedef boost::transform_iterator<func_t, Iterator> iterator;
  typedef std::pair<iterator, iterator> result_type;
  
  result_type operator()(Iterator begin, Iterator end) const {
    return std::make_pair(get_iterator(begin), get_iterator(end));
  }
  
private:
  iterator get_iterator(Iterator iter) const {
    return iterator(iter, boost::bind(&JBox::get_contents, _1));
  }    
};

template <typename Iterator>
struct get_dimensions { 
  typedef boost::function<Dimensions (JNoun::Ptr)> func_t;
  typedef boost::transform_iterator<func_t, Iterator> iterator;
  typedef std::pair<iterator, iterator> result_type;

  result_type operator()(Iterator begin, Iterator end) const {
    return std::make_pair(get_iterator(begin), get_iterator(end));
  }

private:
  iterator get_iterator(Iterator iter) const {
    return iterator(iter, boost::bind(&JNoun::get_dims, _1));
  }
};

template <typename Iterator>
struct filter_empty { 
  typedef boost::function<bool (JNoun::Ptr)> func_t;
  typedef boost::filter_iterator<func_t, Iterator> iterator;
  typedef std::pair<iterator, iterator> result_type;

  result_type operator()(Iterator begin, Iterator end) const { 
    return std::make_pair(get_iterator(begin, end), get_iterator(end, end));
  } 

private:
  iterator get_iterator(Iterator begin, Iterator end) const {
    return iterator(boost::bind(&Dimensions::number_of_elems, boost::bind(&JNoun::get_dims, _1)) != 0,
		    begin, end);
  }
};

template <typename Iterator>
struct get_value_type { 
  typedef boost::function<j_value_type (JNoun::Ptr)> func_t;
  typedef boost::transform_iterator<func_t, Iterator> iterator;
  typedef std::pair<iterator, iterator> result_type;

  result_type operator()(Iterator begin, Iterator end) const { 
    return std::make_pair(get_iterator(begin), get_iterator(end));
  }

private:
  iterator get_iterator(Iterator iter) const {
    return iterator(iter, boost::bind(&JNoun::get_value_type, _1));
  }
};

template <typename Iterator>
optional<j_value_type> find_common_type(Iterator iter, Iterator end) {
  if (iter == end) return optional<j_value_type>();
  j_value_type res = *iter;
  for(;iter != end; ++iter) {
    optional<j_value_type> t(TypeConversions::get_instance()->find_best_type_conversion(*iter, res));
    if (!t) return optional<j_value_type>();
    res = *t;
  }
  
  return boost::make_optional(res);
}

template <typename Iterator>
optional<j_value_type> find_common_type_dropping_empty(Iterator in_begin, Iterator in_end) {
  typedef filter_empty<Iterator> empty_filter;
  typename empty_filter::result_type filtered(empty_filter()(in_begin, in_end));
  
  typedef get_value_type<typename empty_filter::iterator> value_type_iterator;
  typename value_type_iterator::result_type value_type_iter(value_type_iterator()(filtered.first, filtered.second));

  return find_common_type(value_type_iter.first, value_type_iter.second);
}
  
template <typename Iterator>
Dimensions find_common_dims_from_nouns(Iterator begin, Iterator end) {
  typedef get_dimensions<Iterator> get_dims;
  typename get_dims::result_type dims_iters(get_dims()(begin, end));
  
  return find_common_dims(dims_iters.first, dims_iters.second);
}
  
template <typename Iterator>
Dimensions find_common_dims(Iterator iter, Iterator end) { 
  assert(iter != end);
  
  shared_ptr<vector<int> > dim_cand(make_shared<vector<int> >(iter->begin(), iter->end()));
  ++iter;
  for(;iter != end; ++iter) { 
    const Dimensions& dims = *iter;
    int rank_diff = dim_cand->size() - dims.get_rank();
    if(rank_diff >= 0) {
      transform(dim_cand->begin() + rank_diff, dim_cand->end(), dims.begin(), 
		dim_cand->begin() + rank_diff, boost::bind(&std::max<int>, _1, _2));
      transform(dim_cand->begin(), dim_cand->begin() + rank_diff, 
		dim_cand->begin(), boost::bind(&std::max<int>, 1, _1));
    } else {
      dim_cand->insert(dim_cand->begin(), dims.begin(), dims.begin() - rank_diff);
      transform(dim_cand->begin(), dim_cand->begin() - rank_diff, dim_cand->begin(),
		boost::bind(&std::max<int>, _1, 1));
      transform(dim_cand->begin() - rank_diff, dim_cand->end(), dims.begin() - rank_diff,
		dim_cand->begin() - rank_diff, boost::bind(&std::max<int>, _1, _2));
    }
  }
  
  return Dimensions(dim_cand);
}

template <typename T>
struct AllocateArray { 
  template <typename Iterator>
  JNoun::Ptr operator()(Dimensions result_dims, Iterator begin, Iterator end) const {
    assert(result_dims.get_rank() > 0);
    
    shared_ptr<vector<T> > v(make_shared<vector<T> >(result_dims.number_of_elems(),
						     JTypeTrait<T>::base_elem()));
    
    typename vector<T>::iterator out_iter(v->begin());
    
    int result_rank = result_dims.get_rank();
    Dimensions item_dim(result_dims.suffix(-1));
    int elems_per_item(item_dim.number_of_elems());
    
    for(;begin != end; ++begin) {
      JArray<T> arr(require_type<T>(**begin));
      if (arr.get_rank() == 0) {
	fill_n(out_iter, elems_per_item, (*arr.begin()));
	out_iter += elems_per_item;
      } else if (arr.get_rank() == result_rank) {
	int highest_dim = arr.get_dims()[0];
	for (int i = 0; i < highest_dim; ++i) {
	  static_cast<JArray<T>&> (*arr.coordinate(1, i)).extend_into(item_dim, out_iter);
	  out_iter += elems_per_item;
	}
	
      } else { 
	arr.extend_into(item_dim, out_iter);
	out_iter += elems_per_item;
      }
    }
    
    return static_pointer_cast<JNoun>(make_shared<JArray<T> >(result_dims, v));
  }
};

template <typename Iterator>
JNoun::Ptr concatenate_nouns(Iterator in_begin, Iterator in_end) { 
  if (in_begin == in_end) {
    return JNoun::Ptr(new JArray<JInt>(Dimensions(1, 0)));
  }
  
  Dimensions dims(find_common_dims_from_nouns(in_begin, in_end));
  if (dims.get_rank() == 0) {
    dims = Dimensions(1, std::distance(in_begin, in_end));
  }
  
  optional<j_value_type> otype(find_common_type_dropping_empty(in_begin, in_end));
  j_value_type type;
  
  if(otype) {
    type = *otype;
  } else {
    if(dims.number_of_elems() != 0) 
      throw JIllegalValueTypeException();
    
    type = j_value_type_int;
  } 
  
  int highest_coord = 0;

  typedef get_dimensions<Iterator> get_dims;
  typename get_dims::result_type dims_iters(get_dims()(in_begin, in_end));
  
  for (typename get_dims::iterator iter(dims_iters.first); iter != dims_iters.second; ++iter) {
    if ((*iter).get_rank() == dims.get_rank()) {
      highest_coord += (*iter)[0];
    } else {
      ++highest_coord;
    }
  };
  
  shared_ptr<vector<int> > new_dims_vec(boost::make_shared<vector<int> >(dims.begin(), dims.end()));
  (*new_dims_vec)[0] = highest_coord;
  
  Dimensions new_dims(new_dims_vec);
  
  return JTypeDispatcher<AllocateArray, JNoun::Ptr>()(type, new_dims, in_begin, in_end);
}

}}

#endif
