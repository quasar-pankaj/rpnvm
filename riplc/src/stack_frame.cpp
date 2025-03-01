#include "stack_frame.hpp"
#include <iostream>

void ripl::StackFrame::addToBreaks(int offset) {
  if (_loopLevel == 0) {
    std::cerr << "No loops to break out of." << std::endl;
    return;
  }
  if (_branchType == BranchType::LOOPING) {
    _breaks.push_back(offset);
    return;
  }
  _parent->addToBreaks(offset);
}

void ripl::StackFrame::addToContinues(int offset) {
  if (_loopLevel == 0) {
    std::cerr << "No loops to continue." << std::endl;
    return;
  }
  if (_branchType == BranchType::LOOPING) {
    _continues.push_back(offset);
    return;
  }
  _parent->addToContinues(offset);
}

