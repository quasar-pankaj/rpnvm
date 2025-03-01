#pragma once

#include <memory>
#include <utility>
#include <vector>
namespace ripl {
enum class BranchType {
  CONDITIONAL = 0,
  LOOPING,
};
class StackFrame {
public:
  StackFrame(int looplevel, BranchType branchType,
             std::shared_ptr<StackFrame> parent)
      : _loopLevel(looplevel), _branchType(branchType), _parent(parent) {}
  ~StackFrame() {
    _continues.clear();
    _breaks.clear();
  }

  void addToContinues(int diskOffset);
  void addToBreaks(int diskOffset);
  std::vector<int> fetchBreaks() { return _breaks; }
  std::vector<int> fetchContinues() { return _continues; }

  int loopLevel() { return _loopLevel; }
  int diskOffset() { return _diskOffset; }
  int memoryOffset() { return _memoryOffset; }

  void diskOffset(int offset) { _diskOffset = offset; }
  void memoryOffset(int offset) { _memoryOffset = offset; }

private:
  std::shared_ptr<StackFrame> _parent;
  int _loopLevel;
  int _diskOffset;
  int _memoryOffset;
  BranchType _branchType;
  std::vector<int> _continues; //[diskOffset]
  std::vector<int> _breaks;    //[diskOffset]
};
} // namespace ripl
