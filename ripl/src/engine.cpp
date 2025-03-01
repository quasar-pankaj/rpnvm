#include "engine.hpp"

ripl::Engine::Engine() {}

ripl::Engine::~Engine() {
  _variables.clear();
  _code.clear();
}

void ripl::Engine::run() {
  for (_ip = _code.begin(); _ip != _code.end() && !_isFinished; _ip++) {
    (*_ip)->execute(this);
  }
}
