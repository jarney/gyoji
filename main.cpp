#include "jsyntax.hpp"
#include "ast.hpp"
#include "target/jlang.l.hpp"
#include "target/jlang.y.hpp"

#include "jbackend.hpp"
#include "jbackend-format-identity.hpp"
#include "jbackend-format-tree.hpp"
#include "jbackend-format-pretty.hpp"
#include "jbackend-toc.hpp"

int main(int argc, char **argv)
{
    yyscan_t scanner;
    yylex_init(&scanner);

    if (argc != 2) {
      fprintf(stderr, "Invalid number of arguments %d\n", argc);
      fprintf(stderr, "Usage: parser backend < file\n");
      exit(1);
    }
    std::shared_ptr<JBackend> backend;
    if (std::string("format-identity") == std::string(argv[1])) {
        backend = std::make_shared<JBackendFormatIdentity>();
    }
    else if (std::string("format-tree") == std::string(argv[1])) {
        backend = std::make_shared<JBackendFormatTree>();
    }
    /*
      else if (std::string("toc") == std::string(argv[1])) {
        backend = std::make_shared<JBackendToC>();
    }
    */
    else {
      fprintf(stderr, "Invalid backend %s\n", argv[1]);
      return 1;
    }
    return_data_t data;
    calc::Parser parser{ scanner, &data };

    int rc = parser.parse();
    if (rc != 0) {
      printf("Syntax error\n");
    }
    else {
      backend->process(data.parsed);
    }
    yylex_destroy(scanner);
}
