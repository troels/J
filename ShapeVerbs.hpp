#ifndef SHAPEVERBS_HPP
#define SHAPEVERBS_HPP

#include "JVerbs.hpp"
#include "JNoun.hpp"
#include "VerbHelpers.hpp"
#include "JExceptions.hpp"
#include <boost/bind.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/iterator/filter_iterator.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

namespace J {

template <typename T>
struct ShapeDyadOp {
  JNoun::Ptr operator()(const JArray<T>& rarg, const JNoun& noun) const;
};
    
class ShapeVerb : public JVerb {
  struct MonadOp { 
    JNoun::Ptr operator()(JMachine::Ptr, const JNoun& arg) const;
  };

  struct DyadOp {
    JNoun::Ptr operator()(JMachine::Ptr, const JNoun& larg, const JNoun& rarg) const;
  };

public:
  ShapeVerb(): JVerb(DefaultMonad<MonadOp>::Instantiate(rank_infinity, MonadOp()),
		     DefaultDyad<DyadOp>::Instantiate(1, rank_infinity, DyadOp())) {}
};

template <typename T>
struct RavelOp {
  JNoun::Ptr operator()(const JArray<T>& arg) const;
};

template <typename T>
struct AppendOp {
  JNoun::Ptr operator()(const JArray<T>& larg, const JNoun& rarg_, JMachine::Ptr) const;
};

class RavelAppendVerb: public JVerb { 
  struct MonadOp {
    JNoun::Ptr operator()(JMachine::Ptr, const JNoun& arg) const;
  };

  struct DyadOp {
    JNoun::Ptr operator()(JMachine::Ptr m, const JNoun& larg, const JNoun& rarg) const;
  };

public:
  RavelAppendVerb(): JVerb(DefaultMonad<MonadOp>::Instantiate(rank_infinity, MonadOp()),
			   DefaultDyad<DyadOp>::Instantiate(rank_infinity, rank_infinity, DyadOp())) {}
};

namespace RazeVerbNS { 

template <typename T>
struct AllocateArray { 
  template <typename Iterator>
  JNoun::Ptr operator()(Dimensions result_dims, Iterator begin, Iterator end) const {
    assert(result_dims.get_rank() > 0);

    shared_ptr<vector<T> > v(boost::make_shared<vector<T> >(result_dims.number_of_elems(),
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
	  static_cast<JArray<T>&>(*arr.coordinate(1, i)).extend_into(item_dim, out_iter);
	  out_iter += elems_per_item;
	}
      } else { 
	arr.extend_into(item_dim, out_iter);
	out_iter += elems_per_item;
      }
    }
    
    return boost::static_pointer_cast<JNoun>(make_shared<JArray<T> >(result_dims, v));
  }
};

template <typename T>
struct get_boxed_content { 
  typedef boost::function<JNoun::Ptr (JBox)> func_t;
  typedef boost::transform_iterator<func_t, T> iterator;
  typedef std::pair<iterator, iterator> result_type;
  
  result_type operator()(T begin, T end) const {
    return std::make_pair(get_iterator(begin), get_iterator(end));
  }
  
private:
  iterator get_iterator(T iter) const {
    return iterator(iter, boost::bind(&JBox::get_contents, _1));
  }    
};


  
template <typename T>
struct get_dimensions { 
  typedef boost::function<Dimensions (JNoun::Ptr)> func_t;
  typedef boost::transform_iterator<func_t, T> iterator;
  typedef std::pair<iterator, iterator> result_type;

  result_type operator()(T begin, T end) const {
    return std::make_pair(get_iterator(begin), get_iterator(end));
  }

private:
  iterator get_iterator(T iter) const {
    return iterator(iter, boost::bind(&JNoun::get_dims, _1));
  }
};

template <typename T>
struct filter_empty { 
  typedef boost::function<bool (JNoun::Ptr)> func_t;
  typedef boost::filter_iterator<func_t, T> iterator;
  typedef std::pair<iterator, iterator> result_type;

  result_type operator()(T begin, T end) const { 
    return std::make_pair(get_iterator(begin, end), get_iterator(end, end));
  } 

private:
  iterator get_iterator(T begin, T end) const {
    return iterator(boost::bind(&Dimensions::number_of_elems, boost::bind(&JNoun::get_dims, _1)) != 0,
		    begin, end);
  }
};
  
JNoun::Ptr perform_op(JMachine::Ptr m, const JNoun& arg);
}

class RazeLinkVerb: public JVerb { 
  struct MonadOp { 
    JNoun::Ptr operator()(JMachine::Ptr m, const JNoun& arg) const { 
      return RazeVerbNS::perform_op(m, arg);
    }
  };

  struct DyadOp {
    JNoun::Ptr operator()(JMachine::Ptr, const JNoun&, const JNoun&) const {
      return JNoun::Ptr();
    }
  };

public:
  RazeLinkVerb(): JVerb(DefaultMonad<MonadOp>::Instantiate(rank_infinity, MonadOp()),
			DefaultDyad<DyadOp>::Instantiate(rank_infinity, rank_infinity, DyadOp())) {}
};

}	

#endif
