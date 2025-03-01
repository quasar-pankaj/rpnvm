#pragma once

/********************************************
 * All the arithmetic operators are here    *
 ********************************************/

#include "executable.hpp"
#include "run_context.hpp"
#include <any>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <ostream>
#include <type_traits>
namespace ripl {
// This is the base class for all the arithmetic operators
// this is a templated class that takes the type of the
// derived class. This pattern is known as CRTP or the Curiously Recurring
// Template Pattern. There is a templated fetch method in the Context class.
// This is the class that does all the heavy lifting delegating The
// operations to the subclasses.
template <typename Derived> class ArithmeticOperator : public Executable {
public:
  // execute method is from the base class Executable
  void execute(RunContext *context) override {
    // we will first try fetching the value as a double. the method returns a
    // pair first value is a bool which will be true if the conversion is
    // successful, false otherwise. The second value will be the actual value.
    auto [dvalidrhs, dvaluerhs] = context->fetch<double>();

    // here we check if the value is valid
    if (dvalidrhs) {
      // if the value is valid we try the same for the second operand
      if (tryOperate(dvaluerhs, context)) {
        // if the operation is successful we return from this method.
        return;
      }
    }

    // We come here if the fetch was unsuccessful and we repeat the above
    // operations again.
    auto [lvalidrhs, lvaluerhs] = context->fetch<long>();

    if (lvalidrhs) {
      if (tryOperate(lvaluerhs, context)) {
        return;
      }
    }

    // the value was not recognised so give an error message.
    std::cerr << "Expected a long or a double value as rhs." << std::endl;
  }

private:
  // This is a templated method which implements the CRTP pattern.
  // the pointer to this is type casted to that of the derived type
  // and a call to the function which is in the derived class is made.

  template <typename L, typename R, typename RES = std::common_type<L, R>::type>
  void doOperate(L lhs, R rhs, RunContext *context) {
    RES result =
        dynamic_cast<Derived *>(this)->template operate<L, R>(lhs, rhs);
    // we need to store the value in the stack in a type safe manner so
    // we use std::any class here and push it back to the stack.
    std::any a = std::make_any<RES>(result);
    std::shared_ptr<std::any> resultptr = std::make_shared<std::any>(a);
    context->push(resultptr);
  }

  // here we try to operate  successively on double and long. I have only
  // used double and long but you are free to add others. You get the idea
  // of what it involves.
  template <typename T> bool tryOperate(T rhs, RunContext *context) {
    auto [dvalidlhs, dvaluelhs] = context->fetch<double>();
    if (dvalidlhs) {
      doOperate(dvaluelhs, rhs, context);
      return true;
    }
    auto [lvalidlhs, lvaluelhs] = context->fetch<long>();
    if (lvalidlhs) {
      doOperate(lvaluelhs, rhs, context);
      return true;
    }
    std::cerr << "Expected a long or a double value as lhs." << std::endl;
    return false;
  }
};

class Add : public ArithmeticOperator<Add> {
public:
  template <typename L, typename R, typename RES = std::common_type<L, R>::type>
  RES operate(L lhs, R rhs) {
    return lhs + rhs;
  }
};

class Sub : public ArithmeticOperator<Sub> {
public:
  template <typename L, typename R, typename RES = std::common_type<L, R>::type>
  RES operate(L lhs, R rhs) {
    return lhs - rhs;
  }
};

class Mul : public ArithmeticOperator<Mul> {
public:
  template <typename L, typename R, typename RES = std::common_type<L, R>::type>
  RES operate(L lhs, R rhs) {
    return lhs * rhs;
  }
};

class Div : public ArithmeticOperator<Div> {
public:
  template <typename L, typename R, typename RES = std::common_type<L, R>::type>
  RES operate(L lhs, R rhs) {
    return lhs / rhs;
  }
};

class Mod : public Executable {
  /*
   * this is the class that is not a direct descentant of ArithmeticOperator as
   * mod operator only works with integral types such as int or long. We limit
   * ourselves to long but you can add more.
   */
public:
  void execute(RunContext *context) override {
    auto [rvalid, rhs] = context->fetch<long>();
    if (!rvalid) {
      std::cerr << "RHS of the expression is not integral as required by Mod. "
                << std::endl;
      return;
    }
    auto [lvalid, lhs] = context->fetch<long>();
    if (!lvalid) {
      std::cerr << "LHS of the expression is not integral as required by Mod. "
                << std::endl;
      context->push(std::make_shared<std::any>(std::any(rhs)));
      return;
    }

    long result = lhs % rhs;

    std::shared_ptr<std::any> res =
        std::make_shared<std::any>(std::any(result));
    context->push(res);
  }
};

} // namespace ripl
