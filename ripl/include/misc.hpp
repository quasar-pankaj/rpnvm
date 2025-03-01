#pragma once

#include "executable.hpp"
#include "run_context.hpp"
#include <iostream>
#include <memory>
#include <string>
namespace ripl {

class Print : public Executable {
public:
  void execute(RunContext *context) override;
};

class Input : public Executable {
public:
  void execute(RunContext *context) override;
};

class End : public Executable {
public:
  void execute(RunContext *context) override {
    context->finish();
    std::cout << "Stack Size: " << context->size() << std::endl;
  }
};

class Assign : public Executable {
public:
  Assign(std::string &name) : _varname(name) {}

  void execute(RunContext *context) override {
    auto value = context->top();
    context->pop();
    context->replaceValue(_varname, value);
  }

private:
  std::string _varname;
};

class Extract : public Executable {
public:
  Extract(std::string &name) : _varname(name) {}

  void execute(RunContext *context) override {
    auto var = context->fetchValue(_varname);
    context->push(var);
  }

private:
  std::string _varname;
};

} // namespace ripl
