#include "engine.hpp"
#include "instruction_set.hpp"
#include "utils.hpp"
#include <any>
#include <cstring>
#include <fstream>
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

ripl::Engine::~Engine() { _variables.clear(); }

void ripl::Engine::run() {
  _ip = _code;
  const char *end = _code + _codeLen;
  while (!(_ip == end || _isFinished)) {
    Instruction mnemonic = (Instruction)*_ip;

    switch (mnemonic) {
    case Instruction::NOP:
      break;
    case Instruction::PUSHL: {
      long l = read<long>();
      push(l);
    } break;
    case Instruction::PUSHD: {
      double d = read<double>();
      push(d);
    } break;
    case Instruction::PUSHB: {
      bool b = read<bool>();
      push(b);
    } break;
    case Instruction::PUSHS: {
      std::string s = read<std::string>();
      push(s);
    } break;
    case Instruction::ADD: {
      auto [drvalid, drhs] = fetch<double>();
      if (drvalid) {
        auto [dlvalid, dlhs] = fetch<double>();
        if (dlvalid) {
          auto result = dlhs + drhs;
          push(result);
          break;
        }
        auto [llvalid, llhs] = fetch<long>();
        if (llvalid) {
          auto result = llhs + drhs;
          push(result);
          break;
        }
        auto [slvalid, slhs] = fetch<std::string>();
        if (slvalid) {
          auto result = slhs + std::to_string(drhs);
          push(result);
          break;
        }
        push(drhs);
        std::cerr << "Invalid lhs value." << std::endl;
        break;
      }
      auto [lrvalid, lrhs] = fetch<long>();
      if (lrvalid) {
        auto [dlvalid, dlhs] = fetch<double>();
        if (dlvalid) {
          auto result = dlhs + lrhs;
          push(result);
          break;
        }
        auto [llvalid, llhs] = fetch<long>();
        if (llvalid) {
          auto result = llhs + lrhs;
          push(result);
          break;
        }
        auto [slvalid, slhs] = fetch<std::string>();
        if (slvalid) {
          auto result = slhs + std::to_string(lrhs);
          push(result);
          break;
        }
        push(lrhs);
        std::cerr << "Invalid lhs value." << std::endl;
        break;
      }
      auto [srvalid, srhs] = fetch<std::string>();
      if (srvalid) {
        auto [dlvalid, dlhs] = fetch<double>();
        if (dlvalid) {
          auto result = std::to_string(dlhs) + srhs;
          push(result);
          break;
        }
        auto [llvalid, llhs] = fetch<long>();
        if (llvalid) {
          auto result = std::to_string(llhs) + srhs;
          push(result);
          break;
        }
        auto [slvalid, slhs] = fetch<std::string>();
        if (slvalid) {
          auto result = slhs + srhs;
          push(result);
          break;
        }
        push(lrhs);
        std::cerr << "Invalid lhs value." << std::endl;
        break;
      }
      std::cerr << "Invalid RHS value." << std::endl;
    } break;
    case Instruction::SUB: {
      auto [drvalid, drvalue] = fetch<double>();
      if (drvalid) {
        auto [dlvalid, dlvalue] = fetch<double>();
        if (dlvalid) {
          auto result = dlvalue - drvalue;
          push(result);
          break;
        }
        auto [llvalid, llvalue] = fetch<long>();
        if (llvalid) {
          auto result = llvalue - drvalue;
          push(result);
          break;
        }
        std::cerr << "Invalid LHS value." << std::endl;
      }
      auto [lrvalid, lrvalue] = fetch<long>();
      if (lrvalid) {
        auto [dlvalid, dlvalue] = fetch<double>();
        if (dlvalid) {
          auto result = dlvalue - lrvalue;
          push(result);
          break;
        }
        auto [llvalid, llvalue] = fetch<long>();
        if (llvalid) {
          auto result = llvalue - lrvalue;
          push(result);
          break;
        }
        std::cerr << "Invalid RHS value." << std::endl;
      }
    } break;
    case Instruction::MUL: {
      auto [drvalid, drvalue] = fetch<double>();
      if (drvalid) {
        auto [dlvalid, dlvalue] = fetch<double>();
        if (dlvalid) {
          auto result = dlvalue * drvalue;
          push(result);
          break;
        }
        auto [llvalid, llvalue] = fetch<long>();
        if (llvalid) {
          auto result = llvalue * drvalue;
          push(result);
          break;
        }
        std::cerr << "Invalid LHS value." << std::endl;
      }
      auto [lrvalid, lrvalue] = fetch<long>();
      if (lrvalid) {
        auto [dlvalid, dlvalue] = fetch<double>();
        if (dlvalid) {
          auto result = dlvalue * lrvalue;
          push(result);
          break;
        }
        auto [llvalid, llvalue] = fetch<long>();
        if (llvalid) {
          auto result = llvalue * lrvalue;
          push(result);
          break;
        }
        std::cerr << "Invalid RHS value." << std::endl;
      }
    } break;
    case Instruction::DIV: {
      auto [drvalid, drvalue] = fetch<double>();
      if (drvalid) {
        auto [dlvalid, dlvalue] = fetch<double>();
        if (dlvalid) {
          auto result = dlvalue / drvalue;
          push(result);
          break;
        }
        auto [llvalid, llvalue] = fetch<long>();
        if (llvalid) {
          auto result = llvalue / drvalue;
          push(result);
          break;
        }
        std::cerr << "Invalid LHS value." << std::endl;
      }
      auto [lrvalid, lrvalue] = fetch<long>();
      if (lrvalid) {
        auto [dlvalid, dlvalue] = fetch<double>();
        if (dlvalid) {
          auto result = dlvalue / lrvalue;
          push(result);
          break;
        }
        auto [llvalid, llvalue] = fetch<long>();
        if (llvalid) {
          auto result = llvalue / lrvalue;
          push(result);
          break;
        }
        std::cerr << "Invalid RHS value." << std::endl;
      }
    } break;
    case Instruction::MOD: {
      auto [rvalid, rvalue] = fetch<long>();
      if (!rvalid) {
        std::cerr << "Expected a long on the left hand side." << std::endl;
        break;
      }
      auto [lvalid, lvalue] = fetch<long>();
      if (!lvalid) {
        std::cerr << "expected a long on lvalue." << std::endl;
        break;
      }
      auto result = lvalue % rvalue;
      push(result);
    } break;
    case Instruction::AND: {
      auto [rvalid, rvalue] = fetch<bool>();
      if (!rvalid) {
        std::cerr << "Expected a boolean on right hand side." << std::endl;
        break;
      }
      auto [lvalid, lvalue] = fetch<bool>();
      if (!lvalid) {
        std::cerr << "Expected a boolean on left hand side." << std::endl;
        break;
      }
      auto result = lvalue && rvalue;
      push(result);
    } break;
    case Instruction::OR: {
      auto [rvalid, rvalue] = fetch<bool>();
      if (!rvalid) {
        std::cerr << "Expected a boolean on right hand side." << std::endl;
        break;
      }
      auto [lvalid, lvalue] = fetch<bool>();
      if (!lvalid) {
        std::cerr << "Expected a boolean on left hand side." << std::endl;
        break;
      }
      auto result = lvalue || rvalue;
      push(result);
    } break;
    case Instruction::NOT: {
      auto [valid, value] = fetch<bool>();
      if (!valid) {
        std::cerr << "Expected a bool on stack." << std::endl;
        break;
      }
      auto result = !value;
      push(result);
    } break;
    case Instruction::EQ: {
      auto [drvalid, drvalue] = fetch<double>();
      if (drvalid) {
        auto [dlvalid, dlvalue] = fetch<double>();
        if (dlvalid) {
          auto result = dlvalue == drvalue;
          push(result);
          break;
        }
        auto [llvalid, llvalue] = fetch<long>();
        auto result = llvalue == drvalue;
        push(result);
        break;
      }
      auto [lrvalid, lrvalue] = fetch<long>();
      if (lrvalid) {
        auto [dlvalid, dlvalue] = fetch<double>();
        if (dlvalid) {
          auto result = dlvalue == lrvalue;
          push(result);
          break;
        }
        auto [llvalid, llvalue] = fetch<long>();
        auto result = llvalue == lrvalue;
        push(result);
        break;
      }
    } break;
    case Instruction::NEQ: {
      auto [drvalid, drvalue] = fetch<double>();
      if (drvalid) {
        auto [dlvalid, dlvalue] = fetch<double>();
        if (dlvalid) {
          auto result = dlvalue != drvalue;
          push(result);
          break;
        }
        auto [llvalid, llvalue] = fetch<long>();
        auto result = llvalue != drvalue;
        push(result);
        break;
      }
      auto [lrvalid, lrvalue] = fetch<long>();
      if (lrvalid) {
        auto [dlvalid, dlvalue] = fetch<double>();
        if (dlvalid) {
          auto result = dlvalue != lrvalue;
          push(result);
          break;
        }
        auto [llvalid, llvalue] = fetch<long>();
        auto result = llvalue != lrvalue;
        push(result);
        break;
      }
    } break;
    case Instruction::GT: {
      auto [drvalid, drvalue] = fetch<double>();
      if (drvalid) {
        auto [dlvalid, dlvalue] = fetch<double>();
        if (dlvalid) {
          auto result = dlvalue > drvalue;
          push(result);
          break;
        }
        auto [llvalid, llvalue] = fetch<long>();
        auto result = llvalue > drvalue;
        push(result);
        break;
      }
      auto [lrvalid, lrvalue] = fetch<long>();
      if (lrvalid) {
        auto [dlvalid, dlvalue] = fetch<double>();
        if (dlvalid) {
          auto result = dlvalue > lrvalue;
          push(result);
          break;
        }
        auto [llvalid, llvalue] = fetch<long>();
        auto result = llvalue > lrvalue;
        push(result);
        break;
      }
    } break;
    case Instruction::LT: {
      auto [drvalid, drvalue] = fetch<double>();
      if (drvalid) {
        auto [dlvalid, dlvalue] = fetch<double>();
        if (dlvalid) {
          auto result = dlvalue < drvalue;
          push(result);
          break;
        }
        auto [llvalid, llvalue] = fetch<long>();
        auto result = llvalue < drvalue;
        push(result);
        break;
      }
      auto [lrvalid, lrvalue] = fetch<long>();
      if (lrvalid) {
        auto [dlvalid, dlvalue] = fetch<double>();
        if (dlvalid) {
          auto result = dlvalue < lrvalue;
          push(result);
          break;
        }
        auto [llvalid, llvalue] = fetch<long>();
        auto result = llvalue < lrvalue;
        push(result);
        break;
      }
    } break;
    case Instruction::GTE: {
      auto [drvalid, drvalue] = fetch<double>();
      if (drvalid) {
        auto [dlvalid, dlvalue] = fetch<double>();
        if (dlvalid) {
          auto result = dlvalue >= drvalue;
          push(result);
          break;
        }
        auto [llvalid, llvalue] = fetch<long>();
        auto result = llvalue >= drvalue;
        push(result);
        break;
      }
      auto [lrvalid, lrvalue] = fetch<long>();
      if (lrvalid) {
        auto [dlvalid, dlvalue] = fetch<double>();
        if (dlvalid) {
          auto result = dlvalue >= lrvalue;
          push(result);
          break;
        }
        auto [llvalid, llvalue] = fetch<long>();
        auto result = llvalue >= lrvalue;
        push(result);
        break;
      }
    } break;
    case Instruction::LTE: {
      auto [drvalid, drvalue] = fetch<double>();
      if (drvalid) {
        auto [dlvalid, dlvalue] = fetch<double>();
        if (dlvalid) {
          auto result = dlvalue <= drvalue;
          push(result);
          break;
        }
        auto [llvalid, llvalue] = fetch<long>();
        auto result = llvalue <= drvalue;
        push(result);
        break;
      }
      auto [lrvalid, lrvalue] = fetch<long>();
      if (lrvalid) {
        auto [dlvalid, dlvalue] = fetch<double>();
        if (dlvalid) {
          auto result = dlvalue <= lrvalue;
          push(result);
          break;
        }
        auto [llvalid, llvalue] = fetch<long>();
        auto result = llvalue <= lrvalue;
        push(result);
        break;
      }
    } break;
    case Instruction::JZ: {
      int offset = read<int>();
      auto [valid, value] = fetch<long>();
      if (valid && (value == 0)) {
        _ip = _code + offset;
      }
    } break;
    case Instruction::JF: {
      int offset = read<int>();
      auto [valid, value] = fetch<bool>();
      if (valid && value) {
        _ip = _code + offset;
      }
    } break;
    case Instruction::JMP: {
      int offset = read<int>();
      _ip = _code + offset;
    } break;
    case Instruction::ID:
      break;
    case Instruction::VAR: {
      auto name = read<std::string>();
      _variables.insert({name, std::make_shared<std::any>(std::any(0))});
    } break;
    case Instruction::ASSIGN: {
      auto name = read<std::string>();
      auto value = _ds.top();
      _ds.pop();
      _variables.insert_or_assign(name, value);
    } break;
    case Instruction::DEREF: {
      auto name = read<std::string>();
      auto itr = _variables.find(name);
      _ds.push(itr->second);
    } break;
    case Instruction::CALL: {
      _rs.push(_ip);
      auto addr = read<int>();
      _ip = _code + addr;
    } break;
    case Instruction::RET: {
      _ip = _rs.top();
      _rs.pop();
    } break;
    case Instruction::DUP: {
      auto value = _ds.top();
      _ds.push(value);
    } break;
    case Instruction::SWAP: {
      auto top = _ds.top();
      _ds.pop();
      auto bottom = _ds.top();
      _ds.pop();
      _ds.push(top);
      _ds.push(bottom);
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
    } break;
    case Instruction::DROP: {
      _ds.pop();
    } break;
    case Instruction::INC: {
      auto [valid, value] = fetch<long>();
      if (valid) {
        value++;
        push(value);
      }
    } break;
    case Instruction::DEC: {
      auto [valid, value] = fetch<long>();
      if (valid) {
        value--;
        push(value);
      }
    } break;
    case Instruction::EXPECT: {
      char input[INPUT_SIZE];
      std::cin.getline(input, INPUT_SIZE);
      std::string token(input);
      std::any value;
      std::shared_ptr<std::any> valueptr;
      if (ripl::isIntegral(token)) {
        long l = std::stol(token);
        value = l;
        valueptr = std::make_shared<std::any>(value);
        _ds.push(valueptr);
        return;
      }
      if (ripl::isFloat(token)) {
        double d = std::stod(token);
        value = d;
        valueptr = std::make_shared<std::any>(value);
        _ds.push(valueptr);
        return;
      }
      if (ripl::isBool(token)) {
        value = token == "true" ? true : false;
        valueptr = std::make_shared<std::any>(value);
        _ds.push(valueptr);
        return;
      }
      valueptr = std::make_shared<std::any>(token);
      _ds.push(valueptr);
    } break;
    case Instruction::PRINT: {
      auto value = _ds.top();
      _ds.pop();
      if (value->type() == typeid(double)) {
        std::cout << std::any_cast<double>(*value) << std::endl;
        return;
      }
      if (value->type() == typeid(long)) {
        std::cout << std::any_cast<long>(*value) << std::endl;
        return;
      }
      if (value->type() == typeid(std::string)) {
        std::cout << std::any_cast<std::string>(*value) << std::endl;
        return;
      }
      if (value->type() == typeid(bool)) {
        std::cout << std::any_cast<bool>(*value) << std::endl;
        return;
      }
      std::cout << "Unknown type " << value->type().name() << std::endl;
    } break;
    case Instruction::END: {
      _isFinished = true;
      std::cout << "Stack Size: " << _ds.size() << std::endl;
    } break;
    }
    _ip++;
  }
}
