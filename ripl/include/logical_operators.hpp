#pragma once

#include "executable.hpp"
#include "run_context.hpp"
#include <any>
#include <iostream>
#include <memory>
namespace ripl {
class LogicalOperator : public Executable {
public:
  void execute(RunContext *context) override {
    auto [rvalid, rhs] = context->fetch<bool>();
    if (!rvalid) {
      std::cerr << "RHS is not a valid boolean." << std::endl;
      return;
    }
    auto [lvalid, lhs] = context->fetch<bool>();
    if (!lvalid) {
      std::cerr << "LHS is not a valid boolean." << std::endl;
      return;
    }

    bool result = operate(lhs, rhs);
    std::shared_ptr<std::any> res =
        std::make_shared<std::any>(std::any(result));
    context->push(res);
  }

  virtual bool operate(bool lhs, bool rhs) = 0;
};

class And : public LogicalOperator {
public:
  bool operate(bool lhs, bool rhs) override { return lhs && rhs; }
};

class Or : public LogicalOperator {
public:
  bool operate(bool lhs, bool rhs) override { return lhs || rhs; }
};

class Not : public Executable {
public:
  void execute(RunContext *context) override {
    auto [valid, op] = context->fetch<bool>();
    if (!valid) {
      std::cerr << "Top value is not a boolean." << std::endl;
      return;
    }
    std::shared_ptr<std::any> res = std::make_shared<std::any>(std::any(!op));
    context->push(res);
  }
};
} // namespace ripl
