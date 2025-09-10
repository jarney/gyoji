#include "jsyntax.hpp"
#include "ast.hpp"
#include "namespace.hpp"
#include "target/jlang.l.hpp"
#include "target/jlang.y.hpp"

#include "jbackend.hpp"
#include "jbackend-format-identity.hpp"
#include "jbackend-format-tree.hpp"
#include "jbackend-format-pretty.hpp"
#include "jbackend-llvm.hpp"
#include "jbackend-toc.hpp"

using namespace JLang::Backend::LLVM;

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
    else if (std::string("llvm") == std::string(argv[1])) {
      backend = std::make_shared<JBackendLLVM>();
    }
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
    
    jlang::Parser parser{ scanner, &data };
    int rc = parser.parse();
    if (rc != 0) {
      printf("Syntax error\n");
    }
    else {
      rc = backend->process(data.translation_unit);
    }
    yylex_destroy(scanner);
    fclose(input);
    return rc;
}
