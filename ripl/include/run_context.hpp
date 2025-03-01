#pragma once

#include <any>
#include <memory>
#include <vector>
namespace ripl {
class Executable;
class RunContext {
public:
  virtual void run() = 0;

  // data stack
  virtual std::shared_ptr<std::any> top() = 0;
  virtual void push(std::shared_ptr<std::any> value) = 0;
  virtual void pop() = 0;
  virtual int size() = 0;

  // return stack
  virtual void
  pushRS(std::vector<std::shared_ptr<Executable>>::iterator value) = 0;
  virtual std::vector<std::shared_ptr<Executable>>::iterator topRS() = 0;
  virtual void popRS() = 0;

  // Execution management
  virtual std::vector<std::shared_ptr<Executable>>::iterator beginning() = 0;
  virtual std::vector<std::shared_ptr<Executable>>::iterator IP() = 0;
  virtual void IP(std::vector<std::shared_ptr<Executable>>::iterator itr) = 0;
  virtual int currentPipelineOffset() = 0;

  // Subroutines
  virtual int subOffset(std::string &name) = 0;
  virtual std::vector<std::shared_ptr<Executable>>::iterator start() = 0;

  // variables
  virtual std::shared_ptr<std::any> fetchValue(std::string &name) = 0;
  virtual void replaceValue(std::string &varname,
                            std::shared_ptr<std::any> value) = 0;

  template <typename T> std::pair<bool, T> fetch() {
    auto value = top();
    if (value->type() == typeid(T)) {
      pop();
      return std::make_pair(true, std::any_cast<T>(*value));
    }
    return std::make_pair(false, T());
  }

  virtual void finish() = 0;
};
} // namespace ripl
