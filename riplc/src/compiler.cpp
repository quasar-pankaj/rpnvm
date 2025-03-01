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

    if (t.type == TokenType::LONG) {
      emitInstruction(ripl::Instruction::PUSHL);
      long l = std::stol(t.lexeme);
      emitLong(l);
      continue;
    }
    if (t.type == TokenType::DOUBLE) {
      emitInstruction(ripl::Instruction::PUSHD);
      double l = std::stod(t.lexeme);
      emitDouble(l);
      continue;
    }
    if (t.type == TokenType::BOOL) {
      emitInstruction(ripl::Instruction::PUSHB);
      std::string l = ripl::toLower(t.lexeme);
      bool truth = (l == "true");
      emitBool(truth);
      continue;
    }
    if (t.type == TokenType::STRING) {
      emitInstruction(ripl::Instruction::PUSHS);
      int len = t.lexeme.length();
      emitString(t.lexeme);
      continue;
    }
    if (t.type == TokenType::IDENTIFIER) {
      if (t.lexeme == "+") {
        emitInstruction(Instruction::ADD);
        continue;
      }
      if (t.lexeme == "-") {
        emitInstruction(Instruction::SUB);
        continue;
      }
      if (t.lexeme == "*") {
        emitInstruction(Instruction::MUL);
        continue;
      }
      if (t.lexeme == "/") {
        emitInstruction(Instruction::DIV);
        continue;
      }
      if (t.lexeme == "%") {
        emitInstruction(Instruction::MOD);
        continue;
      }
      if (t.lexeme == "&&") {
        emitInstruction(Instruction::AND);
        continue;
      }
      if (t.lexeme == "||") {
        emitInstruction(Instruction::OR);
        continue;
      }
      if (t.lexeme == "!") {
        emitInstruction(Instruction::NOT);
        continue;
      }
      if (t.lexeme == "==") {
        emitInstruction(Instruction::EQ);
        continue;
      }
      if (t.lexeme == "!=") {
        emitInstruction(Instruction::NEQ);
        continue;
      }
      if (t.lexeme == ">") {
        emitInstruction(Instruction::GT);
        continue;
      }
      if (t.lexeme == "<") {
        emitInstruction(Instruction::LT);
        continue;
      }
      if (t.lexeme == ">=") {
        emitInstruction(Instruction::GTE);
        continue;
      }
      if (t.lexeme == "<=") {
        emitInstruction(Instruction::LTE);
        continue;
      }
      if (t.lexeme == "dup") {
        emitInstruction(Instruction::DUP);
        continue;
      }
      if (t.lexeme == "swap") {
        emitInstruction(Instruction::SWAP);
        continue;
      }
      if (t.lexeme == "rotup") {
        emitInstruction(Instruction::ROTUP);
        continue;
      }
      if (t.lexeme == "rotdn") {
        emitInstruction(Instruction::ROTDN);
        continue;
      }
      if (t.lexeme == "drop") {
        emitInstruction(Instruction::DROP);
        continue;
      }
      if (t.lexeme == "++") {
        emitInstruction(Instruction::INC);
        continue;
      }
      if (t.lexeme == "--") {
        emitInstruction(Instruction::DEC);
        continue;
      }
      if (t.lexeme == "return" || t.lexeme == "}") {
        emitInstruction(Instruction::RET);
        continue;
      }
      if (t.lexeme == "expect") {
        emitInstruction(Instruction::EXPECT);
        continue;
      }
      if (t.lexeme == "end") {
        emitInstruction(Instruction::END);
        continue;
      }
      if (t.lexeme == "=") {
        emitInstruction(Instruction::PRINT);
        continue;
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
            frame->addCall(currentDiskOffset());
            emitInt(0);
          } else {
            emitInt(frame->address());
          }
          continue;
        }
        CallFrame frame;
        std::shared_ptr<CallFrame> ptr = std::make_shared<CallFrame>(frame);
        ptr->addCall(currentDiskOffset());
        _callMap.insert({_lastToken, ptr});
        emitInt(0); // emit a place holder int. this will be overwritten later
                    // using the disk offset above.
        continue;
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
          int currAddr = currentDiskOffset();
          for (int offset : frame->getCalls()) {
            seekToOffset(offset);
            emitInt(currentMemoryOffset());
          }
          seekToOffset(currAddr);
        } else {
          CallFrame frame;
          frame.address(currentMemoryOffset() - 1);
          std::shared_ptr<CallFrame> ptr = std::make_shared<CallFrame>(frame);
          _callMap.insert({_lastToken, ptr});
        }

        continue;
      }
      if (t.lexeme == "var") {
        emitInstruction(Instruction::VAR);
        emitString(_lastToken);
        continue;
      }
      if (t.lexeme == "<-") {
        emitInstruction(Instruction::ASSIGN);
        emitString(_lastToken);
        continue;
      }
      if (t.lexeme == "->") {
        emitInstruction(Instruction::DEREF);
        emitString(_lastToken);
        continue;
      }
      if (t.lexeme == "if") {
        createStackFrame(BranchType::CONDITIONAL);
        saveCurrentMemoryOffset();
        emitInstruction(Instruction::JF);
        saveCurrentDiskOffset();
        emitInt(0);
        continue;
      }
      if (t.lexeme == "endif") {
        int curr = currentDiskOffset();
        auto frame = currentStackFrame();
        seekToOffset(frame->diskOffset());
        emitInt(currentMemoryOffset());
        seekToOffset(curr);
        dropStackFrame();
        continue;
      }
      if (t.lexeme == "else") {
        auto frame = currentStackFrame();
        dropStackFrame();
        createStackFrame(BranchType::CONDITIONAL);
        saveCurrentMemoryOffset();
        int pos = currentMemoryOffset();
        emitInstruction(Instruction::JMP);
        saveCurrentDiskOffset();
        emitInt(0);
        int diskOffset = currentDiskOffset();
        seekToOffset(frame->diskOffset());
        emitInt(pos);
        seekToOffset(diskOffset);
        continue;
      }
      if (t.lexeme == "for") {
        emitInstruction(Instruction::DUP);
        startLoop(Instruction::JZ);
        continue;
      }
      if (t.lexeme == "endfor") {
        fillOutContinues();
        emitInstruction(Instruction::DEC);
        emitInstruction(Instruction::DUP);
        addClosingJump();
        fillOutStartingJump();
        fillOutBreaks();
        closeLoop();
        continue;
      }
      if (t.lexeme == "while") {
        startLoop(Instruction::JF);
        continue;
      }
      if (t.lexeme == "endwhile") {
        fillOutContinues();
        addClosingJump();
        fillOutStartingJump();
        fillOutBreaks();
        closeLoop();
        continue;
      }
      if (t.lexeme == "break") {
        emitInstruction(Instruction::JMP);
        addBreak();
        emitInt(0);
        continue;
      }
      if (t.lexeme == "continue") {
        emitInstruction(Instruction::JMP);
        addContinue();
        emitInt(0);
        continue;
      }
      _lastToken = t.lexeme;
    }
  }
}

void ripl::Compiler::emit(const unsigned char c) { _out.put(c); }

void ripl::Compiler::emit(const char *bytes, int len) {
  _out.write(bytes, len);
}

void ripl::Compiler::emitInstruction(const ripl::Instruction &instruction) {
  emit((unsigned char)instruction);
  _instructionOffset++;
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

int ripl::Compiler::currentMemoryOffset() { return _instructionOffset; }

int ripl::Compiler::currentDiskOffset() { return _out.tellp(); }

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
  saveCurrentMemoryOffset();
  emitInstruction(instruction);
  saveCurrentDiskOffset();
  emitInt(0);
}

int ripl::Compiler::loopLevel() { return _loopLevel; }

void ripl::Compiler::addBreak() {
  auto frame = currentStackFrame();
  frame->addToBreaks(currentDiskOffset());
}

void ripl::Compiler::addContinue() {
  auto frame = currentStackFrame();
  frame->addToContinues(currentDiskOffset());
}

void ripl::Compiler::saveCurrentDiskOffset() {
  auto frame = currentStackFrame();
  frame->diskOffset(currentDiskOffset());
}

void ripl::Compiler::saveCurrentMemoryOffset() {
  auto frame = currentStackFrame();
  frame->memoryOffset(currentMemoryOffset());
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
  int pos = currentDiskOffset();
  for (int diskoffset : offsets) {
    seekToOffset(diskoffset);
    emitInt(currentMemoryOffset());
  }
  seekToOffset(pos);
}

void ripl::Compiler::addClosingJump() {
  auto frame = currentStackFrame();
  emitInstruction(Instruction::JMP);
  emitInt(frame->memoryOffset());
}

void ripl::Compiler::fillOutStartingJump() {
  auto frame = currentStackFrame();
  int pos = currentDiskOffset();
  seekToOffset(frame->diskOffset());
  emitInt(currentMemoryOffset());
  seekToOffset(pos);
}
