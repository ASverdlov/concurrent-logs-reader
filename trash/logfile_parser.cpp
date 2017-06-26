#include "logfile_parser.h"

#include <boost/optional.hpp>

void LogParser::ReadOpenBracketAndSetState(ParserState next_state) {
  char c;
  if (in >> c) {
    assert(c == '{');
    State = next_state;
  }
}

void LogParser::ReadJsonCloseBracket() {
  char c;
  if (in >> c) {
    assert(c == '}');

    Action result_action;
    {
      /* Fill in present action fields */
      for (int i = 0; i < 10; ++i) {
        assert(CurrentAction.Properties[i] && "All 10 properties must be provided in JSON");
        result_action.Properties[i] = CurrentAction.Properties[i].value();
      }
      if (timestamp) result_action.Timestamp = CurrentAction.timestamp;
      if (name)      result_action.Name = CurrentAction.Name;
      if (actor_id)  result_action.ActorId = CurrentAction.ActorId;

      /* Clear CurrentAction */
      CurrentAction = ActionWithOptionalFields();
    }

    OnActionParsed(action);
    State = JSON_OPEN_BRACKET_IS_NEXT;
  }
}

static std::string ReadQuotedString(std::ifstream& in) {
  char c;
  assert((in >> c) && c == '"' && "Quoted string must open with \"");

  std::string keyword;
  while (in.get(c)) {
    if (c == '"')
      break;
    keyword += c;
  }
  assert(c == '"' && "Quoted string must end with \"");
  return keyword;
}

void LogParser::ReadKeyword(std::ifstream& in) {
  char c;
  assert((in >> c) && c == '"');

  std::string keyword = ReadQuotedString(in);
  assert((in >> c) && c == ':' && "There must be : after key in JSON dict");

  if (keyword == "ts_fact")
    State = TIMESTAMP_VALUE_IS_NEXT;
  else if (keyword == "fact_name")
    State = NAME_VALUE_IS_NEXT;
  else if (keyword == "actor_id")
    State = ACTOR_ID_VALUE_IS_NEXT;
  else if (keyword == "props")
    State = PROPERTIES_OPEN_BRACKET_IS_NEXT;
  else
    assert(false && "Incorrect JSON key");
}

void LogParser::ReadAfterValue(std::ifstream& in) {
}

void LogParser::ReadTimestampValue(std::ifstream& in) {
  assert((in >> CurrentAction.Timestamp) && "Timestamp value must follow");
  ReadAfterValue(in);
}

void LogParser::ReadNameValue(std::ifstream& in) {
  CurrentAction.Name = ReadQuotedString(in);
  ReadAfterValue(in);
}

void LogParser::ReadActorIdValue(std::ifstream& in) {
  assert((in >> CurrentAction.ActorId) && "Actor ID value must follow");
  ReadAfterValue(in);
}

void LogParser::Parse(const fs::path& logfile,
                          OnActionParsedCallback OnActionParsed) {
  State = JSON_OPEN_BRACKET_IS_NEXT;
  CurrentAction = ActionWithOptionalFields(); /* Initialized empty */

  std::ifstream file(logfile);
  while (file) {
    switch (State) {
    case JSON_OPEN_BRACKET_IS_NEXT:
      ReadOpenBracketAndSetState(in, KEYWORD_IS_NEXT);
      break;

    case JSON_CLOSE_BRACKET_IS_NEXT:
      ReadJsonCloseBracket(in);
      break;

    case KEYWORD_IS_NEXT:
      ReadKeyword(in);
      break;

    /* Values */
    case TIMESTAMP_VALUE_IS_NEXT:
      ReadTimestampValue(in);
      break;

    case NAME_VALUE_IS_NEXT:
      ReadNameValue(in);
      break;

    case ACTOR_ID_VALUE_IS_NEXT:
      ReadActorIdValue(in);
      break;

    /* Properties value */
    case PROPERTIES_OPEN_BRACKET_IS_NEXT:
      ReadOpenBracket(in, PROPERTIES_KEYWORD_IS_NEXT);
      break;

    case PROPERTIES_CLOSE_BRACKET_IS_NEXT:
      ReadOpenBracket(in, PROPERTIES_KEYWORD_IS_NEXT);
      break;

    default:
      assert(false && "Invalid parser state");
    }
  }
}
