#pragma once

namespace JLang::frontend {

  class InputSource {
  public:
    InputSource();
    ~InputSource();
    virtual void yy_input(char *buf, int &result, int max_size) = 0;
  };
  
};
