#pragma once

#include "instruction_set.hpp"
#include <string>
namespace ripl {
class BuildContext {
public:
  virtual void load() = 0;

  virtual Instruction loadInstruction() = 0;
  virtual int readInt() = 0;
  virtual long readLong() = 0;
  virtual double readDouble() = 0;
  virtual bool readBool() = 0;
  virtual std::string readString() = 0;
};
} // namespace ripl
