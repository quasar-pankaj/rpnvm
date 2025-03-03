#include "engine.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " <scriptname>.bc" << std::endl;
    return 0;
  }
  ripl::Engine engine(argv[1]);
  engine.run();
  return 0;
}
