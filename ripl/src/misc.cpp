#include "misc.hpp"
#include "run_context.hpp"
#include "utils.hpp"
#include <iostream>
#include <cctype>

#define INPUT_SIZE 255

void ripl::Print::execute(ripl::RunContext *context) {
  auto value = context->top();
  context->pop();
  if (value->type() == typeid(double)) {
    std::cout << std::any_cast<double>(*value) << std::endl;
    return;
  }
  if (value->type() == typeid(long)) {
    std::cout << std::any_cast<long>(*value) << std::endl;
    return;
  }
  if (value->type() == typeid(std::string)) {
    std::cout << std::any_cast<std::string>(*value) << std::endl;
    return;
  }
  if (value->type() == typeid(bool)) {
    std::cout << std::any_cast<bool>(*value) << std::endl;
    return;
  }
  std::cout << "Unknown type " << value->type().name() << std::endl;
}

void ripl::Input::execute(ripl::RunContext *context) {
  char input[INPUT_SIZE];
  std::cin.getline(input, INPUT_SIZE);
  std::string token(input);
  std::any value;
  std::shared_ptr<std::any> valueptr;
  if (ripl::isIntegral(token)) {
    long l = std::stol(token);
    value = l;
    valueptr = std::make_shared<std::any>(value);
    context->push(valueptr);
    return;
  }
  if (ripl::isFloat(token)) {
    double d = std::stod(token);
    value = d;
    valueptr = std::make_shared<std::any>(value);
    context->push(valueptr);
    return;
  }
  if (ripl::isBool(token)) {
    value = token == "true" ? true : false;
    valueptr = std::make_shared<std::any>(value);
    context->push(valueptr);
    return;
  }
  valueptr = std::make_shared<std::any>(token);
  context->push(valueptr);
}
