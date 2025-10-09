#include <gyoji-context/errors.hpp>
#include <gyoji-context/token-stream.hpp>

using namespace Gyoji::context;

int main(int argc, char **argv)
{
    
    TokenStream token_stream;
    size_t lineno = 1;
    token_stream.add_token(0, "{",           "foo.j", lineno++, 0);
    token_stream.add_token(0, "    x = 12;", "foo.j", lineno++, 0);
    token_stream.add_token(0, "    a = asdfasdf::23;", "foo.j", lineno++, 0);
    token_stream.add_token(0, "123456789", "foo.j", lineno++, 0);
    token_stream.add_token(0, "    y = 14;", "foo.j", lineno++, 0);
    token_stream.add_token(0, "    p = x + y;", "foo.j", lineno++, 0);
    
    Errors errors(token_stream);

    {
    SourceReference src_ref(std::string("asdf.h"), (size_t)3, (size_t)9, 8);
    std::unique_ptr<Error> error = std::make_unique<Error>("Syntax Error");
    error->add_message(src_ref, "Invalid namespace asdfsdf");
    errors.add_error(std::move(error));
    }
    {
    SourceReference src_ref(std::string("asdf.h"), (size_t)5, (size_t)5, 1);
    std::unique_ptr<Error> error = std::make_unique<Error>("Syntax Error");
    error->add_message(src_ref, "Undeclared identifier y");
    errors.add_error(std::move(error));
    }

    errors.print();
}
