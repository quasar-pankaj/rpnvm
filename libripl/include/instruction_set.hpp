#pragma once

namespace ripl {
enum class Instruction : unsigned char {
  NOP = 0, // No Operation
  PUSHL,   // push long
  PUSHD,   // push double
  PUSHB,   // push bool
  PUSHS,   // push string
  ADD,     // ADD
  SUB,     // subtract
  MUL,     // multiply
  DIV,     // divide
  MOD,     // modulo
  AND,     // logical AND
  OR,      // logical or
  NOT,     // logical NOT
  EQ,      // equals
  NEQ,     // not equals
  GT,      // greater than
  LT,      // less than
  GTE,     // greater than equals
  LTE,     // less than equals
  JZ,      // jump on zero
  JF,      // jump on false
  JMP,     // jump
  ID,      // identifier
  VAR,     // variable declaration
  ASSIGN,  // assignment
  DEREF,   // Dereference a variable
  CALL,    // call subroutine
  RET,     // return
  DUP,     // Duplicate the top item
  SWAP,    // swap the top two
  ROTUP,   // bring 2nd and 3rd up moving 1st down
  ROTDN,   // move 1st and 2nd down bringing the 3rd to top
  DROP,    // discard the top
  INC,     // increment the top
  DEC,     // decrement the top
  EXPECT,  // wait for user input
  PRINT,   // Print the top of the stack
  // add more instructions here...
  END = 255,
};
} // namespace ripl
