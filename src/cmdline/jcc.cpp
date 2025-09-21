#include <jlang-frontend.hpp>
#include <jlang-misc/input-source-file.hpp>

#include <jlang-backend/jbackend.hpp>
#include <jlang-backend/jbackend-format-identity.hpp>
#include <jlang-backend/jbackend-format-tree.hpp>

//#include <jlang-backend/jbackend-format-pretty.hpp>
//#include <jlang-codegen/jbackend-llvm.hpp>
//using namespace JLang::Backend::LLVM;
using namespace JLang::context;
using namespace JLang::frontend;
using namespace JLang::frontend::ast;
using namespace JLang::frontend::tree;
using namespace JLang::frontend::namespaces;
using namespace JLang::backend;

int main(int argc, char **argv)
{

#if 0
  //Desired pseuto-code:
  auto options = Arguments::parse(argc, argv);

  CompilerContext context;

  // Parser gets access to context (token stream, errors)
  Parser parser(context);
  // Parser produces an MIR representation of the code.
  // 
  auto mir = parser.parse();
  // Parser may now go out of scope, leaving only the MIR behind.
  // Parse tree and namespaces can leave scope.
  
  // If errors are severe, we may jump to end without
  // additional analysis or backend.

  // Analysis receives an immutable reference to the MIR,
  Analysis analysis(context, mir);
  // Analyze MIR and possibly produce errors.
  analysis.analyze();
  // Analysis can now leave scope leaving the context and MIR live.

  // Code generation receives context and
  // immutable MIR.
  CodeGeneration codegen(context, mir);
  
  // MIR goes out of scope after code generation.
  // MIR may now be dropped.

  // Report Errors
  //
  // Compiler context goes out of scope at end.
#endif
  
    if (argc != 2) {
      fprintf(stderr, "Invalid number of arguments %d\n", argc);
      exit(1);
    }
    
    int input = open(argv[1], O_RDONLY);
    if (input == -1) {
      fprintf(stderr, "Cannot open file %s\n", argv[1]);
      exit(1);
    }

    CompilerContext context;
    JLang::misc::InputSourceFile input_source(input);

    JLang::owned<ParseResult> parse_result = 
        Parser::parse(
                      context,
                      input_source
                      );
    close(input);
    
    if (parse_result->has_errors()) {
      parse_result->get_errors().print();
      return -1;
    }

    const TranslationUnit & translation_unit = parse_result->get_translation_unit();

    fprintf(stderr, "TODO: code generation phase\n");

    return 0;
}
