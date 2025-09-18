#include <jlang-frontend/input-source.hpp>
#include <stdio.h>

namespace JLang::frontend {

  class InputSourceFile : public InputSource {
  public:
    InputSourceFile(FILE *_file);
    ~InputSourceFile();
    void yy_input(char *buf, int &result, int max_size);
  private:
    FILE *file;
  };

};
