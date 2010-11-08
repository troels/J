#ifndef JAST_HPP
#define JAST_HPP

#include <vector>
#include <algorithm>
#include <string>
#include <boost/shared_ptr.hpp>
#include "JNoun.hpp"

namespace J { namespace JAST { 
using boost::shared_ptr;
using std::vector;
using std::string;

enum j_ast_elem_type { 
  j_ast_elem_type_noun,
  j_ast_elem_type_builtin,
  j_ast_elem_type_user_defined,
  j_ast_elem_type_sequence
};

class JASTBase {  
  j_ast_elem_type type;

public:
  typedef shared_ptr<JASTBase> Ptr;

  JASTBase(j_ast_elem_type type): type(type) {} 
  virtual ~JASTBase() = 0;
  j_ast_elem_type get_j_ast_elem_type() const { 
    return type;
  }
};

class JASTNoun: public JASTBase {
  JNoun::Ptr noun;

public:
  JASTNoun(JNoun::Ptr noun): JASTBase(j_ast_elem_type_noun), noun(noun) {} 
  JNoun::Ptr get_noun() const { return noun; }
};

class JASTBuiltin: public JASTBase {
  string str;
  
public:
  JASTBuiltin(const string& str): JASTBase(j_ast_elem_type_builtin), str(str) {}
  
  string get_string() const { 
    return str;
  }
};

class JASTUserDefined: public JASTBase {
  string str;
  
public:
  JASTUserDefined(const string& str): JASTBase(j_ast_elem_type_user_defined), str(str) {} 
  
  string get_string() const { 
    return str;
  }
};

class JASTSequence: public JASTBase {
  vector<JASTBase::Ptr> v;

public:  
  typedef vector<JASTBase::Ptr>::const_iterator const_iterator;
  typedef shared_ptr<JASTSequence > Ptr;
  
  template <typename T>
  static Ptr Instantiate(T begin, T end) {
    return Ptr(new JASTSequence(begin, end));
  }

  template <typename T>
  JASTSequence(T begin, T end): JASTBase(j_ast_elem_type_sequence), v(begin, end) {}
  
  const_iterator begin() const { return v.begin(); }
  const_iterator end() const { return v.end(); }
};

}}


#endif
