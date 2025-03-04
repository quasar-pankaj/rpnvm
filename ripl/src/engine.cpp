#include "engine.hpp"
#include "instruction_set.hpp"
#include "utils.hpp"
#include <any>
#include <cstring>
#include <fstream>
#include <functional>
#include <ios>
#include <iostream>
#include <memory>
#include <string>

#define INPUT_SIZE 255

ripl::Engine::Engine(char *filename) {
  std::ifstream in(filename);
  if (!in) {
    std::cerr << "Could not open file " << filename << " for input."
              << std::endl;
    return;
  }

  in.seekg(0, std::ios::end);
  _codeLen = in.tellg();
  in.seekg(0, std::ios::beg);

  _code = new char[_codeLen];
  in.read(_code, _codeLen);
}

template <typename T> T ripl::Engine::read() {
  T value;
  int len = sizeof(T);
  std::memcpy((char *)&value, _ip, len);
  _ip += len;
  return value;
}

template <> std::string ripl::Engine::read<std::string>() {
  int len = read<int>();
  char *str = new char[len + 1];
  std::strncpy(str, _ip, len);
  str[len] = '\0';
  std::string s(str);
  delete[] str;
  _ip += len;
  return s;
}

template <typename T> void ripl::Engine::push(T value) {
  std::any a(value);
  std::shared_ptr<std::any> ptr = std::make_shared<std::any>(a);
  _ds.push(ptr);
}

ripl::Engine::~Engine() {
  _variables.clear();
  delete[] _code;
}

template <typename L, typename R>
bool ripl::Engine::tryOperate(R rhs, std::function<void(L, R)> operate) {
  auto [valid, lhs] = fetch<L>();
  if (!valid) {
    return false;
  }
  operate(lhs, rhs);
  return true;
}

void ripl::Engine::run() {
  _ip = _code;
  const char *end = _code + _codeLen;
  while (_ip < end && !_isFinished) {
    Instruction mnemonic = (Instruction)*_ip;

    switch (mnemonic) {
    case Instruction::NOP:
      break;
    case Instruction::PUSHL: {
      _ip++;
      long l = read<long>();
      push(l);
    } break;
    case Instruction::PUSHD: {
      _ip++;
      double d = read<double>();
      push(d);
    } break;
    case Instruction::PUSHB: {
      _ip++;
      bool b = read<bool>();
      push(b);
    } break;
    case Instruction::PUSHS: {
      _ip++;
      std::string s = read<std::string>();
      push(s);
    } break;
    case Instruction::ADD: {
      auto [drvalid, drvalue] = fetch<double>();
      if (drvalid) {
        bool dlvalid =
            tryOperate<double, double>(drvalue, [this](double lhs, double rhs) {
              double result = lhs + rhs;
              push(result);
            });
        if (dlvalid) {
          _ip++;
          continue;
        }
        bool llvalid =
            tryOperate<long, double>(drvalue, [this](long lhs, double rhs) {
              double result = lhs + rhs;
              push(result);
            });
        if (llvalid) {
          _ip++;
          continue;
        }
        bool slvalid = tryOperate<std::string, double>(
            drvalue, [this](std::string lhs, double rhs) {
              std::string result = lhs + std::to_string(rhs);
              push(result);
            });
        if (slvalid) {
          _ip++;
          continue;
        }
        std::cerr << "Invalid LHS value." << std::endl;
      }
      auto [lrvalid, lrvalue] = fetch<long>();
      if (lrvalid) {
        bool dlvalid =
            tryOperate<double, long>(lrvalue, [this](double lhs, long rhs) {
              double result = lhs + rhs;
              push(result);
            });
        if (dlvalid) {
          _ip++;
          continue;
        }
        bool llvalid =
            tryOperate<long, long>(lrvalue, [this](long lhs, long rhs) {
              long result = lhs + rhs;
              push(result);
            });
        if (llvalid) {
          _ip++;
          continue;
        }
        bool slvalid = tryOperate<std::string, long>(
            lrvalue, [this](std::string lhs, long rhs) {
              std::string result = lhs + std::to_string(rhs);
              push(result);
            });
        if (slvalid) {
          _ip++;
          continue;
        }
        std::cerr << "Invalid LHS value." << std::endl;
      }
      auto [srvalid, srvalue] = fetch<std::string>();
      if (srvalid) {
        bool dlvalid = tryOperate<double, std::string>(
            srvalue, [this](double lhs, std::string rhs) {
              std::string result = std::to_string(lhs) + rhs;
              push(result);
            });
        if (dlvalid) {
          _ip++;
          continue;
        }
        bool llvalid = tryOperate<long, std::string>(
            srvalue, [this](long lhs, std::string rhs) {
              std::string result = std::to_string(lhs) + rhs;
              push(result);
            });
        if (llvalid) {
          _ip++;
          continue;
        }
        bool slvalid = tryOperate<std::string, std::string>(
            srvalue, [this](std::string lhs, std::string rhs) {
              std::string result = lhs + rhs;
              push(result);
            });
        if (slvalid) {
          _ip++;
          continue;
        }
        std::cerr << "Invalid LHS value." << std::endl;
      }
      _ip++;
    } break;
    case Instruction::SUB: {
      auto [drvalid, drvalue] = fetch<double>();
      if (drvalid) {
        bool dlvalid =
            tryOperate<double, double>(drvalue, [this](double lhs, double rhs) {
              double result = lhs - rhs;
              push(result);
            });
        if (dlvalid) {
          _ip++;
          continue;
        }
        bool llvalid =
            tryOperate<long, double>(drvalue, [this](long lhs, double rhs) {
              double result = lhs - rhs;
              push(result);
            });
        if (llvalid) {
          _ip++;
          continue;
        }
        std::cerr << "Invalid LHS." << std::endl;
      }
      auto [lrvalid, lrvalue] = fetch<long>();
      if (lrvalid) {
        bool dlvalid =
            tryOperate<double, long>(lrvalue, [this](double lhs, long rhs) {
              double result = lhs - rhs;
              push(result);
            });
        if (dlvalid) {
          _ip++;
          continue;
        }
        bool llvalid =
            tryOperate<long, long>(lrvalue, [this](long lhs, long rhs) {
              long result = lhs - rhs;
              push(result);
            });
        if (llvalid) {
          _ip++;
          continue;
        }
        std::cerr << "Invalid LHS." << std::endl;
      }
      std::cerr << "Invalid RHS." << std::endl;
      _ip++;
    } break;
    case Instruction::MUL: {
      auto [drvalid, drvalue] = fetch<double>();
      if (drvalid) {
        bool dlvalid =
            tryOperate<double, double>(drvalue, [this](double lhs, double rhs) {
              double result = lhs * rhs;
              push(result);
            });
        if (dlvalid) {
          _ip++;
          continue;
        }
        bool llvalid =
            tryOperate<long, double>(drvalue, [this](long lhs, double rhs) {
              double result = lhs * rhs;
              push(result);
            });
        if (llvalid) {
          _ip++;
          continue;
        }
        std::cerr << "Invalid LHS." << std::endl;
      }
      auto [lrvalid, lrvalue] = fetch<long>();
      if (lrvalid) {
        bool dlvalid =
            tryOperate<double, long>(lrvalue, [this](double lhs, long rhs) {
              double result = lhs * rhs;
              push(result);
            });
        if (dlvalid) {
          _ip++;
          continue;
        }
        bool llvalid =
            tryOperate<long, long>(lrvalue, [this](long lhs, long rhs) {
              long result = lhs * rhs;
              push(result);
            });
        if (llvalid) {
          _ip++;
          continue;
        }
        std::cerr << "Invalid LHS." << std::endl;
      }
      std::cerr << "Invalid RHS." << std::endl;
      _ip++;
    } break;
    case Instruction::DIV: {
      auto [drvalid, drvalue] = fetch<double>();
      if (drvalid) {
        bool dlvalid =
            tryOperate<double, double>(drvalue, [this](double lhs, double rhs) {
              double result = lhs / rhs;
              push(result);
            });
        if (dlvalid) {
          _ip++;
          continue;
        }
        bool llvalid =
            tryOperate<long, double>(drvalue, [this](long lhs, double rhs) {
              double result = lhs / rhs;
              push(result);
            });
        if (llvalid) {
          _ip++;
          continue;
        }
        std::cerr << "Invalid LHS." << std::endl;
      }
      auto [lrvalid, lrvalue] = fetch<long>();
      if (lrvalid) {
        bool dlvalid =
            tryOperate<double, long>(lrvalue, [this](double lhs, long rhs) {
              double result = lhs / rhs;
              push(result);
            });
        if (dlvalid) {
          _ip++;
          continue;
        }
        bool llvalid =
            tryOperate<long, long>(lrvalue, [this](long lhs, long rhs) {
              double result = (double)lhs / rhs;
              push(result);
            });
        if (llvalid) {
          _ip++;
          continue;
        }
        std::cerr << "Invalid LHS." << std::endl;
      }
      std::cerr << "Invalid RHS." << std::endl;
      _ip++;
    } break;
    case Instruction::MOD: {
      auto [rvalid, rvalue] = fetch<long>();
      if (!rvalid) {
        std::cerr << "Expected a long on the left hand side." << std::endl;
        continue;
      }
      auto [lvalid, lvalue] = fetch<long>();
      if (!lvalid) {
        std::cerr << "expected a long on lvalue." << std::endl;
        continue;
      }
      auto result = lvalue % rvalue;
      push(result);
      _ip++;
    } break;
    case Instruction::AND: {
      auto [rvalid, rvalue] = fetch<bool>();
      if (!rvalid) {
        std::cerr << "Expected a boolean on right hand side." << std::endl;
        continue;
      }
      auto [lvalid, lvalue] = fetch<bool>();
      if (!lvalid) {
        std::cerr << "Expected a boolean on left hand side." << std::endl;
        continue;
      }
      auto result = lvalue && rvalue;
      push(result);
      _ip++;
    } break;
    case Instruction::OR: {
      auto [rvalid, rvalue] = fetch<bool>();
      if (!rvalid) {
        std::cerr << "Expected a boolean on right hand side." << std::endl;
        continue;
      }
      auto [lvalid, lvalue] = fetch<bool>();
      if (!lvalid) {
        std::cerr << "Expected a boolean on left hand side." << std::endl;
        continue;
      }
      auto result = lvalue || rvalue;
      push(result);
      _ip++;
    } break;
    case Instruction::NOT: {
      auto [valid, value] = fetch<bool>();
      if (!valid) {
        std::cerr << "Expected a bool on stack." << std::endl;
        continue;
      }
      auto result = !value;
      push(result);
      _ip++;
    } break;
    case Instruction::EQ: {
      auto [drvalid, drvalue] = fetch<double>();
      if (drvalid) {
        auto [dlvalid, dlvalue] = fetch<double>();
        if (dlvalid) {
          auto result = dlvalue == drvalue;
          push(result);
          _ip++;
          continue;
        }
        auto [llvalid, llvalue] = fetch<long>();
        auto result = llvalue == drvalue;
        push(result);
        _ip++;
        continue;
      }
      auto [lrvalid, lrvalue] = fetch<long>();
      if (lrvalid) {
        auto [dlvalid, dlvalue] = fetch<double>();
        if (dlvalid) {
          auto result = dlvalue == lrvalue;
          push(result);
          _ip++;
          continue;
        }
        auto [llvalid, llvalue] = fetch<long>();
        auto result = llvalue == lrvalue;
        push(result);
        _ip++;
        continue;
      }
      auto [srvalid, srvalue] = fetch<std::string>();
      auto [slvalid, slvalue] = fetch<std::string>();
      auto result = (srvalid && slvalid && slvalue == srvalue);
      push(result);
      _ip++;
    } break;
    case Instruction::NEQ: {
      auto [drvalid, drvalue] = fetch<double>();
      if (drvalid) {
        auto [dlvalid, dlvalue] = fetch<double>();
        if (dlvalid) {
          auto result = dlvalue != drvalue;
          push(result);
          _ip++;
          continue;
        }
        auto [llvalid, llvalue] = fetch<long>();
        auto result = llvalue != drvalue;
        push(result);
        _ip++;
        continue;
      }
      auto [lrvalid, lrvalue] = fetch<long>();
      if (lrvalid) {
        auto [dlvalid, dlvalue] = fetch<double>();
        if (dlvalid) {
          auto result = dlvalue != lrvalue;
          push(result);
          _ip++;
          continue;
        }
        auto [llvalid, llvalue] = fetch<long>();
        auto result = llvalue != lrvalue;
        push(result);
        _ip++;
        continue;
      }
      auto [srvalid, srvalue] = fetch<std::string>();
      auto [slvalid, slvalue] = fetch<std::string>();
      auto result = (srvalid && slvalid && slvalue != srvalue);
      push(result);
      _ip++;
    } break;
    case Instruction::GT: {
      auto [drvalid, drvalue] = fetch<double>();
      if (drvalid) {
        auto [dlvalid, dlvalue] = fetch<double>();
        if (dlvalid) {
          auto result = dlvalue > drvalue;
          push(result);
          _ip++;
          continue;
        }
        auto [llvalid, llvalue] = fetch<long>();
        auto result = llvalue > drvalue;
        push(result);
        _ip++;
        continue;
      }
      auto [lrvalid, lrvalue] = fetch<long>();
      if (lrvalid) {
        auto [dlvalid, dlvalue] = fetch<double>();
        if (dlvalid) {
          auto result = dlvalue > lrvalue;
          push(result);
          _ip++;
          continue;
        }
        auto [llvalid, llvalue] = fetch<long>();
        auto result = llvalue > lrvalue;
        push(result);
        _ip++;
        continue;
      }
      auto [srvalid, srvalue] = fetch<std::string>();
      auto [slvalid, slvalue] = fetch<std::string>();
      auto result = (srvalid && slvalid && slvalue > srvalue);
      push(result);
      _ip++;
    } break;
    case Instruction::LT: {
      auto [drvalid, drvalue] = fetch<double>();
      if (drvalid) {
        auto [dlvalid, dlvalue] = fetch<double>();
        if (dlvalid) {
          auto result = dlvalue < drvalue;
          push(result);
          _ip++;
          continue;
        }
        auto [llvalid, llvalue] = fetch<long>();
        auto result = llvalue < drvalue;
        push(result);
        _ip++;
        continue;
      }
      auto [lrvalid, lrvalue] = fetch<long>();
      if (lrvalid) {
        auto [dlvalid, dlvalue] = fetch<double>();
        if (dlvalid) {
          auto result = dlvalue < lrvalue;
          push(result);
          _ip++;
          continue;
        }
        auto [llvalid, llvalue] = fetch<long>();
        auto result = llvalue < lrvalue;
        push(result);
        _ip++;
        continue;
      }
      auto [srvalid, srvalue] = fetch<std::string>();
      auto [slvalid, slvalue] = fetch<std::string>();
      auto result = (srvalid && slvalid && slvalue < srvalue);
      push(result);
      _ip++;
    } break;
    case Instruction::GTE: {
      auto [drvalid, drvalue] = fetch<double>();
      if (drvalid) {
        auto [dlvalid, dlvalue] = fetch<double>();
        if (dlvalid) {
          auto result = dlvalue >= drvalue;
          push(result);
          _ip++;
          continue;
        }
        auto [llvalid, llvalue] = fetch<long>();
        auto result = llvalue >= drvalue;
        push(result);
        _ip++;
        continue;
      }
      auto [lrvalid, lrvalue] = fetch<long>();
      if (lrvalid) {
        auto [dlvalid, dlvalue] = fetch<double>();
        if (dlvalid) {
          auto result = dlvalue >= lrvalue;
          push(result);
          _ip++;
          continue;
        }
        auto [llvalid, llvalue] = fetch<long>();
        auto result = llvalue >= lrvalue;
        push(result);
        _ip++;
        continue;
      }
      auto [srvalid, srvalue] = fetch<std::string>();
      auto [slvalid, slvalue] = fetch<std::string>();
      auto result = (srvalid && slvalid && slvalue >= srvalue);
      push(result);
      _ip++;
    } break;
    case Instruction::LTE: {
      auto [drvalid, drvalue] = fetch<double>();
      if (drvalid) {
        auto [dlvalid, dlvalue] = fetch<double>();
        if (dlvalid) {
          auto result = dlvalue <= drvalue;
          push(result);
          _ip++;
          continue;
        }
        auto [llvalid, llvalue] = fetch<long>();
        auto result = llvalue <= drvalue;
        push(result);
        _ip++;
        continue;
      }
      auto [lrvalid, lrvalue] = fetch<long>();
      if (lrvalid) {
        auto [dlvalid, dlvalue] = fetch<double>();
        if (dlvalid) {
          auto result = dlvalue <= lrvalue;
          push(result);
          _ip++;
          continue;
        }
        auto [llvalid, llvalue] = fetch<long>();
        auto result = llvalue <= lrvalue;
        push(result);
        _ip++;
        continue;
      }
      auto [srvalid, srvalue] = fetch<std::string>();
      auto [slvalid, slvalue] = fetch<std::string>();
      auto result = (srvalid && slvalid && slvalue <= srvalue);
      push(result);
      _ip++;
    } break;
    case Instruction::JZ: {
      _ip++;
      int offset = read<int>();
      auto [valid, value] = fetch<long>();
      if (valid && (value == 0)) {
        _ip = _code + offset;
      }
    } break;
    case Instruction::JF: {
      _ip++;
      int offset = read<int>();
      auto [valid, value] = fetch<bool>();
      if (valid && !value) {
        _ip = _code + offset;
      }
    } break;
    case Instruction::JMP: {
      _ip++;
      int offset = read<int>();
      _ip = _code + offset;
    } break;
    case Instruction::ID:
      break;
    case Instruction::VAR: {
      _ip++;
      auto name = read<std::string>();
      _variables.insert({name, std::make_shared<std::any>(std::any(0))});
    } break;
    case Instruction::ASSIGN: {
      _ip++;
      auto name = read<std::string>();
      auto value = _ds.top();
      _ds.pop();
      _variables.insert_or_assign(name, value);
    } break;
    case Instruction::DEREF: {
      _ip++;
      auto name = read<std::string>();
      auto itr = _variables.find(name);
      _ds.push(itr->second);
    } break;
    case Instruction::CALL: {
      _ip++;
      auto addr = read<int>();
      _rs.push(_ip);
      _ip = _code + addr;
    } break;
    case Instruction::RET: {
      _ip = _rs.top();
      _rs.pop();
    } break;
    case Instruction::DUP: {
      auto value = _ds.top();
      _ds.push(value);
      _ip++;
    } break;
    case Instruction::SWAP: {
      auto top = _ds.top();
      _ds.pop();
      auto bottom = _ds.top();
      _ds.pop();
      _ds.push(top);
      _ds.push(bottom);
      _ip++;
    } break;
    case Instruction::ROTUP: {
      auto top = _ds.top();
      _ds.pop();
      auto second = _ds.top();
      _ds.pop();
      auto third = _ds.top();
      _ds.pop();
      _ds.push(top);
      _ds.push(third);
      _ds.push(second);
      _ip++;
    } break;
    case Instruction::ROTDN: {
      auto top = _ds.top();
      _ds.pop();
      auto second = _ds.top();
      _ds.pop();
      auto third = _ds.top();
      _ds.pop();
      _ds.push(second);
      _ds.push(top);
      _ds.push(third);
      _ip++;
    } break;
    case Instruction::DROP: {
      _ds.pop();
      _ip++;
    } break;
    case Instruction::INC: {
      auto [valid, value] = fetch<long>();
      if (valid) {
        value++;
        push(value);
      }
      _ip++;
    } break;
    case Instruction::DEC: {
      auto [valid, value] = fetch<long>();
      if (valid) {
        value--;
        push(value);
      }
      _ip++;
    } break;
    case Instruction::EXPECT: {
      char input[INPUT_SIZE];
      std::cin.getline(input, INPUT_SIZE);
      std::string token(input);
      if (ripl::isIntegral(token)) {
        long l = std::stol(token);
        push(l);
        _ip++;
        continue;
      }
      if (ripl::isFloat(token)) {
        double d = std::stod(token);
        push(d);
        _ip++;
        continue;
      }
      if (ripl::isBool(token)) {
        bool b = token == "true" ? true : false;
        push(b);
        _ip++;
        continue;
      }
      std::string s(token);
      push(s);
      _ip++;
    } break;
    case Instruction::PRINT: {
      auto value = _ds.top();
      _ds.pop();
      if (value->type() == typeid(double)) {
        std::cout << std::any_cast<double>(*value) << std::endl;
        _ip++;
        continue;
      }
      if (value->type() == typeid(long)) {
        std::cout << std::any_cast<long>(*value) << std::endl;
        _ip++;
        continue;
      }
      if (value->type() == typeid(std::string)) {
        std::cout << std::any_cast<std::string>(*value) << std::endl;
        _ip++;
        continue;
      }
      if (value->type() == typeid(bool)) {
        auto truth = std::any_cast<bool>(*value);
        if (truth) {
          std::cout << "true" << std::endl;
        } else {
          std::cout << "false" << std::endl;
        }
        _ip++;
        continue;
      }
      std::cout << "Unknown type " << value->type().name() << std::endl;
      _ip++;
    } break;
    case Instruction::END: {
      _isFinished = true;
      std::cout << "Stack Size: " << _ds.size() << std::endl;
    } break;
    }
  }
}
