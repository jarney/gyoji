#pragma once

#include <jlang-misc/pointers.hpp>
#include <string>
#include <map>
#include <vector>

namespace JLang::mir {
  class Function;
  
  class Functions {
  public:
    Functions();
    ~Functions();
  };

  class Function {
  public:
    Function();
    ~Function();
  };
};
