#include "Locale.hpp"

namespace J {

const string default_locale_name("*default_locale*");

Locale::Locale(const string& name): name(name), 
				    imports(new LocaleCollection()), 
				    symbols(new SymbolMap()) {}


optional<JWord::Ptr> Locale::add_public_symbol(const string& name, JWord::Ptr word) {
  return symbols->add_public_symbol(name,  word);
}

optional<JWord::Ptr> Locale::add_private_symbol(const string& name, JWord::Ptr word) {
  return symbols->add_private_symbol(name, word);
}

optional<JWord::Ptr> Locale::lookup_public_symbol(const string& name) const { 
  optional<JWord::Ptr> res(symbols->lookup_public_symbol(name));
  if (res) return res;
  return imports->lookup_symbol(name);
}


optional<JWord::Ptr> Locale::lookup_symbol(const string& name) const {
  optional<JWord::Ptr> res(symbols->lookup_symbol(name));
  if (res) return res;
  return imports->lookup_symbol(name);
}

void Locale::import_locale(Ptr l) {
  imports->import_locale(l);
}

optional<JWord::Ptr> SymbolMap::add_symbol(const string& name, Symbol::Ptr symbol) {
  map_iter it = symbol_map.find(name);
  optional<JWord::Ptr> o; 
  
  if (it != symbol_map.end()) {
    o = optional<JWord::Ptr>(it->second->get_word());
  } 
  
  symbol_map[name] = symbol;
  return o;
} 

optional<JWord::Ptr> SymbolMap::add_public_symbol(const string& name, JWord::Ptr word) {
  return add_symbol(name, Symbol::PublicSymbol(word));
}

optional<JWord::Ptr> SymbolMap::add_private_symbol(const string& name, JWord::Ptr word) { 
  return add_symbol(name, Symbol::PrivateSymbol(word));
}

optional<JWord::Ptr> SymbolMap::lookup_public_symbol(const string& name) const {
  map_iter it(symbol_map.find(name));
  
  if (it == symbol_map.end() || it->second->is_private())
    return optional<JWord::Ptr>();
    
  return optional<JWord::Ptr>(it->second->get_word());
}

optional<JWord::Ptr> SymbolMap::lookup_symbol(const string& name) const { 
  map_iter it(symbol_map.find(name));
  
  if (it == symbol_map.end()) 
    return optional<JWord::Ptr>();
  
  return optional<JWord::Ptr>(it->second->get_word());
}

optional<Locale::Ptr> LocaleCollection::get_locale(const string& name) const {
  loc_iter it(locales.find(name));
  if (it == locales.end()) return optional<Locale::Ptr>();
  
  Locale::Ptr ptr(it->second.lock());
  if (!ptr) return optional<Locale::Ptr>();
  return optional<Locale::Ptr>(ptr);
}

void LocaleCollection::import_locale(Locale::Ptr l) {
  if (get_locale(l->get_name())) { 
    throw JIllegalImportException("Collection already has module with name");
  }
  
  Locale::WeakPtr wl(l);
  locales.insert(std::pair<string, Locale::WeakPtr>(l->get_name(), wl));
}

optional<JWord::Ptr> LocaleCollection::lookup_symbol(const string& name) const { 
  for(loc_iter it(locales.begin()), end(locales.end()); it != end; ++it) {
    Locale::Ptr l(it->second);
    optional<JWord::Ptr> w(l->lookup_public_symbol(name));
    if (w) return w;
  }
  return optional<JWord::Ptr>();
}
}


