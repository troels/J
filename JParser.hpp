#ifndef JPARSER_HPP
#define JPARSER_HPP

#include <boost/spirit/include/qi.hpp>
#include "JGrammar.hpp"

namespace J { namespace JParser {
  namespace qi = boost::spirit::qi;
  
  class ParserNumber { 
    j_value_type value_type;
  public:
    ParserNumber(j_value_type value_type): value_type(value_type) {}
    virtual ~ParserNumber() {}

    j_value_type get_value_type() const { return value_type; }
  };

  class ParserFloat: public ParserNumber { 
    JFloat nr;
  public:
    ParserFloat(JFloat nr): ParserNumber(j_value_type_float), nr(nr) {}
    JFloat get_float() const { return nr; }
  };
  
  class ParserInt: public ParserNumber { 
    JInt nr;
  public:
    ParserInt(JInt nr): ParserNumber(j_value_type_int), nr(nr) {}
    JInt get_int() const { return nr; }
  };

  class ParserComplex: public ParserNumber { 
    JComplex nr;
  public:
    ParserComplex(JComplex nr): ParserNumber(j_value_type_complex), nr(nr) {}
    JComplex get_complex() const { return nr; }
  };
    
  // template <typename Iterator>
  // struct parse_number 
  //   : qi::grammar<Iterator, 
    
}}

#endif
