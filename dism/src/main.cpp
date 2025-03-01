#include "dism.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " <filename>" << std::endl;
    return 0;
  }
  ripl::Dism dism(argv[1]);
  dism.disassemble();
  return 0;
}
