#pragma once

#include "executable.hpp"
#include "run_context.hpp"
#include <memory>
namespace ripl {
class Dup : public Executable {
public:
  void execute(RunContext *context) override {
    auto ob = context->top();
    context->push(ob);
  }
};

class Swap : public Executable {
public:
  void execute(RunContext *context) override {
    auto top = context->top();
    context->pop();
    auto next = context->top();
    context->pop();
    context->push(top);
    context->push(next);
  }
};

class RotDn : public Executable {
public:
  void execute(RunContext *context) override {
    auto top = context->top();
    context->pop();
    auto second = context->top();
    context->pop();
    auto third = context->top();
    context->pop();
    context->push(second);
    context->push(top);
    context->push(third);
  }
};

class RotUp : public Executable {
public:
  void execute(RunContext *context) override {
    auto top = context->top();
    context->pop();
    auto second = context->top();
    context->pop();
    auto third = context->top();
    context->pop();
    context->push(top);
    context->push(third);
    context->push(second);
  }
};

class Drop : public Executable {
public:
  void execute(RunContext *context) override { context->pop(); }
};

} // namespace ripl
