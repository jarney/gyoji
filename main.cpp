#include <jlang-frontend.hpp>
#include <jlang-frontend/input-source-file.hpp>

#include <jlang-backend/jbackend.hpp>
#include <jlang-backend/jbackend-format-identity.hpp>
#include <jlang-backend/jbackend-format-tree.hpp>

//#include <jlang-backend/jbackend-format-pretty.hpp>
//#include <jlang-codegen/jbackend-llvm.hpp>
//using namespace JLang::Backend::LLVM;
using namespace JLang::errors;
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
    Errors errors;

    Parser parser(namespace_context, errors);
    int rc = parser.parse(input_source);
    if (rc != 0) {
      printf("Syntax error\n");
    }
    else {
      ParseResult_owned_ptr parse_result = std::move(parser.get_parse_result());
      const TranslationUnit & translation_unit = parse_result->get_translation_unit();
      rc = backend->process(translation_unit.get_syntax_node());
    }
    fclose(input);
    return rc;
}
