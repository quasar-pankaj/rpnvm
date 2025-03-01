#pragma once

#include "executable.hpp"
#include "run_context.hpp"
namespace ripl {
class Return : public Executable {
public:
  void execute(RunContext *context) override {
    auto offset = context->topRS();
    context->popRS();
    context->IP(offset);
  }
};

class Call : public Executable {
public:
  Call(int offset) : _offset(offset - 1) {}

  void execute(RunContext *context) override {
    context->pushRS(context->IP());
    context->IP(context->start() + _offset);
  }

private:
  int _offset;
};
} // namespace ripl
