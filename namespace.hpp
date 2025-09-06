#pragma once
#include <string>
#include <memory>
#include <list>
#include <map>

class NamespaceType {
public:
  typedef std::shared_ptr<NamespaceType> ptr;
  NamespaceType(std::string _name);
  ~NamespaceType();

  std::string name;
};

class Namespace {
public:
  typedef std::shared_ptr<Namespace> ptr;
  Namespace(std::string _name);
  ~Namespace();

  std::string name;                                // Name of the namespace.
  std::map<std::string, Namespace::ptr> children;  // This is the list of child namespaces.
  std::map<std::string, NamespaceType::ptr> types;          // This is the set of types inside this namespace.

};

void namespace_init();
void namespace_begin(std::string name);

void namespace_type_define(std::string name);
bool namespace_type_exists(std::string name);
bool namespace_exists(std::string name);

void namespace_using(std::string name, std::string alias);

void namespace_end();
void namespace_dump();
