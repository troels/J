#ifndef DIMENSIONS_HPP
#define DIMENSIONS_HPP

#include <vector>
#include <cstdarg>
#include <cassert>
#include <string>
#include <numeric>
#include <sstream>
#include <boost/shared_ptr.hpp>

namespace J {
using std::vector;
using std::string;
using boost::shared_ptr;
using std::stringstream;

class Dimensions {
  typedef vector<int>::iterator iter;

  shared_ptr<vector<int> > dims;
  iter begin_iter;
  iter end_iter;
  int rank;
    
public:
  Dimensions(int rank, ...);

  Dimensions();

  Dimensions(shared_ptr<vector<int> > dims); 
  Dimensions(shared_ptr<vector<int> > dims, iter begin, iter end);
    

  iter begin() const { return begin_iter; }
  iter end() const { return end_iter; }
  
  int get_rank() const { return rank; }
  string to_string() const;
    
  bool operator!=(const Dimensions& d) const { 
    return !(*this == d); 
  }

  bool operator==(const Dimensions& d) const;
    
  int operator[](int n) const; 

  Dimensions suffix(int n) const; 
  Dimensions prefix(int n) const;

  bool prefix_match(const Dimensions &d) const;
  bool suffix_match(const Dimensions &d) const;
    
  int number_of_elems() const;

  Dimensions operator+(const Dimensions &d) const;

  friend std::ostream& operator<<(std::ostream& os, const Dimensions& d);
};
}
#endif

