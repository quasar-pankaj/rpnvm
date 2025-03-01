#include "compiler.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << "Usage " << argv[0] << " <filename>" << std::endl;
    return 0;
  }
  ripl::Compiler compiler(argv[1]);
  compiler.compile();

  return 0;
}
