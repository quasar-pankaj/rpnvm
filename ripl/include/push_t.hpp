#pragma once

#include "executable.hpp"
#include "run_context.hpp"
#include <any>
#include <memory>
namespace ripl {
template <typename T> class PushT : public Executable {
public:
  PushT(T value) : _value(std::make_shared<std::any>(std::any(value))) {}

  void execute(RunContext *context) override { context->push(_value); }

private:
  std::shared_ptr<std::any> _value;
};
}
