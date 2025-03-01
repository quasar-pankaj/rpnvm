#include "engine.hpp"
#include "builder.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " <scriptname>.bc" << std::endl;
    return 0;
  }
  ripl::Engine engine;
  ripl::Builder builder(&engine, argv[1]);
  builder.load();
  engine.run();
  return 0;
}
