#pragma once

#include <sstream>
#include <string>
namespace ripl {
class Dism {
public:
  Dism(char *filename);
  ~Dism();

  int readInt();
  long readLong();
  double readDouble();
  bool readBool();
  std::string readString();

  void disassemble();

private:
  int _ip = 0;
  std::stringstream _buf;
};
} // namespace ripl
