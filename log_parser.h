#ifndef LOGFILE_PARSER_H
#define LOGFILE_PARSER_H

#include "action.h"

#include <boost/optional.hpp>

#include <fstream>
#include <string>

class LogParser {
public:
  using OnActionParsedCallback = std::function<void(const Action&)>;

  void Parse(const std::string& logfile, OnActionParsedCallback on_action_parsed);

private:
  struct ActionWithOptionalFields {
    boost::optional<uint32_t> Timestamp;
    boost::optional<std::string> Name;
    boost::optional<uint32_t> ActorId;
    boost::optional<int> Properties[Action::NUM_PROPERTIES];
  };

  enum FailType {
    IGNORE_FAIL,
    TERMINATE_ON_FAIL
  };

  void ReadJson();
  bool ReadJsonKeyAndValue();

  void ReadProperties();
  bool ReadPropertiesKeyAndValue();

  bool NextFieldsPresent();

  void ReadQuotedString(std::string& output_string);

  template<typename T>
  inline void ReadOptional(boost::optional<T>& value, FailType fail = TERMINATE_ON_FAIL);

  bool ReadCharacter(char need, FailType fail = TERMINATE_ON_FAIL);

  OnActionParsedCallback OnActionParsed;
  std::ifstream Input;
  ActionWithOptionalFields CurrentAction;
};

template<typename T>
inline void LogParser::ReadOptional(boost::optional<T>& output, FailType fail) {
  T value;
  if (Input >> value) {
    output = value;
  } else if (fail == TERMINATE_ON_FAIL) {
    assert(false && "Failed to read");
  } else /* IGNORE_FAIL */ {
    ; /* do nothing */
  }
}

#endif
