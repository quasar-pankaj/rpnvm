#include "stack_frame.hpp"
#include <iostream>

void ripl::StackFrame::addToBreaks(int diskOffset) {
  if (_loopLevel == 0) {
    std::cerr << "No loops to break out of." << std::endl;
    return;
  }
  if (_branchType == BranchType::LOOPING) {
    _breaks.push_back(diskOffset);
    return;
  }
  _parent->addToBreaks(diskOffset);
}

void ripl::StackFrame::addToContinues(int diskOffset) {
  if (_loopLevel == 0) {
    std::cerr << "No loops to continue." << std::endl;
    return;
  }
  if (_branchType == BranchType::LOOPING) {
    _continues.push_back(diskOffset);
    return;
  }
  _parent->addToContinues(diskOffset);
}

