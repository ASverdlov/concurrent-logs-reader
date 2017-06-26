#include "logfile_parser.h"

#include <boost/optional.hpp>

/* Debugging */
#include <iostream>
using std::cerr;
using std::endl;

bool LogParser::ReadCharacter(char need, bool soft_fail /* = false */) {
  char c;
  bool read_what_was_needed = (Input >> c) && c == need;
  if (!read_what_was_needed) {
    if (soft_fail)
      Input.putback(c);
    else
      assert(false);
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
  assert( Input >> CurrentAction.Properties[property_id] );

  if (ReadCharacter('}', true)) {
    return false;
  } else {
    ReadCharacter(',');
    return true;
  }
}

void LogParser::ReadProperties() {
  assert(ReadCharacter('{') && "Properties dict must begin with {");
  while (ReadPropertiesKeyAndValue())
    ;
  assert(ReadCharacter('}') && "Properties dict must end with }");
}

bool LogParser::ReadJsonKeyAndValue() {
  string key;
  ReadQuotedString(key);

  ReadCharacter(':');

  if (key == "ts_fact")
    Read(CurrentAction.Timestamp);
  else if (key == "fact_name")
    ReadQuotedString(CurrentAction.Name);
  else if (key == "actor_id")
    Read(CurrentAction.ActorId);
  else if (key == "props")
    ReadProperties();
  else
    assert(false && "Incorrect JSON key");

  if (ReadCharacter('}', true)) {
    return false;
  } else {
    ReadCharacter(',');
    return true;
  }
}

void LogParser::ReadJson() {

  assert(ReadCharacter('{') && "Json must open with {");
  cerr << "Opened JSON" << endl;
  while (ReadJsonKeyAndValue(file)) {
    /* empty */
  }
  assert(ReadCharacter('}') && "Json must close with }");
  cerr << "Closed JSON" << endl;

  Action action;
  {
    /* Fill in present action fields */
    for (int i = 0; i < 10; ++i) {
      assert(CurrentAction.Properties[i] && "All 10 properties must be provided in JSON");
      action.Properties[i] = CurrentAction.Properties[i].value();
    }
    if (timestamp) action.Timestamp = CurrentAction.Timestamp;
    if (name)      action.Name = CurrentAction.Name;
    if (actor_id)  action.ActorId = CurrentAction.ActorId;

    /* Clear CurrentAction */
    CurrentAction = ActionWithOptionalFields();
  }
  cerr << "JSON is good" << endl;
  OnActionParsed(action);

}

void LogParser::Parse(const fs::path& logfile,
                          OnActionParsedCallback on_action_parsed) {
  CurrentAction = ActionWithOptionalFields(); /* Initialized empty */
  Input = std::ifstream(logfile);
  OnActionParsed = on_action_parsed;

  while (InputFile)
    ReadJson();
}
