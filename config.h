#ifndef CONFIG_H
#define CONFIG_H

#include <string>

struct Config {
  void ParseOptions(int argc, char* argv[]);

  std::string Directory;
  unsigned FilesNum;
  unsigned JobsNum;
  std::string OutputFile;
};

#endif
