#include "jsyntax.hpp"


static std::map<std::string, ASTNode::ptr> types;

bool type_exists(std::string name)
{
    if(types.find(name) == types.end()) {
      return false;
    }
    return true;
}

void define_type(std::string name, ASTNode::ptr definition)
{
  types[name] = definition;
}

/*
  JSyntaxFile::JSyntaxFile(std::shared_ptr<JSyntaxImportList> _imports, std::shared_ptr<JSyntaxFileStatementList> _fileDefs)
  : imports(_imports)
  , fileDefs(_fileDefs)
{}

JSyntaxFile::~JSyntaxFile()
{}

void JSyntaxFile::print(void)
{
  printf("Really got a parsed artifact\n");
}

*/
