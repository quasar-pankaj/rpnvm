#include "parser.hpp"
#include "token.hpp"
#include "utils.hpp"
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

ripl::Parser::Parser(char *filename) {
  std::ifstream in(filename);
  std::stringstream buf;

  if (!in.is_open()) {
    std::cerr << "Could not open the file " << filename
              << " for input.\nDid you specify the filename correctly?"
              << std::endl;
    return;
  }
  buf << in.rdbuf();

  text = buf.str();
  in.close();
}

ripl::Parser::~Parser() {}

void ripl::Parser::build() {
  char ch;

  while (!_endOfText()) {
    ch = _nextChar();
    if (ch == '#') {
      _discardComments();
      continue;
    }
    if (ch == '\"') {
      std::string s = _getQuotedString();
      Token tok(TokenType::STRING, s, _line, _column);
      _tokens.push_back(tok);
      continue;
    }
    if (isspace(ch)) {
      _skipWhiteSpaces();
      continue;
    }
    _unget();
    std::string lexeme = _nextToken();
    if (isIntegral(lexeme)) {
      Token tok(TokenType::LONG, lexeme, _line, _column);
      _tokens.push_back(tok);
      continue;
    }
    if (isFloat(lexeme)) {
      Token tok(TokenType::DOUBLE, lexeme, _line, _column);
      _tokens.push_back(tok);
      continue;
    }
    if (isBool(lexeme)) {
      Token tok(TokenType::BOOL, lexeme, _line, _column);
      _tokens.push_back(tok);
      continue;
    }
    Token tok(TokenType::IDENTIFIER, lexeme, _line, _column);
    _tokens.push_back(tok);
    continue;
  }
}

void ripl::Parser::_skipWhiteSpaces() {
  char ch = _nextChar();
  while (std::isspace(ch)) {
    if (_endOfText()) {
      break;
    }
    ch = _nextChar();
  }
  _unget();
}

std::string ripl::Parser::_nextToken() {
  char ch = _nextChar();
  std::string token = "";
  while (!std::isspace(ch)) {
    if (_endOfText()) {
      break;
    }
    token += ch;
    ch = _nextChar();
  }
  _unget();
  return token;
}

std::string ripl::Parser::_getQuotedString() {
  char ch;
  std::string str = "";
  while ((ch = _nextChar()) != '\"') {
    if (_endOfText()) {
      _unget();
      std::cerr << "Reached End before encountering \" here: " << str
                << std::endl;
      break;
    }
    str += ch;
  }
  return str;
}

void ripl::Parser::_discardComments() {
  char ch;
  while ((ch = _nextChar()) != '\n') {
    if (_endOfText()) {
      _unget();
      break;
    }
  }
}

int ripl::Parser::_nextChar() {
  int ch = text[_pos++];
  if (ch == '\n') {
    _line++;
    _column = 0;
  } else if (ch == '\t') {
    _column += 8;
  } else {
    _column++;
  }
  _lastline = _line;
  _lastcol = _column;

  return ch;
}

void ripl::Parser::_unget() {
  _pos--;
  _column = _lastcol;
  _line = _lastline;
}
