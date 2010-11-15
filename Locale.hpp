#ifndef LOCALE_HPP
#define LOCALE_HPP

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/optional.hpp>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include "JGrammar.hpp"
#include "JExceptions.hpp"

namespace J {
using std::map;
using std::vector;
using std::string;
using boost::weak_ptr;
using boost::shared_ptr;
using boost::optional;

class LocaleCollection;
class SymbolMap;

extern const string default_locale_name;

class Locale { 
  string name;
  shared_ptr<LocaleCollection> imports;
  shared_ptr<SymbolMap> symbols;

  Locale(const string& name = default_locale_name);

public:
  typedef shared_ptr<Locale> Ptr;
  typedef weak_ptr<Locale> WeakPtr;
  
  static Ptr Instantiate(const string& name) {
    return Ptr(new Locale(name));
  }

  static Ptr Instantiate() {
    return Ptr(new Locale());
  }
  
  optional<JWord::Ptr> add_public_symbol(const string& name, JWord::Ptr word);
  optional<JWord::Ptr> add_private_symbol(const string& name, JWord::Ptr word);
  

  optional<JWord::Ptr> lookup_public_symbol(const string& name) const;
  optional<JWord::Ptr> lookup_symbol(const string& name) const;

  void import_locale(Ptr l);
  string get_name() const { return name; }
};

class SymbolMap { 
  class Symbol {
    enum symbol_type_t { public_symbol, private_symbol };
    symbol_type_t symbol_type;
    JWord::Ptr word;
    
    Symbol(symbol_type_t symbol_type, JWord::Ptr word):
      symbol_type(symbol_type), word(word) {}
    
  public:
    typedef shared_ptr<Symbol> Ptr;
    
    static Ptr PrivateSymbol(JWord::Ptr word) {
      return Ptr(new Symbol(private_symbol, word));
    } 

    static Ptr PublicSymbol(JWord::Ptr word) {
      return Ptr(new Symbol(public_symbol, word));
    }

    bool is_private() const { return symbol_type == private_symbol; }
    bool is_public() const { return symbol_type == public_symbol; }
    
    JWord::Ptr get_word() const { return word; } 
  };

  typedef map<string, Symbol::Ptr>::const_iterator map_iter;

  map<string, Symbol::Ptr> symbol_map;

  optional<JWord::Ptr> add_symbol(const string& name, Symbol::Ptr symbol);
  
public: 
  SymbolMap(): symbol_map() {}
  
  optional<JWord::Ptr> add_public_symbol(const string& name, JWord::Ptr word);
  optional<JWord::Ptr> add_private_symbol(const string& name, JWord::Ptr word);

  optional<JWord::Ptr> lookup_public_symbol(const string& name) const;
  optional<JWord::Ptr> lookup_symbol(const string& name) const;
};

class LocaleCollection { 
  typedef map<string, Locale::WeakPtr>::const_iterator loc_iter;
  map<string, Locale::WeakPtr> locales;
  
  optional<Locale::Ptr> get_locale(const string& name) const;

public:
  void import_locale(Locale::Ptr l);
  optional<JWord::Ptr> lookup_symbol(const string& name) const;
};

}

#endif
