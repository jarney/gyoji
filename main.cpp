#include "jsyntax.hpp"
#include "ast.hpp"
#include "namespace.hpp"
#include "target/jlang.l.hpp"
#include "target/jlang.y.hpp"

#include "jbackend.hpp"
#include "jbackend-format-identity.hpp"
#include "jbackend-format-tree.hpp"
#include "jbackend-format-pretty.hpp"
#include "jbackend-toc.hpp"

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
    yyscan_t scanner;
    yylex_init(&scanner);
    yyset_in(input, scanner);
    
    std::shared_ptr<JBackend> backend;
    if (std::string("format-identity") == std::string(argv[1])) {
        backend = std::make_shared<JBackendFormatIdentity>();
    }
    else if (std::string("format-tree") == std::string(argv[1])) {
        backend = std::make_shared<JBackendFormatTree>();
    }
    else {
      fprintf(stderr, "Invalid backend %s\n", argv[1]);
      return 1;
    }
    return_data_t data;
    
    namespace_init();
    namespace_type_define("char");
    namespace_type_define("int");
    namespace_type_define("float");
    namespace_type_define("double");
    namespace_type_define("long");
    namespace_type_define("short");
    namespace_type_define("unsigned");
    namespace_type_define("void");
    
    calc::Parser parser{ scanner, &data };
    int rc = parser.parse();
    if (rc != 0) {
      printf("Syntax error\n");
    }
    else {
      backend->process(data.parsed);
    }
    yylex_destroy(scanner);
    fclose(input);
}
