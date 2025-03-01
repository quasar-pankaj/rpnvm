#pragma once

#include "build_context.hpp"
#include <fstream>
#include <string>
namespace ripl {
class Engine;
class Builder : public BuildContext {
public:
  Builder(Engine *engine, char *filename) : _engine(engine) {
    _in.open(filename);
  }
  ~Builder() { _in.close(); }

  void load() override;
  Instruction loadInstruction() override;
  int readInt() override;
  long readLong() override;
  double readDouble() override;
  bool readBool() override;
  std::string readString() override;

private:
  std::ifstream _in;
  Engine *_engine;
};
} // namespace ripl
