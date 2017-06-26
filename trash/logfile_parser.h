#ifndef LOGFILE_PARSER_H
#define LOGFILE_PARSER_H

#include "action.h"

#include <fstream>
#include <string>

class LogParser {
public:
  using OnActionParsedCallback = std::function<void(const Action&)>;

  void Parse(const fs::path& logfile, OnActionParsedCallback on_action_parsed);

private:
  struct ActionWithOptionalFields {
    boost::optional<uint32_t> Timestamp;
    boost::optional<std::string> Name;
    boost::optional<uint32_t> ActorId;
    boost::optional<int> Properties[10];
  };

  void ReadJson();
  bool ReadJsonKeyAndValue();

  void ReadProperties();
  bool ReadPropertiesKeyAndValue();

  void ReadQuotedString(std::string& output_string);

  template<typename T>
  void Read(T value, bool soft_fail = false);

  bool ReadCharacter(char need, bool soft_fail = false);

  OnActonParsedCallback OnActionParsed;
  std::ifstream Input;
  ActionWithOptionalFields CurrentAction;
};

template<typename T>
void LogParser::Read(T value, bool soft_fail) {
  if (soft_fail)
    Input >> value;
  else
    assert(Input && (Input >> value));
}


#endif
