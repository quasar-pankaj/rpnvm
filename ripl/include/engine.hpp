#pragma once

#include <any>
#include <functional>
#include <map>
#include <memory>
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

  template <typename L, typename R>
  bool tryOperate(R rhs, std::function<void(L, R)> operate);

  template <typename T> std::pair<bool, T> fetch() {
    auto value = _ds.top();
    if (value->type() == typeid(T)) {
      _ds.pop();
      return std::make_pair(true, std::any_cast<T>(*value));
    }
    return std::make_pair(false, T());
  }

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
