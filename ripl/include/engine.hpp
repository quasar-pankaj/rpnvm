#pragma once

#include <any>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <stack>
#include <string>
#include <utility>
namespace ripl {
class Engine {
public:
  Engine(char *filename);
  ~Engine();

  void run();

  template <typename T> T read();
  template <typename T> void push(T value);

  void push(std::shared_ptr<std::any> value) { _ds.push(value); }
  std::shared_ptr<std::any> top() { return _ds.top(); }
  void pop() { _ds.pop(); }
  int size() { return _ds.size(); }

  template <typename T> std::pair<bool, T> fetch() {
    auto value = _ds.top();
    if (value->type() == typeid(T)) {
      _ds.pop();
      return std::make_pair(true, std::any_cast<T>(*value));
    }
    return std::make_pair(false, T());
  }

  void replaceValue(std::string &name, std::shared_ptr<std::any> value) {
    _variables.insert_or_assign(name, value);
  }

  std::shared_ptr<std::any> fetchValue(std::string &name) {
    auto itr = _variables.find(name);
    if (itr == _variables.end()) {
      std::cerr << "Variable (" << name
                << ") not found. Make sure you have spelled it correctly."
                << std::endl;
      return nullptr;
    }
    return itr->second;
  }

  void finish() { _isFinished = true; }

  void pushRS(char *value) { _rs.push(value); }
  char *topRS() { return _rs.top(); }
  void popRS() { _rs.pop(); }

private:
  int _codeLen;
  char *_code;
  char *_ip;
  std::stack<std::shared_ptr<std::any>> _ds;
  std::map<std::string, std::shared_ptr<std::any>> _variables;
  std::stack<char *> _rs; // return stack

  bool _isFinished = false;
};
} // namespace ripl
