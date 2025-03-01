#pragma once

#include "call_frame.hpp"
#include "instruction_set.hpp"
#include "stack_frame.hpp"
#include <fstream>
#include <map>
#include <memory>
#include <stack>
#include <string>
#include <vector>
namespace ripl {
class Compiler {
public:
  Compiler(char *filename);
  ~Compiler();

  void compile();

  void emit(const unsigned char c);
  void emit(const char *bytes, int numbytes);
  void emitLength(const int len);
  void emitInt(const int value);
  void emitBool(const bool value);
  void emitLong(const long value);
  void emitDouble(const double value);
  void emitString(const std::string &s);
  void emitInstruction(const Instruction &instruction);

  int currentOffset();
  void saveCurrentOffset();
  void fillOutBreaks();
  void fillOutContinues();
  void fillOutExits(std::vector<int> &offsets);
  void addClosingJump();
  void fillOutStartingJump();

  void seekToOffset(int offset);

  std::string lastToken() { return _lastToken; }

  std::shared_ptr<StackFrame> currentStackFrame();
  void createStackFrame(BranchType branchType);
  void dropStackFrame();

  int loopLevel();
  void openNewLoop();
  void closeLoop();
  void startLoop(Instruction instruction);
  void addBreak();
  void addContinue();

private:
  char *_filename;
  std::ofstream _out;
  std::string _lastToken;

  std::stack<std::shared_ptr<StackFrame>> _buildStack; // Build Stack
  std::map<std::string, std::shared_ptr<CallFrame>> _callMap;
  int _loopLevel = 0;
};
} // namespace ripl
