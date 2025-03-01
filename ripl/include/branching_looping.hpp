#pragma once

#include "executable.hpp"
#include "run_context.hpp"
#include <iostream>
namespace ripl {
class Branch : public Executable {
public:
  Branch(int offset) : _offset(offset - 1) {}
  virtual void execute(RunContext *context) override {
    context->IP(context->beginning() + _offset);
  }

private:
  int _offset;
};

class BranchOnFalse : public Branch {
public:
  BranchOnFalse(int offset) : Branch(offset) {}
  void execute(RunContext *context) override {
    auto [valid, truth] = context->fetch<bool>();
    if (!valid) {
      std::cerr << "Expected a boolean value on top." << std::endl;
      return;
    }
    if (!truth) {
      Branch::execute(context);
    }
  }
};

class BranchOnZero : public Branch {
public:
  BranchOnZero(int offset) : Branch(offset) {}
  void execute(RunContext *context) override {
    auto [valid, value] = context->fetch<long>();
    if (!valid) {
      std::cerr << "Expected an integral value on top but found "
                << typeid(value).name() << "." << std::endl;
      return;
    }
    if (value == 0) {
      Branch::execute(context);
    }
  }
};
} // namespace ripl
