#pragma once

#include "executable.hpp"
#include "run_context.hpp"
#include <any>
#include <iostream>
#include <memory>
#include <string>
namespace ripl {
/*********************************************
 * All the comparison operators in one place.*
 *********************************************/
template <typename Derived> class ComparisonOperator : public Executable {
public:
  // This class is the base class for all the comparison operators. We are
  // limiting the comparisons to same types for simplicity. To compare
  // say long and double, the long value will need to be upcasted to double.
  // This class does all the heavy lifting and delegates the operations to
  // subclasses.
  void execute(RunContext *context) override {
    auto rhs = context->top();
    context->pop();
    auto lhs = context->top();
    if (rhs->type() != lhs->type()) { // check if the types are same
      std::cerr << "Can't compare dissimilar types." << std::endl;
      context->push(rhs); // if not the stack is restored
      return;
    }
    context->pop();
    bool result;
    if (lhs->type() == typeid(double)) {
      double l = std::any_cast<double>(*lhs);
      double r = std::any_cast<double>(*rhs);
      result = dynamic_cast<Derived *>(this)->template compare<double>(l, r);
    } else if (lhs->type() == typeid(long)) {
      long l = std::any_cast<long>(*lhs);
      long r = std::any_cast<long>(*rhs);
      result = dynamic_cast<Derived *>(this)->template compare<long>(l, r);
    } else if (lhs->type() == typeid(std::string)) {
      std::string l = std::any_cast<std::string>(*lhs);
      std::string r = std::any_cast<std::string>(*rhs);
      result =
          dynamic_cast<Derived *>(this)->template compare<std::string>(l, r);
    } else if (lhs->type() == typeid(bool)) {
      bool l = std::any_cast<bool>(*lhs);
      bool r = std::any_cast<bool>(*rhs);
      result = dynamic_cast<Derived *>(this)->template compare<bool>(l, r);
    } else {
      std::cerr << "Unknown type. " << lhs->type().name() << std::endl;
    }
    context->push(std::make_shared<std::any>(
        std::any(result))); // push the result on stack.
  }
};

class Equality : public ComparisonOperator<Equality> {
public:
  template <typename T> bool compare(T lhs, T rhs) { return lhs == rhs; }
};

class NotEquality : public ComparisonOperator<NotEquality> {
public:
  template <typename T> bool compare(T lhs, T rhs) { return lhs != rhs; }
};

class LessThan : public ComparisonOperator<LessThan> {
public:
  template <typename T> bool compare(T lhs, T rhs) { return lhs < rhs; }
};

class GreaterThan : public ComparisonOperator<GreaterThan> {
public:
  template <typename T> bool compare(T lhs, T rhs) { return lhs > rhs; }
};

class GreaterThanEquals : public ComparisonOperator<GreaterThanEquals> {
public:
  template <typename T> bool compare(T lhs, T rhs) { return lhs >= rhs; }
};

class LessThanEquals : public ComparisonOperator<LessThanEquals> {
public:
  template <typename T> bool compare(T lhs, T rhs) { return lhs <= rhs; }
};

} // namespace ripl
