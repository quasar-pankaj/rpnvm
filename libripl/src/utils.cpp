#include "utils.hpp"
#include <string>

bool ripl::isIntegral(std::string &token) {
  for (int i = 0; i < token.length(); i++) {
    if (!isdigit(token[i])) {
      return false;
    }
  }
  return true;
}

bool ripl::isFloat(std::string &token) {
  bool hasPoint = false;
  for (int i = 0; i < token.length(); i++) {
    if (token[i] == '.' && hasPoint) {
      return false;
    }
    if (token[i] == '.') {
      hasPoint = true;
      continue;
    }
    if (!isdigit(token[i])) {
      return false;
    }
  }
  return true;
}

bool ripl::isBool(std::string &token) {
  std::string lower = toLower(token);

  if (lower == "true" || lower == "false") {
    return true;
  }

  return false;
}

std::string ripl::toLower(std::string &token) {
  std::string lower;
  for (int i = 0; i < token.length(); i++) {
    lower += tolower(token[i]);
  }
  return lower;
}
