#include <jlang-frontend/frontend.hpp>
#include <jlang-frontend/input-source-file.hpp>
#include <jlang-misc/test.hpp>

using namespace JLang::frontend;
using namespace JLang::frontend::tree;
using namespace JLang::frontend::namespaces;

void initialize_namespace(NamespaceContext & namespace_context)
{
}

class ParseTester {
public:
  ParseTester(std::string _path);
  ~ParseTester();
  TranslationUnit_owned_ptr parse(std::string _test_file);
  void initialize_namespace(NamespaceContext & namespace_context) const;
private:
  std::string path;
};

ParseTester::ParseTester(std::string _path)
  : path(_path)
{}
ParseTester::~ParseTester()
{}
void ParseTester::initialize_namespace(NamespaceContext & namespace_context) const
{
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
}

TranslationUnit_owned_ptr
ParseTester::parse(std::string base_filename)
{
  std::string filename = path + std::string("/") + base_filename;
  FILE *input = fopen(filename.c_str(), "rb");
  if (input == NULL) {
    fprintf(stderr, "Cannot open file %s\n", filename.c_str());
    return nullptr;
  }

  NamespaceContext namespace_context;
  initialize_namespace(namespace_context);
  InputSourceFile input_source(input);
  Parser parser(namespace_context);
  int rc = parser.parse(input_source);
  if (rc != 0) {
    fprintf(stderr, "Syntax error : %s\n", base_filename.c_str());
    return nullptr;
  }
  TranslationUnit_owned_ptr translation_unit = parser.get_translation_unit();
  return std::move(translation_unit);
}

int main(int argc, char **argv)
{
  if (argc != 2) {
    fprintf(stderr, "First argument must be the path of the source directory\n");
    return -1;
  }
  std::string path(argv[1]);

  ParseTester tester(path);

  // Check that we have the right number of statements in this file.
  {
    auto tu = tester.parse("tests/llvm-decl-var.j");
    ASSERT_NOT_NULL(tu, "Parse of known-good thing should not be null");
    ASSERT_INT_EQUAL(tu->get_statements().size(), 6, "Wrong number of statements in file");
  }

  {
    auto tu = tester.parse("tests/syntax-empty.j");
    ASSERT_NOT_NULL(tu, "Empty file should parse");
    ASSERT_INT_EQUAL(tu->get_statements().size(), 0, "Empty file should have no statements");
  }

  {
    printf("Expecting syntax error on line 4\n");
    auto tu = tester.parse("tests/syntax-invalid-garbage.j");
    ASSERT_NULL(tu, "Garbage file should not parse");
    printf("Expectation met\n");
  }
  
  printf("PASSED\n");
  
  return 0;
}
