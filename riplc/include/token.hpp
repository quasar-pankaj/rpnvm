#pragma once

#include <string>
namespace ripl {
enum class TokenType {
  LONG,
  DOUBLE,
  BOOL,
  STRING,
  IDENTIFIER,
};
struct Token {
  TokenType type;
  std::string lexeme;
  int line, column;

  Token(TokenType ttype, std::string lex, int l, int col)
      : type(ttype), lexeme(lex), line(l), column(col) {}
};
} // namespace ripl
