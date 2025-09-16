#include <jlang-frontend/frontend.hpp>
#include <jlang-frontend/input-source-file.hpp>

#include <jlang-backend/jbackend.hpp>
#include <jlang-backend/jbackend-format-identity.hpp>
#include <jlang-backend/jbackend-format-tree.hpp>

//#include <jlang-backend/jbackend-format-pretty.hpp>
//#include <jlang-codegen/jbackend-llvm.hpp>
//using namespace JLang::Backend::LLVM;
using namespace JLang::frontend;
using namespace JLang::frontend::ast;
using namespace JLang::frontend::tree;
using namespace JLang::frontend::namespaces;
using namespace JLang::backend;

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

    NamespaceContext namespace_context;
    namespace_context.namespace_new("u8", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PUBLIC);
    
    namespace_context.namespace_new("i16", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PUBLIC);
    namespace_context.namespace_new("i32", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PUBLIC);
    namespace_context.namespace_new("i64", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PUBLIC);

    namespace_context.namespace_new("u16", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PUBLIC);
    namespace_context.namespace_new("u32", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PUBLIC);
    namespace_context.namespace_new("u64", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PUBLIC);

    namespace_context.namespace_new("f32", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PUBLIC);
    namespace_context.namespace_new("f64", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PUBLIC);

    namespace_context.namespace_new("void", Namespace::TYPE_TYPEDEF, Namespace::VISIBILITY_PUBLIC);
    
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
    InputSourceFile input_source(input);

    Parser parser(namespace_context);
    int rc = parser.parse(input_source);
    if (rc != 0) {
      printf("Syntax error\n");
    }
    else {
      const TranslationUnit & translation_unit = parser.get_translation_unit();
      rc = backend->process(translation_unit.get_syntax_node());
    }
    fclose(input);
    return rc;
}
