#include "log_parser.h"

#include <boost/optional.hpp>

#include <string>
#include <cassert>

using std::string;

/* Debugging */
#include <iostream>
using std::cout;
using std::endl;

bool LogParser::ReadCharacter(char need,
                              FailType fail /* = TERMINATE_ON_FAIL */) {
  char c;
  bool read_what_was_needed = (Input >> c) && c == need;
  if (!read_what_was_needed) {
    if (fail == IGNORE_FAIL)
      Input.putback(c);
    else {
      cout << "Fail. Needed '" << need << "', but read '" << c << "'" << endl;
      assert(false);
    }
    return false;
  }
  return true;
}

void LogParser::ReadQuotedString(string& output_string) {
  char c;
  ReadCharacter('"');
  c = '\0'; // Reset

  while (Input && Input.get(c)) {
    if (c == '"')
      break;
    output_string += c;
  }
  assert(c == '"' && "Quoted string must end with \"");
}

bool LogParser::NextFieldsPresent() {
  if (ReadCharacter('}', IGNORE_FAIL)) {
    return false;
  } else {
    ReadCharacter(',');
    return true;
  }
}

bool LogParser::ReadPropertiesKeyAndValue() {
  string key;
  ReadQuotedString(key);

  string prefix = key.substr(0, 4);
  assert( prefix == "prop" );

  string suffix = key.substr(4);
  int property_id = stoi(suffix);
  --property_id; // Start from 0
  assert( 0 <= property_id && property_id < 10 );

  ReadCharacter(':');

  ReadOptional(CurrentAction.Properties[property_id]);

  return NextFieldsPresent();
}

void LogParser::ReadProperties() {
  assert(ReadCharacter('{') && "Properties dict must begin with {");
  while (ReadPropertiesKeyAndValue())
    ;
}

bool LogParser::ReadJsonKeyAndValue() {
  string key;
  ReadQuotedString(key);

  ReadCharacter(':');

  if (key == "ts_fact") {
    ReadOptional(CurrentAction.Timestamp);
  } else if (key == "fact_name") {
    string name;
    ReadQuotedString(name);
    CurrentAction.Name = name;
  } else if (key == "actor_id") {
    ReadOptional(CurrentAction.ActorId);
  } else if (key == "props") {
    ReadProperties();
  } else {
    assert(false && "Incorrect JSON key");
  }

  return NextFieldsPresent();
}

void LogParser::ReadJson() {
  bool json_present = ReadCharacter('{', IGNORE_FAIL);
  if (!json_present)
    return;

  while (ReadJsonKeyAndValue()) {
    /* empty */
  }

  Action action;
  {
    /* Fill in present action fields */
    for (int i = 0; i < 10; ++i) {
      assert(CurrentAction.Properties[i] && "All 10 properties must be provided in JSON");
      action.Properties[i] = CurrentAction.Properties[i].value();
    }
    if (CurrentAction.Timestamp) action.Timestamp = CurrentAction.Timestamp.value();
    if (CurrentAction.Name)      action.Name = CurrentAction.Name.value();
    if (CurrentAction.ActorId)   action.ActorId = CurrentAction.ActorId.value();

    /* Clear CurrentAction */
    CurrentAction = ActionWithOptionalFields();
  }
  OnActionParsed(action);
}

void LogParser::Parse(const string& logfile,
                      OnActionParsedCallback on_action_parsed) {
  CurrentAction = ActionWithOptionalFields(); /* Initialized empty */
  Input = std::ifstream(logfile);
  OnActionParsed = on_action_parsed;

  while (Input)
    ReadJson();
}
