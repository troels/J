#ifndef JMACHINE_HPP
#define JMACHINE_HPP

#include <boost/shared_ptr.hpp>

namespace J {
  using boost::shared_ptr;

  class JMachine { 
    JMachine();

  public:
    static shared_ptr<JMachine> new_machine();
  };
}
#endif
