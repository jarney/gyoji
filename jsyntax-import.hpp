#pragma once

class JSyntaxImport {
public:
  typedef std::shared_ptr<JSyntaxImport> ptr;
  JSyntaxImport(std::string str);
  ~JSyntaxImport();
  std::string import_name;
};

/**
 * This class represents a list of import statements in the J programming language.
 * Import statements must all appear prior to the start of declarations within the
 * file.
 */
class JSyntaxImportList {
public:
  typedef std::shared_ptr<JSyntaxImportList> ptr;
  JSyntaxImportList();
  ~JSyntaxImportList();
  std::list<std::shared_ptr<JSyntaxImport>> imports;
};
