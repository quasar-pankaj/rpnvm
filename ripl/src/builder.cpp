#include "builder.hpp"
#include "arithmetic_operators.hpp"
#include "branching_looping.hpp"
#include "comparison_operators.hpp"
#include "engine.hpp"
#include "increment_decrement.hpp"
#include "instruction_set.hpp"
#include "logical_operators.hpp"
#include "misc.hpp"
#include "push_t.hpp"
#include "stack_operators.hpp"
#include "subroutines.hpp"
#include <any>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

void ripl::Builder::load() {
  if (!_in) {
    std::cerr << "Could not open file " << std::endl;
    return;
  }

  int offset = -1;
  while (!_in.eof()) {
    offset++;
    Instruction instruction = loadInstruction();
    if (instruction == Instruction::PUSHL) {
      long l = readLong();
      PushT<long> pushLong(l);
      _engine->_code.push_back(std::make_shared<PushT<long>>(pushLong));
      continue;
    }
    if (instruction == Instruction::PUSHD) {
      double d = readDouble();
      PushT<double> pushDouble(d);
      _engine->_code.push_back(std::make_shared<PushT<double>>(pushDouble));
      continue;
    }
    if (instruction == Instruction::PUSHB) {
      bool b = readBool();
      PushT<bool> pushBool(b);
      _engine->_code.push_back(std::make_shared<PushT<bool>>(pushBool));
      continue;
    }
    if (instruction == Instruction::PUSHS) {
      std::string str = readString();
      PushT<std::string> pushString(str);
      _engine->_code.push_back(
          std::make_shared<PushT<std::string>>(pushString));
      continue;
    }
    if (instruction == Instruction::ADD) {
      _engine->_code.push_back(std::make_shared<Add>(Add()));
      continue;
    }
    if (instruction == Instruction::SUB) {
      _engine->_code.push_back(std::make_shared<Sub>(Sub()));
      continue;
    }
    if (instruction == Instruction::MUL) {
      _engine->_code.push_back(std::make_shared<Mul>(Mul()));
      continue;
    }
    if (instruction == Instruction::DIV) {
      _engine->_code.push_back(std::make_shared<Div>(Div()));
      continue;
    }
    if (instruction == Instruction::MOD) {
      _engine->_code.push_back(std::make_shared<Mod>(Mod()));
      continue;
    }
    if (instruction == Instruction::AND) {
      _engine->_code.push_back(std::make_shared<And>(And()));
      continue;
    }
    if (instruction == Instruction::OR) {
      _engine->_code.push_back(std::make_shared<Or>(Or()));
      continue;
    }
    if (instruction == Instruction::NOT) {
      _engine->_code.push_back(std::make_shared<Not>(Not()));
      continue;
    }
    if (instruction == Instruction::EQ) {
      _engine->_code.push_back(std::make_shared<Equality>(Equality()));
      continue;
    }
    if (instruction == Instruction::NEQ) {
      _engine->_code.push_back(std::make_shared<NotEquality>(NotEquality()));
      continue;
    }
    if (instruction == Instruction::GT) {
      _engine->_code.push_back(std::make_shared<GreaterThan>(GreaterThan()));
      continue;
    }
    if (instruction == Instruction::LT) {
      _engine->_code.push_back(std::make_shared<LessThan>(LessThan()));
      continue;
    }
    if (instruction == Instruction::GTE) {
      _engine->_code.push_back(
          std::make_shared<GreaterThanEquals>(GreaterThanEquals()));
      continue;
    }
    if (instruction == Instruction::LTE) {
      _engine->_code.push_back(
          std::make_shared<LessThanEquals>(LessThanEquals()));
      continue;
    }
    if (instruction == Instruction::JZ) {
      int offset = readInt();
      _engine->_code.push_back(
          std::make_shared<BranchOnZero>(BranchOnZero(offset)));
      continue;
    }
    if (instruction == Instruction::JF) {
      int offset = readInt();
      _engine->_code.push_back(
          std::make_shared<BranchOnFalse>(BranchOnFalse(offset)));
      continue;
    }
    if (instruction == Instruction::JMP) {
      int offset = readInt();
      _engine->_code.push_back(std::make_shared<Branch>(Branch(offset)));
      continue;
    }
    if (instruction == Instruction::VAR) {
      std::string name = readString();
      _engine->_variables.insert(
          std::make_pair(name, std::make_shared<std::any>(0)));
      continue;
    }
    if (instruction == Instruction::ASSIGN) {
      std::string name = readString();
      _engine->_code.push_back(std::make_shared<Assign>(Assign(name)));
      continue;
    }
    if (instruction == Instruction::DEREF) {
      std::string name = readString();
      _engine->_code.push_back(std::make_shared<Extract>(Extract(name)));
      continue;
    }
    if (instruction == Instruction::CALL) {
      _engine->_code.push_back(std::make_shared<Call>(Call(readInt())));
      continue;
    }
    if (instruction == Instruction::RET) {
      _engine->_code.push_back(std::make_shared<Return>(Return()));
      continue;
    }
    if (instruction == Instruction::DUP) {
      _engine->_code.push_back(std::make_shared<Dup>(Dup()));
      continue;
    }
    if (instruction == Instruction::SWAP) {
      _engine->_code.push_back(std::make_shared<Swap>(Swap()));
      continue;
    }
    if (instruction == Instruction::ROTUP) {
      _engine->_code.push_back(std::make_shared<RotUp>(RotUp()));
      continue;
    }
    if (instruction == Instruction::ROTDN) {
      _engine->_code.push_back(std::make_shared<RotDn>(RotDn()));
      continue;
    }
    if (instruction == Instruction::DROP) {
      _engine->_code.push_back(std::make_shared<Drop>(Drop()));
      continue;
    }
    if (instruction == Instruction::INC) {
      _engine->_code.push_back(std::make_shared<Inc>(Inc()));
      continue;
    }
    if (instruction == Instruction::DEC) {
      _engine->_code.push_back(std::make_shared<Dec>(Dec()));
      continue;
    }
    if (instruction == Instruction::EXPECT) {
      _engine->_code.push_back(std::make_shared<Input>(Input()));
      continue;
    }
    if (instruction == Instruction::PRINT) {
      _engine->_code.push_back(std::make_shared<Print>(Print()));
      continue;
    }
    if (instruction == Instruction::END) {
      _engine->_code.push_back(std::make_shared<End>(End()));
      continue;
    }
    std::cerr << "Unknown Instruction." << std::endl;
  }
}

ripl::Instruction ripl::Builder::loadInstruction() {
  Instruction instruction = (Instruction)_in.get();
  return instruction;
}

int ripl::Builder::readInt() {
  int value = 0;
  _in.read((char *)&value, sizeof(int));
  return value;
}

long ripl::Builder::readLong() {
  long value = 0;
  _in.read((char *)&value, sizeof(long));
  return value;
}

double ripl::Builder::readDouble() {
  double value = 0;
  _in.read((char *)&value, sizeof(double));
  return value;
}

bool ripl::Builder::readBool() {
  bool value = 0;
  _in.read((char *)&value, sizeof(bool));
  return value;
}

std::string ripl::Builder::readString() {
  int len = readInt();
  char *str = new char[len + 1];
  _in.read(str, len);
  str[len] = '\0';
  std::string s(str);
  delete[] str;
  return s;
}
