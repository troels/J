#include "Dimensions.hpp"

namespace J {
  Dimensions::Dimensions(int rank, ...): dims(new vector<int>(rank)), 
					 begin_iter(dims->begin()), end_iter(dims->end()),
					 rank(rank) {
      va_list va;
      va_start(va, rank);
      
      for (iter i = begin(); i != end(); ++i) {
	*i = va_arg(va, int);
      }
      
      va_end(va);
    }

  Dimensions::Dimensions(): dims(new vector<int>(0)), begin_iter(dims->begin()), 
			    end_iter(dims->end()), rank(0) {}

  Dimensions::Dimensions(shared_ptr<vector<int> > dims): dims(dims), begin_iter(dims->begin()), 
					     end_iter(dims->end()), rank(dims->size()) {}

  Dimensions::Dimensions(shared_ptr<vector<int> > dims, iter begin, iter end): dims(dims), begin_iter(begin), 
									       end_iter(end), 
									       rank(distance(begin_iter, end_iter)) {}
  
  bool Dimensions::operator==(const Dimensions& d) const {
    return d.get_rank() == get_rank() && equal(begin(), end(), d.begin());
  }

      
  int Dimensions::operator[](int n) const {
    assert(n < get_rank() && n >= 0);
      
    return *(begin() + n);
  }    
  
  Dimensions Dimensions::suffix(int n) const {
    if (n >= get_rank()) return *this;
    if (-n >= get_rank()) return Dimensions();
    
    const iter start = begin() + (n >= 0 ? (get_rank() - n) : -n);
    return Dimensions(dims, start, end());
  }
  
  Dimensions Dimensions::prefix(int n) const {
    if (n >= get_rank()) return *this;
    if (-n >= get_rank()) return Dimensions();
    
    const iter stop = end() - (n >= 0 ? (get_rank() - n) : -n);
    return Dimensions(dims, begin(), stop);
  }

  bool Dimensions::prefix_match(const Dimensions &d) const {
    return d.get_rank() <= get_rank() && equal(d.begin(), d.end(), begin());
  }

  bool Dimensions::suffix_match(const Dimensions &d) const {
    return d.get_rank() <= get_rank() && equal(d.begin(), d.end(), begin() + (get_rank() - d.get_rank()));
  }

  int Dimensions::number_of_elems() const { 
    return std::accumulate(begin(), end(), 1, std::multiplies<int>());
  }

  
  string Dimensions::to_string() const {
    std::stringstream ss;
    ss << "J::Dimensions[ ";
    for (iter p = begin(); p != end(); ++p) {
      ss << (*p) << " ";
    }
    ss << "]";
    return ss.str();
  }


  Dimensions Dimensions::operator+(const Dimensions &d) const {
    shared_ptr<vector<int> > v(new vector<int>(d.get_rank() + get_rank()));

    vector<int>::iterator output = v->begin();
    iter ptr = begin();

    while (ptr != end()) {
      *output = *ptr;
      ++output;
      ++ptr;
    }
    ptr = d.begin();
    while (ptr != d.end()) {
      *output = *ptr;
      ++output;
      ++ptr;
    }
    
    return Dimensions(v);
  }

  std::ostream& operator<<(std::ostream& os, const Dimensions& d) {
    return (os << d.to_string());
  }
}

