#include <jlang-frontend/input-source.hpp>
#include <stdio.h>

namespace JLang::frontend {

  /**
   * This is an implementation of an input source based
   * on a stdio FILE* descriptor, reading the data
   * from the given file.
   */
  class InputSourceFile : public InputSource {
  public:
    /**
     * Constructs an input source reader for
     * the given file.  Note that the caller is
     * responsible for opening and closing the file.
     * This reader merely uses the file provided to gather
     * the data, but assumes no responsibility for its
     * overall lifetime.
     */
    InputSourceFile(FILE *_file);
    /**
     * Destructor, nothing fancy.  In particular, this does NOT
     * close the file provided.
     */
    ~InputSourceFile();
    /**
     * This provides the parser with data from the given
     * file using 'fread' to read from the file and
     * provide data directly to the buffer.  The 'result'
     * represents the number of bytes actually read.
     */
    void yy_input(char *buf, int &result, int max_size);
  private:
    FILE *file;
  };

};
