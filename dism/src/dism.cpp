#include "dism.hpp"
#include "instruction_set.hpp"
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

ripl::Dism::Dism(char *filename) {
  std::ifstream in(filename);

  if (!in) {
    std::cerr << "Could not open the file " << filename << " for input."
              << std::endl;
    return;
  }

  _buf << in.rdbuf();
  in.close();
}

ripl::Dism::~Dism() { _buf.clear(); }

void ripl::Dism::disassemble() {
  int addr = 0;
  for (_ip = 0; !_buf.eof(); _ip++, addr++) {
    char c;
    _buf.get(c);
    Instruction mnemonic = (Instruction)c;

    switch (mnemonic) {
    case Instruction::NOP:
      break;
    case Instruction::PUSHL: {
      long l = readLong();
      std::cout << addr << " " << "PUSHL " << l << std::endl;
      _ip += sizeof(l);
    } break;
    case Instruction::PUSHD: {
      double l = readDouble();
      std::cout << addr << " " << "PUSHD " << l << std::endl;
      _ip += sizeof(l);
    } break;
    case Instruction::PUSHB: {
      bool l = readBool();
      std::cout << addr << " " << "PUSHB " << l << std::endl;
      _ip += sizeof(l);
    } break;
    case Instruction::PUSHS: {
      std::string l = readString();
      std::cout << addr << " " << "PUSHS \"" << l << "\"" << std::endl;
      _ip += sizeof(int);
      _ip += l.length();
    } break;
    case Instruction::ADD: {
      std::cout << addr << " ADD" << std::endl;
    } break;
    case Instruction::SUB: {
      std::cout << addr << " SUB" << std::endl;
    } break;
    case Instruction::MUL: {
      std::cout << addr << " MUL" << std::endl;
    } break;
    case Instruction::DIV: {
      std::cout << addr << " DIV" << std::endl;
    } break;
    case Instruction::MOD: {
      std::cout << addr << " MOD" << std::endl;
    } break;
    case Instruction::AND: {
      std::cout << addr << " AND" << std::endl;
    } break;
    case Instruction::OR: {
      std::cout << addr << " OR" << std::endl;
    } break;
    case Instruction::NOT: {
      std::cout << addr << " NOT" << std::endl;
    } break;
    case Instruction::EQ: {
      std::cout << addr << " EQ" << std::endl;
    } break;
    case Instruction::NEQ: {
      std::cout << addr << " NEQ" << std::endl;
    } break;
    case Instruction::GT: {
      std::cout << addr << " GT" << std::endl;
    } break;
    case Instruction::LT: {
      std::cout << addr << " LT" << std::endl;
    } break;
    case Instruction::GTE: {
      std::cout << addr << " GTE" << std::endl;
    } break;
    case Instruction::LTE: {
      std::cout << addr << " LTE" << std::endl;
    } break;
    case Instruction::JZ: {
      int addrparm = readInt();
      std::cout << addr << " JZ " << addrparm << std::endl;
      _ip += sizeof(int);
    } break;
    case Instruction::JF: {
      int addrparm = readInt();
      std::cout << addr << " JF " << addrparm << std::endl;
      _ip += sizeof(int);
    } break;
    case Instruction::JMP: {
      int addrparm = readInt();
      std::cout << addr << " JMP " << addrparm << std::endl;
      _ip += sizeof(int);
    } break;
    case Instruction::ID:
      break;
    case Instruction::VAR: {
      std::string name = readString();
      std::cout << addr << " " << "VAR " << name << std::endl;
      _ip += sizeof(int);
      _ip += name.length();
    } break;
    case Instruction::ASSIGN: {
      std::string name = readString();
      std::cout << addr << " " << "ASSIGN " << name << std::endl;
      _ip += sizeof(int);
      _ip += name.length();
    } break;
    case Instruction::DEREF: {
      std::string name = readString();
      std::cout << addr << " " << "DEREF " << name << std::endl;
      _ip += sizeof(int);
      _ip += name.length();
    } break;
    case Instruction::CALL: {
      int addrparm = readInt();
      std::cout << addr << " " << "CALL " << addrparm << std::endl;
      _ip += sizeof(int);
    } break;
    case Instruction::RET: {
      std::cout << addr << " RET" << std::endl;
    } break;
    case Instruction::DUP: {
      std::cout << addr << " DUP" << std::endl;
    } break;
    case Instruction::SWAP: {
      std::cout << addr << " SWAP" << std::endl;
    } break;
    case Instruction::ROTUP: {
      std::cout << addr << " ROTUP" << std::endl;
    } break;
    case Instruction::ROTDN: {
      std::cout << addr << " ROTDN" << std::endl;
    } break;
    case Instruction::DROP: {
      std::cout << addr << " DROP" << std::endl;
    } break;
    case Instruction::INC: {
      std::cout << addr << " INC" << std::endl;
    } break;
    case Instruction::DEC: {
      std::cout << addr << " DEC" << std::endl;
    } break;
    case Instruction::EXPECT: {
      std::cout << addr << " EXPECT" << std::endl;
    } break;
    case Instruction::PRINT: {
      std::cout << addr << " PRINT" << std::endl;
    } break;
    case Instruction::END: {
      std::cout << addr << " END" << std::endl;
    } break;
    }
  }
}

int ripl::Dism::readInt() {
  int value;
  _buf.read((char *)&value, sizeof(int));
  return value;
}

long ripl::Dism::readLong() {
  long value;
  _buf.read((char *)&value, sizeof(long));
  return value;
}

double ripl::Dism::readDouble() {
  double value;
  _buf.read((char *)&value, sizeof(double));
  return value;
}

bool ripl::Dism::readBool() {
  bool value;
  _buf.read((char *)&value, sizeof(bool));
  return value;
}

std::string ripl::Dism::readString() {
  int len = readInt();
  char *value = new char[len + 1];
  _buf.read(value, len);
  value[len] = '\0';
  std::string s(value);
  delete[] value;
  return s;
}
