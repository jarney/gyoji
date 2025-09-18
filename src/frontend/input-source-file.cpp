#include <jlang-frontend/input-source-file.hpp>
#include <errno.h>

using namespace JLang::frontend;

InputSourceFile::InputSourceFile(FILE *_file)
  : file(_file)
{}
InputSourceFile::~InputSourceFile()
{}
                                
void InputSourceFile::yy_input(char *buf, int &result, int max_size)
{
  errno=0;
  while ( (result = (int) fread(buf, 1, (size_t) max_size, file)) == 0 && ferror(file)) {
    if( errno != EINTR) {
      fprintf(stderr, "Fatal error reading input buffer\n");
      break;
    }
    errno=0;
    clearerr(file);
  }
}

