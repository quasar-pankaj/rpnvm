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
  for (_ip = 0; !_buf.eof(); _ip++) {
    char c;
    _buf.get(c);
    Instruction mnemonic = (Instruction)c;

    switch (mnemonic) {
    case Instruction::NOP: // This instruction is unused
      break;
    case Instruction::PUSHL: {
      long l = readLong();
      std::cout << _ip << " " << "PUSHL " << l << std::endl;
      _ip += sizeof(l);
    } break;
    case Instruction::PUSHD: {
      double l = readDouble();
      std::cout << _ip << " " << "PUSHD " << l << std::endl;
      _ip += sizeof(l);
    } break;
    case Instruction::PUSHB: {
      bool l = readBool();
      std::cout << _ip << " " << "PUSHB " << l << std::endl;
      _ip += sizeof(l);
    } break;
    case Instruction::PUSHS: {
      std::string l = readString();
      std::cout << _ip << " " << "PUSHS \"" << l << "\"" << std::endl;
      _ip += sizeof(int);
      _ip += l.length();
    } break;
    case Instruction::ADD: {
      std::cout << _ip << " ADD" << std::endl;
    } break;
    case Instruction::SUB: {
      std::cout << _ip << " SUB" << std::endl;
    } break;
    case Instruction::MUL: {
      std::cout << _ip << " MUL" << std::endl;
    } break;
    case Instruction::DIV: {
      std::cout << _ip << " DIV" << std::endl;
    } break;
    case Instruction::MOD: {
      std::cout << _ip << " MOD" << std::endl;
    } break;
    case Instruction::AND: {
      std::cout << _ip << " AND" << std::endl;
    } break;
    case Instruction::OR: {
      std::cout << _ip << " OR" << std::endl;
    } break;
    case Instruction::NOT: {
      std::cout << _ip << " NOT" << std::endl;
    } break;
    case Instruction::EQ: {
      std::cout << _ip << " EQ" << std::endl;
    } break;
    case Instruction::NEQ: {
      std::cout << _ip << " NEQ" << std::endl;
    } break;
    case Instruction::GT: {
      std::cout << _ip << " GT" << std::endl;
    } break;
    case Instruction::LT: {
      std::cout << _ip << " LT" << std::endl;
    } break;
    case Instruction::GTE: {
      std::cout << _ip << " GTE" << std::endl;
    } break;
    case Instruction::LTE: {
      std::cout << _ip << " LTE" << std::endl;
    } break;
    case Instruction::JZ: {
      int addrparm = readInt();
      std::cout << _ip << " JZ " << addrparm << std::endl;
      _ip += sizeof(int);
    } break;
    case Instruction::JF: {
      int addrparm = readInt();
      std::cout << _ip << " JF " << addrparm << std::endl;
      _ip += sizeof(int);
    } break;
    case Instruction::JMP: {
      int addrparm = readInt();
      std::cout << _ip << " JMP " << addrparm << std::endl;
      _ip += sizeof(int);
    } break;
    case Instruction::ID:
      break;
    case Instruction::VAR: {
      std::string name = readString();
      std::cout << _ip << " " << "VAR " << name << std::endl;
      _ip += sizeof(int);
      _ip += name.length();
    } break;
    case Instruction::ASSIGN: {
      std::string name = readString();
      std::cout << _ip << " " << "ASSIGN " << name << std::endl;
      _ip += sizeof(int);
      _ip += name.length();
    } break;
    case Instruction::DEREF: {
      std::string name = readString();
      std::cout << _ip << " " << "DEREF " << name << std::endl;
      _ip += sizeof(int);
      _ip += name.length();
    } break;
    case Instruction::CALL: {
      int addrparm = readInt();
      std::cout << _ip << " " << "CALL " << addrparm << std::endl;
      _ip += sizeof(int);
    } break;
    case Instruction::RET: {
      std::cout << _ip << " RET" << std::endl;
    } break;
    case Instruction::DUP: {
      std::cout << _ip << " DUP" << std::endl;
    } break;
    case Instruction::SWAP: {
      std::cout << _ip << " SWAP" << std::endl;
    } break;
    case Instruction::ROTUP: {
      std::cout << _ip << " ROTUP" << std::endl;
    } break;
    case Instruction::ROTDN: {
      std::cout << _ip << " ROTDN" << std::endl;
    } break;
    case Instruction::DROP: {
      std::cout << _ip << " DROP" << std::endl;
    } break;
    case Instruction::INC: {
      std::cout << _ip << " INC" << std::endl;
    } break;
    case Instruction::DEC: {
      std::cout << _ip << " DEC" << std::endl;
    } break;
    case Instruction::EXPECT: {
      std::cout << _ip << " EXPECT" << std::endl;
    } break;
    case Instruction::PRINT: {
      std::cout << _ip << " PRINT" << std::endl;
    } break;
    case Instruction::END: {
      std::cout << _ip << " END" << std::endl;
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
