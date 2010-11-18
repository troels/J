#ifndef JEXCEPTION_HPP
#define JEXCEPTION_HPP

#include <stdexcept>
#include <string>

namespace J {
using std::runtime_error;
using std::string;

class JException: public runtime_error {
public:
  JException(string msg, string prefix = "JException"): runtime_error(prefix + ": " + msg) {};
};

class JIllegalDimensionsException: public JException {
public:
  JIllegalDimensionsException(string msg = "Illegal dimensions given"): 
    JException(msg, "JIllegalDimensionsException") {}
};

class JIllegalRankException: public JException {
public:
  JIllegalRankException(string msg = "Illegal rank given"): JException(msg, "JIllegalRankException") {}
};

class JIllegalValueTypeException: public JException {
public:
  JIllegalValueTypeException(string msg = "Illegal value type given"): 
    JException(msg, "JIllegalValuetypeException") {}
};

class JNoUnitException: public JException {
public:
  JNoUnitException(string msg = "No unit for verb"): 
    JException(msg, "JNoUnitException") {}
};

class JIllegalGrammarClassException: public JException {
public:
  JIllegalGrammarClassException(string msg = "Illegal grammar class given"): 
    JException(msg, "JIllegalGrammarClassException") {}
};

class JIllegalSyntaxException: public JException { 
public:
  JIllegalSyntaxException(string msg = "Illegal syntax"):
    JException(msg, "JIllegalSyntaxException") {}
};

class JIllegalImportException: public JException { 
public:
  JIllegalImportException(string msg = "Illegal import performed"): 
    JException(msg, "JIllegalImportException") {}
};

class JUnimplementedOperationException: public JException {
public:
  JUnimplementedOperationException(string msg = "Unimplemented operation"): 
    JException(msg) {}
};
}
  
#endif

