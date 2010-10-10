#include "JNoun.hpp"
#include "vectorcounter.hpp"

namespace J {

  JNoun::JNoun(const Dimensions& d, j_value_type value_type): 
    JWord(grammar_class_noun), value_type(value_type), dims(d) {}

  template <typename T>
  JArray<T>::JArray(const Dimensions& d, shared_ptr<container> v):
    JNoun(d, JTypeTrait<T>::value_type), content(v), 
    begin_iter(content->begin()), end_iter(content->end()) {}
  
  template <typename T>
  JArray<T>::JArray(const Dimensions& d, shared_ptr<container> v, iter begin, iter end):
    JNoun(d, JTypeTrait<T>::value_type), content(v), 
    begin_iter(begin), end_iter(end) {}
  
  template <typename T>
  JArray<T>::JArray(const Dimensions& d, const JArray<T>& arr, iter begin, iter end):
    JNoun(d, JTypeTrait<T>::value_type), content(arr.content), 
    begin_iter(begin), end_iter(end) {}
  
  template <typename T>
  JArray<T>::JArray(): 
    JNoun(Dimensions(), JTypeTrait<T>::value_type), content(new container(0)),
    begin_iter(content->begin()), end_iter(content->end()) {}
  
  template <typename T>
  JArray<T>::JArray(const Dimensions &d, ...): 
    JNoun(d, JTypeTrait<T>::value_type), content(new container(d.number_of_elems(), JTypeTrait<T>::base_elem())),
    begin_iter(content->begin()), end_iter(content->end()) 
  {
    va_list va;
    va_start(va, d);
    
    for (iter i = begin_iter; i != end_iter; ++i) {
      *i = va_arg(va, T);
    }

    va_end(va);
  }
  
  template <typename T> 
  JArray<T> JArray<T>::operator[](int n) const {
    assert(get_rank() > 0);
    assert(n >= 0 && n < get_dims()[0]);
    Dimensions suffix = get_dims().suffix(-1);
    
    int nr_of_elems = suffix.number_of_elems();
    iter beg = begin() + n * nr_of_elems;
    
    return JArray<T>(suffix, content, beg, beg + nr_of_elems);
  }

  template <typename T> 
  shared_ptr<JNoun> JArray<T>::extend(const Dimensions &d) const {
    assert(d.get_rank() == get_rank());
    
    if (d == get_dims()) return shared_ptr<JNoun>(new JArray<T>(*this));
    
    shared_ptr<container> nv(new container(d.number_of_elems(), JTypeTrait<T>::base_elem()));
    
    if (get_rank() == 1) {
      copy(begin(), end(), nv->begin());
    } else {
      iter old_ptr = begin(), new_ptr = nv->begin(), new_end = nv->end();
      
      VectorCounter vc1(get_dims());
      VectorCounter vc2(d);
      
      while (new_ptr != new_end) {
	pair<int, int> p(add_row(vc1, vc2));
	copy(old_ptr, old_ptr + p.first, new_ptr);
	old_ptr += p.first;
	new_ptr += p.second;
      }
    }
    
    return shared_ptr<JNoun>(new JArray<T>(d, nv));
  }

  template <typename T>
  string JArray<T>::to_string() const {
    std::stringstream ss;
    ss << "J:JArray[ " << get_dims().to_string() << '\n' << content_string() << ']';
    return ss.str();
  }
  
  template <typename T> 
  string JArray<T>::content_string() const { 
    std::stringstream ss;
    content_string(ss, get_field_width());
    return ss.str();
  }

  template <typename T> 
  void JArray<T>::content_string(std::stringstream &ss, int field_width) const {
    if (get_rank() == 0) {
      ss << std::setw(field_width) << std::setfill(' ') << *begin();
    } else if (get_rank() == 1) {
      for (iter p = begin(); p != end(); ++p) {
	ss << std::setw(field_width) << std::setfill(' ') << (*p) << " ";
      }
    } else {
      for (int i = 0; i < get_dims()[0]; ++i) {
	(*this)[i].content_string(ss, field_width);
	ss << std::endl;
      }
    }
  }

  template <typename T>
  shared_ptr<JNoun> JArray<T>::coordinate(int coord_nr, ...) const {
    assert(get_rank() >= coord_nr);
    assert(coord_nr >= 0);
    
    va_list va;
    va_start(va, coord_nr);
    
    int offset = 0, i = 0;
    for(; i < coord_nr; ++i) {
      offset *= get_dims()[i];
      int coord = va_arg(va, int);
      assert(coord >= 0 && coord < get_dims()[i]);
      offset += coord;
    }
    va_end(va);
    Dimensions suffix = get_dims().suffix(-i);
    int suffix_len = suffix.number_of_elems();
    iter ptr = begin() + offset * suffix_len;
    assert(distance(ptr, end()) > 0);
    
    return shared_ptr<JNoun>(new JArray<T>(suffix, content, ptr, ptr + suffix_len));
  }

  template <typename T>
  shared_ptr<JNoun> JArray<T>::clone() const { 
    return shared_ptr<JNoun>(new JArray<T>(*this));
  }
    
  template <typename T>
  bool JArray<T>::operator==(const JNoun& other) const {
    return other.get_value_type() == get_value_type() &&
      get_dims() == other.get_dims() &&
      equal(begin(), end(), static_cast< const JArray<T>& >(other).begin());
  }

  template <>
  int JArray<int>::get_field_width() const {
    iter max = std::max_element(begin(), end());
    iter min = std::min_element(begin(), end());
    
    int max_len = max != end() && *max > 0 ? std::floor(std::log10(*max)) + 1 : 1;
    int min_len = min != end() && *min < 0 ? std::floor(std::log10(std::abs(*min))) + 2 : 1;
    
    return std::max(max_len, min_len);
  }

  template class JArray<JInt>;

  std::ostream& operator<<(std::ostream& os, const JNoun& noun) {
    return (os << noun.to_string());
  }
}
    
  
    