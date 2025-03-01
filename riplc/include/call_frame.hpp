#pragma once

#include <vector>
namespace ripl {
class CallFrame {
public:
  CallFrame() {}
  ~CallFrame() { _diskOffsets.clear(); }

  void address(int offset) { _offset = offset; }
  int address() { return _offset; }
  void addCall(int offset) { _diskOffsets.push_back(offset); }
  std::vector<int> getCalls() { return _diskOffsets; }

private:
  int _offset = -1;
  std::vector<int> _diskOffsets;
};
} // namespace ripl
