#pragma once

#include "executable.hpp"
#include "run_context.hpp"
#include <any>
#include <iostream>
#include <map>
#include <memory>
#include <stack>
#include <string>
#include <utility>
#include <vector>
namespace ripl {
class Builder;
class Engine : public RunContext {
public:
  Engine();
  ~Engine();

  void run() override;

  std::vector<std::shared_ptr<Executable>>::iterator beginning() override {
    return _code.begin();
  }
  std::vector<std::shared_ptr<Executable>>::iterator IP() override {
    return _ip;
  }
  void IP(std::vector<std::shared_ptr<Executable>>::iterator value) override {
    _ip = value;
  }
  std::vector<std::shared_ptr<Executable>>::iterator start() override {
    return _code.begin();
  }
  int currentPipelineOffset() override { return _code.size() - 1; }

  void push(std::shared_ptr<std::any> value) override { _ds.push(value); }
  std::shared_ptr<std::any> top() override { return _ds.top(); }
  void pop() override { _ds.pop(); }
  int size() override { return _ds.size(); }

  template <typename T> std::pair<bool, T> fetch() {
    auto value = _ds.top();
    if (value->type() == typeid(T)) {
      _ds.pop();
      return std::make_pair(true, std::any_cast<T>(*value));
    }
    return std::make_pair(false, T());
  }

  void replaceValue(std::string &name,
                    std::shared_ptr<std::any> value) override {
    _variables.insert_or_assign(name, value);
  }

  std::shared_ptr<std::any> fetchValue(std::string &name) override {
    auto itr = _variables.find(name);
    if (itr == _variables.end()) {
      std::cerr << "Variable (" << name
                << ") not found. Make sure you have spelled it correctly."
                << std::endl;
      return nullptr;
    }
    return itr->second;
  }

  void finish() override { _isFinished = true; }

  void
  pushRS(std::vector<std::shared_ptr<Executable>>::iterator value) override {
    _rs.push(value);
  }
  std::vector<std::shared_ptr<Executable>>::iterator topRS() override {
    return _rs.top();
  }
  void popRS() override { _rs.pop(); }

  int subOffset(std::string &name) override {
    auto itr = _subroutines.find(name);
    if (itr == _subroutines.end()) {
      std::cerr << "Subroutine (" << name
                << ") not found. Make sure you have not misspelt it."
                << std::endl;
      return -1;
    }
    return itr->second;
  }

private:
  std::vector<std::shared_ptr<Executable>> _code;
  std::vector<std::shared_ptr<Executable>>::iterator _ip;
  std::stack<std::shared_ptr<std::any>> _ds;
  std::map<std::string, std::shared_ptr<std::any>> _variables;
  std::stack<std::vector<std::shared_ptr<Executable>>::iterator>
      _rs; // return stack
  std::map<std::string, int> _subroutines;

  bool _isFinished = false;

  friend class Builder;
};
} // namespace ripl
