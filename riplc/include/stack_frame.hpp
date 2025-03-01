#pragma once

#include <memory>
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

  int offset() { return _offset; }
  void offset(int offset) { _offset = offset; }

private:
  std::shared_ptr<StackFrame> _parent;
  int _loopLevel;
  int _offset;
  BranchType _branchType;
  std::vector<int> _continues; //[Offset]
  std::vector<int> _breaks;    //[Offset]
};
} // namespace ripl
