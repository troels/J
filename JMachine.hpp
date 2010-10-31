#ifndef JMACHINE_HPP
#define JMACHINE_HPP

#include <boost/shared_ptr.hpp>
#include <map>
#include <string>
#include "JGrammar.hpp"

namespace J {

using boost::shared_ptr;
using std::map;
using std::string;

class JMachine { 
  map<string, JWord::Ptr> operators;
  JMachine();
  
public:
  typedef shared_ptr<JMachine> Ptr;
  static Ptr new_machine();
};


}
#endif
