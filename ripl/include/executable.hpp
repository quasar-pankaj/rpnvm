#pragma once

namespace ripl {
class RunContext;
class Executable {
public:
  virtual void execute(RunContext *context) = 0;
};
} // namespace ripl
