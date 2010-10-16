#include "JMachine.hpp"

namespace J {
  JMachine::JMachine() {}
  shared_ptr<JMachine> JMachine::new_machine() { 
    return shared_ptr<JMachine>(new JMachine());
  }
}
  
