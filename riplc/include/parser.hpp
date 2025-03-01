#pragma once

#include "token.hpp"
#include <vector>
namespace ripl {
class Parser {
public:
  Parser(char *filename);
  ~Parser();

  void build();
  Token get() { return _tokens[_index++]; }
  bool eof() { return _index >= _tokens.size(); }
  int tokenCount() { return _tokens.size(); }

private:
  std::string text;
  int _line = 0, _column = 0, _lastline, _lastcol;
  int _pos = 0;
  int _index = 0;

  void _skipWhiteSpaces();
  std::string _nextToken();
  std::string _getQuotedString();
  void _discardComments();
  int _nextChar();
  void _unget();
  bool _endOfText() { return _pos >= text.length(); }

  std::vector<Token> _tokens;
};
} // namespace ripl
