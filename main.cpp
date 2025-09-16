#include <jlang-frontend/jsyntax.hpp>
#include <jlang.l.hpp>
#include <jlang.y.hpp>

#include <jlang-backend/jbackend.hpp>
#include <jlang-backend/jbackend-format-identity.hpp>
#include <jlang-backend/jbackend-format-tree.hpp>
//#include <jlang-backend/jbackend-format-pretty.hpp>
//#include <jlang-codegen/jbackend-llvm.hpp>
//using namespace JLang::Backend::LLVM;
using namespace JLang::frontend::ast;
using namespace JLang::frontend::tree;
using namespace JLang::frontend::namespaces;
using namespace JLang::frontend::yacc;
using namespace JLang::backend;

#if 0
// Last piece before we button up the
// interface to the parser and document
// the whole thing.
class Parser {
public:
  Parser(FILE *f);
  int parse();

  const TranslationUnit & get_translation_unit() const;
  const SyntaxNode & get_syntax_node() const;
private:
    TranslationUnit_owned_ptr translation_unit;
    NamespaceContext namespace_context;
private:
};

int
Parser::parse()
{
  return_data_t data;
  yacc::Parser parser{ scanner, &data };
  int rc = parser.parse();
 return rc;
}

const TranslationUnit &
Parser::get_translation_unit() const
{
  return *translation_unit;
}
const SyntaxNode &
Parser::get_syntax_node() const
{
  return *translation_unit;
}
const NamespaceContext & get_namespace_context() const
{
  return &namespace_context;
}
#endif

int main(int argc, char **argv)
{

    if (argc != 3) {
      fprintf(stderr, "Invalid number of arguments %d\n", argc);
      fprintf(stderr, "Usage: parser backend file\n");
      exit(1);
    }
    
    FILE *input = fopen(argv[2], "rb");
    if (input == NULL) {
      fprintf(stderr, "Cannot open file %s\n", argv[2]);
      exit(1);
    }

    return_data_t data;

    yyscan_t scanner;
    yylex_init(&scanner);
    yyset_extra (&data.namespace_context, scanner);
    yyset_in(input, scanner);
    
    std::shared_ptr<JBackend> backend;
    if (std::string("format-identity") == std::string(argv[1])) {
        backend = std::make_shared<JBackendFormatIdentity>();
    }
    else if (std::string("format-tree") == std::string(argv[1])) {
        backend = std::make_shared<JBackendFormatTree>();
    }
    //    else if (std::string("llvm") == std::string(argv[1])) {
    //      backend = std::make_shared<JBackendLLVM>();
    //    }
    else {
      fprintf(stderr, "Invalid backend %s\n", argv[1]);
      return 1;
    }

    data.namespace_context.namespace_new("u8", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PUBLIC);
    
    data.namespace_context.namespace_new("i16", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PUBLIC);
    data.namespace_context.namespace_new("i32", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PUBLIC);
    data.namespace_context.namespace_new("i64", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PUBLIC);

    data.namespace_context.namespace_new("u16", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PUBLIC);
    data.namespace_context.namespace_new("u32", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PUBLIC);
    data.namespace_context.namespace_new("u64", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PUBLIC);

    data.namespace_context.namespace_new("f32", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PUBLIC);
    data.namespace_context.namespace_new("f64", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PUBLIC);

    data.namespace_context.namespace_new("void", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PUBLIC);
    
    Parser parser{ scanner, &data };
    int rc = parser.parse();
    if (rc != 0) {
      printf("Syntax error\n");
    }
    else {
      rc = backend->process(data.translation_unit->get_syntax_node());
    }
    yylex_destroy(scanner);
    fclose(input);
    return rc;
}
