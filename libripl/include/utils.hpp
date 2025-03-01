#pragma once

#include <string>
namespace ripl {
bool isIntegral(std::string &token);
bool isFloat(std::string &token);
bool isBool(std::string &token);
std::string toLower(std::string &token);
} // namespace ripl
