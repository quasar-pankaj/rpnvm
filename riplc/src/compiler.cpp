#include "compiler.hpp"
#include "call_frame.hpp"
#include "instruction_set.hpp"
#include "parser.hpp"
#include "stack_frame.hpp"
#include "token.hpp"
#include "utils.hpp"
#include <cstring>
#include <iostream>
#include <memory>
#include <stack>
#include <string>
#include <vector>

ripl::Compiler::Compiler(char *filename) : _filename(filename) {
  int len = std::strlen(filename);
  char *outFile = new char[len + 4];
  strcpy(outFile, filename);
  strcat(outFile, ".bc");
  _out.open(outFile); // bc=byte code
  delete[] outFile;

  if (!_out) {
    std::cerr << "could not open file for output." << std::endl;
    return;
  }
}

ripl::Compiler::~Compiler() {
  _out.flush();
  _out.close();
}

void ripl::Compiler::compile() {
  ripl::Parser parser(_filename);
  parser.build();

  while (!parser.eof()) {
    Token t = parser.get();

    switch (t.type) {
    case TokenType::LONG: {
      emitInstruction(ripl::Instruction::PUSHL);
      long l = std::stol(t.lexeme);
      emitLong(l);
    } break;
    case TokenType::DOUBLE: {
      emitInstruction(ripl::Instruction::PUSHD);
      double l = std::stod(t.lexeme);
      emitDouble(l);
    } break;
    case TokenType::BOOL: {
      emitInstruction(ripl::Instruction::PUSHB);
      std::string l = ripl::toLower(t.lexeme);
      bool truth = (l == "true");
      emitBool(truth);
    } break;
    case TokenType::STRING: {
      emitInstruction(ripl::Instruction::PUSHS);
      int len = t.lexeme.length();
      emitString(t.lexeme);
    } break;
    case TokenType::IDENTIFIER: {
      if (t.lexeme == "+") {
        emitInstruction(Instruction::ADD);
        break;
      }
      if (t.lexeme == "-") {
        emitInstruction(Instruction::SUB);
        break;
      }
      if (t.lexeme == "*") {
        emitInstruction(Instruction::MUL);
        break;
      }
      if (t.lexeme == "/") {
        emitInstruction(Instruction::DIV);
        break;
      }
      if (t.lexeme == "%") {
        emitInstruction(Instruction::MOD);
        break;
      }
      if (t.lexeme == "&&") {
        emitInstruction(Instruction::AND);
        break;
      }
      if (t.lexeme == "||") {
        emitInstruction(Instruction::OR);
        break;
      }
      if (t.lexeme == "!") {
        emitInstruction(Instruction::NOT);
        break;
      }
      if (t.lexeme == "==") {
        emitInstruction(Instruction::EQ);
        break;
      }
      if (t.lexeme == "!=") {
        emitInstruction(Instruction::NEQ);
        break;
      }
      if (t.lexeme == ">") {
        emitInstruction(Instruction::GT);
        break;
      }
      if (t.lexeme == "<") {
        emitInstruction(Instruction::LT);
        break;
      }
      if (t.lexeme == ">=") {
        emitInstruction(Instruction::GTE);
        break;
      }
      if (t.lexeme == "<=") {
        emitInstruction(Instruction::LTE);
        break;
      }
      if (t.lexeme == "dup") {
        emitInstruction(Instruction::DUP);
        break;
      }
      if (t.lexeme == "swap") {
        emitInstruction(Instruction::SWAP);
        break;
      }
      if (t.lexeme == "rotup") {
        emitInstruction(Instruction::ROTUP);
        break;
      }
      if (t.lexeme == "rotdn") {
        emitInstruction(Instruction::ROTDN);
        break;
      }
      if (t.lexeme == "drop") {
        emitInstruction(Instruction::DROP);
        break;
      }
      if (t.lexeme == "++") {
        emitInstruction(Instruction::INC);
        break;
      }
      if (t.lexeme == "--") {
        emitInstruction(Instruction::DEC);
        break;
      }
      if (t.lexeme == "return" || t.lexeme == "}") {
        emitInstruction(Instruction::RET);
        break;
      }
      if (t.lexeme == "expect") {
        emitInstruction(Instruction::EXPECT);
        break;
      }
      if (t.lexeme == "end") {
        emitInstruction(Instruction::END);
        break;
      }
      if (t.lexeme == "=") {
        emitInstruction(Instruction::PRINT);
        break;
      }
      // The algorithm for call-subroutine works as follows:-
      // First a check is performed to see if the entry exists the frame is
      // extracted then another check is performed to see if the address of the
      // jump is -1 or not if it is then we have not yet created the subroutine
      // so we only add the offset to the call frame to be updated later. But if
      // it is not -1 then we have the address of the subroutine so we update
      // the call. If the entry does not exist we simply create a new one and
      // add the current disk address to it.
      if (t.lexeme == "call") {
        emitInstruction(Instruction::CALL);
        if (_callMap.contains(_lastToken)) {
          auto frame = _callMap[_lastToken];
          if (frame->address() == -1) {
            frame->addCall(currentOffset());
            emitInt(0);
          } else {
            emitInt(frame->address());
          }
          break;
        }
        CallFrame frame;
        std::shared_ptr<CallFrame> ptr = std::make_shared<CallFrame>(frame);
        ptr->addCall(currentOffset());
        _callMap.insert({_lastToken, ptr});
        emitInt(0); // emit a place holder int. this will be overwritten later
                    // using the disk offset above.
        break;
      }
      // the logic of the following token is as follows:-
      // if the entry for the _lastToken exists we have at least one call
      // registered. We then loop through the list and add the current offset of
      // the instruction we also set the address feld in the frame so we can use
      // it for later calls. In case the entry doesn't exist we create a new one
      // and set the address.
      if (t.lexeme == "{") {
        if (_callMap.contains(_lastToken)) {
          auto frame = _callMap[_lastToken];
          int currAddr = currentOffset();
          for (int offset : frame->getCalls()) {
            seekToOffset(offset);
            emitInt(currAddr);
          }
          seekToOffset(currAddr);
        } else {
          CallFrame frame;
          frame.address(currentOffset() - 1);
          std::shared_ptr<CallFrame> ptr = std::make_shared<CallFrame>(frame);
          _callMap.insert({_lastToken, ptr});
        }

        break;
      }
      if (t.lexeme == "var") {
        emitInstruction(Instruction::VAR);
        emitString(_lastToken);
        break;
      }
      if (t.lexeme == "<-") {
        emitInstruction(Instruction::ASSIGN);
        emitString(_lastToken);
        break;
      }
      if (t.lexeme == "->") {
        emitInstruction(Instruction::DEREF);
        emitString(_lastToken);
        break;
      }
      if (t.lexeme == "if") {
        createStackFrame(BranchType::CONDITIONAL);
        emitInstruction(Instruction::JF);
        saveCurrentOffset();
        emitInt(0);
        break;
      }
      if (t.lexeme == "endif") {
        int curr = currentOffset();
        auto frame = currentStackFrame();
        seekToOffset(frame->offset());
        emitInt(curr);
        seekToOffset(curr);
        dropStackFrame();
        break;
      }
      if (t.lexeme == "else") {
        auto frame = currentStackFrame();
        dropStackFrame();
        createStackFrame(BranchType::CONDITIONAL);
        emitInstruction(Instruction::JMP);
        saveCurrentOffset();
        emitInt(0);
        int diskOffset = currentOffset();
        seekToOffset(frame->offset());
        emitInt(diskOffset);
        seekToOffset(diskOffset);
        break;
      }
      if (t.lexeme == "for") {
        emitInstruction(Instruction::DUP);
        startLoop(Instruction::JZ);
        break;
      }
      if (t.lexeme == "endfor") {
        fillOutContinues();
        emitInstruction(Instruction::DEC);
        emitInstruction(Instruction::DUP);
        addClosingJump();
        fillOutStartingJump();
        fillOutBreaks();
        closeLoop();
        break;
      }
      if (t.lexeme == "while") {
        startLoop(Instruction::JF);
        break;
      }
      if (t.lexeme == "endwhile") {
        fillOutContinues();
        addClosingJump();
        fillOutStartingJump();
        fillOutBreaks();
        closeLoop();
        break;
      }
      if (t.lexeme == "break") {
        emitInstruction(Instruction::JMP);
        addBreak();
        emitInt(0);
        break;
      }
      if (t.lexeme == "continue") {
        emitInstruction(Instruction::JMP);
        addContinue();
        emitInt(0);
        break;
      }
      _lastToken = t.lexeme;
    }
    }
  }
}

void ripl::Compiler::emit(const unsigned char c) { _out.put(c); }

void ripl::Compiler::emit(const char *bytes, int len) {
  _out.write(bytes, len);
}

void ripl::Compiler::emitInstruction(const ripl::Instruction &instruction) {
  emit((unsigned char)instruction);
}

void ripl::Compiler::emitLength(const int len) { emitInt(len); }

void ripl::Compiler::emitInt(const int value) {
  emit((char *)&value, sizeof(int));
}

void ripl::Compiler::emitLong(const long value) {
  emit((char *)&value, sizeof(long));
}

void ripl::Compiler::emitDouble(const double value) {
  emit((char *)&value, sizeof(double));
}

void ripl::Compiler::emitBool(const bool value) {
  emit((char *)&value, sizeof(bool));
}

void ripl::Compiler::emitString(const std::string &value) {
  int len = value.length();
  char *bytes = new char[len + 1];
  std::strcpy(bytes, value.c_str());
  emitLength(len);
  _out.write(bytes, len);
  delete[] bytes;
}

int ripl::Compiler::currentOffset() { return _out.tellp(); }

std::shared_ptr<ripl::StackFrame> ripl::Compiler::currentStackFrame() {
  auto frame = _buildStack.top();
  return frame;
}

void ripl::Compiler::createStackFrame(ripl::BranchType branchType) {
  StackFrame frame(loopLevel(), branchType,
                   _buildStack.size() == 0 ? nullptr : _buildStack.top());
  std::shared_ptr<StackFrame> ptr = std::make_shared<StackFrame>(frame);
  _buildStack.push(ptr);
}

void ripl::Compiler::dropStackFrame() { _buildStack.pop(); }

void ripl::Compiler::openNewLoop() {
  _loopLevel++;
  createStackFrame(BranchType::LOOPING);
}

void ripl::Compiler::closeLoop() {
  dropStackFrame();
  _loopLevel--;
}

void ripl::Compiler::startLoop(ripl::Instruction instruction) {
  openNewLoop();
  saveCurrentOffset();
  emitInstruction(instruction);
  emitInt(0);
}

int ripl::Compiler::loopLevel() { return _loopLevel; }

void ripl::Compiler::addBreak() {
  auto frame = currentStackFrame();
  frame->addToBreaks(currentOffset());
}

void ripl::Compiler::addContinue() {
  auto frame = currentStackFrame();
  frame->addToContinues(currentOffset());
}

void ripl::Compiler::saveCurrentOffset() {
  auto frame = currentStackFrame();
  frame->offset(currentOffset());
}

void ripl::Compiler::seekToOffset(int offset) { _out.seekp(offset); }

void ripl::Compiler::fillOutContinues() {
  auto frame = _buildStack.top();
  auto continues = frame->fetchContinues();
  fillOutExits(continues);
}

void ripl::Compiler::fillOutBreaks() {
  auto frame = _buildStack.top();
  auto breaks = frame->fetchBreaks();
  fillOutExits(breaks);
}

void ripl::Compiler::fillOutExits(std::vector<int> &offsets) {
  int pos = currentOffset();
  for (int diskoffset : offsets) {
    seekToOffset(diskoffset);
    emitInt(pos);
  }
  seekToOffset(pos);
}

void ripl::Compiler::addClosingJump() {
  auto frame = currentStackFrame();
  emitInstruction(Instruction::JMP);
  emitInt(frame->offset());
}

void ripl::Compiler::fillOutStartingJump() {
  auto frame = currentStackFrame();
  int pos = currentOffset();
  seekToOffset(frame->offset() +
               1); // skip the instruction and get to the address
  emitInt(pos);
  seekToOffset(pos);
}
