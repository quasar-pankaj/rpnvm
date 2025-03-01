#pragma once

#include "executable.hpp"
#include "run_context.hpp"
#include <any>
#include <iostream>
#include <memory>
namespace ripl {
class Inc : public Executable {
public:
  void execute(RunContext *context) override {
    auto [valid, value] = context->fetch<long>();
    if (!valid) {
      std::cerr
          << "Top of stack doesn't contain an integral value cannot increment."
          << std::endl;
      return;
    }
    value++;
    context->push(std::make_shared<std::any>(std::any(value)));
  }
};

class Dec : public Executable {
public:
  void execute(RunContext *context) override {
    auto [valid, value] = context->fetch<long>();
    if (!valid) {
      std::cerr
          << "Top of stack doesn't contain an integral value cannot decrement."
          << std::endl;
      return;
    }
    value--;
    context->push(std::make_shared<std::any>(std::any(value)));
  }
};
} // namespace ripl
